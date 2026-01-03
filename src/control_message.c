#include <ddnet_protocol/control_message.h>

#include <ddnet_protocol/packer.h>
#include <ddnet_protocol/packet.h>
#include <ddnet_protocol/token.h>

size_t ddproto_decode_control(const uint8_t *buf, size_t len, DDProtoControlMessage *msg, DDProtoError *err) {
	msg->kind = buf[0];
	msg->reason = NULL;

	switch(msg->kind) {
	case DDPROTO_CTRL_MSG_CONNECT:
	case DDPROTO_CTRL_MSG_CONNECTACCEPT: {
		const DDProtoToken token_magic = ddproto_read_token(&buf[1]);

		if(token_magic != DDPROTO_TOKEN_MAGIC) {
			if(err) {
				*err = DDPROTO_ERR_INVALID_TOKEN_MAGIC;
			}

			return 0;
		}

		return sizeof(DDProtoToken) + 1;
	}
	case DDPROTO_CTRL_MSG_CLOSE:
		if(len - 1 > sizeof(DDProtoToken)) {
			msg->reason = (const char *)&buf[1];

			return strlen(msg->reason) + 1 + 1;
		}

		break;
	case DDPROTO_CTRL_MSG_KEEPALIVE:
	case DDPROTO_CTRL_MSG_ACCEPT:
		break;
	default:
		if(err) {
			*err = DDPROTO_ERR_INVALID_CONTROL_MESSAGE;
		}

		return 0;
	}

	return 1;
}

size_t ddproto_encode_control(const DDProtoControlMessage *msg, uint8_t *buf, DDProtoError *err) {
	DDProtoPacker packer;
	ddproto_packer_init(&packer);
	buf[0] = msg->kind;

	switch(msg->kind) {
	case DDPROTO_CTRL_MSG_CONNECTACCEPT:
		if(err) {
			// TODO: implement
			*err = DDPROTO_ERR_INVALID_CONTROL_MESSAGE;
		}
		break;
	case DDPROTO_CTRL_MSG_KEEPALIVE:
	case DDPROTO_CTRL_MSG_ACCEPT:
		break;
	case DDPROTO_CTRL_MSG_CONNECT:
		ddproto_write_token(DDPROTO_TOKEN_MAGIC, buf + 1);
		return sizeof(DDProtoToken) + 1;
	case DDPROTO_CTRL_MSG_CLOSE:
		if(msg->reason && msg->reason[0]) {
			ddproto_packer_add_string(&packer, msg->reason);
		}
		break;
	};
	if(ddproto_packer_size(&packer) > 0) {
		memcpy(buf + 1, ddproto_packer_data(&packer), ddproto_packer_size(&packer));
	}
	return ddproto_packer_size(&packer) + 1;
}
