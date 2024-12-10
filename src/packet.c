#include "packet.h"
#include "errors.h"
#include "fetch_chunks.h"
#include "packet_control.h"

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
	memset(packet, 0, sizeof(*packet));
	packet->header = decode_packet_header(buf);

	if(packet->header.flags & PACKET_FLAG_CONTROL) {
		packet->kind = PACKET_CONTROL;
		packet->control = decode_control(&buf[3], len - 3, &packet->header, err);
	} else {
		packet->kind = PACKET_NORMAL;
		Error chunk_error = fetch_chunks(&buf[3], len - 3, packet);
		if(chunk_error != ERR_NONE) {
			if(err) {
				*err = chunk_error;
			}
			free_packet(packet);
			return NULL;
		}
	}

	return packet;
}

Error free_packet(Packet *packet) {
	if(packet->kind == PACKET_NORMAL) {
		for(size_t i = 0; i < MAX_CHUNKS; i++) {
			free(packet->chunks[i].msg.unused);
		}
	}
	free(packet);
	return ERR_NONE;
}
