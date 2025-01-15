#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"
#include "common.h"
#include "errors.h"

// Message id of system and game net messages
// that is sent over the network.
typedef enum {
	// used for ddnet extensions
	MSG_NULL = 0,

	// system messages
	MSG_RCON_CMD = 17,

	// game messages
	MSG_CL_STARTINFO = 20,
} MessageId;

// Every chunk contains either a system or game message
// This type is set as a bit flag in the first byte of the chunk payload
typedef enum {
	MESSAGE_GAME = 0,
	MESSAGE_SYSTEM = 1,
} MessageKind;

// The given chunk has to have a correct header set
// and its `msg` will be filled based on the content of
// `buf` which has to point to the beginning of the chunk payload
//
// this function could also be called decode_chunk_payload()
Error decode_message(Chunk *chunk, uint8_t *buf);

size_t encode_message(Chunk *chunk, uint8_t *buf, Error *err);

#ifdef __cplusplus
}
#endif
