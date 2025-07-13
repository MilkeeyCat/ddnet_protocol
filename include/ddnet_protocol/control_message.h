#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "packet.h"

// Given a buffer containing the packet payload without packet header.
// It will extract one control message.
ControlMessage decode_control(uint8_t *buf, size_t len, PacketHeader *header, Error *err);

// Given a ControlMessage it will write the network presentation without packet header into buf
// And it returns the amount of bytes written.
size_t encode_control(const ControlMessage *msg, uint8_t *buf, Error *err);

#ifdef __cplusplus
}
#endif
