#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"
#include "errors.h"

// Message id of system and game net messages
// that is sent over the network.
typedef enum {
	// used for ddnet extensions
	DDNET_MSG_NULL = 0,

	// system messages
	DDNET_MSG_INFO = 1,
	DDNET_MSG_MAP_CHANGE = 2,
	DDNET_MSG_MAP_DATA = 3,
	DDNET_MSG_CON_READY = 4,
	DDNET_MSG_SNAP = 5,
	DDNET_MSG_SNAPEMPTY = 6,
	DDNET_MSG_SNAPSINGLE = 7,
	DDNET_MSG_SNAPSMALL = 8,
	DDNET_MSG_INPUTTIMING = 9,
	DDNET_MSG_RCON_AUTH_STATUS = 10,
	DDNET_MSG_RCON_LINE = 11,
	DDNET_MSG_AUTH_CHALLENGE = 12, // unused
	DDNET_MSG_AUTH_RESULT = 13, // unused
	DDNET_MSG_READY = 14,
	DDNET_MSG_ENTERGAME = 15,
	DDNET_MSG_INPUT = 16,
	DDNET_MSG_RCON_CMD = 17,
	DDNET_MSG_RCON_AUTH = 18,
	DDNET_MSG_REQUEST_MAP_DATA = 19,
	DDNET_MSG_AUTH_START = 20, // unused
	DDNET_MSG_AUTH_RESPONSE = 21, // unused
	DDNET_MSG_PING = 22,
	DDNET_MSG_PING_REPLY = 23,
	DDNET_MSG_ERROR = 24, // unused
	DDNET_MSG_RCON_CMD_ADD = 25,
	DDNET_MSG_RCON_CMD_REM = 26,

	// game messages
	DDNET_MSG_SV_MOTD = 1,
	DDNET_MSG_SV_BROADCAST = 2,
	DDNET_MSG_SV_CHAT = 3,
	DDNET_MSG_SV_KILLMSG = 4,
	DDNET_MSG_CL_STARTINFO = 20,
} DDNetMessageId;

// Every chunk contains either a system or game message
// This type is set as a bit flag in the first byte of the chunk payload
typedef enum {
	DDNET_GAME = 0,
	DDNET_SYSTEM = 1,
} DDNetMessageCategory;

// The given chunk has to have a correct header set
// and its `msg` will be filled based on the content of
// `buf` which has to point to the beginning of the chunk payload
//
// this function could also be called ddnet_decode_chunk_payload()
DDNetError ddnet_decode_message(DDNetChunk *chunk, uint8_t *buf);

size_t ddnet_encode_message(DDNetChunk *chunk, uint8_t *buf, DDNetError *err);

DDNetMessage ddnet_build_msg_info(const char *password);

#ifdef __cplusplus
}
#endif
