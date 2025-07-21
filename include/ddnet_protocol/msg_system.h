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
} MsgUnknown;

// Sent by the client.
typedef struct {
	const char *version;
	const char *password;
} MsgInfo;

// Sent by the server.
typedef struct {
	const char *name;
	int32_t crc;
	int32_t size;
} MsgMapChange;

// Sent by the server.
typedef struct {
	int32_t last;
	int32_t map_crc;
	int32_t chunk;
	int32_t chunk_size;
	const uint8_t *data;
} MsgMapData;

// Sent by the client.
// The `command` will be executed in the server console.
typedef struct {
	const char *command;
} MsgRconCmd;

// Sent by the server.
typedef struct {
	int32_t intended_tick;
	int32_t time_left;
} MsgInputTiming;

// Sent by the server.
typedef struct {
	int32_t authed;
	int32_t cmdlist;
} MsgRconAuthStatus;

// Sent by the server.
// Log line that shows up in the clients
// remote console.
typedef struct {
	const char *line;
} MsgRconLine;

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
} MsgInput;

#ifdef __cplusplus
}
#endif
