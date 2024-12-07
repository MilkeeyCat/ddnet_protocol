#pragma once

#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "packet.h"
#include "token.h"

// allow the user to define their own max? To reduce memory usage.
#ifndef MAX_CHUNKS
#define MAX_CHUNKS 512
#endif

// Struct holding the packet payload of a regular packet.
// It contains chunks which hold all the gameplay relevant
// net messages.
typedef struct {
	PacketKind _;
	PacketHeader header;
	Token token;
	Chunk chunks[MAX_CHUNKS];
} PacketNormal;

// Given a buffer containing the packet payload without packet header.
// It will extract all system and game messages.
PacketNormal *decode_normal(uint8_t *buf, size_t len, PacketHeader header, Error *err);
