#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/int_string.h>
#include <ddnet_protocol/packer.h>
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
	int32_t type_id = ddnet_unpacker_get_int(unpacker);
	item->item.unknown.type_id = type_id;
	item->item.unknown.id = ddnet_unpacker_get_int(unpacker);

	DDNetError err = DDNET_ERR_NONE;

	// all events start with event common so x and y
	if(type_id >= DDNET_ITEM_EVENT_COMMON && type_id <= DDNET_ITEM_DAMAGE_INDICATOR) {
		item->item.common.x = ddnet_unpacker_get_int(unpacker);
		item->item.common.y = ddnet_unpacker_get_int(unpacker);
	}

	switch(type_id) {
	case DDNET_ITEM_PROJECTILE:
		item->kind = DDNET_ITEM_KIND_PROJECTILE;
		item->item.projectile.x = ddnet_unpacker_get_int(unpacker);
		item->item.projectile.y = ddnet_unpacker_get_int(unpacker);
		item->item.projectile.vel_x = ddnet_unpacker_get_int(unpacker);
		item->item.projectile.vel_y = ddnet_unpacker_get_int(unpacker);
		item->item.projectile.type = ddnet_unpacker_get_int(unpacker);
		item->item.projectile.start_tick = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_LASER:
		item->kind = DDNET_ITEM_KIND_LASER;
		item->item.laser.x = ddnet_unpacker_get_int(unpacker);
		item->item.laser.y = ddnet_unpacker_get_int(unpacker);
		item->item.laser.from_x = ddnet_unpacker_get_int(unpacker);
		item->item.laser.from_y = ddnet_unpacker_get_int(unpacker);
		item->item.laser.start_tick = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_PICKUP:
		item->kind = DDNET_ITEM_KIND_PICKUP;
		item->item.pickup.x = ddnet_unpacker_get_int(unpacker);
		item->item.pickup.y = ddnet_unpacker_get_int(unpacker);
		item->item.pickup.type = ddnet_unpacker_get_int(unpacker);
		item->item.pickup.subtype = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_FLAG:
		item->kind = DDNET_ITEM_KIND_FLAG;
		item->item.flag.x = ddnet_unpacker_get_int(unpacker);
		item->item.flag.y = ddnet_unpacker_get_int(unpacker);
		item->item.flag.team = ddnet_unpacker_get_int(unpacker);
		break;
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
	case DDNET_ITEM_SPECTATOR_INFO:
		item->kind = DDNET_ITEM_KIND_SPECTATOR_INFO;
		item->item.spectator_info.spectator_id = ddnet_unpacker_get_int(unpacker);
		item->item.spectator_info.x = ddnet_unpacker_get_int(unpacker);
		item->item.spectator_info.y = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_EXPLOSION:
		item->kind = DDNET_ITEM_KIND_EXPLOSION;
		break;
	case DDNET_ITEM_SPAWN:
		item->kind = DDNET_ITEM_KIND_SPAWN;
		break;
	case DDNET_ITEM_HAMMER_HIT:
		item->kind = DDNET_ITEM_KIND_HAMMER_HIT;
		break;
	case DDNET_ITEM_DEATH:
		item->kind = DDNET_ITEM_KIND_DEATH;
		item->item.death.client_id = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_SOUND_GLOBAL:
		item->kind = DDNET_ITEM_KIND_SOUND_GLOBAL;
		item->item.sound_global.sound_id = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_SOUND_WORLD:
		item->kind = DDNET_ITEM_KIND_SOUND_WORLD;
		item->item.sound_world.sound_id = ddnet_unpacker_get_int(unpacker);
		break;
	case DDNET_ITEM_DAMAGE_INDICATOR:
		item->kind = DDNET_ITEM_KIND_DAMAGE_INDICATOR;
		item->item.damage_indicator.angle = ddnet_unpacker_get_int(unpacker);
		break;
	default:
		// in classic teeworlds every unknown snap item type should just be
		// skipped and it is not an error. But in the wild this really should
		// not happen. The ddnet extensions use type 0 and types above 30k so if
		// there is something outside of that range chances are high we missed
		// implementing a vanilla snap item
		//
		// this is useful while still adding snap items. Once all items are
		// added we can remove this error and just parse it as unknown item
		// silently
		if(type_id > 0 && type_id < 3000) {
			return DDNET_ERR_UNKNOWN_SNAP_ITEM;
		}

		item->kind = DDNET_ITEM_KIND_UNKNOWN;
		int32_t len = ddnet_unpacker_get_int(unpacker);
		item->item.unknown.size = len;
		item->item.unknown.data = unpacker->buf;

		// skip over the integers but don't read their values we already point
		// into the raw data
		for(int32_t i = 0; i < len; i++) {
			ddnet_unpacker_get_int(unpacker);
			if(unpacker->err != DDNET_ERR_NONE) {
				return unpacker->err;
			}
		}

		item->item.unknown.data_len = unpacker->buf - item->item.unknown.data;
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
	for(size_t i = 0; i < snap->items.len; i++) {
		DDNetSnapItem *item = &snap->items.data[i];
		DDNetError err = ddnet_decode_snap_item(unpacker, item);
		if(err != DDNET_ERR_NONE) {
			return err;
		}
	}

	return DDNET_ERR_NONE;
}
