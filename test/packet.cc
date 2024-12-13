#include <gtest/gtest.h>

#include <ddnet_protocol/packet.h>

TEST(Packet, Header) {
	uint8_t bytes[] = {0x80, 0x02, 0x05};
	PacketHeader header = decode_packet_header(bytes);

	EXPECT_EQ(header.flags, PacketFlag::PACKET_FLAG_COMPRESSION);
	EXPECT_EQ(header.ack, 0x02);
	EXPECT_EQ(header.num_chunks, 0x05);
}
