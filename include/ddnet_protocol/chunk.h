#pragma once

#include "common.h"

typedef enum {
	CHUNK_FLAG_VITAL = 1,
	CHUNK_FLAG_RESEND,
} ChunkFlag;

typedef struct {
	uint8_t flags;
	uint16_t size;
	uint16_t sequence; // Set only if flags & CHUNK_FLAG_VITAL
} ChunkHeader;

ChunkHeader decode_chunk_header(uint8_t **buf_ptr);
