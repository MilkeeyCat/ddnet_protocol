#pragma once

#include "stdint.h"

typedef uint32_t Token;

extern const Token TOKEN_MAGIC;

Token read_token(const uint8_t *buf);
