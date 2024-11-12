#pragma once

#include "common.h"
#include "errors.h"

#define PACKET_MIN_HEADER_SIZE 3
#define PACKET_MAX_HEADER_SIZE 1400

typedef enum {
	PACKET_CONNLESS,
	PACKET_CONTROL,
	PACKET_NORMAL,
} PacketKind;

typedef enum {
	PACKET_FLAG_CONTROL = 1 << 2,
	PACKET_FLAG_CONNLESS = 1 << 3,
	PACKET_FLAG_RESEND = 1 << 4,
	PACKET_FLAG_COMPRESSION = 1 << 5,
} PacketFlag;

typedef struct {
	uint16_t flags;
	uint16_t ack;
	uint8_t num_chunks;
} PacketHeader;

PacketHeader decode_packet_header(uint8_t *buf);
PacketKind *decode(uint8_t *buf, size_t len, Error *err);
