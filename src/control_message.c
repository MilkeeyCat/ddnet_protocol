#include "control_message.h"
#include "packer.h"
#include "packet.h"
#include "token.h"

<<<<<<< Updated upstream
size_t ddnet_decode_control(const uint8_t *buf, size_t len, ControlMessage *msg, DDNetError *err) {
=======
size_t decode_control(const uint8_t *buf, size_t len, DDNetControlMessage *msg, DDNetError *err) {
>>>>>>> Stashed changes
	msg->kind = buf[0];
	msg->reason = NULL;

	switch(msg->kind) {
	case DDNET_CTRL_MSG_CONNECT:
	case DDNET_CTRL_MSG_CONNECTACCEPT: {
		const DDNetToken token_magic = ddnet_read_token(&buf[1]);

		if(token_magic != DDNET_TOKEN_MAGIC) {
			if(err) {
				*err = DDNET_ERR_INVALID_TOKEN_MAGIC;
			}

			return 0;
		}

		return sizeof(DDNetToken) + 1;
	}
	case DDNET_CTRL_MSG_CLOSE:
		if(len - 1 > sizeof(DDNetToken)) {
			msg->reason = (const char *)&buf[1];

			return strlen(msg->reason) + 1 + 1;
		}

		break;
	case DDNET_CTRL_MSG_KEEPALIVE:
	case DDNET_CTRL_MSG_ACCEPT:
		break;
	default:
		if(err) {
			*err = DDNET_ERR_INVALID_CONTROL_MESSAGE;
		}

		return 0;
	}

	return 1;
}

<<<<<<< Updated upstream
size_t ddnet_encode_control(const ControlMessage *msg, uint8_t *buf, DDNetError *err) {
=======
size_t encode_control(const DDNetControlMessage *msg, uint8_t *buf, DDNetError *err) {
>>>>>>> Stashed changes
	DDNetPacker packer;
	ddnet_packer_init(&packer);
	buf[0] = msg->kind;

	switch(msg->kind) {
	case DDNET_CTRL_MSG_KEEPALIVE:
	case DDNET_CTRL_MSG_CONNECTACCEPT:
		if(err) {
			// TODO: implement
			*err = DDNET_ERR_INVALID_CONTROL_MESSAGE;
		}
		break;
	case DDNET_CTRL_MSG_ACCEPT:
		break;
	case DDNET_CTRL_MSG_CONNECT:
		ddnet_write_token(DDNET_TOKEN_MAGIC, buf + 1);
		return sizeof(DDNetToken) + 1;
	case DDNET_CTRL_MSG_CLOSE:
		if(msg->reason && msg->reason[0]) {
			ddnet_packer_add_string(&packer, msg->reason);
		}
		break;
	};
	if(ddnet_packer_size(&packer) > 0) {
		memcpy(buf + 1, ddnet_packer_data(&packer), ddnet_packer_size(&packer));
	}
	return ddnet_packer_size(&packer) + 1;
}
