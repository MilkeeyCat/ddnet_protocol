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

TEST(NormalPacket, Info) {
	uint8_t bytes[] = {
		0x00, 0x00, 0x01, 0x41, 0x07, 0x03, 0x03, 0x30,
		0x2e, 0x36, 0x20, 0x36, 0x32, 0x36, 0x66, 0x63,
		0x65, 0x39, 0x61, 0x37, 0x37, 0x38, 0x64, 0x66,
		0x34, 0x64, 0x34, 0x00, 0x00, 0x3d, 0xe3, 0x94,
		0x8d};

	Error err = Error::ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_NORMAL);
	EXPECT_EQ(packet.header.flags, 0);
	EXPECT_EQ(packet.header.num_chunks, 1);
	EXPECT_EQ(packet.header.ack, 0);
	EXPECT_EQ(packet.header.token, 0x3de3948d);

	EXPECT_EQ(packet.chunks.data[0].payload.kind, DDNET_MSG_KIND_INFO);
	EXPECT_EQ(packet.chunks.data[0].header.sequence, 3);
	MsgInfo info = packet.chunks.data[0].payload.msg.info;
	EXPECT_STREQ(info.version, "0.6 626fce9a778df4d4");
	EXPECT_STREQ(info.password, "");

	free_packet(&packet);
}

TEST(NormalPacket, PackInfo) {
	DDNetPacket packet = {
		.kind = PACKET_NORMAL,
		.header = {.num_chunks = 1, .token = 0x3de3948d}};
	packet.chunks.data = (Chunk *)malloc(sizeof(Chunk) * packet.header.num_chunks);
	packet.chunks.len = packet.header.num_chunks;

	packet.chunks.data[0].header.flags = CHUNK_FLAG_VITAL;
	packet.chunks.data[0].header.sequence = 3;
	packet.chunks.data[0].payload.kind = DDNET_MSG_KIND_INFO;
	packet.chunks.data[0].payload.msg.info.password = "";
	packet.chunks.data[0].payload.msg.info.version = "short version";

	fill_chunk_header(&packet.chunks.data[0]);
	EXPECT_EQ(packet.chunks.data[0].header.size, 16);

	packet.chunks.data[0].payload.msg.info.version = "0.6 626fce9a778df4d4";
	fill_chunk_header(&packet.chunks.data[0]);
	EXPECT_EQ(packet.chunks.data[0].header.size, 23);

	uint8_t buf[MAX_PACKET_SIZE];
	Error err = ERR_NONE;
	size_t len = encode_packet(&packet, buf, sizeof(buf), &err);
	EXPECT_EQ(err, ERR_NONE);
	uint8_t expected[] = {
		0x00, 0x00, 0x01, 0x41, 0x07, 0x03, 0x03, 0x30,
		0x2e, 0x36, 0x20, 0x36, 0x32, 0x36, 0x66, 0x63,
		0x65, 0x39, 0x61, 0x37, 0x37, 0x38, 0x64, 0x66,
		0x34, 0x64, 0x34, 0x00, 0x00, 0x3d, 0xe3, 0x94,
		0x8d};
	EXPECT_EQ(len, sizeof(expected));
	EXPECT_TRUE(std::memcmp(buf, expected, len) == 0);
	free_packet(&packet);
}

TEST(NormalPacket, StartInfoAndRconCmdAndErrors) {
	uint8_t bytes[] = {
		0x00, 0x06, 0x02, 0x42, 0x0d, 0x05, 0x28, 0x43,
		0x68, 0x69, 0x6c, 0x6c, 0x65, 0x72, 0x44, 0x72,
		0x61, 0x67, 0x6f, 0x6e, 0x00, 0x7c, 0x2a, 0x4b,
		0x6f, 0x47, 0x2a, 0x7c, 0x00, 0x80, 0x01, 0x67,
		0x72, 0x65, 0x65, 0x6e, 0x73, 0x77, 0x61, 0x72,
		0x64, 0x00, 0x00, 0x87, 0xc5, 0x8d, 0x0e, 0x8e,
		0xab, 0x9e, 0x02, 0x40, 0x0c, 0x06, 0x23, 0x63,
		0x72, 0x61, 0x73, 0x68, 0x6d, 0x65, 0x70, 0x6c,
		0x78, 0x00, 0x3d, 0xe3, 0x94}; // 1 byte missing at the end from the security token

	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), nullptr);
	free_packet(&packet);
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

	EXPECT_EQ(packet.chunks.data[0].payload.kind, DDNET_MSG_KIND_CL_STARTINFO);
	MsgClStartInfo start_info = packet.chunks.data[0].payload.msg.start_info;
	EXPECT_STREQ(start_info.name, "ChillerDragon");
	EXPECT_STREQ(start_info.clan, "|*KoG*|");
	EXPECT_EQ(start_info.country, 64);
	EXPECT_STREQ(start_info.skin, "greensward");
	EXPECT_EQ(start_info.color_body, 14790983);
	EXPECT_EQ(start_info.color_feet, 2345678);

	EXPECT_EQ(packet.chunks.data[1].payload.kind, DDNET_MSG_KIND_RCON_CMD);
	EXPECT_STREQ(packet.chunks.data[1].payload.msg.rcon_cmd.command, "crashmeplx");

	free_packet(&packet);
}

TEST(NormalPacket, GameMotdAndSysConReady) {
	uint8_t bytes[] = {
		0x00, 0x04, 0x02, 0x40, 0x02, 0x05, 0x02, 0x00,
		0x40, 0x01, 0x06, 0x09, 0xb6, 0xea, 0x17, 0x83};

	Error err = ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);
	EXPECT_EQ(err, ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_NORMAL);
	EXPECT_EQ(packet.header.flags, 0);
	EXPECT_EQ(packet.header.num_chunks, 2);
	EXPECT_EQ(packet.header.ack, 4);
	EXPECT_EQ(packet.header.token, 0xb6ea1783);

	// TODO: check msg 0 once motd is supported

	EXPECT_EQ(packet.chunks.data[1].payload.kind, DDNET_MSG_KIND_CON_READY);

	free_packet(&packet);
}

TEST(NormalPacket, UnknownFakeMessage) {
	// sent from C++ version using the following code
	//
	// CMsgPacker FakeMsg(99, true);
	// FakeMsg.AddString("ABC");
	// SendMsg(Conn, &FakeMsg, MSGFLAG_VITAL | MSGFLAG_FLUSH);

	//                                          two byte message id 99 and system flag
	//                                          |
	//                                          |                               ddnet security token
	//                                          |                    null byte   |
	uint8_t bytes[] = {//                       vvvvvvvvvv                 vvvv  vvvvvvvvvvvvvvvvvvvvvv
		0x00, 0x00, 0x01, 0x40, 0x06, 0x03, 0x87, 0x03, 'A', 'B', 'C', 0x00, 0x8b, 0x6c, 0xdb, 0xc3};
	//      ^^^^^^^^^^^^^^^^  ^^^^^^^^^^^^^^^^
	//      packet header  |  vital chunk header
	//          num_chunks=1  size=6 sequence=3

	Error err = ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);
	EXPECT_EQ(err, ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_NORMAL);
	EXPECT_EQ(packet.header.flags, 0);
	EXPECT_EQ(packet.header.num_chunks, 1);
	EXPECT_EQ(packet.header.ack, 0);
	EXPECT_EQ(packet.header.token, 0x8b6cdbc3);
	EXPECT_EQ(packet.chunks.len, 1);
	EXPECT_EQ(packet.chunks.data[0].payload.kind, DDNET_MSG_KIND_UNKNOWN);
	const size_t msg_id_size = 2;
	const size_t str_size = strlen("ABC") + 1;
	EXPECT_EQ(packet.chunks.data[0].payload.msg.unknown.len, msg_id_size + str_size);
	EXPECT_EQ(packet.chunks.data[0].payload.msg.unknown.buf[0], 0x87); // two byte
	EXPECT_EQ(packet.chunks.data[0].payload.msg.unknown.buf[1], 0x03); // message id and system flag
	EXPECT_EQ(packet.chunks.data[0].payload.msg.unknown.buf[2], 'A');
	EXPECT_EQ(packet.chunks.data[0].payload.msg.unknown.buf[3], 'B');
	EXPECT_EQ(packet.chunks.data[0].payload.msg.unknown.buf[4], 'C');
	EXPECT_EQ(packet.chunks.data[0].payload.msg.unknown.buf[5], 0x00); // null term
	EXPECT_EQ(packet.chunks.data[0].header.flags, CHUNK_FLAG_VITAL);
	EXPECT_EQ(packet.chunks.data[0].header.sequence, 3);
	EXPECT_EQ(packet.chunks.data[0].header.size, 6);

	uint8_t repack[2048];
	size_t repack_size = encode_packet(&packet, repack, sizeof(repack), &err);
	EXPECT_EQ(err, ERR_NONE);
	EXPECT_EQ(repack_size, sizeof(bytes));
	EXPECT_TRUE(std::memcmp(bytes, repack, repack_size) == 0);
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

TEST(NormalPacket, EmptyResendRequest) {
	uint8_t bytes[] = {0x40, 0x00, 0x00, 0x3d, 0xe3, 0x94, 0x8d};

	Error err = Error::ERR_NONE;
	DDNetPacket packet = decode_packet(bytes, sizeof(bytes), &err);

	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet.kind, PacketKind::PACKET_NORMAL);
	EXPECT_NE(packet.header.flags & PACKET_FLAG_RESEND, 0);
	EXPECT_EQ(packet.header.num_chunks, 0);
	EXPECT_EQ(packet.header.ack, 0);
	EXPECT_EQ(packet.header.token, 0x3de3948d);

	free_packet(&packet);
}
