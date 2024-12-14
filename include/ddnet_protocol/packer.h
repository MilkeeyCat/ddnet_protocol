#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"

// Replaces all characters below ASCII 32 with whitespace.
void str_sanitize_cc(char *string);

// Replaces all characters below ASCII 32 except \r, \n and \t with whitespace.
void str_sanitize(char *string);

// Removes leading and trailing spaces and limits the use of multiple spaces.
void str_clean_whitespaces(char *string);

// used by `unpacker_get_string_sanitized()`
// to strip unwanted characters from the strings
// received from the peer
typedef enum {
	STRING_SANITIZE_NONE = 0,
	STRING_SANITIZE = 1 << 0,
	STRING_SANITIZE_CC = 1 << 1,
	STRING_SKIP_START_WHITESPACES = 1 << 2,
} StringSanitize;

// State for the unpacker
// holds the data to be unpacked
// and keeps track of how much data was unpacked
// and also tracks if errors occured
//
// See also `unpacker_init` and `unpacker_get_int`
typedef struct {
	Error err;
	uint8_t *buf_end;
	uint8_t *buf;
} Unpacker;

// maximum output and storage size in bytes
// used by the `Packer`
#define PACKER_BUFFER_SIZE (1024 * 2)

// State for the packer
// holds the currently packed data
// and also tracks errors
typedef struct {
	Error err;
	uint8_t *current;
	uint8_t *end;
	uint8_t buf[PACKER_BUFFER_SIZE];
} Packer;

// initializes a new packer struct
void packer_init(Packer *packer);

// get the size in bytes of the currently packed data
// see also `packer_data()`
size_t packer_size(Packer *packer);

// amount of free bytes in the output buffer
// the packer can pack a maximum of `PACKER_BUFFER_SIZE` bytes
size_t packer_remaining_size(Packer *packer);

// use in combination with `packer_size()`
uint8_t *packer_data(Packer *packer);

// Packs `value` as teeworlds varint
// call `packer_data()` to receive the full packed data
Error packer_add_int(Packer *packer, int32_t value);

// Packs `value` as plain null terminated C string
// call `packer_data()` to receive the full packed data
Error packer_add_string(Packer *packer, const char *value);

// returns a new `Unpacker` instance
// it keeps track of how much data was already unpacked
//
// ```C
// uint8_t bytes[] = {0x05, 0x01, 0x02};
// Unpacker unpacker;
// unpacker_init(&unpacker, bytes, sizeof(bytes));
// unpacker_get_int(&unpacker); // => 5
// unpacker_get_int(&unpacker); // => 1
// unpacker_get_int(&unpacker); // => 2
// unpacker.err; // => Error::ERR_NONE
// ```
void unpacker_init(Unpacker *packer, uint8_t *buf, size_t len);

// amount of bytes that have not yet been unpacked
size_t unpacker_remaining_size(Unpacker *unpacker);

// Use `unpacker_init` to get the value for `Unpacker *unpacker`
// it returns the next integer in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// ```C
// uint8_t bytes[] = {0x05};
// Unpacker unpacker;
// unpacker_init(&unpacker, bytes, sizeof(bytes));
// unpacker_get_int(&unpacker); // => 5
// unpacker.err; // => Error::ERR_NONE
// ```
int32_t unpacker_get_int(Unpacker *unpacker);

// Use `unpacker_init` to get the value for `Unpacker *unpacker`
// it returns the next null terminated string in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// applies `STRING_SANITIZE` by default
// if you want a string without sanitization use
//
// ```C
// uint8_t bytes[] = {'f', 'o', 0x03, 'o', 0x00};
// Unpacker unpacker;
// unpacker_init(&unpacker, bytes, sizeof(bytes));
// unpacker_get_string_sanitized(&unpacker, STRING_SANITIZE_NONE);
// ```
const char *unpacker_get_string(Unpacker *unpacker);

// Use `unpacker_init` to get the value for `Unpacker *unpacker`
// it returns the next null terminated string in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// ```C
// uint8_t bytes[] = {'f', 'o', 'o', 0x00};
// Unpacker unpacker;
// unpacker_init(&unpacker, bytes, sizeof(bytes));
//
// unpacker_get_string_sanitized(&unpacker, STRING_SANITIZE_CC); // => foo
// unpacker.err; // =>  Error::ERR_NONE
// ```
const char *unpacker_get_string_sanitized(Unpacker *unpacker, StringSanitize sanitize);

// Use `unpacker_init` to get the value for `Unpacker *unpacker`
// it returns the next boolean in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// Might set the unpacker->err to `ERR_INVALID_BOOL`
//
// ```C
// uint8_t bytes[] = {0x00, 0x01, 0xcc};
// Unpacker unpacker;
// unpacker_init(&unpacker, bytes, sizeof(bytes));
//
// unpacker_get_bool(&unpacker); // => false
// unpacker_get_bool(&unpacker); // => true
// unpacker_get_bool(&unpacker); // => false (invalid boolean)
// unpacker.err; // => Error::ERR_INVALID_BOOL
// ```
bool unpacker_get_bool(Unpacker *unpacker);

// Use `unpacker_init` to get the value for `Unpacker *unpacker`
// it returns the next `len` amount of bytes in the unpacker data
// and also progresses the internal unpacker state to point to the next element
//
// ```C
// uint8_t bytes[] = {0x05};
// Unpacker unpacker;
// unpacker_init(&unpacker, bytes, sizeof(bytes));
// unpacker_get_raw(&unpacker, 1); // => 0x05
// unpacker.err; // => Error::ERR_NONE
// ```
const uint8_t *unpacker_get_raw(Unpacker *unpacker, size_t len);

#ifdef __cplusplus
}
#endif
