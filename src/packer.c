#include "packer.h"
#include "errors.h"

Unpacker unpacker_new(uint8_t *buf, size_t len) {
	return (Unpacker){
		.err = 0,
		.buf = buf,
		.buf_end = buf + len};
}

int32_t unpacker_get_int(Unpacker *state) {
	size_t len = state->buf_end - state->buf;
	if(len < 1) {
		state->err = ERR_EMPTY_BUFFER;
		return 0;
	}

	const int32_t sign = (*state->buf >> 6) & 1;
	int32_t value = *state->buf & 0x3f;

	do {
		if(!(*state->buf & 0x80)) {
			break;
		}
		state->buf++;
		value |= (*state->buf & 0x7f) << 6;

		if(!(*state->buf & 0x80)) {
			break;
		}
		state->buf++;
		value |= (*state->buf & 0x7f) << (6 + 7);

		if(!(*state->buf & 0x80)) {
			break;
		}
		state->buf++;
		value |= (*state->buf & 0x7f) << (6 + 7 + 7);

		if(!(*state->buf & 0x80)) {
			break;
		}
		state->buf++;
		value |= (*state->buf & 0x0f) << (6 + 7 + 7 + 7);
	} while(0);

	state->buf++;
	value ^= -sign;
	return value;
}
