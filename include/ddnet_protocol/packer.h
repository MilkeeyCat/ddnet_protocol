#pragma once

#include "common.h"
#include "errors.h"

// Replaces all characters below 32 with whitespace.
void str_sanitize_cc(char *string);

// Replaces all characters below 32 with whitespace with
void str_sanitize(char *string);

// Removes leading and trailing spaces and limits the use of multiple spaces.
void str_clean_whitespaces(char *string);

// State for the unpacker
// holds the data to be unpacked
// and keeps track of how much data was unpacked
// and also tracks if errors occured
//
// See also `unpacker_new` and `unpacker_get_int`
typedef struct {
	Error err;
	uint8_t *buf_end;
	uint8_t *buf;
} Unpacker;

// used by `unpacker_get_string_sanitized()`
// to strip unwanted characters from the strings
// received from the peer
typedef enum {
	STRING_SANITIZE_NONE = 0,
	STRING_SANITIZE = 1 << 0,
	STRING_SANITIZE_CC = 1 << 1,
	STRING_SKIP_START_WHITESPACES = 1 << 2,
} StringSanitize;

// returns a new `Unpacker` instance
// it keeps track of how much data was already unpacked
//
// ```C
// uint8_t bytes[] = {0x05, 0x01, 0x02};
// Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
// unpacker_get_int(&unpacker); // => 5
// unpacker_get_int(&unpacker); // => 1
// unpacker_get_int(&unpacker); // => 2
// unpacker.err; // => Error::ERR_NONE
// ```
Unpacker unpacker_new(uint8_t *buf, size_t len);

// amount of bytes that have not yet been unpacked
size_t unpacker_remaining_size(Unpacker *unpacker);

// Use `unpacker_new` to get the value for `Unpacker *unpacker`
// it returns the next integer in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// ```C
// uint8_t bytes[] = {0x05};
// Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
// unpacker_get_int(&unpacker); // => 5
// unpacker.err; // => Error::ERR_NONE
// ```
int32_t unpacker_get_int(Unpacker *unpacker);

// Use `unpacker_new` to get the value for `Unpacker *unpacker`
// it returns the next null terminated string in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// applies `STRING_SANITIZE` by default
// if you want a string without sanitization use
//
// ```C
// uint8_t bytes[] = {'f', 'o', 0x03, 'o', 0x00};
// Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
// unpacker_get_string_sanitized(&unpacker, STRING_SANITIZE_NONE);
// ```
const char *unpacker_get_string(Unpacker *unpacker);

// Use `unpacker_new` to get the value for `Unpacker *unpacker`
// it returns the next null terminated string in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// ```C
// uint8_t bytes[] = {'f', 'o', 'o', 0x00};
// Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
//
// unpacker_get_string_sanitized(&unpacker, STRING_SANITIZE_CC); // => foo
// unpacker.err; // =>  Error::ERR_NONE
// ```
const char *unpacker_get_string_sanitized(Unpacker *unpacker, StringSanitize sanitize);

// Use `unpacker_new` to get the value for `Unpacker *unpacker`
// it returns the next boolean in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// Might set the unpacker->err to `ERR_INVALID_BOOL`
//
// ```C
// uint8_t bytes[] = {0x00, 0x01, 0xcc};
// Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
//
// unpacker_get_bool(&unpacker); // => false
// unpacker_get_bool(&unpacker); // => true
// unpacker_get_bool(&unpacker); // => false (invalid boolean)
// unpacker.err; // => Error::ERR_INVALID_BOOL
// ```
bool unpacker_get_bool(Unpacker *unpacker);

// Use `unpacker_new` to get the value for `Unpacker *unpacker`
// it returns the next `len` amount of bytes in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// ```C
// uint8_t bytes[] = {0x05};
// Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
// unpacker_get_raw(&unpacker, 1); // => 0x05
// unpacker.err; // => Error::ERR_NONE
// ```
const uint8_t *unpacker_get_raw(Unpacker *unpacker, size_t len);
