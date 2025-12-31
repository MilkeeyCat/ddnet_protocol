#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"
#include "errors.h"

// Message id of system and game net messages that is sent over the network.
typedef enum {
	// used for ddnet extensions
	DDPROTO_MSG_NULL = 0,

	// system messages
	DDPROTO_MSG_INFO = 1,
	DDPROTO_MSG_MAP_CHANGE = 2,
	DDPROTO_MSG_MAP_DATA = 3,
	DDPROTO_MSG_CON_READY = 4,
	DDPROTO_MSG_SNAP = 5,
	DDPROTO_MSG_SNAPEMPTY = 6,
	DDPROTO_MSG_SNAPSINGLE = 7,
	DDPROTO_MSG_SNAPSMALL = 8,
	DDPROTO_MSG_INPUTTIMING = 9,
	DDPROTO_MSG_RCON_AUTH_STATUS = 10,
	DDPROTO_MSG_RCON_LINE = 11,
	DDPROTO_MSG_AUTH_CHALLENGE = 12, // unused
	DDPROTO_MSG_AUTH_RESULT = 13, // unused
	DDPROTO_MSG_READY = 14,
	DDPROTO_MSG_ENTERGAME = 15,
	DDPROTO_MSG_INPUT = 16,
	DDPROTO_MSG_RCON_CMD = 17,
	DDPROTO_MSG_RCON_AUTH = 18,
	DDPROTO_MSG_REQUEST_MAP_DATA = 19,
	DDPROTO_MSG_AUTH_START = 20, // unused
	DDPROTO_MSG_AUTH_RESPONSE = 21, // unused
	DDPROTO_MSG_PING = 22,
	DDPROTO_MSG_PING_REPLY = 23,
	DDPROTO_MSG_ERROR = 24, // unused
	DDPROTO_MSG_RCON_CMD_ADD = 25,
	DDPROTO_MSG_RCON_CMD_REM = 26,

	// game messages
	DDPROTO_MSG_SV_MOTD = 1,
	DDPROTO_MSG_SV_BROADCAST = 2,
	DDPROTO_MSG_SV_CHAT = 3,
	DDPROTO_MSG_SV_KILLMSG = 4,
	DDPROTO_MSG_SV_SOUNDGLOBAL = 5,
	DDPROTO_MSG_SV_TUNEPARAMS = 6,
	// msg id 7 was removed in 2015 it used to be extra projectile
	DDPROTO_MSG_SV_READYTOENTER = 8,
	DDPROTO_MSG_SV_WEAPONPICKUP = 9,
	DDPROTO_MSG_SV_EMOTICON = 10,
	DDPROTO_MSG_SV_VOTECLEAROPTIONS = 11,
	DDPROTO_MSG_SV_VOTEOPTIONLISTADD = 12,
	DDPROTO_MSG_SV_VOTEOPTIONADD = 13,
	DDPROTO_MSG_SV_VOTEOPTIONREMOVE = 14,
	DDPROTO_MSG_SV_VOTESET = 15,
	DDPROTO_MSG_CL_SAY = 17,
	DDPROTO_MSG_CL_STARTINFO = 20,
} DDProtoMessageId;

// Every chunk contains either a system or game message. This type is set as a
// bit flag in the first byte of the chunk payload.
typedef enum {
	DDPROTO_GAME = 0,
	DDPROTO_SYSTEM = 1,
} DDProtoMessageCategory;

// The given chunk has to have a correct header set and its `msg` will be filled
// based on the content of `buf` which has to point to the beginning of the
// chunk payload.
//
// This function could also be called `ddproto_decode_chunk_payload()`.
DDProtoError ddproto_decode_message(DDProtoChunk *chunk, const uint8_t *buf);

size_t ddproto_encode_message(DDProtoChunk *chunk, uint8_t *buf, DDProtoError *err);

DDProtoMessage ddproto_build_msg_info(const char *password);

// Initializes a new packer struct.
// And already packs the message id and message kind flag.
//
// See also `ddproto_packer_init()` if you need an empty packer.
//
// ```C
// DDProtoPacker packer;
// ddproto_packer_init_msg(&packer, DDPROTO_MSG_KIND_RCON_CMD);
// ddproto_packer_add_string(&packer, "say hello");
// ```
void ddproto_packer_init_msg(DDProtoPacker *packer, DDProtoMessageKind kind);

#ifdef __cplusplus
}
#endif
