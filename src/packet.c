#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "control_packet.h"
#include "errors.h"
#include "packet.h"

PacketKind *decode(uint8_t *buf, size_t len, Error *err) {
	if(len < PACKET_MIN_HEADER_SIZE || len > PACKET_MAX_HEADER_SIZE) {
		if(err) {
			*err = ERR_INVALID_PACKET;
		}

		return NULL;
	}

	uint8_t flags = buf[0] >> 2;

	if(flags & PACKET_FLAG_CONTROL) {
		return (PacketKind *)decode_control(&buf[3], len - 3, err);
	} else if(err) {
		*err = ERR_INVALID_PACKET;
	}

	return NULL;
}
