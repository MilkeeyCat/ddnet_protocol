#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/msg_system.h>
#include <ddnet_protocol/packer.h>
#include <ddnet_protocol/packet.h>
#include <ddnet_protocol/session.h>
#include <ddnet_protocol/snap_items.h>
#include <ddnet_protocol/snapshot.h>

#include <gtest/gtest.h>

TEST(SnapSinglePacket, DeltaWithRemovedKeys) {
	// this is packet 400 from here:
	// https://github.com/ChillerDragon/teeworlds-traffic-dumps/blob/7ed8d9552bb7649d286ff3d7fd365d2d4220f330/dumps/064_ddnet_join_chat_walk_disconnect/tshark_libtw2.txt#L21
	// this is a delta to a previous snap we do not have so the values look a
	// bit funny. It also contains 3 removed keys which is the key part of this
	// test
	uint8_t bytes[] = {
		0x80, 0x0c, 0x01, 0x9e, 0xdc, 0x1d, 0x2f, 0x05,
		0x10, 0x3f, 0x50, 0x0d, 0x3f, 0xa9, 0x41, 0x3f,
		0xd1, 0xc2, 0xa2, 0x7b, 0x80, 0xd4, 0x4b, 0xba,
		0xf3, 0x23, 0x90, 0x7a, 0x49, 0x77, 0xb4, 0x80,
		0xd4, 0x4b, 0xba, 0xd3, 0x7f, 0x37, 0x66, 0xc8,
		0x2b, 0xfd, 0xc7, 0xdf, 0xbd, 0x92, 0x16, 0xfd,
		0xff, 0xf3, 0x2e, 0x3b, 0x4d, 0x99, 0x54, 0xad,
		0xb8, 0x01};

	DDNetError err = DDNetError::DDNET_ERR_NONE;
	DDNetPacket packet = ddnet_decode_packet(bytes, sizeof(bytes), &err);

	ASSERT_EQ(err, DDNetError::DDNET_ERR_NONE);
	EXPECT_EQ(packet.kind, DDNetPacketKind::DDNET_PACKET_NORMAL);
	EXPECT_EQ(packet.header.flags, DDNET_PACKET_FLAG_COMPRESSION);
	EXPECT_EQ(packet.header.num_chunks, 1);
	EXPECT_EQ(packet.header.ack, 12);
	EXPECT_EQ(packet.header.token, 0x99988aeb);

	EXPECT_EQ(packet.chunks.data[0].payload.kind, DDNET_MSG_KIND_SNAPSINGLE);
	EXPECT_EQ(packet.chunks.data[0].header.flags, 0);

	DDNetMsgSnapSingle snap_single = packet.chunks.data[0].payload.msg.snap_single;
	EXPECT_EQ(snap_single.game_tick, 1852);
	EXPECT_EQ(snap_single.delta_tick, 24);
	EXPECT_EQ(snap_single.crc, 217322961);
	EXPECT_EQ(snap_single.part_size, 65);

	DDNetSnapshot snap = snap_single.snapshot;
	EXPECT_EQ(snap.removed_keys.len, 3);
	ASSERT_NE(snap.removed_keys.data, nullptr);
	EXPECT_EQ(snap.removed_keys.data[0], 2147090437);
	EXPECT_EQ(snap.removed_keys.data[1], 2147090445);
	EXPECT_EQ(snap.removed_keys.data[2], 2147090444);

	ASSERT_EQ(snap.items.len, 3);

	DDNetSnapItem *item = &snap.items.data[0];
	ASSERT_EQ(item->kind, DDNET_ITEM_KIND_PLAYER_INFO);
	DDNetObjPlayerInfo *player_info = &item->item.player_info;
	ASSERT_EQ(player_info->type_id, 10);
	EXPECT_EQ(player_info->id, 0);
	EXPECT_EQ(player_info->local, 0);
	EXPECT_EQ(player_info->client_id, 0);
	EXPECT_EQ(player_info->score, 0);
	EXPECT_EQ(player_info->latency, 7);

	// these values are all low because they are delta values that need to be
	// added to the previous snap data to get the real values
	item = &snap.items.data[1];
	ASSERT_EQ(item->kind, DDNET_ITEM_KIND_CHARACTER);
	DDNetObjCharacter *character = &item->item.character;
	ASSERT_EQ(character->type_id, 9);
	EXPECT_EQ(character->id, 0);
	EXPECT_EQ(character->core.tick, 24);
	EXPECT_EQ(character->core.x, 0);
	EXPECT_EQ(character->core.y, 0);
	EXPECT_EQ(character->core.vel_x, 0);
	EXPECT_EQ(character->core.vel_y, 0);
	EXPECT_EQ(character->core.angle, -1);
	EXPECT_EQ(character->core.direction, 0);
	EXPECT_EQ(character->core.jumped, 0);
	EXPECT_EQ(character->core.hooked_player, 0);
	EXPECT_EQ(character->core.hook_state, 0);
	EXPECT_EQ(character->core.hook_tick, 0);
	EXPECT_EQ(character->core.hook_x, 0);
	EXPECT_EQ(character->core.hook_y, 0);
	EXPECT_EQ(character->core.hook_dx, 0);
	EXPECT_EQ(character->core.hook_dy, 0);
	EXPECT_EQ(character->player_flags, 1);
	EXPECT_EQ(character->health, 0);
	EXPECT_EQ(character->armor, 0);
	EXPECT_EQ(character->ammo_count, 0);
	EXPECT_EQ(character->weapon, 0);
	EXPECT_EQ(character->emote, 0);
	EXPECT_EQ(character->attack_tick, 0);

	item = &snap.items.data[2];
	ASSERT_EQ(item->kind, DDNET_ITEM_KIND_UNKNOWN);
	DDNetObjUnknown *unknown = &item->item.unknown;
	ASSERT_EQ(unknown->type_id, 32763);
	EXPECT_EQ(unknown->id, 0);
	ASSERT_EQ(unknown->size, 11);
	ASSERT_EQ(unknown->data_len, 11);
	DDNetUnpacker unpacker = {};
	ddnet_unpacker_init(&unpacker, unknown->data, unknown->data_len);
	int32_t unknown_field0 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field1 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field2 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field3 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field4 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field5 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field6 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field7 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field8 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field9 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	int32_t unknown_field10 = ddnet_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDNET_ERR_NONE);
	EXPECT_EQ(unknown_field0, 0);
	EXPECT_EQ(unknown_field1, 0);
	EXPECT_EQ(unknown_field2, 0);
	EXPECT_EQ(unknown_field3, 0);
	EXPECT_EQ(unknown_field4, 0);
	EXPECT_EQ(unknown_field5, 0);
	EXPECT_EQ(unknown_field6, 0);
	EXPECT_EQ(unknown_field7, 0);
	EXPECT_EQ(unknown_field8, 0);
	EXPECT_EQ(unknown_field9, -2);
	EXPECT_EQ(unknown_field10, 0);
	ddnet_unpacker_get_raw(&unpacker, 1);
	EXPECT_EQ(unpacker.err, DDNET_ERR_END_OF_BUFFER);

	ddnet_free_packet(&packet);
}
