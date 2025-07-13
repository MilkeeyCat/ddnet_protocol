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
// It will extract all system and game messages
// and store them in the given packet struct.
// And it returns the amount of bytes read.
size_t fetch_chunks(uint8_t *buf, size_t len, PacketHeader *header, OnChunk callback, void *ctx, Error *err);

#ifdef __cplusplus
}
#endif
