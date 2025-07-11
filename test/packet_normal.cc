#include <cstring>
#include <gtest/gtest.h>

#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/packet.h>

TEST(NormalPacket, HeaderOk) {
	PacketHeader header;
	header.ack = 0;
	header.flags = PACKET_FLAG_RESEND;
	header.num_chunks = 1;
	uint8_t bytes[PACKET_HEADER_SIZE];
	Error err = encode_packet_header(&header, bytes);
	EXPECT_EQ(err, ERR_NONE);
	EXPECT_EQ(bytes[0], 0x40);
	EXPECT_EQ(bytes[1], 0x00);
	EXPECT_EQ(bytes[2], 0x01);
}

TEST(NormalPacket, HeaderAckOob) {
	PacketHeader header;
	header.ack = MAX_SEQUENCE + 2;
	header.flags = PACKET_FLAG_RESEND;
	header.num_chunks = 1;
	uint8_t bytes[PACKET_HEADER_SIZE] = {};
	Error err = encode_packet_header(&header, bytes);
	EXPECT_EQ(err, ERR_ACK_OUT_OF_BOUNDS);
	EXPECT_EQ(bytes[0], 0x00);
	EXPECT_EQ(bytes[1], 0x00);
	EXPECT_EQ(bytes[2], 0x00);
}

TEST(NormalPacket, StartInfoAndRconCmd) {
	uint8_t bytes[] = {
		0x00, 0x06, 0x02, 0x42, 0x0d, 0x05, 0x28, 0x43,
		0x68, 0x69, 0x6c, 0x6c, 0x65, 0x72, 0x44, 0x72,
		0x61, 0x67, 0x6f, 0x6e, 0x00, 0x7c, 0x2a, 0x4b,
		0x6f, 0x47, 0x2a, 0x7c, 0x00, 0x80, 0x01, 0x67,
		0x72, 0x65, 0x65, 0x6e, 0x73, 0x77, 0x61, 0x72,
		0x64, 0x00, 0x00, 0x87, 0xc5, 0x8d, 0x0e, 0x8e,
		0xab, 0x9e, 0x02, 0x40, 0x0c, 0x06, 0x23, 0x63,
		0x72, 0x61, 0x73, 0x68, 0x6d, 0x65, 0x70, 0x6c,
		0x78, 0x00, 0x3d, 0xe3, 0x94, 0x8d};

	Error err = Error::ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_NORMAL);
	EXPECT_EQ(packet.header.flags, 0);
	EXPECT_EQ(packet.header.num_chunks, 2);
	EXPECT_EQ(packet.header.ack, 6);
	EXPECT_EQ(packet.header.token, 0x3de3948d);

	EXPECT_EQ(packet.chunks.data[0].kind, CHUNK_KIND_CL_STARTINFO);
	MsgClStartInfo start_info = packet.chunks.data[0].msg.start_info;
	EXPECT_STREQ(start_info.name, "ChillerDragon");
	EXPECT_STREQ(start_info.clan, "|*KoG*|");
	EXPECT_EQ(start_info.country, 64);
	EXPECT_STREQ(start_info.skin, "greensward");
	EXPECT_EQ(start_info.color_body, 14790983);
	EXPECT_EQ(start_info.color_feet, 2345678);

	EXPECT_EQ(packet.chunks.data[1].kind, CHUNK_KIND_RCON_CMD);
	EXPECT_STREQ(packet.chunks.data[1].msg.rcon_cmd.command, "crashmeplx");

	free_packet(&packet);
}

TEST(NormalPacket, PackEmpty) {
	DDNetPacket packet = {.kind = PacketKind::PACKET_NORMAL};
	uint8_t bytes[MAX_PACKET_SIZE];
	Error err = ERR_NONE;
	size_t size = encode_packet(&packet, bytes, sizeof(bytes), &err);
	EXPECT_EQ(err, ERR_NONE);
	EXPECT_EQ(size, 7);
	uint8_t expected[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	EXPECT_TRUE(std::memcmp(bytes, expected, size) == 0);
}

TEST(NormalPacket, PackEmptyWithAck) {
	DDNetPacket packet = {.kind = PacketKind::PACKET_NORMAL};
	packet.header.ack = 2;
	uint8_t bytes[MAX_PACKET_SIZE];
	Error err = ERR_NONE;
	size_t size = encode_packet(&packet, bytes, sizeof(bytes), &err);
	EXPECT_EQ(err, ERR_NONE);
	EXPECT_EQ(size, 7);
	uint8_t expected[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
	EXPECT_TRUE(std::memcmp(bytes, expected, size) == 0);
}
