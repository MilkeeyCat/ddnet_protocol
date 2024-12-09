#pragma once

#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "packet.h"
#include "token.h"

// Given a buffer containing the packet payload without packet header.
// It will extract all system and game messages.
PacketNormal *decode_normal(uint8_t *buf, size_t len, PacketHeader *header, Error *err);
