#pragma once

#include "common.h"
#include "msg_system.h"

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

// Holds the type of `msg` in a `Chunk` struct
//
// Be careful this is not the message id
// that is sent over the network!
typedef enum {
	CHUNK_KIND_RCON_CMD,
	CHUNK_KIND_CL_STARTINFO,
} ChunkKind;

// A chunk is the container of a net message.
// One chunk contains of a chunk header.
// And a chunk payload. The payload contains
// exactly one net message of either type game or system.
//
// To access the net message struct check the `kind`
// and access the `msg` union accordingly.
typedef struct {
	ChunkKind kind;
	ChunkHeader header;
	union {
		MsgRconCmd rcon_cmd;
	} msg;
} Chunk;
