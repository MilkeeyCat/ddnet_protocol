#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "packet.h"

typedef void (*OnDDProtoChunk)(void *ctx, DDProtoChunk *chunk);

// Given a buffer containing the packet payload without packet header. It will
// extract all system and game messages and store them in the given packet
// struct. And it returns the amount of bytes read.
size_t ddproto_fetch_chunks(const uint8_t *buf, size_t len, DDProtoPacketHeader *header, OnDDProtoChunk callback, void *ctx, DDProtoError *err);

#ifdef __cplusplus
}
#endif
