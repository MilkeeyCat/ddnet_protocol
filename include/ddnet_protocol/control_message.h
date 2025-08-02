#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "packet.h"

// Given a buffer containing the packet payload without packet header.
// It will extract one control message. And it returns the amount of bytes read.
<<<<<<< Updated upstream
size_t ddnet_decode_control(const uint8_t *buf, size_t len, ControlMessage *msg, DDNetError *err);

// Given a ControlMessage it will write the network presentation without packet header into buf
// And it returns the amount of bytes written.
size_t ddnet_encode_control(const ControlMessage *msg, uint8_t *buf, DDNetError *err);
=======
size_t decode_control(const uint8_t *buf, size_t len, DDNetControlMessage *msg, DDNetError *err);

// Given a ControlMessage it will write the network presentation without packet header into buf
// And it returns the amount of bytes written.
size_t encode_control(const DDNetControlMessage *msg, uint8_t *buf, DDNetError *err);
>>>>>>> Stashed changes

#ifdef __cplusplus
}
#endif
