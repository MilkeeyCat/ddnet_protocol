#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/packer.h>

#include <cstring>
#include <gtest/gtest.h>

TEST(MessagePacker, Rcon) {
	DDProtoPacker packer;
	ddproto_packer_init_msg(&packer, DDPROTO_MSG_KIND_RCON_CMD);
	ddproto_packer_add_string(&packer, "say hello");
	uint8_t bytes[] = {
		0x23, 0x73, 0x61, 0x79, 0x20, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x00};
	EXPECT_TRUE(std::memcmp(ddproto_packer_data(&packer), bytes, ddproto_packer_size(&packer)) == 0);
}

TEST(Packer, SingleByteInts) {
	DDProtoPacker packer;
	ddproto_packer_init(&packer);

	EXPECT_EQ(ddproto_packer_remaining_size(&packer), DDPROTO_PACKER_BUFFER_SIZE);
	EXPECT_EQ(ddproto_packer_size(&packer), 0);

	ddproto_packer_add_int(&packer, 0);
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);

	ddproto_packer_add_int(&packer, 1);
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);

	uint8_t bytes[] = {0x00, 0x01};
	EXPECT_TRUE(std::memcmp(ddproto_packer_data(&packer), bytes, ddproto_packer_size(&packer)) == 0);

	EXPECT_EQ(ddproto_packer_size(&packer), 2);
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);
}

TEST(Packer, MultiByteInts) {
	DDProtoPacker packer;
	ddproto_packer_init(&packer);

	EXPECT_EQ(ddproto_packer_remaining_size(&packer), DDPROTO_PACKER_BUFFER_SIZE);
	EXPECT_EQ(ddproto_packer_size(&packer), 0);

	ddproto_packer_add_int(&packer, 64);
	EXPECT_EQ(ddproto_packer_size(&packer), 2);
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);
	{
		uint8_t bytes[] = {0x80, 0x01};
		EXPECT_TRUE(std::memcmp(ddproto_packer_data(&packer), bytes, ddproto_packer_size(&packer)) == 0);
	}

	ddproto_packer_add_int(&packer, -66663);
	EXPECT_EQ(ddproto_packer_size(&packer), 5);
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);
	{
		uint8_t bytes[] = {0x80, 0x01, 0xe6, 0x91, 0x08};
		EXPECT_TRUE(std::memcmp(ddproto_packer_data(&packer), bytes, ddproto_packer_size(&packer)) == 0);
	}
}

TEST(Packer, Strings) {
	DDProtoPacker packer;
	ddproto_packer_init(&packer);

	ddproto_packer_add_string(&packer, "");
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(ddproto_packer_size(&packer), 1);
	EXPECT_STREQ((char *)ddproto_packer_data(&packer), "");

	ddproto_packer_add_string(&packer, "foo");
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(ddproto_packer_size(&packer), 5);
	EXPECT_STREQ((char *)ddproto_packer_data(&packer), "");
	EXPECT_STREQ((char *)ddproto_packer_data(&packer) + 1, "foo");
}

TEST(Packer, StringsAndInts) {
	DDProtoPacker packer;
	ddproto_packer_init(&packer);

	ddproto_packer_add_int(&packer, 2);
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(ddproto_packer_size(&packer), 1);
	{
		uint8_t bytes[] = {0x02};
		EXPECT_TRUE(std::memcmp(ddproto_packer_data(&packer), bytes, ddproto_packer_size(&packer)) == 0);
	}

	ddproto_packer_add_string(&packer, "");
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(ddproto_packer_size(&packer), 2);
	{
		uint8_t bytes[] = {0x02, 0x00};
		EXPECT_TRUE(std::memcmp(ddproto_packer_data(&packer), bytes, ddproto_packer_size(&packer)) == 0);
	}

	ddproto_packer_add_string(&packer, "foo");
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(ddproto_packer_size(&packer), 6);
	{
		uint8_t bytes[] = {0x02, 0x00, 'f', 'o', 'o', 0x00};
		EXPECT_TRUE(std::memcmp(ddproto_packer_data(&packer), bytes, ddproto_packer_size(&packer)) == 0);
	}

	ddproto_packer_add_int(&packer, 6);
	EXPECT_EQ(packer.err, DDProtoError::DDPROTO_ERR_NONE);
	{
		uint8_t bytes[] = {0x02, 0x00, 'f', 'o', 'o', 0x00, 0x06};
		EXPECT_TRUE(std::memcmp(ddproto_packer_data(&packer), bytes, ddproto_packer_size(&packer)) == 0);
	}
}

TEST(Unpacker, SingleByteInts) {
	uint8_t bytes[] = {0x05, 0x01, 0x02};
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
	EXPECT_EQ(ddproto_unpacker_remaining_size(&unpacker), 3);
	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), 5);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), 1);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), 2);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
}

TEST(Unpacker, MultiByteInts) {
	uint8_t bytes[] = {0x80, 0x01, 0x81, 0x01};
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), 64);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);

	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), 65);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
}

TEST(Unpacker, InvalidMultiByteInts) {
	uint8_t bytes[] = {0x80, 0x01, 0x81};
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), 64);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);

	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), 0);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_END_OF_BUFFER);
}

TEST(Unpacker, NegativeIntsSingleByte) {
	uint8_t bytes[] = {0x7e, 0x7f};
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), -63);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);

	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), -64);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
}

TEST(Unpacker, NegativeIntsMultiByte) {
	uint8_t bytes[] = {0xc0, 0x01};
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(ddproto_unpacker_get_int(&unpacker), -65);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
}

TEST(Unpacker, Strings) {
	uint8_t bytes[] = {'f', 'o', 'o', 0x00, 'b', 'a', 'r', 0x00, 'A', 0x02, 0x02, 0x00, 'x'};
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_STREQ(ddproto_unpacker_get_string(&unpacker), "foo");
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_STREQ(ddproto_unpacker_get_string(&unpacker), "bar");
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_STREQ(ddproto_unpacker_get_string(&unpacker), "A  "); // expect sanitize by default
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_STREQ(ddproto_unpacker_get_string(&unpacker), "");
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_STR_UNEXPECTED_EOF);
}

TEST(Unpacker, StringsSanitized) {
	uint8_t bytes[] = {'f', 'o', 'o', 0x00, 'b', 'a', 0x02, 0x03, 0x03, 'r', 0x00, ' ', ' ', 'x', 0x00};
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_STREQ(ddproto_unpacker_get_string_sanitized(&unpacker, DDPROTO_STRING_SANITIZE_NONE), "foo");
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_STREQ(ddproto_unpacker_get_string_sanitized(&unpacker, DDPROTO_STRING_SANITIZE_CC), "ba   r");
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_STREQ(ddproto_unpacker_get_string_sanitized(&unpacker, DDPROTO_STRING_SKIP_START_WHITESPACES), "x");
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
}

TEST(Unpacker, Booleans) {
	uint8_t bytes[] = {0x00, 0x01, 0x02, 0xcc};
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

	EXPECT_EQ(ddproto_unpacker_get_bool(&unpacker), false);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(ddproto_unpacker_get_bool(&unpacker), true);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(ddproto_unpacker_get_bool(&unpacker), false);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_INVALID_BOOL); // 2 is out of range for bool
	EXPECT_EQ(ddproto_unpacker_get_bool(&unpacker), false);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_END_OF_BUFFER); // 0xcc has extension bit set for int
}

TEST(Unpacker, Raw) {
	uint8_t bytes[] = {0x7f, 0x69, 0x04, 0x20};
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

	ddproto_unpacker_get_int(&unpacker);
	EXPECT_TRUE(std::memcmp(ddproto_unpacker_get_raw(&unpacker, 3), &bytes[1], 3) == 0);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_NONE);

	EXPECT_EQ(ddproto_unpacker_get_raw(&unpacker, 10), nullptr);
	EXPECT_EQ(unpacker.err, DDProtoError::DDPROTO_ERR_END_OF_BUFFER);
}
