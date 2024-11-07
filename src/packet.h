#pragma once

#include <stddef.h>
#include <stdint.h>

#include "errors.h"

typedef enum {
	PACKET_CONNLESS,
	PACKET_CONTROL,
	PACKET_NORMAL,
} PacketKind;

PacketKind *decode(uint8_t *buf, size_t *n, Error *err);
