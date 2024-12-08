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

void packer_init(Packer *packer) {
	packer->err = ERR_NONE;
	packer->current = packer->buf;
	packer->end = packer->buf + (size_t)PACKER_BUFFER_SIZE;
}

void packer_init_msg(Packer *packer, MessageId msg_id, MessageKind kind) {
	packer_init(packer);

	if(msg_id < 0 || msg_id > 0x3fffffff) {
		packer->err = ERR_MESSAGE_ID_OUT_OF_BOUNDS;
		return;
	}

	packer_add_int(packer, (int32_t)((msg_id << 1) | kind));
}

size_t packer_size(Packer *packer) {
	return packer->current - packer->buf;
}

size_t packer_remaining_size(Packer *packer) {
	return packer->end - packer->current;
}

uint8_t *packer_data(Packer *packer) {
	return packer->buf;
}

Error packer_add_int(Packer *packer, int32_t value) {
	size_t space = packer_remaining_size(packer);
	if(space <= 0) {
		return packer->err = ERR_BUFFER_FULL;
	}

	space--;

	*packer->current = 0;
	if(value < 0) {
		// set sign bit
		*packer->current |= 0x40;
		value = ~value;
	}

	// pack 6 bits into destination
	*packer->current |= value & 0x3f;
	// discard 6 bits
	value >>= 6;

	while(value) {
		if(space <= 0) {
			return packer->err = ERR_BUFFER_FULL;
		}
		// set extend bit
		*packer->current |= 0x80;
		space--;
		packer->current++;
		// pack 7 bits
		*packer->current = value & 0x7f;
		// discard 7 bits
		value >>= 7;
	}

	packer->current++;
	return ERR_NONE;
}

Error packer_add_string(Packer *packer, const char *value) {
	size_t len = strlen(value) + 1;
	if(packer_remaining_size(packer) < len) {
		return packer->err = ERR_BUFFER_FULL;
	}
	strncpy((char *)packer->current, value, len);
	packer->current += len;
}

void unpacker_init(Unpacker *unpacker, uint8_t *buf, size_t len) {
	unpacker->err = ERR_NONE;
	unpacker->buf = buf;
	unpacker->buf_end = buf + len;
}

size_t unpacker_remaining_size(Unpacker *unpacker) {
	return unpacker->buf_end - unpacker->buf;
}

int32_t unpacker_get_int(Unpacker *unpacker) {
	size_t space = unpacker_remaining_size(unpacker);
	if(space < 1) {
		unpacker->err = ERR_EMPTY_BUFFER;
		return 0;
	}

	const int32_t sign = (*unpacker->buf >> 6) & 1;
	int32_t value = *unpacker->buf & 0x3f;

	do {
		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x7f) << 6;

		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x7f) << (6 + 7);

		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x7f) << (6 + 7 + 7);

		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x0f) << (6 + 7 + 7 + 7);
	} while(0);

	unpacker->buf++;
	value ^= -sign;
	return value;
}

const char *unpacker_get_string_sanitized(Unpacker *unpacker, StringSanitize sanitize) {
	if(unpacker->err != ERR_NONE) {
		return "";
	}

	char *str = (char *)unpacker->buf;
	while(*unpacker->buf) // skip the string
	{
		unpacker->buf++;
		if(unpacker->buf == unpacker->buf_end) {
			unpacker->err = ERR_STR_UNEXPECTED_EOF;
			return "";
		}
	}
	unpacker->buf++;

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

const char *unpacker_get_string(Unpacker *unpacker) {
	return unpacker_get_string_sanitized(unpacker, STRING_SANITIZE);
}

bool unpacker_get_bool(Unpacker *unpacker) {
	int32_t val = unpacker_get_int(unpacker);
	if(val != 0 && val != 1) {
		unpacker->err = ERR_INVALID_BOOL;
	}
	return val == 1;
}

const uint8_t *unpacker_get_raw(Unpacker *unpacker, size_t len) {
	if(unpacker_remaining_size(unpacker) < len) {
		unpacker->err = ERR_END_OF_BUFFER;
		return NULL;
	}

	const uint8_t *ptr = unpacker->buf;

	unpacker->buf += len;

	return ptr;
}
