#include "packet.h"
#include "packet_control.h"
#include "packet_normal.h"

PacketHeader decode_packet_header(uint8_t *buf) {
	return (PacketHeader){
		.flags = buf[0] >> 2,
		.ack = ((buf[0] & 0x3) << 8) | buf[1],
		.num_chunks = buf[2],
	};
}

Packet *decode(uint8_t *buf, size_t len, Error *err) {
	if(len < PACKET_HEADER_SIZE || len > MAX_PACKET_SIZE) {
		if(err) {
			*err = ERR_INVALID_PACKET;
		}

		return NULL;
	}

	Packet *packet = malloc(sizeof(Packet));
	packet->header = decode_packet_header(buf);

	if(packet->header.flags & PACKET_FLAG_CONTROL) {
		packet->_ = PACKET_CONTROL;
		packet->control = decode_control(&buf[3], len - 3, &packet->header, err);
	} else {
		packet->_ = PACKET_NORMAL;
		packet->normal = decode_normal(&buf[3], len - 3, &packet->header, err);
	}

	return packet;
}
