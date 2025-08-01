#include "token.h"

const DDNetToken DDNET_TOKEN_MAGIC = 0x544b454e;
const DDNetToken DDNET_TOKEN_NONE = 0xffffffff;

DDNetToken ddnet_read_token(const uint8_t *buf) {
	return ((buf[0] & 0xffU) << 24U) | ((buf[1] & 0xffU) << 16U) | ((buf[2] & 0xffU) << 8U) | (buf[3] & 0xffU);
}

void ddnet_write_token(DDNetToken token, uint8_t *buf) {
	buf[0] = (token >> 24) & 0xff;
	buf[1] = (token >> 16) & 0xff;
	buf[2] = (token >> 8) & 0xff;
	buf[3] = token & 0xff;
}
