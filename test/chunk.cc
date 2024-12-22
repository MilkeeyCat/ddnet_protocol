#include <cstddef>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

#include <ddnet_protocol/chunk.h>

TEST(Chunk, HeaderVital) {
	uint8_t bytes[] = {0x44, 0x04, 0x01, 0x00};
	uint8_t *buf = &bytes[0];
	ChunkHeader header = decode_chunk_header(&buf);

	EXPECT_EQ(header.flags, CHUNK_FLAG_VITAL);
	EXPECT_EQ(header.size, 68);
	EXPECT_EQ(header.sequence, 1);
	EXPECT_EQ(&bytes[sizeof(bytes) - 1], buf);
}

TEST(Chunk, HeaderNotVital) {
	uint8_t bytes[] = {0x02, 0x01, 0x00};
	uint8_t *buf = &bytes[0];
	ChunkHeader header = decode_chunk_header(&buf);

	EXPECT_EQ(header.flags, 0x00);
	EXPECT_EQ(header.size, 33);
	EXPECT_EQ(header.sequence, 0);
	EXPECT_EQ(&bytes[sizeof(bytes) - 1], buf);
}

TEST(Chunk, PackNonVitalHeader) {
	ChunkHeader header = {
		.flags = 0,
		.size = 2};
	uint8_t output[8];
	uint8_t *end = encode_chunk_header(&header, output);
	uint8_t expected[] = {0x00, 0x02};
	EXPECT_TRUE(std::memcmp(output, expected, sizeof(expected)) == 0);
	size_t bytes_written = end - output;
	EXPECT_EQ(bytes_written, 2);
}

TEST(Chunk, PackVitalHeader) {
	ChunkHeader header = {
		.flags = CHUNK_FLAG_VITAL,
		.size = 2,
		.sequence = 4};
	uint8_t output[8];
	uint8_t *end = encode_chunk_header(&header, output);
	uint8_t expected[] = {0x40, 0x02, 0x04};
	EXPECT_TRUE(std::memcmp(output, expected, sizeof(expected)) == 0);
	size_t bytes_written = end - output;
	EXPECT_EQ(bytes_written, 3);
}
