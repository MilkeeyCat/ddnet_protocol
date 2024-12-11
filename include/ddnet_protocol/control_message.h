#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "packet.h"
#include "token.h"

// Given a buffer containing the packet payload without packet header.
// It will extract one control message.
ControlMessage *decode_control(uint8_t *buf, size_t len, PacketHeader *header, Error *err);

#ifdef __cplusplus
}
#endif
