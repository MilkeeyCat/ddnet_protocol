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

Error encode_packet_header(const PacketHeader *header, uint8_t *buf) {
	if(header->ack >= MAX_SEQUENCE) {
		return ERR_ACK_OUT_OF_BOUNDS;
	}
	buf[0] = ((header->flags << 2) & 0xfc | ((header->ack >> 8)) & 0x3);
	buf[1] = header->ack & 0xff;
	buf[2] = header->num_chunks;
	return ERR_NONE;
}

typedef struct {
	Chunk *chunks;
	size_t len;
} Context;

static void on_chunk(void *ctx, Chunk *chunk) {
	Context *context = ctx;

	memcpy(&context->chunks[context->len++], chunk, sizeof(Chunk));
}

size_t get_packet_payload(PacketHeader *header, const uint8_t *full_data, size_t full_len, uint8_t *payload, size_t payload_len, Error *err) {
	full_data += PACKET_HEADER_SIZE;
	full_len -= PACKET_HEADER_SIZE;
	if(header->flags & PACKET_FLAG_COMPRESSION) {
		return huffman_decompress(full_data, full_len, payload, payload_len, err);
	}
	memcpy(payload, full_data, full_len);
	return full_len;
}

DDNetPacket decode_packet(const uint8_t *buf, size_t len, Error *err) {
	DDNetPacket packet = {};

	if(len < PACKET_HEADER_SIZE || len > MAX_PACKET_SIZE) {
		if(err) {
			*err = ERR_INVALID_PACKET;
		}

		return packet;
	}

	packet.header = decode_packet_header(buf);
	packet.payload = malloc(MAX_PACKET_SIZE);
	packet.payload_len = get_packet_payload(&packet.header, buf, len, packet.payload, MAX_PACKET_SIZE, err);
	if(*err != ERR_NONE) {
		return packet;
	}

	if(packet.header.flags & PACKET_FLAG_CONTROL) {
		packet.kind = PACKET_CONTROL;
		packet.control = decode_control(packet.payload, packet.payload_len, &packet.header, err);
	} else {
		packet.kind = PACKET_NORMAL;
		Context ctx = {
			.chunks = malloc(sizeof(Chunk) * packet.header.num_chunks),
			.len = 0,
		};
		Error chunk_error = fetch_chunks(packet.payload, packet.payload_len, &packet.header, on_chunk, &ctx);
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

size_t encode_packet(const DDNetPacket *packet, uint8_t *buf, size_t len, Error *err) {
	if(len < PACKET_HEADER_SIZE) {
		*err = ERR_BUFFER_FULL;
		return 0;
	}

	uint8_t *start = buf;

	if(packet->kind == PACKET_NORMAL) {
		encode_packet_header(&packet->header, buf);
		buf += PACKET_HEADER_SIZE;

		for(size_t i = 0; i < packet->chunks.len; i++) {
			buf += encode_chunk_header(&packet->chunks.data[i].header, buf);
			buf += encode_message(&packet->chunks.data[i], buf, err);
		}

		write_token(packet->header.token, buf);
		buf += sizeof(Token);

		return buf - start;
	}

	*err = ERR_INVALID_PACKET;
	return 0;
}

Error free_packet(DDNetPacket *packet) {
	if(packet->kind == PACKET_NORMAL) {
		free(packet->chunks.data);
	}
	free(packet->payload);

	return ERR_NONE;
}
