#include "packet.h"
#include "common.h"
#include "control_message.h"
#include "errors.h"
#include "fetch_chunks.h"
#include "huffman.h"

PacketHeader decode_packet_header(uint8_t *buf) {
	return (PacketHeader){
		.flags = buf[0] >> 2,
		.ack = ((buf[0] & 0x3) << 8) | buf[1],
		.num_chunks = buf[2],
	};
}

typedef struct {
	Chunk *chunks;
	size_t len;
} Context;

static void on_chunk(void *ctx, Chunk *chunk) {
	Context *context = ctx;

	memcpy(&context->chunks[context->len++], chunk, sizeof(Chunk));
}

size_t get_packet_payload(PacketHeader *header, uint8_t *full_data, size_t full_len, uint8_t *payload, size_t payload_len, Error *err) {
	full_data += PACKET_HEADER_SIZE;
	full_len -= PACKET_HEADER_SIZE;
	if(header->flags & PACKET_FLAG_COMPRESSION) {
		return huffman_decompress(full_data, full_len, payload, payload_len, err);
	}
	memcpy(payload, full_data, payload_len);
	return full_len;
}

Packet decode_packet(uint8_t *buf, size_t len, Error *err) {
	Packet packet = {};

	if(len < PACKET_HEADER_SIZE || len > MAX_PACKET_SIZE) {
		if(err) {
			*err = ERR_INVALID_PACKET;
		}

		return packet;
	}

	packet.header = decode_packet_header(buf);

	uint8_t payload[MAX_PACKET_SIZE];
	size_t payload_len = get_packet_payload(&packet.header, buf, len, payload, sizeof(payload), err);
	if(*err != ERR_NONE) {
		return packet;
	}

	if(packet.header.flags & PACKET_FLAG_CONTROL) {
		packet.kind = PACKET_CONTROL;
		packet.control = decode_control(payload, payload_len, &packet.header, err);
	} else {
		packet.kind = PACKET_NORMAL;
		Context ctx = {
			.chunks = malloc(sizeof(Chunk) * packet.header.num_chunks),
			.len = 0,
		};
		Error chunk_error = fetch_chunks(payload, payload_len, &packet.header, on_chunk, &ctx);
		if(chunk_error != ERR_NONE) {
			if(err) {
				*err = chunk_error;
			}

			return packet;
		}

		packet.chunks.data = ctx.chunks;
		packet.chunks.len = ctx.len;
	}

	return packet;
}

Error free_packet(Packet *packet) {
	if(packet->kind == PACKET_NORMAL) {
		free(packet->chunks.data);
	}

	return ERR_NONE;
}
