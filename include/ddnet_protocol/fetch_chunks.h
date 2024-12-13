#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "packet.h"
#include "token.h"

typedef void (*OnChunk)(void *ctx, Chunk *chunk);

// Given a buffer containing the packet payload without packet header.
// It will extract all system and game messages.
// And store them in the given packet struct.
Error fetch_chunks(uint8_t *buf, size_t len, PacketHeader *header, OnChunk callback, void *ctx);

#ifdef __cplusplus
}
#endif
