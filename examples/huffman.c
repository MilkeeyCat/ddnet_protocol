#include <stdint.h>
#include <stdio.h>

#include <ddnet_protocol/huffman.h>

int main() {
	uint8_t decompressed[512];
	uint8_t compressed[] = {0x74, 0xde, 0x16, 0xd9, 0xa2, 0x8a, 0x1b};
	DDNetError err = DDNET_ERR_NONE;
	ddnet_huffman_decompress(compressed, sizeof(compressed), decompressed, sizeof(decompressed), &err);
	if(err == DDNET_ERR_NONE) {
		puts((const char *)decompressed); // foo
	}
}
