#include <cstring>
#include <gtest/gtest.h>

extern "C" {
#include "../src/errors.h"
#include "../src/packer.h"
}

TEST(Unpacker, SingleByteInts) {
	uint8_t bytes[] = {0x05, 0x01, 0x02};
	Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
	EXPECT_EQ(unpacker_get_int(&unpacker), 5);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_EQ(unpacker_get_int(&unpacker), 1);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_EQ(unpacker_get_int(&unpacker), 2);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, MultiByteInts) {
	uint8_t bytes[] = {0x80, 0x01, 0x81, 0x01};
	Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));

	EXPECT_EQ(unpacker_get_int(&unpacker), 64);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);

	EXPECT_EQ(unpacker_get_int(&unpacker), 65);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, NegativeIntsSingleByte) {
	uint8_t bytes[] = {0x7e, 0x7f};
	Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));

	EXPECT_EQ(unpacker_get_int(&unpacker), -63);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);

	EXPECT_EQ(unpacker_get_int(&unpacker), -64);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, NegativeIntsMutliByte) {
	uint8_t bytes[] = {0xc0, 0x01};
	Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));

	EXPECT_EQ(unpacker_get_int(&unpacker), -65);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, Raw) {
	uint8_t bytes[] = {0x7f, 0x69, 0x04, 0x20};
	Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));

	unpacker_get_int(&unpacker);
	EXPECT_TRUE(std::memcmp(unpacker_get_raw(&unpacker, 3), &bytes[1], 3) == 0);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}
