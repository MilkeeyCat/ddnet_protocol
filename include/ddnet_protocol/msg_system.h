#pragma once

#include "common.h"

// Sent by the client.
// The `command` will be executed in the server console.
typedef struct {
	const char *command;
} MsgRconCmd;
