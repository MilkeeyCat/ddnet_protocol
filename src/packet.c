#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/control_message.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/fetch_chunks.h>
#include <ddnet_protocol/huffman.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/packet.h>
#include <ddnet_protocol/token.h>

DDNetPacketHeader ddnet_decode_packet_header(const uint8_t *buf) {
	return (DDNetPacketHeader){
		.flags = buf[0] >> 2,
		.ack = ((buf[0] & 0x3) << 8) | buf[1],
		.num_chunks = buf[2],
	};
}

DDNetError ddnet_encode_packet_header(const DDNetPacketHeader *header, uint8_t *buf) {
	if(header->ack >= DDNET_MAX_SEQUENCE) {
		return DDNET_ERR_ACK_OUT_OF_BOUNDS;
	}
	buf[0] = ((header->flags << 2) & 0xfc | ((header->ack >> 8)) & 0x3);
	buf[1] = header->ack & 0xff;
	buf[2] = header->num_chunks;
	return DDNET_ERR_NONE;
}

typedef struct {
	DDNetChunk *chunks;
	size_t len;
} Context;

static void on_chunk(void *ctx, DDNetChunk *chunk) {
	Context *context = ctx;

	memcpy(&context->chunks[context->len++], chunk, sizeof(DDNetChunk));
}

size_t ddnet_get_packet_payload(DDNetPacketHeader *header, const uint8_t *full_data, size_t full_len, uint8_t *payload, size_t payload_len, DDNetError *err) {
	full_data += DDNET_PACKET_HEADER_SIZE;
	full_len -= DDNET_PACKET_HEADER_SIZE;
	if(header->flags & DDNET_PACKET_FLAG_COMPRESSION) {
		return ddnet_huffman_decompress(full_data, full_len, payload, payload_len, err);
	}
	memcpy(payload, full_data, full_len);
	return full_len;
}

DDNetPacket ddnet_decode_packet(const uint8_t *buf, size_t len, DDNetError *err) {
	DDNetPacket packet = {};

	if(len < DDNET_PACKET_HEADER_SIZE || len > DDNET_MAX_PACKET_SIZE) {
		if(err) {
			*err = DDNET_ERR_INVALID_PACKET;
		}

		return packet;
	}

	packet.header = ddnet_decode_packet_header(buf);
	packet.payload = malloc(DDNET_MAX_PACKET_SIZE);
	DDNetError payload_err = DDNET_ERR_NONE;
	packet.payload_len = ddnet_get_packet_payload(&packet.header, buf, len, packet.payload, DDNET_MAX_PACKET_SIZE, &payload_err);
	if(payload_err != DDNET_ERR_NONE) {
		if(err) {
			*err = payload_err;
		}
		return packet;
	}

	if(packet.header.flags & DDNET_PACKET_FLAG_CONTROL) {
		packet.kind = DDNET_PACKET_CONTROL;
		size_t size = ddnet_decode_control(packet.payload, packet.payload_len, &packet.control, err); // NOLINT(clang-analyzer-unix.Malloc)
		packet.header.token = ddnet_read_token(packet.payload + size);
	} else {
		packet.kind = DDNET_PACKET_NORMAL;
		Context ctx = {
			.chunks = malloc(sizeof(DDNetChunk) * packet.header.num_chunks),
			.len = 0,
		};
		DDNetError chunk_err = DDNET_ERR_NONE;
		size_t size = ddnet_fetch_chunks(packet.payload, packet.payload_len, &packet.header, on_chunk, &ctx, &chunk_err);
		size_t space = packet.payload_len - size;
		if(chunk_err != DDNET_ERR_NONE) {
			if(err) {
				*err = chunk_err;
			}

			return packet;
		}

		packet.chunks.data = ctx.chunks;
		packet.chunks.len = ctx.len;

		// missing ddnet security token
		// this is an error in the ddnet protocol
		// but expected in the teeworlds protocol
		//
		// because pure teeworlds is not in scope we throw an error
		// https://github.com/MilkeeyCat/ddnet_protocol/issues/48
		if(space < sizeof(DDNetToken)) {
			if(err) {
				*err = DDNET_ERR_MISSING_DDNET_SECURITY_TOKEN;
			}

			return packet;
		}

		if(space > sizeof(DDNetToken)) {
			// we did already parse the expected amount of chunks
			// and the ddnet security token
			// but there are still bytes left!
			if(err) {
				*err = DDNET_ERR_REMAINING_BYTES_IN_BUFFER;
			}

			return packet;
		}

		packet.header.token = ddnet_read_token(packet.payload + size);
	}

	return packet;
}

size_t ddnet_encode_packet(const DDNetPacket *packet, uint8_t *buf, size_t len, DDNetError *err) {
	if(len < DDNET_PACKET_HEADER_SIZE) {
		*err = DDNET_ERR_BUFFER_FULL;
		return 0;
	}

	uint8_t *start = buf;
	ddnet_encode_packet_header(&packet->header, buf);
	buf += DDNET_PACKET_HEADER_SIZE;

	switch(packet->kind) {
	case DDNET_PACKET_NORMAL:
		for(size_t i = 0; i < packet->chunks.len; i++) {
			buf += ddnet_encode_chunk_header(&packet->chunks.data[i].header, buf);
			buf += ddnet_encode_message(&packet->chunks.data[i], buf, err);
		}
		ddnet_write_token(packet->header.token, buf);
		buf += sizeof(DDNetToken);
		return buf - start;
	case DDNET_PACKET_CONTROL:
		buf += ddnet_encode_control(&packet->control, buf, err);
		ddnet_write_token(packet->header.token, buf);
		buf += sizeof(DDNetToken);
		return buf - start;
	case DDNET_PACKET_CONNLESS:
		break;
	}

	*err = DDNET_ERR_INVALID_PACKET;
	return 0;
}

DDNetError ddnet_build_packet(DDNetPacket *packet, const DDNetMessage messages[], uint8_t messages_len, DDNetSession *session) {
	packet->kind = DDNET_PACKET_NORMAL;
	packet->header.flags = 0;
	packet->header.token = session->token;
	packet->header.ack = session->ack;
	packet->header.num_chunks = messages_len;
	packet->chunks.len = messages_len;
	packet->chunks.data = malloc(sizeof(DDNetChunk) * messages_len);
	for(uint8_t i = 0; i < messages_len; i++) {
		const DDNetMessage *msg = &messages[i];
		packet->chunks.data[i].payload.kind = msg->kind;
		packet->chunks.data[i].payload.msg = msg->msg;
		DDNetError chunk_err = ddnet_fill_chunk_header(&packet->chunks.data[i]);
		if(packet->chunks.data[i].header.flags & DDNET_CHUNK_FLAG_VITAL) {
			session->sequence++;
		}
		packet->chunks.data[i].header.sequence = session->sequence;
		if(chunk_err != DDNET_ERR_NONE) {
			return chunk_err;
		}
	}
	return DDNET_ERR_NONE;
}

DDNetError ddnet_free_packet(DDNetPacket *packet) {
	if(packet->kind == DDNET_PACKET_NORMAL) {
		free(packet->chunks.data);
	}
	free(packet->payload);

	return DDNET_ERR_NONE;
}
