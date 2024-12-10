#pragma once

#include "packet.h"
#include "token.h"

// Given a buffer containing the packet payload without packet header.
// It will extract one control message.
PacketControl *decode_control(uint8_t *buf, size_t len, PacketHeader *header, Error *err);
