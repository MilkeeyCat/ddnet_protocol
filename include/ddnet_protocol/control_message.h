#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "packet.h"

// Given a buffer containing the packet payload without packet header. It will
// extract one control message. And it returns the amount of bytes read.
size_t ddproto_decode_control(const uint8_t *buf, size_t len, DDProtoControlMessage *msg, DDProtoError *err);

// Given a `ControlMessage`, it will write the network presentation without
// packet header into `buf`. And it returns the amount of bytes written.
size_t ddproto_encode_control(const DDProtoControlMessage *msg, uint8_t *buf, DDProtoError *err);

#ifdef __cplusplus
}
#endif
