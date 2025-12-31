#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/int_string.h>
#include <ddnet_protocol/packer.h>
#include <ddnet_protocol/snapshot.h>

DDProtoError ddproto_unpack_int_array_as_str(DDProtoUnpacker *unpacker, size_t num_ints, char *str, size_t str_len) {
	uint32_t ints[6] = {};
	for(size_t i = 0; i < num_ints; i++) {
		ints[i] = ddproto_unpacker_get_int(unpacker);
		if(unpacker->err != DDPROTO_ERR_NONE) {
			return unpacker->err;
		}
	}
	return ddproto_ints_to_str(ints, num_ints, str, str_len);
}

DDProtoError ddproto_decode_snap_item(DDProtoUnpacker *unpacker, DDProtoSnapItem *item) {
	int32_t type_id = ddproto_unpacker_get_int(unpacker);
	item->item.unknown.type_id = type_id;
	item->item.unknown.id = ddproto_unpacker_get_int(unpacker);

	DDProtoError err = DDPROTO_ERR_NONE;

	// all events start with event common so x and y
	if(type_id >= DDPROTO_ITEM_EVENT_COMMON && type_id <= DDPROTO_ITEM_DAMAGE_INDICATOR) {
		item->item.common.x = ddproto_unpacker_get_int(unpacker);
		item->item.common.y = ddproto_unpacker_get_int(unpacker);
	}

	switch(type_id) {
	case DDPROTO_ITEM_PROJECTILE:
		item->kind = DDPROTO_ITEM_KIND_PROJECTILE;
		item->item.projectile.x = ddproto_unpacker_get_int(unpacker);
		item->item.projectile.y = ddproto_unpacker_get_int(unpacker);
		item->item.projectile.vel_x = ddproto_unpacker_get_int(unpacker);
		item->item.projectile.vel_y = ddproto_unpacker_get_int(unpacker);
		item->item.projectile.type = ddproto_unpacker_get_int(unpacker);
		item->item.projectile.start_tick = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_LASER:
		item->kind = DDPROTO_ITEM_KIND_LASER;
		item->item.laser.x = ddproto_unpacker_get_int(unpacker);
		item->item.laser.y = ddproto_unpacker_get_int(unpacker);
		item->item.laser.from_x = ddproto_unpacker_get_int(unpacker);
		item->item.laser.from_y = ddproto_unpacker_get_int(unpacker);
		item->item.laser.start_tick = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_PICKUP:
		item->kind = DDPROTO_ITEM_KIND_PICKUP;
		item->item.pickup.x = ddproto_unpacker_get_int(unpacker);
		item->item.pickup.y = ddproto_unpacker_get_int(unpacker);
		item->item.pickup.type = ddproto_unpacker_get_int(unpacker);
		item->item.pickup.subtype = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_FLAG:
		item->kind = DDPROTO_ITEM_KIND_FLAG;
		item->item.flag.x = ddproto_unpacker_get_int(unpacker);
		item->item.flag.y = ddproto_unpacker_get_int(unpacker);
		item->item.flag.team = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_GAME_INFO:
		item->kind = DDPROTO_ITEM_KIND_GAME_INFO;
		item->item.game_info.game_flags = ddproto_unpacker_get_int(unpacker);
		item->item.game_info.game_state_flags = ddproto_unpacker_get_int(unpacker);
		item->item.game_info.round_start_tick = ddproto_unpacker_get_int(unpacker);
		item->item.game_info.warmup_timer = ddproto_unpacker_get_int(unpacker);
		item->item.game_info.score_limit = ddproto_unpacker_get_int(unpacker);
		item->item.game_info.time_limit = ddproto_unpacker_get_int(unpacker);
		item->item.game_info.round_num = ddproto_unpacker_get_int(unpacker);
		item->item.game_info.round_current = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_GAME_DATA:
		item->kind = DDPROTO_ITEM_KIND_GAME_DATA;
		item->item.game_data.teamscore_red = ddproto_unpacker_get_int(unpacker);
		item->item.game_data.teamscore_blue = ddproto_unpacker_get_int(unpacker);
		item->item.game_data.flag_carrier_blue = ddproto_unpacker_get_int(unpacker);
		item->item.game_data.flag_carrier_red = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_CHARACTER:
		item->kind = DDPROTO_ITEM_KIND_CHARACTER;
		item->item.character.core.tick = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.x = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.y = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.vel_x = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.vel_y = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.angle = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.direction = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.jumped = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.hooked_player = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.hook_state = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.hook_tick = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.hook_x = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.hook_y = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.hook_dx = ddproto_unpacker_get_int(unpacker);
		item->item.character.core.hook_dy = ddproto_unpacker_get_int(unpacker);
		item->item.character.player_flags = ddproto_unpacker_get_int(unpacker);
		item->item.character.health = ddproto_unpacker_get_int(unpacker);
		item->item.character.armor = ddproto_unpacker_get_int(unpacker);
		item->item.character.ammo_count = ddproto_unpacker_get_int(unpacker);
		item->item.character.weapon = ddproto_unpacker_get_int(unpacker);
		item->item.character.emote = ddproto_unpacker_get_int(unpacker);
		item->item.character.attack_tick = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_PLAYER_INFO:
		item->kind = DDPROTO_ITEM_KIND_PLAYER_INFO;
		item->item.player_info.local = ddproto_unpacker_get_bool(unpacker);
		item->item.player_info.client_id = ddproto_unpacker_get_int(unpacker);
		item->item.player_info.team = ddproto_unpacker_get_int(unpacker);
		item->item.player_info.score = ddproto_unpacker_get_int(unpacker);
		item->item.player_info.latency = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_CLIENT_INFO:
		item->kind = DDPROTO_ITEM_KIND_CLIENT_INFO;
		err = ddproto_unpack_int_array_as_str(unpacker, 4, item->item.client_info.name, sizeof(item->item.client_info.name));
		if(err != DDPROTO_ERR_NONE) {
			return err;
		}
		err = ddproto_unpack_int_array_as_str(unpacker, 3, item->item.client_info.clan, sizeof(item->item.client_info.clan));
		if(err != DDPROTO_ERR_NONE) {
			return err;
		}
		item->item.client_info.country = ddproto_unpacker_get_int(unpacker);
		err = ddproto_unpack_int_array_as_str(unpacker, 6, item->item.client_info.skin, sizeof(item->item.client_info.skin));
		if(err != DDPROTO_ERR_NONE) {
			return err;
		}
		item->item.client_info.use_custom_color = ddproto_unpacker_get_bool(unpacker);
		item->item.client_info.color_body = ddproto_unpacker_get_int(unpacker);
		item->item.client_info.color_feet = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_SPECTATOR_INFO:
		item->kind = DDPROTO_ITEM_KIND_SPECTATOR_INFO;
		item->item.spectator_info.spectator_id = ddproto_unpacker_get_int(unpacker);
		item->item.spectator_info.x = ddproto_unpacker_get_int(unpacker);
		item->item.spectator_info.y = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_EXPLOSION:
		item->kind = DDPROTO_ITEM_KIND_EXPLOSION;
		break;
	case DDPROTO_ITEM_SPAWN:
		item->kind = DDPROTO_ITEM_KIND_SPAWN;
		break;
	case DDPROTO_ITEM_HAMMER_HIT:
		item->kind = DDPROTO_ITEM_KIND_HAMMER_HIT;
		break;
	case DDPROTO_ITEM_DEATH:
		item->kind = DDPROTO_ITEM_KIND_DEATH;
		item->item.death.client_id = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_SOUND_GLOBAL:
		item->kind = DDPROTO_ITEM_KIND_SOUND_GLOBAL;
		item->item.sound_global.sound_id = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_SOUND_WORLD:
		item->kind = DDPROTO_ITEM_KIND_SOUND_WORLD;
		item->item.sound_world.sound_id = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_ITEM_DAMAGE_INDICATOR:
		item->kind = DDPROTO_ITEM_KIND_DAMAGE_INDICATOR;
		item->item.damage_indicator.angle = ddproto_unpacker_get_int(unpacker);
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
			return DDPROTO_ERR_UNKNOWN_SNAP_ITEM;
		}

		item->kind = DDPROTO_ITEM_KIND_UNKNOWN;
		int32_t len = ddproto_unpacker_get_int(unpacker);
		item->item.unknown.size = len;
		item->item.unknown.data = unpacker->buf;

		// skip over the integers but don't read their values we already point
		// into the raw data
		for(int32_t i = 0; i < len; i++) {
			ddproto_unpacker_get_int(unpacker);
			if(unpacker->err != DDPROTO_ERR_NONE) {
				return unpacker->err;
			}
		}

		item->item.unknown.data_len = unpacker->buf - item->item.unknown.data;
		break;
	}

	if(unpacker->err != DDPROTO_ERR_NONE) {
		return unpacker->err;
	}

	return DDPROTO_ERR_NONE;
}

void ddproto_free_snapshot(DDProtoSnapshot *snap) {
	free(snap->items.data);
	free(snap->removed_keys.data);
}

DDProtoError ddproto_decode_snapshot(DDProtoUnpacker *unpacker, DDProtoSnapshot *snap) {
	snap->removed_keys.len = ddproto_unpacker_get_int(unpacker);
	if(!snap->removed_keys.len) {
		snap->removed_keys.data = NULL;
	}
	snap->items.len = ddproto_unpacker_get_int(unpacker);
	if(!snap->items.len) {
		snap->items.data = NULL;
	}

	// skip unused zero field
	ddproto_unpacker_get_int(unpacker);

	if(unpacker->err != DDPROTO_ERR_NONE) {
		return unpacker->err;
	}

	if(snap->removed_keys.len) {
		snap->removed_keys.data = malloc(sizeof(int32_t) * snap->removed_keys.len);
		for(int32_t i = 0; i < snap->removed_keys.len; i++) {
			snap->removed_keys.data[i] = ddproto_unpacker_get_int(unpacker);
		}
	}

	if(unpacker->err != DDPROTO_ERR_NONE) {
		return unpacker->err;
	}

	if(snap->items.len == 0) {
		return DDPROTO_ERR_NONE;
	}

	snap->items.data = malloc(sizeof(DDProtoSnapItem) * snap->items.len);
	for(size_t i = 0; i < snap->items.len; i++) {
		DDProtoSnapItem *item = &snap->items.data[i];
		DDProtoError err = ddproto_decode_snap_item(unpacker, item);
		if(err != DDPROTO_ERR_NONE) {
			return err;
		}
	}

	return DDPROTO_ERR_NONE;
}
