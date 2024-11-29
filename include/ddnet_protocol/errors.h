#pragma once

// generic error enum
// holds all kind of errors returned
// by different functions
typedef enum {
	ERR_NONE,
	ERR_INVALID_PACKET,
	ERR_INVALID_CONTROL_MESSAGE,
	ERR_INVALID_TOKEN_MAGIC,
	ERR_EMPTY_BUFFER,
	ERR_INVALID_BOOL,
	ERR_STR_UNEXPECTED_EOF,
} Error;
