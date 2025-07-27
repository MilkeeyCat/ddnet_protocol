#include "packet.h"
#include "chunk.h"
#include "control_message.h"
#include "errors.h"
#include "fetch_chunks.h"
#include "huffman.h"
#include "message.h"
#include "token.h"

PacketHeader decode_packet_header(const uint8_t *buf) {
	return (PacketHeader){
		.flags = buf[0] >> 2,
		.ack = ((buf[0] & 0x3) << 8) | buf[1],
		.num_chunks = buf[2],
	};
}

DDNetError encode_packet_header(const PacketHeader *header, uint8_t *buf) {
	if(header->ack >= MAX_SEQUENCE) {
		return DDNET_ERR_ACK_OUT_OF_BOUNDS;
	}
	buf[0] = ((header->flags << 2) & 0xfc | ((header->ack >> 8)) & 0x3);
	buf[1] = header->ack & 0xff;
	buf[2] = header->num_chunks;
	return DDNET_ERR_NONE;
}

typedef struct {
	Chunk *chunks;
	size_t len;
} Context;

static void on_chunk(void *ctx, Chunk *chunk) {
	Context *context = ctx;

	memcpy(&context->chunks[context->len++], chunk, sizeof(Chunk));
}

size_t get_packet_payload(PacketHeader *header, const uint8_t *full_data, size_t full_len, uint8_t *payload, size_t payload_len, DDNetError *err) {
	full_data += PACKET_HEADER_SIZE;
	full_len -= PACKET_HEADER_SIZE;
	if(header->flags & PACKET_FLAG_COMPRESSION) {
		return ddnet_huffman_decompress(full_data, full_len, payload, payload_len, err);
	}
	memcpy(payload, full_data, full_len);
	return full_len;
}

DDNetPacket decode_packet(const uint8_t *buf, size_t len, DDNetError *err) {
	DDNetPacket packet = {};

	if(len < PACKET_HEADER_SIZE || len > MAX_PACKET_SIZE) {
		if(err) {
			*err = DDNET_ERR_INVALID_PACKET;
		}

		return packet;
	}

	packet.header = decode_packet_header(buf);
	packet.payload = malloc(MAX_PACKET_SIZE);
	DDNetError payload_err = DDNET_ERR_NONE;
	packet.payload_len = get_packet_payload(&packet.header, buf, len, packet.payload, MAX_PACKET_SIZE, &payload_err);
	if(payload_err != DDNET_ERR_NONE) {
		if(err) {
			*err = payload_err;
		}
		return packet;
	}

	if(packet.header.flags & PACKET_FLAG_CONTROL) {
		packet.kind = PACKET_CONTROL;
		size_t size = decode_control(packet.payload, packet.payload_len, &packet.control, err); // NOLINT(clang-analyzer-unix.Malloc)
		packet.header.token = read_token(packet.payload + size);
	} else {
		packet.kind = PACKET_NORMAL;
		Context ctx = {
			.chunks = malloc(sizeof(Chunk) * packet.header.num_chunks),
			.len = 0,
		};
		DDNetError chunk_err = DDNET_ERR_NONE;
		size_t size = fetch_chunks(packet.payload, packet.payload_len, &packet.header, on_chunk, &ctx, &chunk_err);
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
		if(space < sizeof(Token)) {
			if(err) {
				*err = DDNET_ERR_MISSING_DDNET_SECURITY_TOKEN;
			}

			return packet;
		}

		if(space > sizeof(Token)) {
			// we did already parse the expected amount of chunks
			// and the ddnet security token
			// but there are still bytes left!
			if(err) {
				*err = DDNET_ERR_REMAINING_BYTES_IN_BUFFER;
			}

			return packet;
		}

		packet.header.token = read_token(packet.payload + size);
	}

	return packet;
}

size_t encode_packet(const DDNetPacket *packet, uint8_t *buf, size_t len, DDNetError *err) {
	if(len < PACKET_HEADER_SIZE) {
		*err = DDNET_ERR_BUFFER_FULL;
		return 0;
	}

	uint8_t *start = buf;
	encode_packet_header(&packet->header, buf);
	buf += PACKET_HEADER_SIZE;

	switch(packet->kind) {
	case PACKET_NORMAL:
		for(size_t i = 0; i < packet->chunks.len; i++) {
			buf += encode_chunk_header(&packet->chunks.data[i].header, buf);
			buf += encode_message(&packet->chunks.data[i], buf, err);
		}
		write_token(packet->header.token, buf);
		buf += sizeof(Token);
		return buf - start;
	case PACKET_CONTROL:
		buf += encode_control(&packet->control, buf, err);
		write_token(packet->header.token, buf);
		buf += sizeof(Token);
		return buf - start;
	case PACKET_CONNLESS:
		break;
	}

	*err = DDNET_ERR_INVALID_PACKET;
	return 0;
}

DDNetError free_packet(DDNetPacket *packet) {
	if(packet->kind == PACKET_NORMAL) {
		free(packet->chunks.data);
	}
	free(packet->payload);

	return DDNET_ERR_NONE;
}
