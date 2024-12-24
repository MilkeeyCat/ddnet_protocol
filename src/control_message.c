#include "control_message.h"
#include "packet.h"

ControlMessage decode_control(uint8_t *buf, size_t len, PacketHeader *header, Error *err) {
	ControlMessage msg = {
		.kind = buf[0],
		.reason = NULL,
	};

	switch(msg.kind) {
	case CTRL_MSG_CONNECT:
	case CTRL_MSG_CONNECTACCEPT: {
		Token token_magic = read_token(&buf[1]);

		if(token_magic != TOKEN_MAGIC) {
			if(err) {
				*err = ERR_INVALID_TOKEN_MAGIC;
			}

			return msg;
		}

		buf += sizeof(Token);

		break;
	}
	case CTRL_MSG_CLOSE: {
		if(len - 1 > sizeof(Token)) {
			msg.reason = (char *)&buf[1];
			buf += strlen(msg.reason) + 1;
		}

		break;
	}
	case CTRL_MSG_KEEPALIVE:
	case CTRL_MSG_ACCEPT:
		break;
	default:
		if(err) {
			*err = ERR_INVALID_CONTROL_MESSAGE;
		}

		return msg;
	}

	header->token = read_token(&buf[1]);

	return msg;
}
