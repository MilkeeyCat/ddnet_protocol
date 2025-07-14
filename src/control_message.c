#include "control_message.h"
#include "packer.h"
#include "packet.h"
#include "token.h"

size_t decode_control(const uint8_t *buf, size_t len, ControlMessage *msg, Error *err) {
	msg->kind = buf[0];
	msg->reason = NULL;

	switch(msg->kind) {
	case CTRL_MSG_CONNECT:
	case CTRL_MSG_CONNECTACCEPT: {
		const Token token_magic = read_token(&buf[1]);

		if(token_magic != TOKEN_MAGIC) {
			if(err) {
				*err = ERR_INVALID_TOKEN_MAGIC;
			}

			return 0;
		}

		return sizeof(Token) + 1;
	}
	case CTRL_MSG_CLOSE:
		if(len - 1 > sizeof(Token)) {
			msg->reason = (const char *)&buf[1];

			return strlen(msg->reason) + 1 + 1;
		}

		break;
	case CTRL_MSG_KEEPALIVE:
	case CTRL_MSG_ACCEPT:
		break;
	default:
		if(err) {
			*err = ERR_INVALID_CONTROL_MESSAGE;
		}

		return 0;
	}

	return 1;
}

size_t encode_control(const ControlMessage *msg, uint8_t *buf, Error *err) {
	Packer packer;
	packer_init(&packer);
	buf[0] = msg->kind;

	switch(msg->kind) {
	case CTRL_MSG_KEEPALIVE:
	case CTRL_MSG_CONNECTACCEPT:
		if(err) {
			// TODO: implement
			*err = ERR_INVALID_CONTROL_MESSAGE;
		}
		break;
	case CTRL_MSG_ACCEPT:
		break;
	case CTRL_MSG_CONNECT:
		write_token(TOKEN_MAGIC, buf + 1);
		return sizeof(Token) + 1;
	case CTRL_MSG_CLOSE:
		if(msg->reason && msg->reason[0]) {
			packer_add_string(&packer, msg->reason);
		}
		break;
	};
	if(packer_size(&packer) > 0) {
		memcpy(buf + 1, packer_data(&packer), packer_size(&packer));
	}
	return packer_size(&packer) + 1;
}
