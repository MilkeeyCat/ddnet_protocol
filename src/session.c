#include <ddnet_protocol/session.h>

#include <ddnet_protocol/chunk.h>

bool ddproto_seq_in_backroom(uint16_t sequence, uint16_t ack) {
	int32_t bottom = ack - (DDPROTO_MAX_SEQUENCE / 2);
	if(bottom < 0) {
		if(sequence <= ack) {
			return true;
		}
		if((int32_t)sequence >= (bottom + DDPROTO_MAX_SEQUENCE)) {
			return true;
		}
	} else if(sequence <= ack && (int32_t)sequence >= bottom) {
		return true;
	}
	return false;
}
