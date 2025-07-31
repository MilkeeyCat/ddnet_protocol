#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

// message of the day
// sent by the server
// and displayed in the middle of the screen
// with transparent background on the client side
typedef struct {
	const char *message;
} DDNetMsgSvMotd;

// sent by the server
// and displayed in the middle of the screen as white text
typedef struct {
	const char *message;
} DDNetMsgSvBroadcast;

// sent by the server
typedef struct {
	// 0 - public chat
	// 1 - team chat
	// 2 - whisper send (ddnet extension)
	// 3 - whisper receive (ddnet extension)
	int32_t team;

	// client id of the message author
	// can be -1 if the message was sent by the server
	int32_t client_id;

	// chat message
	const char *message;
} DDNetMsgSvChat;

typedef struct {
	const char *name;
	const char *clan;
	size_t country;
	const char *skin;
	bool use_custom_color;
	size_t color_body;
	size_t color_feet;
} DDNetMsgClStartInfo;

#ifdef __cplusplus
}
#endif
