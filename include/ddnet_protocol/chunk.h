#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"
#include "msg_game.h"
#include "msg_system.h"

// The sequence and acknowledge number can never be higher than 1024.
#define DDPROTO_MAX_SEQUENCE (1 << 10)

// These flags are used by the chunk header. Vital chunks contain a sequence
// number and are reliable. If the peer does not acknowledge the sequence number
// the chunk is resend with the resend flag set.
typedef enum {
	// Vital chunks are reliable. They contain a sequence number and will be
	// resend if the peer did not acknowledge that sequence number.
	DDPROTO_CHUNK_FLAG_VITAL = 0b01000000,

	// If this exact message was already sent the resend flag is set. This can
	// happen if there is lag or packet loss.
	DDPROTO_CHUNK_FLAG_RESEND = 0b10000000,
} DDProtoChunkFlag;

// Every game or system message is packed in a chunk. Every chunk has a 2 or 3
// byte header. This struct is the parsed representation of this header.
//
// Be careful! The `sequence` field is only used by vital chunks!
typedef struct {
	// Bit flags that can be any combination of the `DDProtoChunkFlag` enum.
	uint8_t flags;

	// Size in bytes of the chunk payload. Excluding the chunk headers size.
	uint16_t size;

	// Set only if flags & `DDPROTO_CHUNK_FLAG_VITAL`.
	// Is the amount of vital chunks that were already sent. But the number
	// flips back to 0 when `DDPROTO_MAX_SEQUENCE` is reached.
	uint16_t sequence;
} DDProtoChunkHeader;

// Parses a byte buffer as a chunk header.
// Consumes 2 or 3 bytes and stores the result in `header`. Returns the number
// of bytes consumed.
size_t ddproto_decode_chunk_header(const uint8_t *buf, DDProtoChunkHeader *header);

// Given a filled header struct it it will pack it into `buf` writing either 2
// or 3 bytes depending on the header type. Returns a number of bytes written.
//
// Example usage:
// ```C
// DDProtoChunkHeader header = {
// 	.flags = DDPROTO_CHUNK_FLAG_VITAL,
// 	.size = 2,
// 	.sequence = 4};
// uint8_t buf[8];
// size_t bytes_written = ddproto_encode_chunk_header(&header, buf);
// ```
size_t ddproto_encode_chunk_header(const DDProtoChunkHeader *header, uint8_t *buf);

// Be careful this is not the message id that is sent over the network!
typedef enum {
	DDPROTO_MSG_KIND_UNKNOWN,
	DDPROTO_MSG_KIND_INFO,
	DDPROTO_MSG_KIND_MAP_CHANGE,
	DDPROTO_MSG_KIND_MAP_DATA,
	DDPROTO_MSG_KIND_CON_READY,
	DDPROTO_MSG_KIND_SNAP,
	DDPROTO_MSG_KIND_SNAPEMPTY,
	DDPROTO_MSG_KIND_SNAPSINGLE,
	DDPROTO_MSG_KIND_SNAPSMALL,
	DDPROTO_MSG_KIND_INPUTTIMING,
	DDPROTO_MSG_KIND_RCON_AUTH_STATUS,
	DDPROTO_MSG_KIND_RCON_LINE,
	DDPROTO_MSG_KIND_READY,
	DDPROTO_MSG_KIND_ENTERGAME,
	DDPROTO_MSG_KIND_INPUT,
	DDPROTO_MSG_KIND_RCON_CMD,
	DDPROTO_MSG_KIND_RCON_AUTH,
	DDPROTO_MSG_KIND_REQUEST_MAP_DATA,
	DDPROTO_MSG_KIND_PING,
	DDPROTO_MSG_KIND_PING_REPLY,
	DDPROTO_MSG_KIND_RCON_CMD_ADD,
	DDPROTO_MSG_KIND_RCON_CMD_REM,
	DDPROTO_MSG_KIND_SV_MOTD,
	DDPROTO_MSG_KIND_SV_BROADCAST,
	DDPROTO_MSG_KIND_SV_CHAT,
	DDPROTO_MSG_KIND_SV_KILLMSG,
	DDPROTO_MSG_KIND_SV_SOUNDGLOBAL,
	DDPROTO_MSG_KIND_SV_TUNEPARAMS,
	DDPROTO_MSG_KIND_SV_READYTOENTER,
	DDPROTO_MSG_KIND_SV_WEAPONPICKUP,
	DDPROTO_MSG_KIND_SV_EMOTICON,
	DDPROTO_MSG_KIND_SV_VOTECLEAROPTIONS,
	DDPROTO_MSG_KIND_SV_VOTEOPTIONLISTADD,
	DDPROTO_MSG_KIND_SV_VOTEOPTIONADD,
	DDPROTO_MSG_KIND_SV_VOTEOPTIONREMOVE,
	DDPROTO_MSG_KIND_SV_VOTESET,
	DDPROTO_MSG_KIND_CL_SAY,
	DDPROTO_MSG_KIND_CL_STARTINFO,
} DDProtoMessageKind;

// Union abstracting away any kind of game or system message. Check the
// `DDProtoMessageKind` to know which one to use.
typedef union {
	// ddnet_protocol specific message to represent a unknown message. This
	// message kind does not exist in the reference implementation.
	DDProtoMsgUnknown unknown;

	// system messages
	DDProtoMsgInfo info;
	DDProtoMsgMapChange map_change;
	DDProtoMsgMapData map_data;
	DDProtoMsgSnapSingle snap_single;
	DDProtoMsgInputTiming input_timing;
	DDProtoMsgRconAuthStatus rcon_auth_status;
	DDProtoMsgRconLine rcon_line;
	DDProtoMsgInput input;
	DDProtoMsgRconCmd rcon_cmd;
	DDProtoMsgRconAuth rcon_auth;
	DDProtoMsgRequestMapData request_map_data;
	DDProtoMsgRconCmdAdd rcon_cmd_add;
	DDProtoMsgRconCmdRem rcon_cmd_rem;

	// game messages
	DDProtoMsgSvMotd motd;
	DDProtoMsgSvBroadcast broadcast;
	DDProtoMsgSvChat chat;
	DDProtoMsgSvKillMsg kill_msg;
	DDProtoMsgSvSoundGlobal sound_global;
	DDProtoMsgSvTuneParams tune_params;
	DDProtoMsgSvWeaponPickup weapon_pickup;
	DDProtoMsgSvEmoticon emoticon;
	DDProtoMsgSvVoteOptionListAdd vote_option_list_add;
	DDProtoMsgSvVoteOptionAdd vote_option_add;
	DDProtoMsgSvVoteOptionRemove vote_option_remove;
	DDProtoMsgSvVoteSet vote_set;
	DDProtoMsgClSay say;
	DDProtoMsgClStartInfo start_info;
} DDProtoGenericMessage;

// To access the net message struct check the `kind` and access the `msg` union
// accordingly.
typedef struct {
	DDProtoMessageKind kind;
	DDProtoGenericMessage msg;
} DDProtoMessage;

// A chunk is the container of a net message. One chunk contains of a chunk
// header. And a chunk payload. The payload contains exactly one net message of
// either type game or system.
typedef struct {
	DDProtoChunkHeader header;
	DDProtoMessage payload;
} DDProtoChunk;

// Returns true if the passed in message kind is a vital message.
// Vital messages are reliable and should be resend if they are not acknowledged
// by the peer. Not all messages are vital to improve performance and not resend
// outdated data such as game state and inputs.
bool ddproto_is_vital_msg(DDProtoMessageKind kind);

// Given an entire chunk that has all values for message in the payload already
// set. This function will fill the chunk header accordingly. For now this only
// means setting the correct size based on the payload.
DDProtoError ddproto_fill_chunk_header(DDProtoChunk *chunk);

#ifdef __cplusplus
}
#endif
