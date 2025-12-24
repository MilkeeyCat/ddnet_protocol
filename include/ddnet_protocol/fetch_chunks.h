#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "packet.h"

typedef void (*OnDDNetChunk)(void *ctx, DDNetChunk *chunk);

// Given a buffer containing the packet payload without packet header. It will
// extract all system and game messages and store them in the given packet
// struct. And it returns the amount of bytes read.
size_t ddnet_fetch_chunks(uint8_t *buf, size_t len, DDNetPacketHeader *header, OnDDNetChunk callback, void *ctx, DDNetError *err);

#ifdef __cplusplus
}
#endif
