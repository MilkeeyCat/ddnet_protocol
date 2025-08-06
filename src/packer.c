#include "packer.h"
#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "message.h"

void ddnet_str_sanitize_cc(char *string) {
	uint8_t *str = (uint8_t *)string;
	while(*str) {
		if(*str < 32) {
			*str = ' ';
		}
		str++;
	}
}

void ddnet_str_sanitize(char *string) {
	uint8_t *str = (uint8_t *)string;
	while(*str) {
		if(*str < 32 && !(*str == '\r') && !(*str == '\n') && !(*str == '\t')) {
			*str = ' ';
		}
		str++;
	}
}

void ddnet_str_clean_whitespaces(char *string) {
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

void ddnet_packer_init(DDNetPacker *packer) {
	packer->err = DDNET_ERR_NONE;
	packer->current = packer->buf;
	packer->end = packer->buf + (size_t)DDNET_PACKER_BUFFER_SIZE;
}

void ddnet_packer_init_msg(DDNetPacker *packer, DDNetMessageKind kind) {
	ddnet_packer_init(packer);

	DDNetMessageId msg_id;
	DDNetMessageCategory msg_category;

	switch(kind) {
	case DDNET_MSG_KIND_UNKNOWN:
		return;
	case DDNET_MSG_KIND_INFO:
		msg_id = DDNET_MSG_INFO;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_MAP_CHANGE:
		msg_id = DDNET_MSG_MAP_CHANGE;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_MAP_DATA:
		msg_id = DDNET_MSG_MAP_DATA;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_CON_READY:
		msg_id = DDNET_MSG_CON_READY;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_SNAP:
		msg_id = DDNET_MSG_SNAP;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_SNAPEMPTY:
		msg_id = DDNET_MSG_SNAPEMPTY;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_SNAPSINGLE:
		msg_id = DDNET_MSG_SNAPSINGLE;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_SNAPSMALL:
		msg_id = DDNET_MSG_SNAPSMALL;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_INPUTTIMING:
		msg_id = DDNET_MSG_INPUTTIMING;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_AUTH_STATUS:
		msg_id = DDNET_MSG_RCON_AUTH_STATUS;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_LINE:
		msg_id = DDNET_MSG_RCON_LINE;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_READY:
		msg_id = DDNET_MSG_READY;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_ENTERGAME:
		msg_id = DDNET_MSG_ENTERGAME;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_INPUT:
		msg_id = DDNET_MSG_INPUT;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_CMD:
		msg_id = DDNET_MSG_RCON_CMD;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_AUTH:
		msg_id = DDNET_MSG_RCON_AUTH;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_REQUEST_MAP_DATA:
		msg_id = DDNET_MSG_REQUEST_MAP_DATA;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_PING:
		msg_id = DDNET_MSG_PING;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_PING_REPLY:
		msg_id = DDNET_MSG_PING_REPLY;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_CMD_ADD:
		msg_id = DDNET_MSG_RCON_CMD_ADD;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_RCON_CMD_REM:
		msg_id = DDNET_MSG_RCON_CMD_REM;
		msg_category = DDNET_SYSTEM;
		break;
	case DDNET_MSG_KIND_SV_MOTD:
		msg_id = DDNET_MSG_SV_MOTD;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_SV_BROADCAST:
		msg_id = DDNET_MSG_SV_BROADCAST;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_SV_CHAT:
		msg_id = DDNET_MSG_SV_CHAT;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_SV_KILLMSG:
		msg_id = DDNET_MSG_SV_KILLMSG;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_SV_SOUNDGLOBAL:
		msg_id = DDNET_MSG_SV_SOUNDGLOBAL;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_SV_TUNEPARAMS:
		msg_id = DDNET_MSG_SV_TUNEPARAMS;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_SV_READYTOENTER:
		msg_id = DDNET_MSG_SV_READYTOENTER;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_SV_WEAPONPICKUP:
		msg_id = DDNET_MSG_SV_WEAPONPICKUP;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_CL_SAY:
		msg_id = DDNET_MSG_CL_SAY;
		msg_category = DDNET_GAME;
		break;
	case DDNET_MSG_KIND_CL_STARTINFO:
		msg_id = DDNET_MSG_CL_STARTINFO;
		msg_category = DDNET_GAME;
		break;
	}

	ddnet_packer_add_int(packer, (int32_t)((msg_id << 1) | msg_category));
}

size_t ddnet_packer_size(DDNetPacker *packer) {
	return packer->current - packer->buf;
}

size_t ddnet_packer_remaining_size(DDNetPacker *packer) {
	return packer->end - packer->current;
}

uint8_t *ddnet_packer_data(DDNetPacker *packer) {
	return packer->buf;
}

DDNetError ddnet_packer_add_int(DDNetPacker *packer, int32_t value) {
	size_t space = ddnet_packer_remaining_size(packer);
	if(space <= 0) {
		return packer->err = DDNET_ERR_BUFFER_FULL;
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
			return packer->err = DDNET_ERR_BUFFER_FULL;
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
	return DDNET_ERR_NONE;
}

DDNetError ddnet_packer_add_string(DDNetPacker *packer, const char *value) {
	size_t len = strlen(value) + 1;
	if(ddnet_packer_remaining_size(packer) < len) {
		return packer->err = DDNET_ERR_BUFFER_FULL;
	}
	strncpy((char *)packer->current, value, len);
	packer->current += len;

	return DDNET_ERR_NONE;
}

DDNetError ddnet_packer_add_raw(DDNetPacker *packer, const uint8_t *data, size_t size) {
	if(ddnet_packer_remaining_size(packer) < size) {
		return packer->err = DDNET_ERR_BUFFER_FULL;
	}
	memcpy(packer->current, data, size);
	packer->current += size;
	return DDNET_ERR_NONE;
}

void ddnet_unpacker_init(DDNetUnpacker *unpacker, uint8_t *buf, size_t len) {
	unpacker->err = DDNET_ERR_NONE;
	unpacker->buf = buf;
	unpacker->buf_end = buf + len;
}

size_t ddnet_unpacker_remaining_size(DDNetUnpacker *unpacker) {
	return unpacker->buf_end - unpacker->buf;
}

int32_t ddnet_unpacker_get_int(DDNetUnpacker *unpacker) {
	size_t space = ddnet_unpacker_remaining_size(unpacker);
	if(space < 1) {
		unpacker->err = DDNET_ERR_EMPTY_BUFFER;
		return 0;
	}

	const int32_t sign = (*unpacker->buf >> 6) & 1;
	int32_t value = *unpacker->buf & 0x3f;

	do {
		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = DDNET_ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x7f) << 6;

		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = DDNET_ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x7f) << (6 + 7);

		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = DDNET_ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x7f) << (6 + 7 + 7);

		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = DDNET_ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x0f) << (6 + 7 + 7 + 7);
	} while(0);

	unpacker->buf++;
	value ^= -sign;
	return value;
}

const char *ddnet_unpacker_get_string_sanitized(DDNetUnpacker *unpacker, DDNetStringSanitize sanitize) {
	if(unpacker->err != DDNET_ERR_NONE) {
		return "";
	}

	char *str = (char *)unpacker->buf;
	while(*unpacker->buf) // skip the string
	{
		unpacker->buf++;
		if(unpacker->buf == unpacker->buf_end) {
			unpacker->err = DDNET_ERR_STR_UNEXPECTED_EOF;
			return "";
		}
	}
	unpacker->buf++;

	if(sanitize & DDNET_STRING_SANITIZE) {
		ddnet_str_sanitize(str);
	} else if(sanitize & DDNET_STRING_SANITIZE_CC) {
		ddnet_str_sanitize_cc(str);
	}
	if(sanitize & DDNET_STRING_SKIP_START_WHITESPACES) {
		ddnet_str_clean_whitespaces(str);
	}
	return str;
}

const char *ddnet_unpacker_get_string(DDNetUnpacker *unpacker) {
	return ddnet_unpacker_get_string_sanitized(unpacker, DDNET_STRING_SANITIZE);
}

bool ddnet_unpacker_get_bool(DDNetUnpacker *unpacker) {
	int32_t val = ddnet_unpacker_get_int(unpacker);
	if(val != 0 && val != 1) {
		unpacker->err = DDNET_ERR_INVALID_BOOL;
	}
	return val == 1;
}

const uint8_t *ddnet_unpacker_get_raw(DDNetUnpacker *unpacker, size_t len) {
	if(ddnet_unpacker_remaining_size(unpacker) < len) {
		unpacker->err = DDNET_ERR_END_OF_BUFFER;
		return NULL;
	}

	const uint8_t *ptr = unpacker->buf;

	unpacker->buf += len;

	return ptr;
}
