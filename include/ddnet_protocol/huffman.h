#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"

// Applies huffman compression to the given `input` and stores the compressed
// result in `output`. This should be applied to the teeworlds packet payload if
// the `PACKET_FLAG_COMPRESSION` is set. Returns the size of the compressed
// `output`.
//
// See also https://chillerdragon.github.io/teeworlds-protocol/06/fundamentals.html#huffman.
size_t ddproto_huffman_compress(const uint8_t *input, size_t input_len, uint8_t *output, size_t output_len, DDProtoError *err);

// Applies huffman decompression to the given `input` and stores the result in
// `output`. This should be applied to the teeworlds packet payload if the
// `PACKET_FLAG_COMPRESSION` is set. Returns the size of the decompressed
// `output`.
//
// See also https://chillerdragon.github.io/teeworlds-protocol/06/fundamentals.html#huffman.
size_t ddproto_huffman_decompress(const uint8_t *input, size_t input_len, uint8_t *output, size_t output_len, DDProtoError *err);

#ifdef __cplusplus
}
#endif
