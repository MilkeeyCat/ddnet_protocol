#pragma once

#include "common.h"
#include "errors.h"

// minimum size in bytes required for a valid packet header
#define PACKET_MIN_HEADER_SIZE 3

// maximum amount of total packet size
#define PACKET_MAX_HEADER_SIZE 1400

// internal enum for packet types
// not sent over the network
typedef enum {
	PACKET_CONNLESS,
	PACKET_CONTROL,
	PACKET_NORMAL,
} PacketKind;

// packet flags
// used for the packet header
// multiple flags can be combined
//
// but if control is set compression should not be set
typedef enum {
	PACKET_FLAG_CONTROL = 1 << 2,
	PACKET_FLAG_CONNLESS = 1 << 3,
	PACKET_FLAG_RESEND = 1 << 4,
	PACKET_FLAG_COMPRESSION = 1 << 5,
} PacketFlag;

// teeworlds packet header
// parsed version of the first 7 bytes in
// the udp payload
typedef struct {
	uint16_t flags;
	uint16_t ack;
	uint8_t num_chunks;
} PacketHeader;

// unpack packet header
PacketHeader decode_packet_header(uint8_t *buf);

// given a pointer to the beginning of a udp payload
// this determins the type of packet
PacketKind *decode(uint8_t *buf, size_t len, Error *err);
