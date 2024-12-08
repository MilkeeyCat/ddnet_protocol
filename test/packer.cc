#include <cstring>
#include <gtest/gtest.h>

extern "C" {
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/packer.h>
}

TEST(Packer, SingleByteInts) {
	Packer packer;
	packer_init(&packer);

	EXPECT_EQ(packer_remaining_size(&packer), PACKER_BUFFER_SIZE);
	EXPECT_EQ(packer_size(&packer), 0);

	packer_add_int(&packer, 0);
	EXPECT_EQ(packer.err, Error::ERR_NONE);

	packer_add_int(&packer, 1);
	EXPECT_EQ(packer.err, Error::ERR_NONE);

	uint8_t bytes[] = {0x00, 0x01};
	EXPECT_TRUE(std::memcmp(packer_data(&packer), bytes, packer_size(&packer)) == 0);

	EXPECT_EQ(packer_size(&packer), 2);
	EXPECT_EQ(packer.err, Error::ERR_NONE);
}

TEST(Packer, MultiByteInts) {
	Packer packer;
	packer_init(&packer);

	EXPECT_EQ(packer_remaining_size(&packer), PACKER_BUFFER_SIZE);
	EXPECT_EQ(packer_size(&packer), 0);

	packer_add_int(&packer, 64);
	EXPECT_EQ(packer_size(&packer), 2);
	EXPECT_EQ(packer.err, Error::ERR_NONE);
	{
		uint8_t bytes[] = {0x80, 0x01};
		EXPECT_TRUE(std::memcmp(packer_data(&packer), bytes, packer_size(&packer)) == 0);
	}

	packer_add_int(&packer, -66663);
	EXPECT_EQ(packer_size(&packer), 5);
	EXPECT_EQ(packer.err, Error::ERR_NONE);
	{
		uint8_t bytes[] = {0x80, 0x01, 0xe6, 0x91, 0x08};
		EXPECT_TRUE(std::memcmp(packer_data(&packer), bytes, packer_size(&packer)) == 0);
	}
}

TEST(Packer, Strings) {
	Packer packer;
	packer_init(&packer);

	packer_add_string(&packer, "");
	EXPECT_EQ(packer.err, Error::ERR_NONE);
	EXPECT_EQ(packer_size(&packer), 1);
	EXPECT_STREQ((char *)packer_data(&packer), "");

	packer_add_string(&packer, "foo");
	EXPECT_EQ(packer.err, Error::ERR_NONE);
	EXPECT_EQ(packer_size(&packer), 5);
	EXPECT_STREQ((char *)packer_data(&packer), "");
	EXPECT_STREQ((char *)packer_data(&packer) + 1, "foo");
}

TEST(Packer, StringsAndInts) {
	Packer packer;
	packer_init(&packer);

	packer_add_int(&packer, 2);
	EXPECT_EQ(packer.err, Error::ERR_NONE);
	EXPECT_EQ(packer_size(&packer), 1);
	{
		uint8_t bytes[] = {0x02};
		EXPECT_TRUE(std::memcmp(packer_data(&packer), bytes, packer_size(&packer)) == 0);
	}

	packer_add_string(&packer, "");
	EXPECT_EQ(packer.err, Error::ERR_NONE);
	EXPECT_EQ(packer_size(&packer), 2);
	{
		uint8_t bytes[] = {0x02, 0x00};
		EXPECT_TRUE(std::memcmp(packer_data(&packer), bytes, packer_size(&packer)) == 0);
	}

	packer_add_string(&packer, "foo");
	EXPECT_EQ(packer.err, Error::ERR_NONE);
	EXPECT_EQ(packer_size(&packer), 6);
	{
		uint8_t bytes[] = {0x02, 0x00, 'f', 'o', 'o', 0x00};
		EXPECT_TRUE(std::memcmp(packer_data(&packer), bytes, packer_size(&packer)) == 0);
	}

	packer_add_int(&packer, 6);
	EXPECT_EQ(packer.err, Error::ERR_NONE);
	{
		uint8_t bytes[] = {0x02, 0x00, 'f', 'o', 'o', 0x00, 0x06};
		EXPECT_TRUE(std::memcmp(packer_data(&packer), bytes, packer_size(&packer)) == 0);
	}
}

TEST(Unpacker, SingleByteInts) {
	uint8_t bytes[] = {0x05, 0x01, 0x02};
	Unpacker unpacker;
	unpacker_init(&unpacker, bytes, sizeof(bytes));
	EXPECT_EQ(unpacker_remaining_size(&unpacker), 3);
	EXPECT_EQ(unpacker_get_int(&unpacker), 5);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_EQ(unpacker_get_int(&unpacker), 1);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_EQ(unpacker_get_int(&unpacker), 2);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, MultiByteInts) {
	uint8_t bytes[] = {0x80, 0x01, 0x81, 0x01};
	Unpacker unpacker;
	unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(unpacker_get_int(&unpacker), 64);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);

	EXPECT_EQ(unpacker_get_int(&unpacker), 65);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, InvalidMultiByteInts) {
	uint8_t bytes[] = {0x80, 0x01, 0x81};
	Unpacker unpacker;
	unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(unpacker_get_int(&unpacker), 64);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);

	EXPECT_EQ(unpacker_get_int(&unpacker), 0);
	EXPECT_EQ(unpacker.err, Error::ERR_END_OF_BUFFER);
}

TEST(Unpacker, NegativeIntsSingleByte) {
	uint8_t bytes[] = {0x7e, 0x7f};
	Unpacker unpacker;
	unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(unpacker_get_int(&unpacker), -63);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);

	EXPECT_EQ(unpacker_get_int(&unpacker), -64);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, NegativeIntsMutliByte) {
	uint8_t bytes[] = {0xc0, 0x01};
	Unpacker unpacker;
	unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(unpacker_get_int(&unpacker), -65);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, Strings) {
	uint8_t bytes[] = {'f', 'o', 'o', 0x00, 'b', 'a', 'r', 0x00, 'A', 0x02, 0x02, 0x00, 'x'};
	Unpacker unpacker;
	unpacker_init(&unpacker, bytes, sizeof(bytes));

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
	Unpacker unpacker;
	unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_STREQ(unpacker_get_string_sanitized(&unpacker, STRING_SANITIZE_NONE), "foo");
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_STREQ(unpacker_get_string_sanitized(&unpacker, STRING_SANITIZE_CC), "ba   r");
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_STREQ(unpacker_get_string_sanitized(&unpacker, STRING_SKIP_START_WHITESPACES), "x");
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
}

TEST(Unpacker, Booleans) {
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0xcc};
	Unpacker unpacker;
	unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(unpacker_get_bool(&unpacker), false);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_EQ(unpacker_get_bool(&unpacker), true);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);
	EXPECT_EQ(unpacker_get_bool(&unpacker), false);
	EXPECT_EQ(unpacker.err, Error::ERR_INVALID_BOOL); // 2 is out of range for bool
	EXPECT_EQ(unpacker_get_bool(&unpacker), false);
	EXPECT_EQ(unpacker.err, Error::ERR_END_OF_BUFFER); // 0xcc has extension bit set for int
}

TEST(Unpacker, Raw) {
	uint8_t bytes[] = {0x7f, 0x69, 0x04, 0x20};
	Unpacker unpacker;
	unpacker_init(&unpacker, bytes, sizeof(bytes));

	unpacker_get_int(&unpacker);
	EXPECT_TRUE(std::memcmp(unpacker_get_raw(&unpacker, 3), &bytes[1], 3) == 0);
	EXPECT_EQ(unpacker.err, Error::ERR_NONE);

	EXPECT_EQ(unpacker_get_raw(&unpacker, 10), nullptr);
	EXPECT_EQ(unpacker.err, Error::ERR_END_OF_BUFFER);
}
