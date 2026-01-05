#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/msg_game.h>
#include <ddnet_protocol/msg_system.h>
#include <ddnet_protocol/packer.h>
#include <ddnet_protocol/packet.h>
#include <ddnet_protocol/session.h>
#include <ddnet_protocol/snap_items.h>
#include <ddnet_protocol/snapshot.h>

#include <gtest/gtest.h>

TEST(SnapSinglePacket, SnapWithFlagsAndPickups) {
	// this packet is from the ddnet_protocol example client connecting to a
	// ddnet++ server
	uint8_t bytes[] = {
		0x00, 0x04, 0x01, 0x21, 0x06, 0x0F, 0xB6, 0x7C,
		0xB7, 0x7C, 0x8B, 0xF6, 0xBF, 0xF6, 0x0A, 0x8A,
		0x08, 0x00, 0x1C, 0x00, 0x00, 0xBF, 0xFF, 0x03,
		0x04, 0xD5, 0xD6, 0x90, 0xCC, 0x0D, 0xD5, 0xBC,
		0xB6, 0xD8, 0x04, 0xCF, 0xD4, 0x83, 0xE7, 0x0A,
		0x85, 0xA5, 0xCD, 0xE8, 0x06, 0x00, 0xBE, 0xFF,
		0x03, 0x04, 0xD6, 0xE6, 0xD0, 0xBE, 0x02, 0xD8,
		0xB2, 0xF6, 0x9E, 0x06, 0xF1, 0x9B, 0x8A, 0x97,
		0x0A, 0x95, 0xE7, 0xBD, 0xB2, 0x0B, 0x00, 0xBD,
		0xFF, 0x03, 0x04, 0xCD, 0xE1, 0xE7, 0xEC, 0x05,
		0xF4, 0xA0, 0xB6, 0xDE, 0x0C, 0xF8, 0xE0, 0x9F,
		0x87, 0x0E, 0x83, 0xE2, 0x90, 0x85, 0x03, 0x00,
		0xBC, 0xFF, 0x03, 0x04, 0xAF, 0xA5, 0xE5, 0xE5,
		0x0B, 0xC2, 0xBC, 0xBE, 0x8D, 0x07, 0xE4, 0xAA,
		0xE7, 0x84, 0x08, 0x96, 0x95, 0xC9, 0x76, 0x00,
		0xBB, 0xFF, 0x03, 0x04, 0x8D, 0xCE, 0xD4, 0xAC,
		0x04, 0xAB, 0xF8, 0x81, 0xB8, 0x02, 0xEA, 0xB6,
		0xA1, 0x98, 0x0C, 0xEE, 0x87, 0xCA, 0x95, 0x0E,
		0x00, 0xBA, 0xFF, 0x03, 0x04, 0x9B, 0x95, 0xF2,
		0xEC, 0x0E, 0xF7, 0x96, 0xA6, 0x61, 0xF4, 0xFE,
		0xC0, 0xA2, 0x0A, 0xE3, 0xEE, 0x97, 0xBE, 0x0D,
		0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0xBF, 0xFF, 0x03, 0x00, 0x03, 0xB3,
		0xB0, 0xBF, 0xEC, 0x01, 0x0A, 0x80, 0x09, 0xBE,
		0xFF, 0x03, 0x00, 0x11, 0x02, 0x07, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
		0x00, 0x41, 0x41, 0x0B, 0x00, 0xDA, 0xD8, 0x98,
		0xC1, 0x03, 0xDC, 0xC4, 0xDA, 0x90, 0x02, 0xFF,
		0xFD, 0xFB, 0xF7, 0x0F, 0xFF, 0xFF, 0xFB, 0xF7,
		0x0F, 0xFF, 0xFD, 0xFB, 0xF7, 0x0F, 0xFF, 0xFD,
		0xFB, 0xF7, 0x0F, 0xFF, 0xFF, 0xFB, 0xF7, 0x0F,
		0x00, 0xDA, 0xE8, 0xE8, 0x80, 0x03, 0xDE, 0xA0,
		0xE0, 0x90, 0x02, 0xFF, 0xFD, 0xDB, 0xD1, 0x01,
		0xFF, 0xFD, 0xFB, 0xF7, 0x0F, 0xFF, 0xFD, 0xFB,
		0xF7, 0x0F, 0xFF, 0xFF, 0xFB, 0xF7, 0x0F, 0x00,
		0x00, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x00, 0xCE,
		0x9C, 0x01, 0x00, 0xBD, 0xFF, 0x03, 0x00, 0x05,
		0x00, 0xA8, 0x0F, 0x00, 0x00, 0x00, 0xBC, 0xFF,
		0x03, 0x00, 0x01, 0x00, 0xBB, 0xFF, 0x03, 0x00,
		0x04, 0x01, 0x00, 0x40, 0x00, 0x0B, 0x0F, 0xFF,
		0xFD, 0xFB, 0xF7, 0x0B, 0xFF, 0xFD, 0xFB, 0xF7,
		0x0F, 0xFF, 0xFD, 0xFB, 0xF7, 0x0F, 0xFF, 0xFF,
		0xFB, 0xF7, 0x0F, 0xFF, 0xFD, 0xFB, 0xF7, 0x0F,
		0xFF, 0xFD, 0xFB, 0xF7, 0x0F, 0xFF, 0xFF, 0xFB,
		0xF7, 0x0F, 0x00, 0xDE, 0xE4, 0xD0, 0xB1, 0x03,
		0xFF, 0xAD, 0x98, 0xA1, 0x01, 0xFF, 0xFD, 0xFB,
		0xF7, 0x0F, 0xFF, 0xFD, 0xFB, 0xF7, 0x0F, 0xFF,
		0xFD, 0xFB, 0xF7, 0x0F, 0xFF, 0xFF, 0xFB, 0xF7,
		0x0F, 0x00, 0x00, 0x00, 0x09, 0x00, 0xB1, 0x7C,
		0xB0, 0x1B, 0x91, 0x10, 0x00, 0x80, 0x02, 0xD1,
		0x06, 0x00, 0x00, 0x40, 0x00, 0x00, 0xB0, 0x1B,
		0x90, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x40,
		0x01, 0x05, 0x00, 0xBA, 0xFF, 0x03, 0x00, 0x0B,
		0x80, 0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x40, 0x40, 0x03, 0x98, 0x13,
		0x90, 0x15, 0xB0, 0x0A, 0x90, 0x12, 0xB0, 0x0A,
		0xB6, 0x7C, 0x03, 0x97, 0x13, 0xB0, 0x12, 0xB0,
		0x0A, 0xB0, 0x15, 0xB0, 0x0A, 0xB6, 0x7C, 0x04,
		0x9F, 0x13, 0x90, 0x2D, 0xB0, 0x0F, 0x00, 0x00,
		0x04, 0x9E, 0x13, 0x90, 0x2B, 0xB0, 0x0F, 0x02,
		0x04, 0x04, 0x9D, 0x13, 0x90, 0x2A, 0xB0, 0x0F,
		0x02, 0x02, 0x04, 0x9C, 0x13, 0x90, 0x29, 0xB0,
		0x0F, 0x02, 0x03, 0x04, 0x9B, 0x13, 0x90, 0x28,
		0xB0, 0x0F, 0x00, 0x00, 0x04, 0x9A, 0x13, 0xB0,
		0x27, 0xB0, 0x0F, 0x01, 0x00, 0x05, 0x01, 0xB0,
		0x23, 0x90, 0x10, 0x01, 0x05, 0x00, 0x90, 0x22,
		0x90, 0x10, 0x00, 0x6B, 0x66, 0x92, 0x11};

	DDProtoError err = DDPROTO_ERR_NONE;
	DDProtoPacket packet = ddproto_decode_packet(bytes, sizeof(bytes), &err);

	ASSERT_EQ(err, DDPROTO_ERR_NONE);
	EXPECT_EQ(packet.kind, DDPROTO_PACKET_NORMAL);

	DDProtoMsgSnapSingle snap_single = packet.chunks.data[0].payload.msg.snap_single;
	EXPECT_EQ(snap_single.game_tick, 7990);
	EXPECT_EQ(snap_single.delta_tick, 7991);
	EXPECT_EQ(snap_single.crc, 1466432907);
	EXPECT_EQ(snap_single.part_size, 522);

	DDProtoSnapshot snap = snap_single.snapshot;
	EXPECT_EQ(snap.removed_keys.len, 0);
	EXPECT_EQ(snap.removed_keys.data, nullptr);

	ASSERT_EQ(snap.items.len, 28);

	DDProtoSnapItem *item = &snap.items.data[0];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	item = &snap.items.data[1];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	item = &snap.items.data[2];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	item = &snap.items.data[3];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	item = &snap.items.data[4];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	item = &snap.items.data[5];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[6];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_GAME_INFO);
	DDProtoObjGameInfo *game_info = &item->item.game_info;
	ASSERT_EQ(game_info->type_id, 6);
	EXPECT_EQ(game_info->id, 0);
	EXPECT_EQ(game_info->game_flags, DDPROTO_GAMEFLAG_FLAGS);
	EXPECT_EQ(game_info->game_state_flags, 0);
	EXPECT_EQ(game_info->round_start_tick, 0);
	EXPECT_EQ(game_info->warmup_timer, 0);
	EXPECT_EQ(game_info->score_limit, 0);
	EXPECT_EQ(game_info->time_limit, 0);
	EXPECT_EQ(game_info->round_num, 0);
	EXPECT_EQ(game_info->round_current, 1);

	item = &snap.items.data[7];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	item = &snap.items.data[8];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[9];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_GAME_DATA);
	DDProtoObjGameData *game_data = &item->item.game_data;
	ASSERT_EQ(game_data->type_id, 7);
	ASSERT_EQ(game_data->teamscore_red, 0);
	ASSERT_EQ(game_data->teamscore_blue, 0);
	ASSERT_EQ(game_data->flag_carrier_red, DDPROTO_FLAG_ATSTAND);
	ASSERT_EQ(game_data->flag_carrier_blue, DDPROTO_FLAG_ATSTAND);

	item = &snap.items.data[10];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_CLIENT_INFO);
	DDProtoObjClientInfo *client_info = &item->item.client_info;
	ASSERT_EQ(client_info->type_id, 11);
	EXPECT_EQ(client_info->id, 0);
	EXPECT_STREQ(client_info->name, "client.c");
	EXPECT_STREQ(client_info->clan, "");
	EXPECT_EQ(client_info->country, 0);
	EXPECT_STREQ(client_info->skin, "greensward");
	EXPECT_EQ(client_info->use_custom_color, 0);
	EXPECT_EQ(client_info->color_body, 0);
	EXPECT_EQ(client_info->color_feet, 0);

	item = &snap.items.data[11];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_PLAYER_INFO);
	DDProtoObjPlayerInfo *player_info = &item->item.player_info;
	ASSERT_EQ(player_info->type_id, 10);
	ASSERT_EQ(player_info->id, 0);
	ASSERT_EQ(player_info->client_id, 0);
	ASSERT_EQ(player_info->score, -9999);
	ASSERT_EQ(player_info->latency, 0);

	item = &snap.items.data[12];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	item = &snap.items.data[13];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);
	item = &snap.items.data[14];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[15];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_CLIENT_INFO);
	client_info = &item->item.client_info;
	ASSERT_EQ(client_info->type_id, 11);
	EXPECT_EQ(client_info->id, 15);
	EXPECT_STREQ(client_info->name, " "); // TODO: this looks wrong? Is that a delta value?
	EXPECT_STREQ(client_info->clan, "");
	EXPECT_EQ(client_info->country, 0);
	EXPECT_STREQ(client_info->skin, "default");
	EXPECT_EQ(client_info->use_custom_color, 0);
	EXPECT_EQ(client_info->color_body, 0);
	EXPECT_EQ(client_info->color_feet, 0);

	item = &snap.items.data[16];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_CHARACTER);
	DDProtoObjCharacter *character = &item->item.character;
	ASSERT_EQ(character->type_id, 9);
	EXPECT_EQ(character->id, 0);
	EXPECT_EQ(character->core.tick, 7985);
	EXPECT_EQ(character->core.x, 1776);
	EXPECT_EQ(character->core.y, 1041);
	EXPECT_EQ(character->core.vel_x, 0);
	EXPECT_EQ(character->core.vel_y, 128);
	EXPECT_EQ(character->core.angle, -402);
	EXPECT_EQ(character->core.direction, 0);
	EXPECT_EQ(character->core.jumped, 0);
	EXPECT_EQ(character->core.hooked_player, -1);
	EXPECT_EQ(character->core.hook_state, 0);
	EXPECT_EQ(character->core.hook_tick, 0);
	EXPECT_EQ(character->core.hook_x, 1776);
	EXPECT_EQ(character->core.hook_y, 1040);
	EXPECT_EQ(character->core.hook_dx, 0);
	EXPECT_EQ(character->core.hook_dy, 0);
	EXPECT_EQ(character->player_flags, 0);
	EXPECT_EQ(character->health, 10);
	EXPECT_EQ(character->armor, 10);
	EXPECT_EQ(character->ammo_count, -1);
	EXPECT_EQ(character->weapon, 1);
	EXPECT_EQ(character->emote, 5);
	EXPECT_EQ(character->attack_tick, 0);

	item = &snap.items.data[17];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_UNKNOWN);

	item = &snap.items.data[18];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_LASER);
	DDProtoObjLaser *laser = &item->item.laser;
	ASSERT_EQ(laser->type_id, 3);
	EXPECT_EQ(laser->x, 1360);
	EXPECT_EQ(laser->y, 688);
	EXPECT_EQ(laser->from_x, 1168);
	EXPECT_EQ(laser->from_y, 688);
	EXPECT_EQ(laser->start_tick, 7990);

	item = &snap.items.data[19];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_LASER);
	laser = &item->item.laser;
	ASSERT_EQ(laser->type_id, 3);
	EXPECT_EQ(laser->x, 1200);
	EXPECT_EQ(laser->y, 688);
	EXPECT_EQ(laser->from_x, 1392);
	EXPECT_EQ(laser->from_y, 688);
	EXPECT_EQ(laser->start_tick, 7990);

	item = &snap.items.data[20];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_PICKUP);
	DDProtoObjPickup *pickup = &item->item.pickup;
	ASSERT_EQ(pickup->type_id, 4);
	EXPECT_EQ(pickup->x, 2896);
	EXPECT_EQ(pickup->y, 1008);
	EXPECT_EQ(pickup->type, 0);
	EXPECT_EQ(pickup->subtype, 0);

	item = &snap.items.data[21];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_PICKUP);
	pickup = &item->item.pickup;
	ASSERT_EQ(pickup->type_id, 4);
	EXPECT_EQ(pickup->x, 2768);
	EXPECT_EQ(pickup->y, 1008);
	EXPECT_EQ(pickup->type, DDPROTO_POWERUP_WEAPON);
	EXPECT_EQ(pickup->subtype, DDPROTO_PICKUP_WEAPON_LASER);

	item = &snap.items.data[22];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_PICKUP);
	pickup = &item->item.pickup;
	ASSERT_EQ(pickup->type_id, 4);
	EXPECT_EQ(pickup->x, 2704);
	EXPECT_EQ(pickup->y, 1008);
	EXPECT_EQ(pickup->type, DDPROTO_POWERUP_WEAPON);
	EXPECT_EQ(pickup->subtype, DDPROTO_PICKUP_WEAPON_SHOTGUN);

	item = &snap.items.data[23];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_PICKUP);
	pickup = &item->item.pickup;
	ASSERT_EQ(pickup->type_id, 4);
	EXPECT_EQ(pickup->x, 2640);
	EXPECT_EQ(pickup->y, 1008);
	EXPECT_EQ(pickup->type, DDPROTO_POWERUP_WEAPON);
	EXPECT_EQ(pickup->subtype, DDPROTO_PICKUP_WEAPON_GRENADE);

	item = &snap.items.data[24];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_PICKUP);
	pickup = &item->item.pickup;
	ASSERT_EQ(pickup->type_id, 4);
	EXPECT_EQ(pickup->x, 2576);
	EXPECT_EQ(pickup->y, 1008);
	EXPECT_EQ(pickup->type, 0);
	EXPECT_EQ(pickup->subtype, 0);

	item = &snap.items.data[25];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_PICKUP);
	pickup = &item->item.pickup;
	ASSERT_EQ(pickup->type_id, 4);
	EXPECT_EQ(pickup->x, 2544);
	EXPECT_EQ(pickup->y, 1008);
	EXPECT_EQ(pickup->type, DDPROTO_POWERUP_ARMOR);
	EXPECT_EQ(pickup->subtype, 0);

	item = &snap.items.data[26];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_FLAG);
	DDProtoObjFlag *flag = &item->item.flag;
	EXPECT_EQ(flag->x, 2288);
	EXPECT_EQ(flag->y, 1040);
	EXPECT_EQ(flag->team, DDPROTO_TEAM_BLUE);

	item = &snap.items.data[27];
	ASSERT_EQ(item->kind, DDPROTO_ITEM_KIND_FLAG);
	flag = &item->item.flag;
	EXPECT_EQ(flag->x, 2192);
	EXPECT_EQ(flag->y, 1040);
	EXPECT_EQ(flag->team, DDPROTO_TEAM_RED);

	ddproto_free_packet(&packet);
}
