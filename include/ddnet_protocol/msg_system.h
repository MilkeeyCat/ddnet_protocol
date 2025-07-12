#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

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

#ifdef __cplusplus
}
#endif
