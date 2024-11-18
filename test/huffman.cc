#include <gtest/gtest.h>

extern "C" {
#include "../src/huffman.h"
}

TEST(Huffman, Decompress) {
	uint8_t decompressed[512];
	uint8_t compressed[] = {0x74, 0xde, 0x16, 0xd9, 0xa2, 0x8a, 0x1b};
	int len = huffman_decompress(compressed, sizeof(compressed), decompressed, sizeof(decompressed));
	EXPECT_EQ(len, 4);
	EXPECT_STREQ((const char *)decompressed, "foo");
}
