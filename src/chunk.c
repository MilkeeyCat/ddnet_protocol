#include "chunk.h"

ChunkHeader decode_chunk_header(uint8_t **buf_ptr) {
	ChunkHeader header;
	uint8_t *buf = *buf_ptr;

	header.flags = (buf[0] >> 6) & 3;
	header.size = ((buf[0] & 0x3f) << 4) | (buf[1] & ((1 << 4) - 1));
	header.sequence = 0;

	if(header.flags & CHUNK_FLAG_VITAL) {
		header.sequence = ((buf[1] & (~((1 << 4) - 1))) << 2) | buf[2];

		*buf_ptr += 3;
	} else {
		*buf_ptr += 2;
	}

	return header;
}
