#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "snapshot.h"

// Holds the raw data including the message id of a message unknown to
// the ddnet_protocol library.
typedef struct {
	const uint8_t *buf;
	size_t len;
} DDProtoMsgUnknown;

// sent by the client
typedef struct {
	const char *version;
	const char *password;
} DDProtoMsgInfo;

// sent by the server
typedef struct {
	const char *name;
	int32_t crc;
	int32_t size;
} DDProtoMsgMapChange;

// sent by the server
typedef struct {
	bool last;
	int32_t map_crc;
	int32_t chunk;
	int32_t chunk_size;
	const uint8_t *data;
} DDProtoMsgMapData;

// Sent by the server.
typedef struct {
	int32_t game_tick;
	int32_t delta_tick;
	int32_t crc;
	int32_t part_size;
	DDProtoSnapshot snapshot;
} DDProtoMsgSnapSingle;

// Sent by the client.
// The `command` will be executed in the server console.
typedef struct {
	const char *command;
} DDProtoMsgRconCmd;

// sent by the client
typedef struct {
	// has to be set to an empty string
	// when trying to login with one of the non user passwords
	// sv_rcon_password, sv_rcon_mod_password or sv_rcon_helper_password
	const char *name;
	const char *password;

	// If set to true the server will send the available rcon commands to the
	// client.
	bool send_rcon_cmds;
} DDProtoMsgRconAuth;

// sent by the client
// This is sent multiple times during the map download process. The server is
// supposed to answer with a `DDPROTO_MSG_KIND_MAP_DATA`
typedef struct {
	int32_t chunk;
} DDProtoMsgRequestMapData;

// sent by the server
typedef struct {
	const char *name;
	const char *help;
	const char *params;
} DDProtoMsgRconCmdAdd;

// sent by the server
typedef struct {
	const char *name;
} DDProtoMsgRconCmdRem;

// sent by the server
typedef struct {
	int32_t intended_tick;
	int32_t time_left;
} DDProtoMsgInputTiming;

// sent by the server
typedef struct {
	bool authed;
	bool cmdlist;
} DDProtoMsgRconAuthStatus;

// sent by the server
// Log line that shows up in the clients remote console.
typedef struct {
	const char *line;
} DDProtoMsgRconLine;

// sent by the client
typedef struct {
	int32_t ack_game_tick;
	int32_t prediction_tick;
	int32_t size;
	int32_t direction;
	int32_t target_x;
	int32_t target_y;
	int32_t jump;
	int32_t fire;
	int32_t hook;
	int32_t player_flags;
	int32_t wanted_weapon;
	int32_t next_weapon;
	int32_t prev_weapon;
} DDProtoMsgInput;

#ifdef __cplusplus
}
#endif
