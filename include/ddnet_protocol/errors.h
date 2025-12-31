#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define DDPROTO_ERRORS \
	X(DDPROTO_ERR_NONE) \
	X(DDPROTO_ERR_INVALID_PACKET) \
	X(DDPROTO_ERR_UNKNOWN_MESSAGE) \
	X(DDPROTO_ERR_UNKNOWN_SNAP_ITEM) \
	X(DDPROTO_ERR_INVALID_CONTROL_MESSAGE) \
	X(DDPROTO_ERR_INVALID_TOKEN_MAGIC) \
	X(DDPROTO_ERR_INVALID_BOOL) \
	X(DDPROTO_ERR_STR_UNEXPECTED_EOF) \
	X(DDPROTO_ERR_EMPTY_BUFFER) \
	X(DDPROTO_ERR_END_OF_BUFFER) \
	X(DDPROTO_ERR_BUFFER_FULL) \
	X(DDPROTO_ERR_OUTPUT_VAR_TOO_SMALL) \
	X(DDPROTO_ERR_REMAINING_BYTES_IN_BUFFER) \
	X(DDPROTO_ERR_MISSING_DDNET_SECURITY_TOKEN) \
	X(DDPROTO_ERR_HUFFMAN_NODE_NULL) \
	X(DDPROTO_ERR_MESSAGE_ID_OUT_OF_BOUNDS) \
	X(DDPROTO_ERR_ACK_OUT_OF_BOUNDS)

// Generic error enum, holds all kinds of errors returned by different
// functions.
typedef enum {
#define X(err) err,
	DDPROTO_ERRORS
#undef X
} DDProtoError;

// Turns error enum into human readable string.
const char *ddproto_error_str(DDProtoError err);

#ifdef __cplusplus
}
#endif
