#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/int_string.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/packer.h>
#include <ddnet_protocol/packet.h>
#include <ddnet_protocol/snapshot.h>

DDNetError ddnet_unpack_int_array_as_str(DDNetUnpacker *unpacker, size_t num_ints, char *str, size_t str_len) {
	uint32_t ints[6] = {};
	for(size_t i = 0; i < num_ints; i++) {
		ints[i] = ddnet_unpacker_get_int(unpacker);
		if(unpacker->err != DDNET_ERR_NONE) {
			return unpacker->err;
		}
	}
	return ddnet_ints_to_str(ints, num_ints, str, str_len);
}

DDNetError ddnet_decode_snap_item(DDNetUnpacker *unpacker, DDNetSnapItem *item) {
	item->item.unknown.type_id = ddnet_unpacker_get_int(unpacker);
	item->item.unknown.id = ddnet_unpacker_get_int(unpacker);

	DDNetError err = DDNET_ERR_NONE;

	switch(item->item.unknown.type_id) {
	case DDNET_ITEM_GAME_INFO:
		item->kind = DDNET_ITEM_KIND_GAME_INFO;
		item->item.game_info.game_flags = ddnet_unpacker_get_int(unpacker);
		item->item.game_info.game_state_flags = ddnet_unpacker_get_int(unpacker);
		item->item.game_info.round_start_tick = ddnet_unpacker_get_int(unpacker);
		item->item.game_info.warmup_timer = ddnet_unpacker_get_int(unpacker);
		item->item.game_info.score_limit = ddnet_unpacker_get_int(unpacker);
		item->item.game_info.time_limit = ddnet_unpacker_get_int(unpacker);
		item->item.game_info.round_num = ddnet_unpacker_get_int(unpacker);
		item->item.game_info.round_current = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_GAME_DATA:
		item->kind = DDNET_ITEM_KIND_GAME_DATA;
		item->item.game_data.teamscore_red = ddnet_unpacker_get_int(unpacker);
		item->item.game_data.teamscore_blue = ddnet_unpacker_get_int(unpacker);
		item->item.game_data.flag_carrier_blue = ddnet_unpacker_get_int(unpacker);
		item->item.game_data.flag_carrier_red = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_CHARACTER:
		item->kind = DDNET_ITEM_KIND_CHARACTER;
		item->item.character.core.tick = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.x = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.y = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.vel_x = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.vel_y = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.angle = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.direction = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.jumped = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.hooked_player = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.hook_state = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.hook_tick = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.hook_x = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.hook_y = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.hook_dx = ddnet_unpacker_get_int(unpacker);
		item->item.character.core.hook_dy = ddnet_unpacker_get_int(unpacker);
		item->item.character.player_flags = ddnet_unpacker_get_int(unpacker);
		item->item.character.health = ddnet_unpacker_get_int(unpacker);
		item->item.character.armor = ddnet_unpacker_get_int(unpacker);
		item->item.character.ammo_count = ddnet_unpacker_get_int(unpacker);
		item->item.character.weapon = ddnet_unpacker_get_int(unpacker);
		item->item.character.emote = ddnet_unpacker_get_int(unpacker);
		item->item.character.attack_tick = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_PLAYER_INFO:
		item->kind = DDNET_ITEM_KIND_PLAYER_INFO;
		item->item.player_info.local = ddnet_unpacker_get_bool(unpacker);
		item->item.player_info.client_id = ddnet_unpacker_get_int(unpacker);
		item->item.player_info.team = ddnet_unpacker_get_int(unpacker);
		item->item.player_info.score = ddnet_unpacker_get_int(unpacker);
		item->item.player_info.latency = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_CLIENT_INFO:
		item->kind = DDNET_ITEM_KIND_CLIENT_INFO;
		err = ddnet_unpack_int_array_as_str(unpacker, 4, item->item.client_info.name, sizeof(item->item.client_info.name));
		if(err != DDNET_ERR_NONE) {
			return err;
		}
		err = ddnet_unpack_int_array_as_str(unpacker, 3, item->item.client_info.clan, sizeof(item->item.client_info.clan));
		if(err != DDNET_ERR_NONE) {
			return err;
		}
		item->item.client_info.country = ddnet_unpacker_get_int(unpacker);
		err = ddnet_unpack_int_array_as_str(unpacker, 6, item->item.client_info.skin, sizeof(item->item.client_info.skin));
		if(err != DDNET_ERR_NONE) {
			return err;
		}
		item->item.client_info.use_custom_color = ddnet_unpacker_get_bool(unpacker);
		item->item.client_info.color_body = ddnet_unpacker_get_int(unpacker);
		item->item.client_info.color_feet = ddnet_unpacker_get_int(unpacker);
		break;
	default:
		item->kind = DDNET_ITEM_KIND_UNKNOWN;
		int32_t len = ddnet_unpacker_get_int(unpacker);
		item->item.unknown.size = len;
		item->item.unknown.data = unpacker->buf;

		// skip over the integers but don't read their values
		// we already point into the raw data
		for(int32_t i = 0; i < len; i++) {
			ddnet_unpacker_get_int(unpacker);
			if(unpacker->err != DDNET_ERR_NONE) {
				return unpacker->err;
			}
		}

		item->item.unknown.data_len = unpacker->buf - item->item.unknown.data;

		// // TODO: remove debug
		// if(item->item.unknown.type_id > 0 && item->item.unknown.type_id < 3000) {
		// 	printf("unknown snap item type %d, size=%d\n", item->item.unknown.type_id, item->item.unknown.size);
		// }

		break;
	}

	if(unpacker->err != DDNET_ERR_NONE) {
		return unpacker->err;
	}

	return DDNET_ERR_NONE;
}

void ddnet_free_snapshot(DDNetSnapshot *snap) {
	free(snap->items.data);
	free(snap->removed_keys.data);
}

DDNetError ddnet_decode_snapshot(DDNetUnpacker *unpacker, DDNetSnapshot *snap) {
	snap->removed_keys.len = ddnet_unpacker_get_int(unpacker);
	if(!snap->removed_keys.len) {
		snap->removed_keys.data = NULL;
	}
	snap->items.len = ddnet_unpacker_get_int(unpacker);
	if(!snap->items.len) {
		snap->items.data = NULL;
	}

	// skip unused zero field
	ddnet_unpacker_get_int(unpacker);

	if(unpacker->err != DDNET_ERR_NONE) {
		return unpacker->err;
	}

	if(snap->removed_keys.len) {
		snap->removed_keys.data = malloc(sizeof(int32_t) * snap->removed_keys.len);
		for(int32_t i = 0; i < snap->removed_keys.len; i++) {
			snap->removed_keys.data[i] = ddnet_unpacker_get_int(unpacker);
		}
	}

	if(unpacker->err != DDNET_ERR_NONE) {
		return unpacker->err;
	}

	if(snap->items.len == 0) {
		return DDNET_ERR_NONE;
	}

	snap->items.data = malloc(sizeof(DDNetSnapItem) * snap->items.len);
	for(int32_t i = 0; i < snap->items.len; i++) {
		DDNetSnapItem *item = &snap->items.data[i];
		DDNetError err = ddnet_decode_snap_item(unpacker, item);
		if(err != DDNET_ERR_NONE) {
			return err;
		}
	}

	return DDNET_ERR_NONE;
}
