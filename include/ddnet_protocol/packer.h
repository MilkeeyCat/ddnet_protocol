#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"

/// Replaces all characters below ASCII 32 with whitespace.
void ddproto_str_sanitize_cc(char *string);

/// Replaces all characters below ASCII 32 except \\r, \\n and \\t with
/// whitespace.
void ddproto_str_sanitize(char *string);

/// Removes leading and trailing spaces and limits the use of multiple spaces.
void ddproto_str_clean_whitespaces(char *string);

/// Used by @ref ddproto_unpacker_get_string_sanitized to strip unwanted
/// characters from the strings received from the peer.
typedef enum {
	DDPROTO_STRING_SANITIZE_NONE = 0,
	DDPROTO_STRING_SANITIZE = 1 << 0,
	DDPROTO_STRING_SANITIZE_CC = 1 << 1,
	DDPROTO_STRING_SKIP_START_WHITESPACES = 1 << 2,
} DDProtoStringSanitize;

/// @brief State for the unpacker.
///
/// Holds the data to be unpacked and keeps track of how much data was unpacked
/// and also tracks if errors occurred.
///
/// See also @ref ddproto_unpacker_init and @ref ddproto_unpacker_get_int.
typedef struct {
	DDProtoError err;
	const uint8_t *buf_end;
	const uint8_t *buf;
} DDProtoUnpacker;

/// Maximum output and storage size in bytes used by the `DDProtoPacker`.
#define DDPROTO_PACKER_BUFFER_SIZE (1024 * 2)

/// @brief State for the packer.
///
/// Holds the currently packed data and also tracks errors.
typedef struct {
	DDProtoError err;
	uint8_t *current;
	uint8_t *end;
	uint8_t buf[DDPROTO_PACKER_BUFFER_SIZE];
} DDProtoPacker;

/// @brief Initializes a new packer struct.
///
/// See also @ref ddproto_packer_init_msg if you want to send a net message.
void ddproto_packer_init(DDProtoPacker *packer);

/// @brief Get the size in bytes of the currently packed data.
///
/// See also @ref ddproto_packer_data.
size_t ddproto_packer_size(DDProtoPacker *packer);

/// @brief Amount of free bytes in the output buffer.
///
/// The packer can pack a maximum of @ref DDPROTO_PACKER_BUFFER_SIZE bytes.
size_t ddproto_packer_remaining_size(DDProtoPacker *packer);

/// Use in combination with @ref ddproto_packer_size.
uint8_t *ddproto_packer_data(DDProtoPacker *packer);

/// @brief Packs `value` as teeworlds varint.
///
/// Call @ref ddproto_packer_data to receive the full packed data.
DDProtoError ddproto_packer_add_int(DDProtoPacker *packer, int32_t value);

/// @brief Packs `value` as plain null terminated C string.
///
/// Call @ref ddproto_packer_data to receive the full packed data.
DDProtoError ddproto_packer_add_string(DDProtoPacker *packer, const char *value);

/// @brief Packs `data` as raw data.
///
/// Call @ref ddproto_packer_data to receive the full packed data.
DDProtoError ddproto_packer_add_raw(DDProtoPacker *packer, const uint8_t *data, size_t size);

/// Returns a new `DDProtoUnpacker` instance.
/// It keeps track of how much data was already unpacked.
///
/// ```C
/// uint8_t bytes[] = {0x05, 0x01, 0x02};
/// DDProtoUnpacker unpacker;
/// ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
/// ddproto_unpacker_get_int(&unpacker); // => 5
/// ddproto_unpacker_get_int(&unpacker); // => 1
/// ddproto_unpacker_get_int(&unpacker); // => 2
/// unpacker.err; // => DDProtoError::DDPROTO_ERR_NONE
/// ```
void ddproto_unpacker_init(DDProtoUnpacker *unpacker, const uint8_t *buf, size_t len);

/// @brief Amount of bytes that have not yet been unpacked.
size_t ddproto_unpacker_remaining_size(DDProtoUnpacker *unpacker);

/// Use @ref ddproto_unpacker_init to get the value for
/// `DDProtoUnpacker *unpacker`. It returns the next integer in the unpacker
/// data and also progresses the internal unpacker state to point to the next
/// element.
///
/// ```C
/// uint8_t bytes[] = {0x05};
/// DDProtoUnpacker unpacker;
/// ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
/// ddproto_unpacker_get_int(&unpacker); // => 5
/// unpacker.err; // => DDProtoError::DDPROTO_ERR_NONE
/// ```
int32_t ddproto_unpacker_get_int(DDProtoUnpacker *unpacker);

/// Use @ref ddproto_unpacker_init to get the value for
/// `DDProtoUnpacker *unpacker`. It returns the next null terminated string in
/// the unpacker data and also progresses the internal unpacker state to point
/// to the next element.
///
/// Applies @ref DDPROTO_STRING_SANITIZE by default. If you want a string
/// without sanitization use:
///
/// ```C
/// uint8_t bytes[] = {'f', 'o', 0x03, 'o', 0x00};
/// DDProtoUnpacker unpacker;
/// ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
/// ddproto_unpacker_get_string_sanitized(&unpacker, DDPROTO_STRING_SANITIZE_NONE);
/// ```
const char *ddproto_unpacker_get_string(DDProtoUnpacker *unpacker);

/// Use @ref ddproto_unpacker_init to get the value for
/// `DDProtoUnpacker *unpacker`. It returns the next null terminated string in
/// the unpacker data and also progresses the internal unpacker state to point
/// to the next element.
///
/// ```C
/// uint8_t bytes[] = {'f', 'o', 'o', 0x00};
/// DDProtoUnpacker unpacker;
/// ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
///
/// ddproto_unpacker_get_string_sanitized(&unpacker, DDPROTO_STRING_SANITIZE_CC); // => foo
/// unpacker.err; // =>  DDProtoError::DDPROTO_ERR_NONE
/// ```
const char *ddproto_unpacker_get_string_sanitized(DDProtoUnpacker *unpacker, DDProtoStringSanitize sanitize);

/// Use @ref ddproto_unpacker_init to get the value for
/// `DDProtoUnpacker *unpacker`. It returns the next boolean in the unpacker
/// data and also progresses the internal unpacker state to point to the next
/// element.
///
/// Might set the `unpacker->err` to @ref DDPROTO_ERR_INVALID_BOOL.
///
/// ```C
/// uint8_t bytes[] = {0x00, 0x01, 0xcc};
/// DDProtoUnpacker unpacker;
/// ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
///
/// ddproto_unpacker_get_bool(&unpacker); // => false
/// ddproto_unpacker_get_bool(&unpacker); // => true
/// ddproto_unpacker_get_bool(&unpacker); // => false (invalid boolean)
/// unpacker.err; // => DDProtoError::DDPROTO_ERR_INVALID_BOOL
/// ```
bool ddproto_unpacker_get_bool(DDProtoUnpacker *unpacker);

/// Use @ref ddproto_unpacker_init to get the value for
/// `DDProtoUnpacker *unpacker`. It returns the next `len` amount of bytes in
/// the unpacker data and also progresses the internal unpacker state to point
/// to the next element.
///
/// ```C
/// uint8_t bytes[] = {0x05};
/// DDProtoUnpacker unpacker;
/// ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
/// ddproto_unpacker_get_raw(&unpacker, 1); // => 0x05
/// unpacker.err; // => DDProtoError::DDPROTO_ERR_NONE
/// ```
const uint8_t *ddproto_unpacker_get_raw(DDProtoUnpacker *unpacker, size_t len);

#ifdef __cplusplus
}
#endif
