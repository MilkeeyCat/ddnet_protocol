#include <cstddef>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/fetch_chunks.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/packet.h>

TEST(Chunk, HeaderVital) {
	uint8_t bytes[] = {0x44, 0x04, 0x01, 0x00};
	ChunkHeader header;
	size_t header_size = decode_chunk_header(bytes, &header);

	EXPECT_EQ(header.flags, CHUNK_FLAG_VITAL);
	EXPECT_EQ(header.size, 68);
	EXPECT_EQ(header.sequence, 1);
	EXPECT_EQ(header_size, 3);
}

TEST(Chunk, HeaderNotVital) {
	uint8_t bytes[] = {0x02, 0x01, 0x00};
	uint8_t *buf = &bytes[0];
	ChunkHeader header;
	size_t header_size = decode_chunk_header(buf, &header);

	EXPECT_EQ(header.flags, 0x00);
	EXPECT_EQ(header.size, 33);
	EXPECT_EQ(header.sequence, 0);
	EXPECT_EQ(header_size, 2);
}

TEST(Chunk, PackNonVitalHeader) {
	ChunkHeader header = {
		.flags = 0,
		.size = 2};
	uint8_t output[8];
	size_t bytes_written = encode_chunk_header(&header, output);
	uint8_t expected[] = {0x00, 0x02};
	EXPECT_TRUE(std::memcmp(output, expected, sizeof(expected)) == 0);
	EXPECT_EQ(bytes_written, 2);
}

TEST(Chunk, PackVitalHeader) {
	ChunkHeader header = {
		.flags = CHUNK_FLAG_VITAL,
		.size = 2,
		.sequence = 4};
	uint8_t output[8];
	size_t bytes_written = encode_chunk_header(&header, output);
	uint8_t expected[] = {0x40, 0x02, 0x04};
	EXPECT_TRUE(std::memcmp(output, expected, sizeof(expected)) == 0);
	EXPECT_EQ(bytes_written, 3);
}

namespace {
typedef struct {
	Chunk *chunks;
	size_t len;
} Context;

void on_chunk(void *ctx, Chunk *chunk) {
	Context *context = (Context *)ctx;

	memcpy(&context->chunks[context->len++], chunk, sizeof(Chunk));
}
} // namespace

TEST(Chunk, BigMessageId) {
	// fake message id sent with the following C++ code
	//
	// CMsgPacker FakeMsg(99 , true);
	// FakeMsg.AddString("ABC");
	// SendMsg(Conn, &FakeMsg, MSGFLAG_VITAL | MSGFLAG_FLUSH);
	uint8_t bytes[] = {0x40, 0x06, 0x03, 0x87, 0x03, 0x41, 0x42, 0x43, 0x00};
	uint8_t *buf = &bytes[0];
	Error err = ERR_NONE;
	PacketHeader header = {
		.num_chunks = 1};
	Context ctx = {
		.chunks = (Chunk *)malloc(sizeof(Chunk) * header.num_chunks),
		.len = 0,
	};
	size_t size = fetch_chunks(buf, sizeof(bytes), &header, on_chunk, &ctx, &err);
	EXPECT_EQ(err, ERR_NONE);
	EXPECT_EQ(ctx.len, 1);

	EXPECT_EQ(ctx.chunks[0].header.flags, CHUNK_FLAG_VITAL);
	EXPECT_EQ(ctx.chunks[0].header.sequence, 3);
	EXPECT_EQ(ctx.chunks[0].header.size, 6);
	free(ctx.chunks);
}
