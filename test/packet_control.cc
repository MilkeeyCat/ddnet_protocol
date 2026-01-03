#include <ddnet_protocol/control_message.h>
#include <ddnet_protocol/packet.h>

#include <cstring>
#include <gtest/gtest.h>

TEST(ControlPacket, Keepalive) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x00, 0x4e, 0xc7, 0x3b, 0x04};
	DDProtoError err = DDProtoError::DDPROTO_ERR_NONE;
	DDProtoPacket packet = ddproto_decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(packet.kind, DDProtoPacketKind::DDPROTO_PACKET_CONTROL);
	EXPECT_EQ(packet.header.flags, DDProtoPacketFlag::DDPROTO_PACKET_FLAG_CONTROL);
	EXPECT_EQ(packet.header.num_chunks, 0);
	EXPECT_EQ(packet.header.ack, 0);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_EQ(packet.control.kind, DDProtoControlMessageKind::DDPROTO_CTRL_MSG_KEEPALIVE);
	EXPECT_TRUE(packet.control.reason == nullptr);
	ddproto_free_packet(&packet);
}

TEST(ControlPacket, Connect) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x01, 0x54, 0x4b, 0x45, 0x4e, 0xff, 0xff, 0xff, 0xff};
	DDProtoError err = DDProtoError::DDPROTO_ERR_NONE;
	DDProtoPacket packet = ddproto_decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(packet.kind, DDProtoPacketKind::DDPROTO_PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, DDProtoControlMessageKind::DDPROTO_CTRL_MSG_CONNECT);
	EXPECT_EQ(packet.header.token, 0xffffffff);
	EXPECT_TRUE(packet.control.reason == nullptr);
	ddproto_free_packet(&packet);
}

TEST(ControlPacket, ConnectAccept) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x02, 0x54, 0x4b, 0x45, 0x4e, 0x4e, 0xc7, 0x3b, 0x04};
	DDProtoError err = DDProtoError::DDPROTO_ERR_NONE;
	DDProtoPacket packet = ddproto_decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(packet.kind, DDProtoPacketKind::DDPROTO_PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, DDProtoControlMessageKind::DDPROTO_CTRL_MSG_CONNECTACCEPT);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_TRUE(packet.control.reason == nullptr);
	ddproto_free_packet(&packet);
}

TEST(ControlPacket, Accept) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x03, 0x4e, 0xc7, 0x3b, 0x04};
	DDProtoError err = DDProtoError::DDPROTO_ERR_NONE;
	DDProtoPacket packet = ddproto_decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(packet.kind, DDProtoPacketKind::DDPROTO_PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, DDProtoControlMessageKind::DDPROTO_CTRL_MSG_ACCEPT);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_TRUE(packet.control.reason == nullptr);
	ddproto_free_packet(&packet);
}

TEST(ControlPacket, Close) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x04, 0x4e, 0xc7, 0x3b, 0x04};
	DDProtoError err = DDProtoError::DDPROTO_ERR_NONE;
	DDProtoPacket packet = ddproto_decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(packet.kind, DDProtoPacketKind::DDPROTO_PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, DDProtoControlMessageKind::DDPROTO_CTRL_MSG_CLOSE);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_TRUE(packet.control.reason == nullptr);
	ddproto_free_packet(&packet);
}

TEST(ControlPacket, CloseWithReason) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x04, 0x74, 0x6f, 0x6f, 0x20, 0x62, 0x61, 0x64, 0x00, 0x4e, 0xc7, 0x3b, 0x04};
	DDProtoError err = DDProtoError::DDPROTO_ERR_NONE;
	DDProtoPacket packet = ddproto_decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, DDProtoError::DDPROTO_ERR_NONE);
	EXPECT_EQ(packet.kind, DDProtoPacketKind::DDPROTO_PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, DDProtoControlMessageKind::DDPROTO_CTRL_MSG_CLOSE);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_STREQ(packet.control.reason, "too bad");
	ddproto_free_packet(&packet);
}

TEST(ControlPacket, EncodeClose) {
	DDProtoPacket packet = {
		.kind = DDProtoPacketKind::DDPROTO_PACKET_CONTROL,
		.header = {
			.flags = DDPROTO_PACKET_FLAG_CONTROL,
			.token = 0x4ec73b04},
		.control = {.kind = DDPROTO_CTRL_MSG_CLOSE}};
	uint8_t bytes[DDPROTO_MAX_PACKET_SIZE];
	DDProtoError err = DDPROTO_ERR_NONE;
	size_t size = ddproto_encode_packet(&packet, bytes, sizeof(bytes), &err);
	EXPECT_EQ(err, DDPROTO_ERR_NONE);
	uint8_t expected[] = {0x10, 0x00, 0x00, 0x04, 0x4e, 0xc7, 0x3b, 0x04};
	EXPECT_EQ(size, sizeof(expected));
	EXPECT_TRUE(std::memcmp(bytes, expected, size) == 0);
}

TEST(ControlPacket, EncodeCloseWithReason) {
	DDProtoPacket packet = {
		.kind = DDProtoPacketKind::DDPROTO_PACKET_CONTROL,
		.header = {
			.flags = DDPROTO_PACKET_FLAG_CONTROL,
			.token = 0x4ec73b04},
		.control = {.kind = DDPROTO_CTRL_MSG_CLOSE, .reason = "too bad"}};
	uint8_t bytes[DDPROTO_MAX_PACKET_SIZE];
	DDProtoError err = DDPROTO_ERR_NONE;
	size_t size = ddproto_encode_packet(&packet, bytes, sizeof(bytes), &err);
	EXPECT_EQ(err, DDPROTO_ERR_NONE);
	uint8_t expected[] = {0x10, 0x00, 0x00, 0x04, 0x74, 0x6f, 0x6f, 0x20, 0x62, 0x61, 0x64, 0x00, 0x4e, 0xc7, 0x3b, 0x04};
	EXPECT_EQ(size, sizeof(expected));
	EXPECT_TRUE(std::memcmp(bytes, expected, size) == 0);
}

TEST(ControlPacket, EncodeConnect) {
	DDProtoPacket packet = {
		.kind = DDProtoPacketKind::DDPROTO_PACKET_CONTROL,
		.header = {
			.flags = DDPROTO_PACKET_FLAG_CONTROL,
			.token = 0xffffffff},
		.control = {.kind = DDPROTO_CTRL_MSG_CONNECT}};
	uint8_t bytes[DDPROTO_MAX_PACKET_SIZE];
	DDProtoError err = DDPROTO_ERR_NONE;
	size_t size = ddproto_encode_packet(&packet, bytes, sizeof(bytes), &err);
	EXPECT_EQ(err, DDPROTO_ERR_NONE);
	uint8_t expected[] = {0x10, 0x00, 0x00, 0x01, 0x54, 0x4b, 0x45, 0x4e, 0xff, 0xff, 0xff, 0xff};
	EXPECT_EQ(size, sizeof(expected));
	EXPECT_TRUE(std::memcmp(bytes, expected, size) == 0);
}
