#include <gtest/gtest.h>

#include <ddnet_protocol/chunk.h>

TEST(ChunkHeader, Vital) {
	uint8_t bytes[] = {0x44, 0x04, 0x01, 0x00};
	uint8_t *buf = &bytes[0];
	ChunkHeader header = decode_chunk_header(&buf);

	EXPECT_EQ(header.flags, 0x01);
	EXPECT_EQ(header.size, 0x44);
	EXPECT_EQ(header.sequence, 0x01);
	EXPECT_EQ(&bytes[sizeof(bytes) - 1], buf);
}

TEST(ChunkHeader, NotVital) {
	uint8_t bytes[] = {0x44, 0x04, 0x01, 0x00};
	uint8_t *buf = &bytes[0];
	ChunkHeader header = decode_chunk_header(&buf);

	EXPECT_EQ(header.flags, 0x01);
	EXPECT_EQ(header.size, 0x44);
	EXPECT_EQ(header.sequence, 0x01);
	EXPECT_EQ(&bytes[sizeof(bytes) - 1], buf);
}
