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

#ifdef __cplusplus
}
#endif
