#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"

// Replaces all characters below ASCII 32 with whitespace.
void ddnet_str_sanitize_cc(char *string);

// Replaces all characters below ASCII 32 except \r, \n and \t with whitespace.
void ddnet_str_sanitize(char *string);

// Removes leading and trailing spaces and limits the use of multiple spaces.
void ddnet_str_clean_whitespaces(char *string);

// Used by `ddnet_unpacker_get_string_sanitized()` to strip unwanted characters
// from the strings received from the peer.
typedef enum {
	DDNET_STRING_SANITIZE_NONE = 0,
	DDNET_STRING_SANITIZE = 1 << 0,
	DDNET_STRING_SANITIZE_CC = 1 << 1,
	DDNET_STRING_SKIP_START_WHITESPACES = 1 << 2,
} DDNetStringSanitize;

// State for the unpacker.
// Holds the data to be unpacked and keeps track of how much data was unpacked
// and also tracks if errors occurred.
//
// See also `ddnet_unpacker_init` and `ddnet_unpacker_get_int`.
typedef struct {
	DDNetError err;
	const uint8_t *buf_end;
	const uint8_t *buf;
} DDNetUnpacker;

// Maximum output and storage size in bytes used by the `DDNetPacker`.
#define DDNET_PACKER_BUFFER_SIZE (1024 * 2)

// State for the packer.
// Holds the currently packed data and also tracks errors.
typedef struct {
	DDNetError err;
	uint8_t *current;
	uint8_t *end;
	uint8_t buf[DDNET_PACKER_BUFFER_SIZE];
} DDNetPacker;

// Initializes a new packer struct.
// See also `ddnet_packer_init_msg()` if you want to send a net message.
void ddnet_packer_init(DDNetPacker *packer);

// Get the size in bytes of the currently packed data.
// See also `ddnet_packer_data()`.
size_t ddnet_packer_size(DDNetPacker *packer);

// Amount of free bytes in the output buffer.
// The packer can pack a maximum of `DDNET_PACKER_BUFFER_SIZE` bytes.
size_t ddnet_packer_remaining_size(DDNetPacker *packer);

// Use in combination with `ddnet_packer_size()`.
uint8_t *ddnet_packer_data(DDNetPacker *packer);

// Packs `value` as teeworlds varint.
// Call `ddnet_packer_data()` to receive the full packed data.
DDNetError ddnet_packer_add_int(DDNetPacker *packer, int32_t value);

// Packs `value` as plain null terminated C string.
// Call `ddnet_packer_data()` to receive the full packed data.
DDNetError ddnet_packer_add_string(DDNetPacker *packer, const char *value);

// Packs `data` as raw data.
// Call `ddnet_packer_data()` to receive the full packed data.
DDNetError ddnet_packer_add_raw(DDNetPacker *packer, const uint8_t *data, size_t size);

// Returns a new `DDNetUnpacker` instance.
// It keeps track of how much data was already unpacked.
//
// ```C
// uint8_t bytes[] = {0x05, 0x01, 0x02};
// DDNetUnpacker unpacker;
// ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
// ddnet_unpacker_get_int(&unpacker); // => 5
// ddnet_unpacker_get_int(&unpacker); // => 1
// ddnet_unpacker_get_int(&unpacker); // => 2
// unpacker.err; // => DDNetError::DDNET_ERR_NONE
// ```
void ddnet_unpacker_init(DDNetUnpacker *unpacker, const uint8_t *buf, size_t len);

// Amount of bytes that have not yet been unpacked.
size_t ddnet_unpacker_remaining_size(DDNetUnpacker *unpacker);

// Use `ddnet_unpacker_init()` to get the value for `DDNetUnpacker *unpacker`.
// It returns the next integer in the unpacker data and also progresses the
// internal unpacker state to point to the next element.
//
// ```C
// uint8_t bytes[] = {0x05};
// DDNetUnpacker unpacker;
// ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
// ddnet_unpacker_get_int(&unpacker); // => 5
// unpacker.err; // => DDNetError::DDNET_ERR_NONE
// ```
int32_t ddnet_unpacker_get_int(DDNetUnpacker *unpacker);

// Use `ddnet_unpacker_init()` to get the value for `DDNetUnpacker *unpacker`.
// It returns the next null terminated string in the unpacker data and also
// progresses the internal unpacker state to point to the next element.
//
// Applies `STRING_SANITIZE` by default. If you want a string without
// sanitization use:
//
// ```C
// uint8_t bytes[] = {'f', 'o', 0x03, 'o', 0x00};
// DDNetUnpacker unpacker;
// ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
// ddnet_unpacker_get_string_sanitized(&unpacker, DDNET_STRING_SANITIZE_NONE);
// ```
const char *ddnet_unpacker_get_string(DDNetUnpacker *unpacker);

// Use `ddnet_unpacker_init` to get the value for `DDNetUnpacker *unpacker`.
// It returns the next null terminated string in the unpacker data and also
// progresses the internal unpacker state to point to the next element.
//
// ```C
// uint8_t bytes[] = {'f', 'o', 'o', 0x00};
// DDNetUnpacker unpacker;
// ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
//
// ddnet_unpacker_get_string_sanitized(&unpacker, DDNET_STRING_SANITIZE_CC); // => foo
// unpacker.err; // =>  DDNetError::DDNET_ERR_NONE
// ```
const char *ddnet_unpacker_get_string_sanitized(DDNetUnpacker *unpacker, DDNetStringSanitize sanitize);

// Use `ddnet_unpacker_init` to get the value for `DDNetUnpacker *unpacker`.
// It returns the next boolean in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// Might set the unpacker->err to `DDNET_ERR_INVALID_BOOL`
//
// ```C
// uint8_t bytes[] = {0x00, 0x01, 0xcc};
// DDNetUnpacker unpacker;
// ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
//
// ddnet_unpacker_get_bool(&unpacker); // => false
// ddnet_unpacker_get_bool(&unpacker); // => true
// ddnet_unpacker_get_bool(&unpacker); // => false (invalid boolean)
// unpacker.err; // => DDNetError::DDNET_ERR_INVALID_BOOL
// ```
bool ddnet_unpacker_get_bool(DDNetUnpacker *unpacker);

// Use `ddnet_unpacker_init` to get the value for `DDNetUnpacker *unpacker`.
// It returns the next `len` amount of bytes in the unpacker data and also
// progresses the internal unpacker state to point to the next element.
//
// ```C
// uint8_t bytes[] = {0x05};
// DDNetUnpacker unpacker;
// ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
// ddnet_unpacker_get_raw(&unpacker, 1); // => 0x05
// unpacker.err; // => DDNetError::DDNET_ERR_NONE
// ```
const uint8_t *ddnet_unpacker_get_raw(DDNetUnpacker *unpacker, size_t len);

#ifdef __cplusplus
}
#endif
