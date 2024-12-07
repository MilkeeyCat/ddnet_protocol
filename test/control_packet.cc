#include <gtest/gtest.h>

extern "C" {
#include <ddnet_protocol/control_packet.h>
#include <ddnet_protocol/packet.h>
}

TEST(ControlPacket, Keepalive) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x00, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	PacketControl *packet = (PacketControl *)decode(bytes, sizeof(bytes), &err);

	EXPECT_TRUE(packet != nullptr);
	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet->_, PacketKind::PACKET_CONTROL);
	EXPECT_EQ(packet->header.flags, PacketFlag::PACKET_FLAG_CONTROL);
	EXPECT_EQ(packet->header.num_chunks, 0);
	EXPECT_EQ(packet->header.ack, 0);
	EXPECT_EQ((packet)->kind, ControlMessageKind::CTRL_MSG_KEEPALIVE);
	EXPECT_TRUE(packet->reason == nullptr);
	EXPECT_EQ(packet->token, 0x4ec73b04);
	free(packet);
}

TEST(ControlPacket, Connect) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x01, 0x54, 0x4b, 0x45, 0x4e, 0xff, 0xff, 0xff, 0xff};
	Error err = Error::ERR_NONE;
	PacketControl *packet = (PacketControl *)decode(bytes, sizeof(bytes), &err);

	EXPECT_TRUE(packet != nullptr);
	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet->_, PacketKind::PACKET_CONTROL);
	EXPECT_EQ((packet)->kind, ControlMessageKind::CTRL_MSG_CONNECT);
	EXPECT_TRUE(packet->reason == nullptr);
	EXPECT_EQ(packet->token, 0xffffffff);
	free(packet);
}

TEST(ControlPacket, ConnectAccept) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x02, 0x54, 0x4b, 0x45, 0x4e, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	PacketControl *packet = (PacketControl *)decode(bytes, sizeof(bytes), &err);

	EXPECT_TRUE(packet != nullptr);
	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet->_, PacketKind::PACKET_CONTROL);
	EXPECT_EQ((packet)->kind, ControlMessageKind::CTRL_MSG_CONNECTACCEPT);
	EXPECT_TRUE(packet->reason == nullptr);
	EXPECT_EQ(packet->token, 0x4ec73b04);
	free(packet);
}

TEST(ControlPacket, Accept) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x03, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	PacketControl *packet = (PacketControl *)decode(bytes, sizeof(bytes), &err);

	EXPECT_TRUE(packet != nullptr);
	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet->_, PacketKind::PACKET_CONTROL);
	EXPECT_EQ((packet)->kind, ControlMessageKind::CTRL_MSG_ACCEPT);
	EXPECT_TRUE(packet->reason == nullptr);
	EXPECT_EQ(packet->token, 0x4ec73b04);
	free(packet);
}

TEST(ControlPacket, Close) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x04, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	PacketControl *packet = (PacketControl *)decode(bytes, sizeof(bytes), &err);

	EXPECT_TRUE(packet != nullptr);
	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet->_, PacketKind::PACKET_CONTROL);
	EXPECT_EQ((packet)->kind, ControlMessageKind::CTRL_MSG_CLOSE);
	EXPECT_TRUE(packet->reason == nullptr);
	EXPECT_EQ(packet->token, 0x4ec73b04);
	free(packet);
}

TEST(ControlPacket, CloseWithReason) {
	uint8_t bytes[] = {0x10, 0x00, 0x00, 0x04, 0x74, 0x6f, 0x6f, 0x20, 0x62, 0x61, 0x64, 0x00, 0x4e, 0xc7, 0x3b, 0x04};
	Error err = Error::ERR_NONE;
	PacketControl *packet = (PacketControl *)decode(bytes, sizeof(bytes), &err);

	EXPECT_TRUE(packet != nullptr);
	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet->_, PacketKind::PACKET_CONTROL);
	EXPECT_EQ((packet)->kind, ControlMessageKind::CTRL_MSG_CLOSE);
	EXPECT_STREQ(packet->reason, "too bad");
	EXPECT_EQ(packet->token, 0x4ec73b04);
	free(packet);
}
