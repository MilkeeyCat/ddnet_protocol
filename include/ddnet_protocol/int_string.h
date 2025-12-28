#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"

// Get the length of a string in bytes.
size_t ddnet_str_length(const char *str);

// Convert list of integers into a string.
// Used by the snapshot which technically only consists of integers, but also holds strings.
DDNetError ddnet_ints_to_str(const uint32_t *ints, size_t num_ints, char *str, size_t str_len);

// Convert string to a list of integers.
// Used by the snapshot which technically only consists of integers, but also holds strings.
// If the string is too big and does not fit into `num_ints` amount of integers the function
// returns an error. If the string is so short that it does not need `num_ints` integers
// to be represented it still fills all the integers with padding.
DDNetError ddnet_str_to_ints(uint32_t *ints, size_t num_ints, const char *str);

#ifdef __cplusplus
}
#endif
