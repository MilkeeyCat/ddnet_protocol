#include "token.h"

const Token TOKEN_MAGIC = 0x544b454e;

Token read_token(const uint8_t *buf) {
	return ((buf[0] & 0xff) << 24) | ((buf[1] & 0xff) << 16) | ((buf[2] & 0xff) << 8) | (buf[3] & 0xff);
}
