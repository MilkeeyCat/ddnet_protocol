#pragma once

#include "common.h"

// Not a real net message.
// Also its field is only there to please the compiler.
// This is a generic message that represents invalid messages.
// Or all messages.
typedef struct {
	// This value is never set. Do not read it.
	uint8_t _;
} MsgNull;

// Sent by the client.
// The `command` will be executed in the server console.
typedef struct {
	const char *command;
} MsgRconCmd;
