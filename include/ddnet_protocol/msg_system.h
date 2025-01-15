#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

// Sent by the client.
// The `command` will be executed in the server console.
typedef struct {
	const char *command;
} MsgRconCmd;

typedef struct {
	int32_t ack;
	int32_t prediction;
	int32_t size;
	// -1 is left
	//  1 is right
	//  0 is stop
	int32_t direction;
	int32_t target_x;
	int32_t target_y;
	// 1 to jump while on ground
	// 1 to double jump while in air
	// 0 to stop holding jump
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
