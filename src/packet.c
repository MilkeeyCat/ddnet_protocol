#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/control_message.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/fetch_chunks.h>
#include <ddnet_protocol/huffman.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/packet.h>
#include <ddnet_protocol/snapshot.h>
#include <ddnet_protocol/token.h>

DDProtoPacketHeader ddproto_decode_packet_header(const uint8_t *buf) {
	return (DDProtoPacketHeader){
		.flags = buf[0] >> 2,
		.ack = ((buf[0] & 0x3) << 8) | buf[1],
		.num_chunks = buf[2],
	};
}

DDProtoError ddproto_encode_packet_header(const DDProtoPacketHeader *header, uint8_t *buf) {
	if(header->ack >= DDPROTO_MAX_SEQUENCE) {
		return DDPROTO_ERR_ACK_OUT_OF_BOUNDS;
	}
	buf[0] = ((header->flags << 2) & 0xfc | ((header->ack >> 8)) & 0x3);
	buf[1] = header->ack & 0xff;
	buf[2] = header->num_chunks;
	return DDPROTO_ERR_NONE;
}

typedef struct {
	DDProtoChunk *chunks;
	size_t len;
} Context;

static void on_chunk(void *ctx, DDProtoChunk *chunk) {
	Context *context = ctx;

	memcpy(&context->chunks[context->len++], chunk, sizeof(DDProtoChunk));
}

size_t ddproto_get_packet_payload(DDProtoPacketHeader *header, const uint8_t *full_data, size_t full_len, uint8_t *payload, size_t payload_len, DDProtoError *err) {
	full_data += DDPROTO_PACKET_HEADER_SIZE;
	full_len -= DDPROTO_PACKET_HEADER_SIZE;
	if(header->flags & DDPROTO_PACKET_FLAG_COMPRESSION) {
		return ddproto_huffman_decompress(full_data, full_len, payload, payload_len, err);
	}
	memcpy(payload, full_data, full_len);
	return full_len;
}

DDProtoPacket ddproto_decode_packet(const uint8_t *buf, size_t len, DDProtoError *err) {
	DDProtoPacket packet = {};

	if(len < DDPROTO_PACKET_HEADER_SIZE || len > DDPROTO_MAX_PACKET_SIZE) {
		if(err) {
			*err = DDPROTO_ERR_INVALID_PACKET;
		}

		return packet;
	}

	packet.header = ddproto_decode_packet_header(buf);
	packet.payload = malloc(DDPROTO_MAX_PACKET_SIZE);
	DDProtoError payload_err = DDPROTO_ERR_NONE;
	packet.payload_len = ddproto_get_packet_payload(&packet.header, buf, len, packet.payload, DDPROTO_MAX_PACKET_SIZE, &payload_err);
	if(payload_err != DDPROTO_ERR_NONE) {
		if(err) {
			*err = payload_err;
		}
		return packet;
	}

	if(packet.header.flags & DDPROTO_PACKET_FLAG_CONTROL) {
		packet.kind = DDPROTO_PACKET_CONTROL;
		size_t size = ddproto_decode_control(packet.payload, packet.payload_len, &packet.control, err); // NOLINT(clang-analyzer-unix.Malloc)
		packet.header.token = ddproto_read_token(packet.payload + size);
	} else {
		packet.kind = DDPROTO_PACKET_NORMAL;
		Context ctx = {
			.chunks = malloc(sizeof(DDProtoChunk) * packet.header.num_chunks),
			.len = 0,
		};
		DDProtoError chunk_err = DDPROTO_ERR_NONE;
		size_t size = ddproto_fetch_chunks(packet.payload, packet.payload_len, &packet.header, on_chunk, &ctx, &chunk_err);
		size_t space = packet.payload_len - size;
		if(chunk_err != DDPROTO_ERR_NONE) {
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
		if(space < sizeof(DDProtoToken)) {
			if(err) {
				*err = DDPROTO_ERR_MISSING_DDNET_SECURITY_TOKEN;
			}

			return packet;
		}

		if(space > sizeof(DDProtoToken)) {
			// we did already parse the expected amount of chunks
			// and the ddnet security token
			// but there are still bytes left!
			if(err) {
				*err = DDPROTO_ERR_REMAINING_BYTES_IN_BUFFER;
			}

			return packet;
		}

		packet.header.token = ddproto_read_token(packet.payload + size);
	}

	return packet;
}

size_t ddproto_encode_packet(const DDProtoPacket *packet, uint8_t *buf, size_t len, DDProtoError *err) {
	if(len < DDPROTO_PACKET_HEADER_SIZE) {
		*err = DDPROTO_ERR_BUFFER_FULL;
		return 0;
	}

	uint8_t *start = buf;
	ddproto_encode_packet_header(&packet->header, buf);
	buf += DDPROTO_PACKET_HEADER_SIZE;

	switch(packet->kind) {
	case DDPROTO_PACKET_NORMAL:
		for(size_t i = 0; i < packet->chunks.len; i++) {
			buf += ddproto_encode_chunk_header(&packet->chunks.data[i].header, buf);
			buf += ddproto_encode_message(&packet->chunks.data[i], buf, err);
		}
		ddproto_write_token(packet->header.token, buf);
		buf += sizeof(DDProtoToken);
		return buf - start;
	case DDPROTO_PACKET_CONTROL:
		buf += ddproto_encode_control(&packet->control, buf, err);
		ddproto_write_token(packet->header.token, buf);
		buf += sizeof(DDProtoToken);
		return buf - start;
	case DDPROTO_PACKET_CONNLESS:
		break;
	}

	*err = DDPROTO_ERR_INVALID_PACKET;
	return 0;
}

DDProtoError ddproto_build_packet(DDProtoPacket *packet, const DDProtoMessage messages[], uint8_t messages_len, DDProtoSession *session) {
	packet->kind = DDPROTO_PACKET_NORMAL;
	packet->header.flags = 0;
	packet->header.token = session->token;
	packet->header.ack = session->ack;
	packet->header.num_chunks = messages_len;
	packet->chunks.len = messages_len;
	packet->chunks.data = malloc(sizeof(DDProtoChunk) * messages_len);
	for(uint8_t i = 0; i < messages_len; i++) {
		const DDProtoMessage *msg = &messages[i];
		packet->chunks.data[i].payload.kind = msg->kind;
		packet->chunks.data[i].payload.msg = msg->msg;
		DDProtoError chunk_err = ddproto_fill_chunk_header(&packet->chunks.data[i]);
		if(packet->chunks.data[i].header.flags & DDPROTO_CHUNK_FLAG_VITAL) {
			session->sequence++;
		}
		packet->chunks.data[i].header.sequence = session->sequence;
		if(chunk_err != DDPROTO_ERR_NONE) {
			return chunk_err;
		}
	}
	return DDPROTO_ERR_NONE;
}

DDProtoError ddproto_free_packet(DDProtoPacket *packet) {
	if(packet->kind == DDPROTO_PACKET_NORMAL) {
		for(size_t i = 0; i < packet->chunks.len; i++) {
			DDProtoChunk *chunk = &packet->chunks.data[i];
			if(chunk->payload.kind == DDPROTO_MSG_KIND_SNAPSINGLE) {
				DDProtoSnapshot *snap = &chunk->payload.msg.snap_single.snapshot;
				ddproto_free_snapshot(snap);
			}
		}
		free(packet->chunks.data);
	}
	free(packet->payload);

	return DDPROTO_ERR_NONE;
}
