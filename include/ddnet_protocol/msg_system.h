#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

// Holds the raw data including the message id
// of a message unknown to the ddnet_protocol library
typedef struct {
	const uint8_t *buf;
	size_t len;
} DDNetMsgUnknown;

// Sent by the client.
typedef struct {
	const char *version;
	const char *password;
} DDNetMsgInfo;

// Sent by the server.
typedef struct {
	const char *name;
	int32_t crc;
	int32_t size;
} DDNetMsgMapChange;

// Sent by the server.
typedef struct {
	bool last;
	int32_t map_crc;
	int32_t chunk;
	int32_t chunk_size;
	const uint8_t *data;
} DDNetMsgMapData;

// Sent by the client.
// The `command` will be executed in the server console.
typedef struct {
	const char *command;
} DDNetMsgRconCmd;

// Sent by the client.
typedef struct {
	// has to be set to an empty string
	// when trying to login with one of the non user passwords
	// sv_rcon_password, sv_rcon_mod_password or sv_rcon_helper_password
	const char *name;
	const char *password;

	// If set to true the server will send the available
	// rcon commands to the client.
	bool send_rcon_cmds;
} DDNetMsgRconAuth;

// Sent by the client.
// This is sent multiple times during the map download process.
// The server is supposed to answer with a `DDNET_MSG_KIND_MAP_DATA`
typedef struct {
	int32_t chunk;
} DDNetMsgRequestMapData;

// Sent by the server.
typedef struct {
	const char *name;
	const char *help;
	const char *params;
} DDNetMsgRconCmdAdd;

// Sent by the server.
typedef struct {
	const char *name;
} DDNetMsgRconCmdRem;

// Sent by the server.
typedef struct {
	int32_t intended_tick;
	int32_t time_left;
} DDNetMsgInputTiming;

// Sent by the server.
typedef struct {
	bool authed;
	bool cmdlist;
} DDNetMsgRconAuthStatus;

// Sent by the server.
// Log line that shows up in the clients
// remote console.
typedef struct {
	const char *line;
} DDNetMsgRconLine;

// Sent by the client.
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
} DDNetMsgInput;

#ifdef __cplusplus
}
#endif
