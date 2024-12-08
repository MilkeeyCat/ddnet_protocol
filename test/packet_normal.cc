#include <gtest/gtest.h>

extern "C" {
#include <ddnet_protocol/packet.h>
#include <ddnet_protocol/packet_normal.h>
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
	PacketNormal *packet = (PacketNormal *)decode(bytes, sizeof(bytes), &err);
	EXPECT_TRUE(packet != nullptr);
	EXPECT_EQ(err, Error::ERR_NONE);
	EXPECT_EQ(packet->_, PacketKind::PACKET_NORMAL);
	EXPECT_EQ(packet->header.flags, 0);
	EXPECT_EQ(packet->header.num_chunks, 2);
	EXPECT_EQ(packet->header.ack, 6);
	EXPECT_EQ(packet->token, 0x3de3948d);

	EXPECT_EQ(packet->chunks[0].kind, CHUNK_KIND_CL_STARTINFO);
	EXPECT_EQ(packet->chunks[1].kind, CHUNK_KIND_RCON_CMD);
	EXPECT_STREQ(packet->chunks[1].msg.rcon_cmd.command, "crashmeplx");

	free(packet);
}
