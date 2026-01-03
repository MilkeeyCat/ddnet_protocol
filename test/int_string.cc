#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/int_string.h>

#include <gtest/gtest.h>

TEST(IntString, StrLength) {
	EXPECT_EQ(ddproto_str_length(""), 0);
	EXPECT_EQ(ddproto_str_length("a"), 1);
	EXPECT_EQ(ddproto_str_length("abc"), 3);
	EXPECT_EQ(ddproto_str_length("🌵"), 4);
}

TEST(IntString, OneIntToStr) {
	uint32_t ints[] = {0xE1808000};
	char str[32];
	DDProtoError err = ddproto_ints_to_str(ints, 1, str, sizeof(str));
	ASSERT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_STREQ(str, "a");

	ints[0] = static_cast<int>(0xE1E28000);
	err = ddproto_ints_to_str(ints, 1, str, sizeof(str));
	ASSERT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_STREQ(str, "ab");
}

TEST(IntString, ThreeIntsToStr) {
	uint32_t ints[] = {
		0xE14E3263,
		0x0104701F,
		0x10188000};
	char str[32];
	DDProtoError err = ddproto_ints_to_str(ints, 3, str, sizeof(str));
	ASSERT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_STREQ(str, "aβい🐘");
}

TEST(IntString, StrToInts) {
	uint32_t ints[4] = {};
	DDProtoError err = ddproto_str_to_ints(ints, 1, "a");
	ASSERT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_EQ(ints[0], 0xE1808000);

	err = ddproto_str_to_ints(ints, 1, "ab");
	ASSERT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_EQ(ints[0], 0xE1E28000);

	err = ddproto_str_to_ints(ints, 3, "aβい🐘");
	ASSERT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_EQ(ints[0], 0xE14E3263);
	EXPECT_EQ(ints[1], 0x0104701F);
	EXPECT_EQ(ints[2], 0x10188000);

	err = ddproto_str_to_ints(ints, 1, "aβい🐘");
	ASSERT_EQ(err, DDPROTO_ERR_OUTPUT_VAR_TOO_SMALL);
}
