#include "chunk.h"

ChunkHeader decode_chunk_header(uint8_t **buf_ptr) {
	ChunkHeader header;
	uint8_t *buf = *buf_ptr;

	header.flags = buf[0] & 0b11000000;
	header.size = ((buf[0] & 0b00111111) << 4) | (buf[1] & ((1 << 4) - 1));
	header.sequence = 0;

	if(header.flags & CHUNK_FLAG_VITAL) {
		header.sequence = ((buf[1] & (~((1 << 4) - 1))) << 2) | buf[2];

		*buf_ptr += 3;
	} else {
		*buf_ptr += 2;
	}

	return header;
}

uint8_t *encode_chunk_header(const ChunkHeader *header, uint8_t *buf) {
	buf[0] = header->flags & 0b11000000 | ((header->size >> 4) & 0b00111111);
	buf[1] = (header->size & 0b00001111);
	if(header->flags & CHUNK_FLAG_VITAL) {
		buf[1] |= (header->sequence >> 2) & 0b11110000;
		buf[2] = header->sequence & 0xff;
		return buf + 3;
	}
	return buf + 2;
}
