#include <cstring>
#include <gtest/gtest.h>

extern "C" {
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/packer.h>
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

TEST(Unpacker, Strings) {
	uint8_t bytes[] = {'f', 'o', 'o', 0x00, 'b', 'a', 'r', 0x00, 'A', 0x02, 0x02, 0x00, 'x'};
	Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));

	EXPECT_STREQ(unpacker_get_string(&unpacker), "foo");
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_STREQ(unpacker_get_string(&unpacker), "bar");
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_STREQ(unpacker_get_string(&unpacker), "A  "); // expect sanitize by default
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_STREQ(unpacker_get_string(&unpacker), "");
	EXPECT_EQ(unpacker.err, Error::ERR_STR_UNEXPECTED_EOF);
}

TEST(Unpacker, StringsSanitized) {
	uint8_t bytes[] = {'f', 'o', 'o', 0x00, 'b', 'a', 0x02, 0x03, 0x03, 'r', 0x00, ' ', ' ', 'x', 0x00};
	Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));

	EXPECT_STREQ(unpacker_get_string_sanitized(&unpacker, STRING_SANITIZE_NONE), "foo");
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_STREQ(unpacker_get_string_sanitized(&unpacker, STRING_SANITIZE_CC), "ba   r");
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_STREQ(unpacker_get_string_sanitized(&unpacker, STRING_SKIP_START_WHITESPACES), "x");
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, Booleans) {
	uint8_t bytes[] = {0x00, 0x01, 0xcc};
	Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));

	EXPECT_EQ(unpacker_get_bool(&unpacker), false);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_EQ(unpacker_get_bool(&unpacker), true);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_EQ(unpacker_get_bool(&unpacker), false);
	EXPECT_EQ(unpacker.err, Error::ERR_INVALID_BOOL);
}

TEST(Unpacker, Raw) {
	uint8_t bytes[] = {0x7f, 0x69, 0x04, 0x20};
	Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));

	unpacker_get_int(&unpacker);
	EXPECT_TRUE(std::memcmp(unpacker_get_raw(&unpacker, 3), &bytes[1], 3) == 0);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}
