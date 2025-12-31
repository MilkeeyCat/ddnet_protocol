#include <gtest/gtest.h>

#include <ddnet_protocol/packet.h>

TEST(PacketHeader, NormalNoFlags) {
	uint8_t bytes[] = {0x00, 0x06, 0x02};
	DDProtoPacketHeader header = ddproto_decode_packet_header(bytes);
	EXPECT_EQ(header.flags, 0);
	EXPECT_EQ(header.ack, 6);
	EXPECT_EQ(header.num_chunks, 2);
}
