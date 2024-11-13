#pragma once

#include "common.h"
#include "errors.h"

typedef struct {
	Error err;
	uint8_t *buf_end;
	uint8_t *buf;
} Unpacker;

Unpacker unpacker_new(uint8_t *buf, size_t len);
int32_t unpacker_get_int(Unpacker *state);
