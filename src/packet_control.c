#include "packet_control.h"
#include "packet.h"

PacketControl *decode_control(uint8_t *buf, size_t len, PacketHeader header, Error *err) {
	ControlMessageKind kind = buf[0];
	char *reason = NULL;

	switch(kind) {
	case CTRL_MSG_CONNECT:
	case CTRL_MSG_CONNECTACCEPT: {
		Token token_magic = read_token(&buf[1]);

		if(token_magic != TOKEN_MAGIC) {
			if(err) {
				*err = ERR_INVALID_TOKEN_MAGIC;
			}

			return NULL;
		}

		buf += sizeof(Token);

		break;
	}
	case CTRL_MSG_CLOSE: {
		if(len - 1 > sizeof(Token)) {
			reason = (char *)&buf[1];
			buf += strlen(reason) + 1;
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

		return NULL;
	}

	PacketControl *packet = malloc(sizeof(PacketControl));

	packet->_ = PACKET_CONTROL;
	packet->header = header;
	packet->header.token = read_token(&buf[1]);
	packet->kind = kind;
	packet->reason = reason;

	return packet;
}
