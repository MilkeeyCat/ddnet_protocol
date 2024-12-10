#pragma once

#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "packet.h"
#include "token.h"

// Given a buffer containing the packet payload without packet header.
// It will extract all system and game messages.
// And store them in the given packet struct.
Error fetch_chunks(uint8_t *buf, size_t len, Packet *packet);
