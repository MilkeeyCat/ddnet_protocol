#include "control_packet.h"
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
			size_t len = MIN(strlen((char *)&buf[1]) + 1, 256);
			reason = malloc(len);

			strncpy(reason, (const char *)&buf[1], len);

			buf += len;
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
	packet->kind = kind;
	packet->reason = reason;
	packet->token = read_token(&buf[1]);

	return packet;
}
