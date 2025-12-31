#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/int_string.h>

size_t ddproto_str_length(const char *str) {
	size_t len = 0;
	while(*(str++)) {
		len++;
	}
	return len;
}

DDProtoError ddproto_ints_to_str(const uint32_t *ints, size_t num_ints, char *str, size_t str_len) {
	if(str_len < num_ints * sizeof(uint32_t)) {
		return DDPROTO_ERR_OUTPUT_VAR_TOO_SMALL;
	}

	// unpack string without validation
	size_t str_index = 0;
	for(size_t i = 0; i < num_ints; i++) {
		const uint32_t current_int = ints[i];
		str[str_index++] = (char)(((current_int >> 24) & 0xff) - 128);
		str[str_index++] = (char)(((current_int >> 16) & 0xff) - 128);
		str[str_index++] = (char)(((current_int >> 8) & 0xff) - 128);
		str[str_index++] = (char)((current_int & 0xff) - 128);
	}
	// ensure null-termination
	str[str_index - 1] = '\0';

	// TODO: check valid utf8
	return DDPROTO_ERR_NONE;
}

DDProtoError ddproto_str_to_ints(uint32_t *ints, size_t num_ints, const char *str) {
	const size_t str_len = ddproto_str_length(str);
	if(str_len > num_ints * sizeof(uint32_t)) {
		return DDPROTO_ERR_OUTPUT_VAR_TOO_SMALL;
	}

	for(size_t i = 0; i < num_ints; i++) {
		// copy to temporary buffer to ensure we don't read past the end of the input string
		char buf[sizeof(uint32_t)] = {0, 0, 0, 0};
		for(size_t buf_idx = 0; buf_idx < sizeof(uint32_t) && (i * sizeof(uint32_t)) + buf_idx < str_len; buf_idx++) {
			buf[buf_idx] = str[(i * sizeof(uint32_t)) + buf_idx];
		}
		ints[i] = ((uint32_t)(buf[0] + 128) << 24) |
			  ((buf[1] + 128) << 16) |
			  ((buf[2] + 128) << 8) |
			  (buf[3] + 128);
	}
	// last byte is always zero and unused in this format
	ints[num_ints - 1] &= 0xffffff00;

	return DDPROTO_ERR_NONE;
}
