#pragma once

#include "common.h"

int32_t huffman_compress(const uint8_t *input, size_t input_len, uint8_t *output, size_t output_len);
int32_t huffman_decompress(const uint8_t *input, size_t input_len, uint8_t *output, size_t output_len);
