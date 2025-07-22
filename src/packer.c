#include "packer.h"
#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "message.h"

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

void packer_init_msg(Packer *packer, DDNetMessageKind kind) {
	packer_init(packer);

	MessageId msg_id;
	DDNetMessageCategory msg_category;

	switch(kind) {
	case DDNET_MSG_KIND_UNKNOWN:
		return;
	case DDNET_MSG_KIND_INFO:
		msg_id = MSG_INFO;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_MAP_CHANGE:
		msg_id = MSG_MAP_CHANGE;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_MAP_DATA:
		msg_id = MSG_MAP_DATA;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_CON_READY:
		msg_id = MSG_CON_READY;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_CL_STARTINFO:
		msg_id = MSG_CL_STARTINFO;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_SNAP:
		msg_id = MSG_SNAP;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_SNAPEMPTY:
		msg_id = MSG_SNAPEMPTY;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_SNAPSINGLE:
		msg_id = MSG_SNAPSINGLE;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_SNAPSMALL:
		msg_id = MSG_SNAPSMALL;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_INPUTTIMING:
		msg_id = MSG_INPUTTIMING;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_AUTH_STATUS:
		msg_id = MSG_RCON_AUTH_STATUS;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_LINE:
		msg_id = MSG_RCON_LINE;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_READY:
		msg_id = MSG_READY;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_ENTERGAME:
		msg_id = MSG_ENTERGAME;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_INPUT:
		msg_id = MSG_INPUT;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_CMD:
		msg_id = MSG_RCON_CMD;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_AUTH:
		msg_id = MSG_RCON_AUTH;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_REQUEST_MAP_DATA:
		msg_id = MSG_REQUEST_MAP_DATA;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_PING:
		msg_id = MSG_PING;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_PING_REPLY:
		msg_id = MSG_PING_REPLY;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_CMD_ADD:
		msg_id = MSG_RCON_CMD_ADD;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_CMD_REM:
		msg_id = MSG_RCON_CMD_REM;
		msg_category = DDNET_SYSTEM;
		break;
	}

	packer_add_int(packer, (int32_t)((msg_id << 1) | msg_category));
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

	return ERR_NONE;
}

Error packer_add_raw(Packer *packer, const uint8_t *data, size_t size) {
	if(packer_remaining_size(packer) < size) {
		return packer->err = ERR_BUFFER_FULL;
	}
	memcpy(packer->current, data, size);
	packer->current += size;
	return ERR_NONE;
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
