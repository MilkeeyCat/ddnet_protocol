#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"
#include "msg_game.h"
#include "msg_system.h"

// The sequence and acknowledge number can never
// be higher than 1024
#define DDNET_MAX_SEQUENCE (1 << 10)

// These flags are used by the chunk header.
// Vital chunks contain a sequence number
// and are reliable.
// If the peer does not acknowledge the sequence number
// the chunk is resend with the resend flag set.
typedef enum {
	// Vital chunks are reliable.
	// They contain a sequence number and will be resend
	// if the peer did not acknowledge that sequence number.
	DDNET_CHUNK_FLAG_VITAL = 0b01000000,

	// If this exact message was already sent the resend flag is set.
	// This can happen if there is lag or packet loss.
	DDNET_CHUNK_FLAG_RESEND = 0b10000000,
} DDNetChunkFlag;

// Every game or system message is packed in a chunk.
// Every chunk has a 2 or 3 byte header.
// This struct is the parsed representation of this header.
//
// Be careful! The `sequence` field is only used by vital chunks!
typedef struct {
	// Bit flags that can be any combination of the `DDNetChunkFlag` enum.
	uint8_t flags;

	// Size in bytes of the chunk payload.
	// Excluding the chunk headers size.
	uint16_t size;

	// Set only if flags & DDNET_CHUNK_FLAG_VITAL
	// Is the amount of vital chunks that were already sent.
	// But the number flips back to 0 when `DDNET_MAX_SEQUENCE` is reached.
	uint16_t sequence;
} DDNetChunkHeader;

// Parses a byte buffer as a chunk header.
// Consumes 2 or 3 bytes and stores the result in `header`. Returns the number
// of bytes consumed.
size_t ddnet_decode_chunk_header(const uint8_t *buf, DDNetChunkHeader *header);

// Given a filled header struct it it will pack it into `buf`
// writing either 2 or 3 bytes depending on the header type.
// Returns a number of bytes written
//
// Example usage:
// ```C
// DDNetChunkHeader header = {
// 	.flags = DDNET_CHUNK_FLAG_VITAL,
// 	.size = 2,
// 	.sequence = 4};
// uint8_t buf[8];
// size_t bytes_written = ddnet_encode_chunk_header(&header, buf);
// ```
size_t ddnet_encode_chunk_header(const DDNetChunkHeader *header, uint8_t *buf);

// Be careful this is not the message id
// that is sent over the network!
typedef enum {
	DDNET_MSG_KIND_UNKNOWN,
	DDNET_MSG_KIND_INFO,
	DDNET_MSG_KIND_MAP_CHANGE,
	DDNET_MSG_KIND_MAP_DATA,
	DDNET_MSG_KIND_CON_READY,
	DDNET_MSG_KIND_SNAP,
	DDNET_MSG_KIND_SNAPEMPTY,
	DDNET_MSG_KIND_SNAPSINGLE,
	DDNET_MSG_KIND_SNAPSMALL,
	DDNET_MSG_KIND_INPUTTIMING,
	DDNET_MSG_KIND_RCON_AUTH_STATUS,
	DDNET_MSG_KIND_RCON_LINE,
	DDNET_MSG_KIND_READY,
	DDNET_MSG_KIND_ENTERGAME,
	DDNET_MSG_KIND_INPUT,
	DDNET_MSG_KIND_RCON_CMD,
	DDNET_MSG_KIND_RCON_AUTH,
	DDNET_MSG_KIND_REQUEST_MAP_DATA,
	DDNET_MSG_KIND_PING,
	DDNET_MSG_KIND_PING_REPLY,
	DDNET_MSG_KIND_RCON_CMD_ADD,
	DDNET_MSG_KIND_RCON_CMD_REM,
	DDNET_MSG_KIND_SV_MOTD,
	DDNET_MSG_KIND_SV_BROADCAST,
	DDNET_MSG_KIND_SV_CHAT,
	DDNET_MSG_KIND_SV_KILLMSG,
	DDNET_MSG_KIND_SV_SOUNDGLOBAL,
	DDNET_MSG_KIND_SV_TUNEPARAMS,
	DDNET_MSG_KIND_SV_READYTOENTER,
	DDNET_MSG_KIND_SV_WEAPONPICKUP,
	DDNET_MSG_KIND_SV_EMOTICON,
	DDNET_MSG_KIND_SV_VOTECLEAROPTIONS,
	DDNET_MSG_KIND_SV_VOTEOPTIONLISTADD,
	DDNET_MSG_KIND_SV_VOTEOPTIONADD,
	DDNET_MSG_KIND_SV_VOTEOPTIONREMOVE,
	DDNET_MSG_KIND_SV_VOTESET,
	DDNET_MSG_KIND_CL_SAY,
	DDNET_MSG_KIND_CL_STARTINFO,
} DDNetMessageKind;

// Union abstracting away any kind of game or system message
// Check the DDNetMessageKind to know which one to use
typedef union {
	// ddnet_protocol specific message to represent
	// a unknown message
	// this message kind does not exist in the reference implementation
	DDNetMsgUnknown unknown;

	// system messages
	DDNetMsgInfo info;
	DDNetMsgMapChange map_change;
	DDNetMsgMapData map_data;
	DDNetMsgInputTiming input_timing;
	DDNetMsgRconAuthStatus rcon_auth_status;
	DDNetMsgRconLine rcon_line;
	DDNetMsgInput input;
	DDNetMsgRconCmd rcon_cmd;
	DDNetMsgRconAuth rcon_auth;
	DDNetMsgRequestMapData request_map_data;
	DDNetMsgRconCmdAdd rcon_cmd_add;
	DDNetMsgRconCmdRem rcon_cmd_rem;

	// game messages
	DDNetMsgSvMotd motd;
	DDNetMsgSvBroadcast broadcast;
	DDNetMsgSvChat chat;
	DDNetMsgSvKillMsg kill_msg;
	DDNetMsgSvSoundGlobal sound_global;
	DDNetMsgSvTuneParams tune_params;
	DDNetMsgSvWeaponPickup weapon_pickup;
	DDNetMsgSvEmoticon emoticon;
	DDNetMsgSvVoteOptionListAdd vote_option_list_add;
	DDNetMsgSvVoteOptionAdd vote_option_add;
	DDNetMsgSvVoteOptionRemove vote_option_remove;
	DDNetMsgSvVoteSet vote_set;
	DDNetMsgClSay say;
	DDNetMsgClStartInfo start_info;
} DDNetGenericMessage;

// To access the net message struct check the `kind`
// and access the `msg` union accordingly.
typedef struct {
	DDNetMessageKind kind;
	DDNetGenericMessage msg;
} DDNetMessage;

// A chunk is the container of a net message.
// One chunk contains of a chunk header.
// And a chunk payload. The payload contains
// exactly one net message of either type game or system.
typedef struct {
	DDNetChunkHeader header;
	DDNetMessage payload;
} DDNetChunk;

// Returns true if the passed in message kind is a vital message
// vital messages are reliable and should be resend if they
// are not acknowledged by the peer.
// Not all messages are vital to improve performance
// and not resend outdated data such as game state and inputs.
bool ddnet_is_vital_msg(DDNetMessageKind kind);

// Given an entire chunk that has all values
// for message in the payload already set
// This function will fill the chunks header accordingly
// For now this only means setting the correct size based on the payload
DDNetError ddnet_fill_chunk_header(DDNetChunk *chunk);

#ifdef __cplusplus
}
#endif
