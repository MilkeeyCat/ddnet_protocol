#include <gtest/gtest.h>

#include <ddnet_protocol/control_message.h>
#include <ddnet_protocol/packet.h>

TEST(ControlPacket, Keepalive) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x00, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_CONTROL);
	EXPECT_EQ(packet.header.flags, PacketFlag::PACKET_FLAG_CONTROL);
	EXPECT_EQ(packet.header.num_chunks, 0);
	EXPECT_EQ(packet.header.ack, 0);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_EQ(packet.control.kind, ControlMessageKind::CTRL_MSG_KEEPALIVE);
	EXPECT_TRUE(packet.control.reason == nullptr);
	free_packet(&packet);
}

TEST(ControlPacket, Connect) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x01, 0x54, 0x4b, 0x45, 0x4e, 0xff, 0xff, 0xff, 0xff};
	Error err = Error::ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, ControlMessageKind::CTRL_MSG_CONNECT);
	EXPECT_EQ(packet.header.token, 0xffffffff);
	EXPECT_TRUE(packet.control.reason == nullptr);
	free_packet(&packet);
}

TEST(ControlPacket, ConnectAccept) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x02, 0x54, 0x4b, 0x45, 0x4e, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, ControlMessageKind::CTRL_MSG_CONNECTACCEPT);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_TRUE(packet.control.reason == nullptr);
	free_packet(&packet);
}

TEST(ControlPacket, Accept) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x03, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, ControlMessageKind::CTRL_MSG_ACCEPT);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_TRUE(packet.control.reason == nullptr);
	free_packet(&packet);
}

TEST(ControlPacket, Close) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x04, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, ControlMessageKind::CTRL_MSG_CLOSE);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_TRUE(packet.control.reason == nullptr);
	free_packet(&packet);
}

TEST(ControlPacket, CloseWithReason) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x04, 0x74, 0x6f, 0x6f, 0x20, 0x62, 0x61, 0x64, 0x00, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_CONTROL);
	EXPECT_EQ(packet.control.kind, ControlMessageKind::CTRL_MSG_CLOSE);
	EXPECT_EQ(packet.header.token, 0x4ec73b04);
	EXPECT_STREQ(packet.control.reason, "too bad");
	free_packet(&packet);
}
