#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/common.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/packer.h>

void ddproto_str_sanitize_cc(char *string) {
	uint8_t *str = (uint8_t *)string;
	while(*str) {
		if(*str < 32) {
			*str = ' ';
		}
		str++;
	}
}

void ddproto_str_sanitize(char *string) {
	uint8_t *str = (uint8_t *)string;
	while(*str) {
		if(*str < 32 && !(*str == '\r') && !(*str == '\n') && !(*str == '\t')) {
			*str = ' ';
		}
		str++;
	}
}

void ddproto_str_clean_whitespaces(char *string) {
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

void ddproto_packer_init(DDProtoPacker *packer) {
	packer->err = DDPROTO_ERR_NONE;
	packer->current = packer->buf;
	packer->end = packer->buf + (size_t)DDPROTO_PACKER_BUFFER_SIZE;
}

void ddproto_packer_init_msg(DDProtoPacker *packer, DDProtoMessageKind kind) {
	ddproto_packer_init(packer);

	DDProtoMessageId msg_id;
	DDProtoMessageCategory msg_category;

	switch(kind) {
	case DDPROTO_MSG_KIND_UNKNOWN:
		return;
	case DDPROTO_MSG_KIND_INFO:
		msg_id = DDPROTO_MSG_INFO;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_MAP_CHANGE:
		msg_id = DDPROTO_MSG_MAP_CHANGE;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_MAP_DATA:
		msg_id = DDPROTO_MSG_MAP_DATA;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_CON_READY:
		msg_id = DDPROTO_MSG_CON_READY;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_SNAP:
		msg_id = DDPROTO_MSG_SNAP;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_SNAPEMPTY:
		msg_id = DDPROTO_MSG_SNAPEMPTY;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_SNAPSINGLE:
		msg_id = DDPROTO_MSG_SNAPSINGLE;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_SNAPSMALL:
		msg_id = DDPROTO_MSG_SNAPSMALL;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_INPUTTIMING:
		msg_id = DDPROTO_MSG_INPUTTIMING;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_RCON_AUTH_STATUS:
		msg_id = DDPROTO_MSG_RCON_AUTH_STATUS;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_RCON_LINE:
		msg_id = DDPROTO_MSG_RCON_LINE;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_READY:
		msg_id = DDPROTO_MSG_READY;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_ENTERGAME:
		msg_id = DDPROTO_MSG_ENTERGAME;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_INPUT:
		msg_id = DDPROTO_MSG_INPUT;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_RCON_CMD:
		msg_id = DDPROTO_MSG_RCON_CMD;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_RCON_AUTH:
		msg_id = DDPROTO_MSG_RCON_AUTH;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_REQUEST_MAP_DATA:
		msg_id = DDPROTO_MSG_REQUEST_MAP_DATA;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_PING:
		msg_id = DDPROTO_MSG_PING;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_PING_REPLY:
		msg_id = DDPROTO_MSG_PING_REPLY;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_RCON_CMD_ADD:
		msg_id = DDPROTO_MSG_RCON_CMD_ADD;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_RCON_CMD_REM:
		msg_id = DDPROTO_MSG_RCON_CMD_REM;
		msg_category = DDPROTO_SYSTEM;
		break;
	case DDPROTO_MSG_KIND_SV_MOTD:
		msg_id = DDPROTO_MSG_SV_MOTD;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_BROADCAST:
		msg_id = DDPROTO_MSG_SV_BROADCAST;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_CHAT:
		msg_id = DDPROTO_MSG_SV_CHAT;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_KILLMSG:
		msg_id = DDPROTO_MSG_SV_KILLMSG;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_SOUNDGLOBAL:
		msg_id = DDPROTO_MSG_SV_SOUNDGLOBAL;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_TUNEPARAMS:
		msg_id = DDPROTO_MSG_SV_TUNEPARAMS;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_READYTOENTER:
		msg_id = DDPROTO_MSG_SV_READYTOENTER;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_WEAPONPICKUP:
		msg_id = DDPROTO_MSG_SV_WEAPONPICKUP;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_EMOTICON:
		msg_id = DDPROTO_MSG_SV_EMOTICON;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_VOTECLEAROPTIONS:
		msg_id = DDPROTO_MSG_SV_VOTECLEAROPTIONS;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_VOTEOPTIONLISTADD:
		msg_id = DDPROTO_MSG_SV_VOTEOPTIONLISTADD;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_VOTEOPTIONADD:
		msg_id = DDPROTO_MSG_SV_VOTEOPTIONADD;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_VOTEOPTIONREMOVE:
		msg_id = DDPROTO_MSG_SV_VOTEOPTIONREMOVE;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_SV_VOTESET:
		msg_id = DDPROTO_MSG_SV_VOTESET;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_CL_SAY:
		msg_id = DDPROTO_MSG_CL_SAY;
		msg_category = DDPROTO_GAME;
		break;
	case DDPROTO_MSG_KIND_CL_STARTINFO:
		msg_id = DDPROTO_MSG_CL_STARTINFO;
		msg_category = DDPROTO_GAME;
		break;
	}

	ddproto_packer_add_int(packer, (int32_t)((msg_id << 1) | msg_category));
}

size_t ddproto_packer_size(DDProtoPacker *packer) {
	return packer->current - packer->buf;
}

size_t ddproto_packer_remaining_size(DDProtoPacker *packer) {
	return packer->end - packer->current;
}

uint8_t *ddproto_packer_data(DDProtoPacker *packer) {
	return packer->buf;
}

DDProtoError ddproto_packer_add_int(DDProtoPacker *packer, int32_t value) {
	size_t space = ddproto_packer_remaining_size(packer);
	if(space <= 0) {
		return packer->err = DDPROTO_ERR_BUFFER_FULL;
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
			return packer->err = DDPROTO_ERR_BUFFER_FULL;
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
	return DDPROTO_ERR_NONE;
}

DDProtoError ddproto_packer_add_string(DDProtoPacker *packer, const char *value) {
	size_t len = strlen(value) + 1;
	if(ddproto_packer_remaining_size(packer) < len) {
		return packer->err = DDPROTO_ERR_BUFFER_FULL;
	}
	strncpy((char *)packer->current, value, len);
	packer->current += len;

	return DDPROTO_ERR_NONE;
}

DDProtoError ddproto_packer_add_raw(DDProtoPacker *packer, const uint8_t *data, size_t size) {
	if(ddproto_packer_remaining_size(packer) < size) {
		return packer->err = DDPROTO_ERR_BUFFER_FULL;
	}
	memcpy(packer->current, data, size);
	packer->current += size;
	return DDPROTO_ERR_NONE;
}

void ddproto_unpacker_init(DDProtoUnpacker *unpacker, const uint8_t *buf, size_t len) {
	unpacker->err = DDPROTO_ERR_NONE;
	unpacker->buf = buf;
	unpacker->buf_end = buf + len;
}

size_t ddproto_unpacker_remaining_size(DDProtoUnpacker *unpacker) {
	return unpacker->buf_end - unpacker->buf;
}

int32_t ddproto_unpacker_get_int(DDProtoUnpacker *unpacker) {
	size_t space = ddproto_unpacker_remaining_size(unpacker);
	if(space < 1) {
		unpacker->err = DDPROTO_ERR_EMPTY_BUFFER;
		return 0;
	}

	const int32_t sign = (*unpacker->buf >> 6) & 1;
	int32_t value = *unpacker->buf & 0x3f;

	do {
		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = DDPROTO_ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x7f) << 6;

		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = DDPROTO_ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x7f) << (6 + 7);

		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = DDPROTO_ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x7f) << (6 + 7 + 7);

		if(!(*unpacker->buf & 0x80)) {
			break;
		}
		if(--space <= 0) {
			unpacker->err = DDPROTO_ERR_END_OF_BUFFER;
			return 0;
		}
		unpacker->buf++;
		value |= (*unpacker->buf & 0x0f) << (6 + 7 + 7 + 7);
	} while(0);

	unpacker->buf++;
	value ^= -sign;
	return value;
}

const char *ddproto_unpacker_get_string_sanitized(DDProtoUnpacker *unpacker, DDProtoStringSanitize sanitize) {
	if(unpacker->err != DDPROTO_ERR_NONE) {
		return "";
	}

	char *str = (char *)unpacker->buf;
	while(*unpacker->buf) // skip the string
	{
		unpacker->buf++;
		if(unpacker->buf == unpacker->buf_end) {
			unpacker->err = DDPROTO_ERR_STR_UNEXPECTED_EOF;
			return "";
		}
	}
	unpacker->buf++;

	if(sanitize & DDPROTO_STRING_SANITIZE) {
		ddproto_str_sanitize(str);
	} else if(sanitize & DDPROTO_STRING_SANITIZE_CC) {
		ddproto_str_sanitize_cc(str);
	}
	if(sanitize & DDPROTO_STRING_SKIP_START_WHITESPACES) {
		ddproto_str_clean_whitespaces(str);
	}
	return str;
}

const char *ddproto_unpacker_get_string(DDProtoUnpacker *unpacker) {
	return ddproto_unpacker_get_string_sanitized(unpacker, DDPROTO_STRING_SANITIZE);
}

bool ddproto_unpacker_get_bool(DDProtoUnpacker *unpacker) {
	int32_t val = ddproto_unpacker_get_int(unpacker);
	if(val != 0 && val != 1) {
		unpacker->err = DDPROTO_ERR_INVALID_BOOL;
	}
	return val == 1;
}

const uint8_t *ddproto_unpacker_get_raw(DDProtoUnpacker *unpacker, size_t len) {
	if(ddproto_unpacker_remaining_size(unpacker) < len) {
		unpacker->err = DDPROTO_ERR_END_OF_BUFFER;
		return NULL;
	}

	const uint8_t *ptr = unpacker->buf;

	unpacker->buf += len;

	return ptr;
}
