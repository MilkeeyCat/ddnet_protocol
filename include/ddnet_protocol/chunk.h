#pragma once

#include "common.h"

// These flags are used by the chunk header.
// Vital chunks contain a sequence number
// and are reliable.
// If the peer does not acknowledge the sequence number
// the chunk is resend with the resend flag set.
typedef enum {
	CHUNK_FLAG_VITAL = 1,
	CHUNK_FLAG_RESEND,
} ChunkFlag;

// Every game or system message is packed in a chunk.
// Every chunk has a 2 or 3 byte header.
// This struct is the parsed representation of this header.
//
// Be careful! The `sequence` field is only used by vital chunks!
typedef struct {
	uint8_t flags;
	uint16_t size;
	uint16_t sequence; // Set only if flags & CHUNK_FLAG_VITAL
} ChunkHeader;

// Given a pointer to a byte buffer it will parse it as a chunk header.
// The pointer is then incremented by 2 or 3 depending on how many bytes
// where consumed.
ChunkHeader decode_chunk_header(uint8_t **buf_ptr);
