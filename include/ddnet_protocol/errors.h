#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define DDNET_ERRORS \
	X(DDNET_ERR_NONE) \
	X(DDNET_ERR_INVALID_PACKET) \
	X(DDNET_ERR_UNKNOWN_MESSAGE) \
	X(DDNET_ERR_INVALID_CONTROL_MESSAGE) \
	X(DDNET_ERR_INVALID_TOKEN_MAGIC) \
	X(DDNET_ERR_INVALID_BOOL) \
	X(DDNET_ERR_STR_UNEXPECTED_EOF) \
	X(DDNET_ERR_EMPTY_BUFFER) \
	X(DDNET_ERR_END_OF_BUFFER) \
	X(DDNET_ERR_BUFFER_FULL) \
	X(DDNET_ERR_OUTPUT_VAR_TOO_SMALL) \
	X(DDNET_ERR_REMAINING_BYTES_IN_BUFFER) \
	X(DDNET_ERR_MISSING_DDNET_SECURITY_TOKEN) \
	X(DDNET_ERR_HUFFMAN_NODE_NULL) \
	X(DDNET_ERR_MESSAGE_ID_OUT_OF_BOUNDS) \
	X(DDNET_ERR_ACK_OUT_OF_BOUNDS)

// Generic error enum, holds all kinds of errors returned by different
// functions.
typedef enum {
#define X(err) err,
	DDNET_ERRORS
#undef X
} DDNetError;

// Turns error enum into human readable string.
const char *ddnet_error_str(DDNetError err);

#ifdef __cplusplus
}
#endif
