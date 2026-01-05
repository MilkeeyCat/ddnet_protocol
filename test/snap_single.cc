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

TEST(SnapSinglePacket, FirstFullSnap) {
	// this is packet 21 from here:
	// https://github.com/ChillerDragon/teeworlds-traffic-dumps/blob/7ed8d9552bb7649d286ff3d7fd365d2d4220f330/dumps/064_ddnet_join_chat_walk_disconnect/tshark_libtw2.txt#L21
	// this is the first snap being sent so it is not a delta. It was on a ddnet
	// server so it includes ddnet ex uuids
	uint8_t bytes[] = {
		0x80, 0x06, 0x01, 0xf0, 0xf4, 0xee, 0x68, 0xd1,
		0xf0, 0x23, 0x0d, 0x1f, 0x52, 0x3c, 0x13, 0xd0,
		0xe4, 0xcf, 0xd2, 0x9e, 0xbe, 0xcd, 0x3f, 0x56,
		0x59, 0xd2, 0xc2, 0x33, 0x0d, 0xc5, 0x33, 0xa9,
		0xf8, 0x8e, 0x69, 0xf0, 0x52, 0xc8, 0x11, 0x4f,
		0x6a, 0xa0, 0xd0, 0x6f, 0x1a, 0x44, 0xef, 0x1e,
		0x06, 0x7c, 0x4b, 0x15, 0x36, 0x2a, 0x4b, 0x5a,
		0xb4, 0x70, 0x15, 0xac, 0xca, 0x3e, 0x0c, 0xf5,
		0x80, 0x99, 0xae, 0xa4, 0x85, 0xa7, 0x78, 0xbe,
		0x2e, 0xf5, 0x74, 0x05, 0x47, 0xe0, 0x72, 0x0c,
		0x67, 0x1b, 0x8e, 0xa3, 0x2b, 0x99, 0x26, 0xd6,
		0x2b, 0xa6, 0x41, 0x5c, 0x80, 0xeb, 0x4f, 0x57,
		0xd2, 0xa2, 0x29, 0xb4, 0xf8, 0xff, 0xfb, 0x77,
		0x0d, 0xbe, 0xea, 0x6b, 0xfb, 0x41, 0x28, 0x3e,
		0xac, 0xd4, 0x53, 0x11, 0xae, 0x81, 0x2a, 0x38,
		0xe2, 0x6b, 0xfb, 0x4b, 0x4a, 0x72, 0xbc, 0xaf,
		0x74, 0x27, 0x49, 0x9d, 0x1c, 0xef, 0x2b, 0xdd,
		0x49, 0x52, 0x27, 0xc7, 0xfb, 0x4a, 0x77, 0x92,
		0x92, 0x1c, 0xef, 0x2b, 0xdd, 0x29, 0x51, 0x1d,
		0xdf, 0x95, 0x7a, 0x2a, 0x16, 0x92, 0xd0, 0xd3,
		0xd4, 0x10, 0x25, 0xa9, 0x93, 0xe3, 0x7d, 0xa5,
		0x3b, 0x49, 0xea, 0xe4, 0x78, 0x5f, 0xe9, 0x4e,
		0x92, 0x3a, 0x39, 0xde, 0x57, 0xba, 0x93, 0x94,
		0xe4, 0x78, 0x5f, 0xe9, 0x2e, 0xd0, 0x60, 0x37,
		0xa0, 0xc1, 0x6e, 0xfa, 0xb8, 0x2b, 0x4b, 0xc7,
		0x5d, 0x90, 0xb4, 0xf0, 0xf8, 0x96, 0x2a, 0x35,
		0x88, 0x0f, 0xb3, 0x4b, 0x25, 0x3f, 0x85, 0x54,
		0x27, 0x57, 0xdc, 0x39, 0x96, 0x53, 0x57, 0xd3,
		0xfb, 0xcd, 0x35, 0x9b, 0x74, 0x8f, 0x85, 0x0b,
		0x92, 0x16, 0xed, 0xb5, 0xb9, 0x7b, 0x5e, 0x92,
		0x16, 0x9e, 0x1f, 0x5d, 0x19, 0x0a, 0xa5, 0x79,
		0xd4, 0x2b, 0xee, 0x7e, 0x52, 0x14, 0xd4, 0xad,
		0x50, 0x43, 0x9a, 0x72, 0x28, 0x76, 0x35, 0x27,
		0x29, 0x46, 0xcf, 0x4b, 0xd2, 0xa2, 0xc0, 0xcc,
		0xb4, 0x68, 0x98, 0xb4, 0x53, 0xd1, 0x3d, 0xdf,
		0xd8, 0x14, 0x4a, 0x9b, 0xb4, 0x53, 0xd1, 0x7d,
		0xbd, 0x5e, 0x09, 0xed, 0xbb, 0x57, 0xd2, 0xc2,
		0xd3, 0x95, 0x62, 0x9a, 0xef, 0xc3, 0xe8, 0x7d,
		0xa5, 0x0c, 0x4b, 0x1c, 0x25, 0x95, 0x1a, 0xac,
		0xeb, 0x3e, 0xeb, 0xe9, 0x04, 0x8a, 0x4d, 0x4c,
		0xd7, 0x71, 0xaf, 0xa4, 0x45, 0x7f, 0x3f, 0x81,
		0x4d, 0xa5, 0x5f, 0x49, 0x49, 0x76, 0x9a, 0x32,
		0xa9, 0x5a, 0x71, 0x03};

	DDProtoError err = DDPROTO_ERR_NONE;
	DDProtoPacket packet = ddproto_decode_packet(bytes, sizeof(bytes), &err);

	ASSERT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_EQ(packet.kind, DDPROTO_PACKET_NORMAL);
	EXPECT_EQ(packet.header.flags, DDPROTO_PACKET_FLAG_COMPRESSION);
	EXPECT_EQ(packet.header.num_chunks, 1);
	EXPECT_EQ(packet.header.ack, 6);
	EXPECT_EQ(packet.header.token, 0x99988aeb);

	EXPECT_EQ(packet.chunks.data[0].payload.kind, DDPROTO_MSG_KIND_SNAPSINGLE);
	EXPECT_EQ(packet.chunks.data[0].header.flags, 0);
	// there is no vital flag set so the chunk header did not include a sequence
	// number. So it is not actually set to 0 it is just unset
	EXPECT_EQ(packet.chunks.data[0].header.sequence, 0);

	DDProtoMsgSnapSingle snap_single = packet.chunks.data[0].payload.msg.snap_single;
	EXPECT_EQ(snap_single.game_tick, 1420);
	EXPECT_EQ(snap_single.delta_tick, 1421);
	EXPECT_EQ(snap_single.crc, -1521333639);
	EXPECT_EQ(snap_single.part_size, 322);

	DDProtoSnapshot snap = snap_single.snapshot;
	EXPECT_EQ(snap.removed_keys.len, 0);
	EXPECT_EQ(snap.removed_keys.data, nullptr);

	ASSERT_EQ(snap.items.len, 14);

	DDProtoSnapItem *item = &snap.items.data[0];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_GAME_INFO);

	DDProtoObjGameInfo *game_info = &item->item.game_info;
	ASSERT_EQ(game_info->type_id, 6);
	EXPECT_EQ(game_info->id, 0);
	EXPECT_EQ(game_info->game_flags, 0);
	EXPECT_EQ(game_info->game_state_flags, 0);
	EXPECT_EQ(game_info->round_start_tick, 0);
	EXPECT_EQ(game_info->warmup_timer, 0);
	EXPECT_EQ(game_info->score_limit, 0);
	EXPECT_EQ(game_info->time_limit, 0);
	EXPECT_EQ(game_info->round_num, 0);
	EXPECT_EQ(game_info->round_current, 1);

	item = &snap.items.data[1];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	DDProtoObjUnknown *unknown = &item->item.unknown;
	ASSERT_EQ(unknown->type_id, 0);
	EXPECT_EQ(unknown->id, 32767);
	ASSERT_EQ(unknown->size, 4);
	EXPECT_EQ(unknown->data_len, 20);
	DDProtoUnpacker unpacker = {};
	ddproto_unpacker_init(&unpacker, unknown->data, unknown->data_len);
	int32_t unknown_field0 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field1 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field2 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field3 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	EXPECT_EQ(unknown_field0, -1824658838);
	EXPECT_EQ(unknown_field1, -629591830);
	EXPECT_EQ(unknown_field2, -1450210576);
	EXPECT_EQ(unknown_field3, 914991429);
	ddproto_unpacker_get_raw(&unpacker, 1);
	EXPECT_EQ(unpacker.err, DDPROTO_ERR_END_OF_BUFFER);

	item = &snap.items.data[2];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[3];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[4];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[5];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_CLIENT_INFO);
	DDProtoObjClientInfo *client_info = &item->item.client_info;
	ASSERT_EQ(client_info->type_id, 11);
	EXPECT_EQ(client_info->id, 0);
	EXPECT_STREQ(client_info->name, "nameless tee");
	EXPECT_STREQ(client_info->clan, "");
	EXPECT_EQ(client_info->country, -1);
	EXPECT_STREQ(client_info->skin, "default");
	EXPECT_EQ(client_info->use_custom_color, 0);
	EXPECT_EQ(client_info->color_body, 65408);
	EXPECT_EQ(client_info->color_feet, 65408);

	item = &snap.items.data[6];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_PLAYER_INFO);
	DDProtoObjPlayerInfo *player_info = &item->item.player_info;
	ASSERT_EQ(player_info->type_id, 10);
	ASSERT_EQ(player_info->id, 0);
	ASSERT_EQ(player_info->client_id, 0);
	ASSERT_EQ(player_info->score, -9999);
	ASSERT_EQ(player_info->latency, 0);

	item = &snap.items.data[7];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[8];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[9];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[10];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[11];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_CHARACTER);
	DDProtoObjCharacter *character = &item->item.character;
	ASSERT_EQ(character->type_id, 9);
	EXPECT_EQ(character->id, 0);
	EXPECT_EQ(character->core.tick, 1420);
	EXPECT_EQ(character->core.x, 336);
	EXPECT_EQ(character->core.y, 2225);
	EXPECT_EQ(character->core.vel_x, 0);
	EXPECT_EQ(character->core.vel_y, 0);
	EXPECT_EQ(character->core.angle, -402);
	EXPECT_EQ(character->core.direction, 0);
	EXPECT_EQ(character->core.jumped, 2);
	EXPECT_EQ(character->core.hooked_player, -1);
	EXPECT_EQ(character->core.hook_state, 0);
	EXPECT_EQ(character->core.hook_tick, 0);
	EXPECT_EQ(character->core.hook_x, 336);
	EXPECT_EQ(character->core.hook_y, 2225);
	EXPECT_EQ(character->core.hook_dx, 0);
	EXPECT_EQ(character->core.hook_dy, 0);
	EXPECT_EQ(character->player_flags, 0);
	EXPECT_EQ(character->health, 10);
	EXPECT_EQ(character->armor, 10);
	EXPECT_EQ(character->ammo_count, -1);
	EXPECT_EQ(character->weapon, 1);
	EXPECT_EQ(character->emote, 5);
	EXPECT_EQ(character->attack_tick, 0);

	item = &snap.items.data[12];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	unknown = &item->item.unknown;
	ASSERT_EQ(unknown->type_id, 0);
	EXPECT_EQ(unknown->id, 32763);
	ASSERT_EQ(unknown->size, 4);
	unpacker = {};
	ddproto_unpacker_init(&unpacker, unknown->data, unknown->data_len);
	unknown_field0 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	unknown_field1 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	unknown_field2 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	unknown_field3 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	EXPECT_EQ(unknown_field0, 1993229659);
	EXPECT_EQ(unknown_field1, -102024632);
	EXPECT_EQ(unknown_field2, -1378361269);
	EXPECT_EQ(unknown_field3, -1810037668);
	ddproto_unpacker_get_raw(&unpacker, 1);
	EXPECT_EQ(unpacker.err, DDPROTO_ERR_END_OF_BUFFER);

	item = &snap.items.data[13];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	unknown = &item->item.unknown;
	ASSERT_EQ(unknown->type_id, 32763);
	EXPECT_EQ(unknown->id, 0);
	ASSERT_EQ(unknown->size, 11);
	EXPECT_EQ(unknown->data_len, 13);
	unpacker = {};
	ddproto_unpacker_init(&unpacker, unknown->data, unknown->data_len);
	unknown_field0 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	unknown_field1 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	unknown_field2 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	unknown_field3 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field4 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field5 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field6 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field7 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field8 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field9 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	int32_t unknown_field10 = ddproto_unpacker_get_int(&unpacker);
	ASSERT_EQ(unpacker.err, DDPROTO_ERR_NONE);
	EXPECT_EQ(unknown_field0, 49153);
	EXPECT_EQ(unknown_field1, 0);
	EXPECT_EQ(unknown_field2, -1);
	EXPECT_EQ(unknown_field3, 0);
	EXPECT_EQ(unknown_field4, 0);
	EXPECT_EQ(unknown_field5, 0);
	EXPECT_EQ(unknown_field6, 0);
	EXPECT_EQ(unknown_field7, 0);
	EXPECT_EQ(unknown_field8, 0);
	EXPECT_EQ(unknown_field9, -1);
	EXPECT_EQ(unknown_field10, -1);
	ddproto_unpacker_get_raw(&unpacker, 1);
	EXPECT_EQ(unpacker.err, DDPROTO_ERR_END_OF_BUFFER);

	ddproto_free_packet(&packet);
}
