#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/huffman.h>

#include <ddcorpus/runner.hpp>

#include <gtest/gtest.h>

TEST(Corpus, Huffman) {
	Corpus corpus("../corpus");

	corpus.add_runner("protocol/huffman/compress", [](const uint8_t *input, uintptr_t size, const char *ext, struct DDCorpBuffer *buf) {
		ASSERT_STREQ(ext, "bin");

		uint8_t output[512];
		DDProtoError err = DDPROTO_ERR_NONE;
		size_t compressed_size = ddproto_huffman_compress(input, size, output, sizeof(output), &err);

		ASSERT_EQ(err, DDPROTO_ERR_NONE);

		ddcorp_buffer_copy(buf, output, compressed_size);
	});

	corpus.add_runner("protocol/huffman/decompress", [](const uint8_t *input, uintptr_t size, const char *ext, struct DDCorpBuffer *buf) {
		ASSERT_STREQ(ext, "bin");

		uint8_t output[512];
		DDProtoError err = DDPROTO_ERR_NONE;
		size_t compressed_size = ddproto_huffman_decompress(input, size, output, sizeof(output), &err);

		if(err != DDPROTO_ERR_NONE) {
			ddcorp_buffer_write_str(buf, "-1");
		} else {
			ddcorp_buffer_copy(buf, output, compressed_size);
		}
	});

	ASSERT_TRUE(corpus.run());
}
