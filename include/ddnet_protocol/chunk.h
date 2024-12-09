#pragma once

#include "common.h"
#include "msg_system.h"

// The sequence and acknowledge number can never
// be higher than 1024
#define MAX_SEQUENCE 1 << 10

// These flags are used by the chunk header.
// Vital chunks contain a sequence number
// and are reliable.
// If the peer does not acknowledge the sequence number
// the chunk is resend with the resend flag set.
typedef enum {
	// Vital chunks are reliable.
	// They contain a sequence number and will be resend
	// if the peer did not acknowledge that sequence number.
	CHUNK_FLAG_VITAL = 1,

	// If this exact message was already sent the resend flag is set.
	// This can happen if there is lag or packet loss.
	CHUNK_FLAG_RESEND,
} ChunkFlag;

// Every game or system message is packed in a chunk.
// Every chunk has a 2 or 3 byte header.
// This struct is the parsed representation of this header.
//
// Be careful! The `sequence` field is only used by vital chunks!
typedef struct {
	// Bit flags that can be any combination of the `ChunkFlag` enum.
	uint8_t flags;

	// Size in bytes of the chunk payload.
	// Excluding the chunk headers size.
	uint16_t size;

	// Set only if flags & CHUNK_FLAG_VITAL
	// Is the amount of vital chunks that were already sent.
	// But the number flips back to 0 when `MAX_SEQUENCE` is reached.
	uint16_t sequence;
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
		MsgNull *null;
		MsgRconCmd *rcon_cmd;
	} msg;
} Chunk;
