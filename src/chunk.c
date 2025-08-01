#include "chunk.h"
#include "errors.h"
#include "message.h"
#include "packet.h"

size_t ddnet_decode_chunk_header(const uint8_t *buf, DDNetChunkHeader *header) {
	header->flags = buf[0] & 0b11000000;
	header->size = ((buf[0] & 0b00111111) << 4) | (buf[1] & ((1 << 4) - 1));
	header->sequence = 0;

	if(header->flags & DDNET_CHUNK_FLAG_VITAL) {
		header->sequence = ((buf[1] & (~((1 << 4) - 1))) << 2) | buf[2];

		return 3;
	}

	return 2;
}

size_t ddnet_encode_chunk_header(const DDNetChunkHeader *header, uint8_t *buf) {
	buf[0] = header->flags & 0b11000000 | ((header->size >> 4) & 0b00111111);
	buf[1] = (header->size & 0b00001111);
	if(header->flags & DDNET_CHUNK_FLAG_VITAL) {
		buf[1] |= (header->sequence >> 2) & 0b11110000;
		buf[2] = header->sequence & 0xff;

		return 3;
	}

	return 2;
}

bool ddnet_is_vital_msg(DDNetMessageKind kind) {
	return kind != DDNET_MSG_KIND_INPUT &&
	       kind != DDNET_MSG_KIND_SNAP &&
	       kind != DDNET_MSG_KIND_SNAPEMPTY &&
	       kind != DDNET_MSG_KIND_SNAPSINGLE &&
	       kind != DDNET_MSG_KIND_SNAPSMALL;
}

DDNetError ddnet_fill_chunk_header(DDNetChunk *chunk) {
	uint8_t chunk_payload[DDNET_MAX_PACKET_SIZE];
	DDNetError encode_err = DDNET_ERR_NONE;
	size_t chunk_payload_len = ddnet_encode_message(chunk, chunk_payload, &encode_err);
	if(encode_err != DDNET_ERR_NONE) {
		return encode_err;
	}
	chunk->header.size = chunk_payload_len;
	chunk->header.flags = ddnet_is_vital_msg(chunk->payload.kind) ? DDNET_CHUNK_FLAG_VITAL : 0;
	return DDNET_ERR_NONE;
}
