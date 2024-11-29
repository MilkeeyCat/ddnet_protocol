#include "packer.h"
#include "errors.h"

#include "common.h"

void str_sanitize_cc(char *string) {
	uint8_t *str = (uint8_t *)string;
	while(*str) {
		if(*str < 32) {
			*str = ' ';
		}
		str++;
	}
}

void str_sanitize(char *string) {
	uint8_t *str = (uint8_t *)string;
	while(*str) {
		if(*str < 32 && !(*str == '\r') && !(*str == '\n') && !(*str == '\t')) {
			*str = ' ';
		}
		str++;
	}
}

void str_clean_whitespaces(char *string) {
	char *read = string;
	char *write = string;

	/* skip initial whitespace */
	while(*read == ' ') {
		read++;
	}

	/* end of read string is detected in the loop */
	while(true) {
		/* skip whitespace */
		bool found_whitespace = false;
		for(; *read == ' '; read++) {
			found_whitespace = true;
		}
		/* if not at the end of the string, put a found whitespace here */
		if(*read) {
			if(found_whitespace) {
				*write++ = ' ';
			}
			*write++ = *read++;
		} else {
			*write = 0;
			break;
		}
	}
}

Unpacker unpacker_new(uint8_t *buf, size_t len) {
	return (Unpacker){
		.err = 0,
		.buf = buf,
		.buf_end = buf + len};
}

int32_t unpacker_get_int(Unpacker *state) {
	size_t len = state->buf_end - state->buf;
	if(len < 1) {
		state->err = ERR_EMPTY_BUFFER;
		return 0;
	}

	const int32_t sign = (*state->buf >> 6) & 1;
	int32_t value = *state->buf & 0x3f;

	do {
		if(!(*state->buf & 0x80)) {
			break;
		}
		state->buf++;
		value |= (*state->buf & 0x7f) << 6;

		if(!(*state->buf & 0x80)) {
			break;
		}
		state->buf++;
		value |= (*state->buf & 0x7f) << (6 + 7);

		if(!(*state->buf & 0x80)) {
			break;
		}
		state->buf++;
		value |= (*state->buf & 0x7f) << (6 + 7 + 7);

		if(!(*state->buf & 0x80)) {
			break;
		}
		state->buf++;
		value |= (*state->buf & 0x0f) << (6 + 7 + 7 + 7);
	} while(0);

	state->buf++;
	value ^= -sign;
	return value;
}

const char *unpacker_get_string_sanitized(Unpacker *state, StringSanitize sanitize) {
	if(state->err != ERR_NONE) {
		return "";
	}

	char *str = (char *)state->buf;
	while(*state->buf) // skip the string
	{
		state->buf++;
		if(state->buf == state->buf_end) {
			state->err = ERR_STR_UNEXPECTED_EOF;
			return "";
		}
	}
	state->buf++;

	if(sanitize & STRING_SANITIZE) {
		str_sanitize(str);
	} else if(sanitize & STRING_SANITIZE_CC) {
		str_sanitize_cc(str);
	}
	if(sanitize & STRING_SKIP_START_WHITESPACES) {
		str_clean_whitespaces(str);
	}
	return str;
}

const char *unpacker_get_string(Unpacker *state) {
	return unpacker_get_string_sanitized(state, STRING_SANITIZE);
}

bool unpacker_get_bool(Unpacker *state) {
	int32_t val = unpacker_get_int(state);
	if(val != 0 && val != 1) {
		state->err = ERR_INVALID_BOOL;
	}
	return val == 1;
}

const uint8_t *unpacker_get_raw(Unpacker *state, size_t len) {
	const uint8_t *ptr = state->buf;

	state->buf += len;

	return ptr;
}
