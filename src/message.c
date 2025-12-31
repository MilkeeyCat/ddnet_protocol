#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/msg_system.h>
#include <ddnet_protocol/packer.h>
#include <ddnet_protocol/snapshot.h>

static DDProtoError decode_game_message(DDProtoChunk *chunk, DDProtoMessageId msg_id, DDProtoUnpacker *unpacker) {
	DDProtoGenericMessage *msg = &chunk->payload.msg;
	switch(msg_id) {
	case DDPROTO_MSG_SV_MOTD:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_MOTD;
		msg->motd.message = ddproto_unpacker_get_string(unpacker);
		break;
	case DDPROTO_MSG_SV_BROADCAST:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_BROADCAST;
		msg->broadcast.message = ddproto_unpacker_get_string(unpacker);
		break;
	case DDPROTO_MSG_SV_CHAT:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_CHAT;
		msg->chat.team = ddproto_unpacker_get_int(unpacker);
		msg->chat.client_id = ddproto_unpacker_get_int(unpacker);
		msg->chat.message = ddproto_unpacker_get_string(unpacker);
		break;
	case DDPROTO_MSG_SV_KILLMSG:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_KILLMSG;
		msg->kill_msg.killer_id = ddproto_unpacker_get_int(unpacker);
		msg->kill_msg.victim_id = ddproto_unpacker_get_int(unpacker);
		msg->kill_msg.weapon = ddproto_unpacker_get_int(unpacker);
		msg->kill_msg.mode_special = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_MSG_SV_SOUNDGLOBAL:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_SOUNDGLOBAL;
		msg->sound_global.sound_id = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_MSG_SV_TUNEPARAMS:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_TUNEPARAMS;
		msg->tune_params.ground_control_speed = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.ground_control_accel = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.ground_friction = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.ground_jump_impulse = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.air_jump_impulse = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.air_control_speed = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.air_control_accel = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.air_friction = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.hook_length = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.hook_fire_speed = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.hook_drag_accel = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.hook_drag_speed = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.gravity = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.velramp_start = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.velramp_range = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.velramp_curvature = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.gun_curvature = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.gun_speed = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.gun_lifetime = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.shotgun_curvature = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.shotgun_speed = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.shotgun_speeddiff = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.shotgun_lifetime = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.grenade_curvature = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.grenade_speed = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.grenade_lifetime = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.laser_reach = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.laser_bounce_delay = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.laser_bounce_num = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.laser_bounce_cost = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.laser_damage = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.player_collision = (float)ddproto_unpacker_get_int(unpacker) / 100;
		msg->tune_params.player_hooking = (float)ddproto_unpacker_get_int(unpacker) / 100;
		break;
	case DDPROTO_MSG_SV_READYTOENTER:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_READYTOENTER;
		break;
	case DDPROTO_MSG_SV_WEAPONPICKUP:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_WEAPONPICKUP;
		msg->weapon_pickup.weapon = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_MSG_SV_EMOTICON:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_EMOTICON;
		msg->emoticon.client_id = ddproto_unpacker_get_int(unpacker);
		msg->emoticon.emoticon = ddproto_unpacker_get_int(unpacker);
		break;
	case DDPROTO_MSG_SV_VOTECLEAROPTIONS:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_VOTECLEAROPTIONS;
		break;
	case DDPROTO_MSG_SV_VOTEOPTIONLISTADD:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_VOTEOPTIONLISTADD;
		msg->vote_option_list_add.num_options = ddproto_unpacker_get_int(unpacker);
		memset((void *)msg->vote_option_list_add.descriptions, 0, sizeof(msg->vote_option_list_add.descriptions));
		for(int32_t i = 0; i < msg->vote_option_list_add.num_options; i++) {
			msg->vote_option_list_add.descriptions[i] = ddproto_unpacker_get_string(unpacker);
		}
		break;
	case DDPROTO_MSG_SV_VOTEOPTIONADD:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_VOTEOPTIONADD;
		msg->vote_option_add.description = ddproto_unpacker_get_string(unpacker);
		break;
	case DDPROTO_MSG_SV_VOTEOPTIONREMOVE:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_VOTEOPTIONREMOVE;
		msg->vote_option_remove.description = ddproto_unpacker_get_string(unpacker);
		break;
	case DDPROTO_MSG_SV_VOTESET:
		chunk->payload.kind = DDPROTO_MSG_KIND_SV_VOTESET;
		msg->vote_set.timeout = ddproto_unpacker_get_int(unpacker);
		msg->vote_set.description = ddproto_unpacker_get_string(unpacker);
		msg->vote_set.reason = ddproto_unpacker_get_string(unpacker);
		break;
	case DDPROTO_MSG_CL_SAY:
		chunk->payload.kind = DDPROTO_MSG_KIND_CL_SAY;
		msg->say.team = ddproto_unpacker_get_int(unpacker);
		msg->say.message = ddproto_unpacker_get_string(unpacker);
		break;
	case DDPROTO_MSG_CL_STARTINFO:
		chunk->payload.kind = DDPROTO_MSG_KIND_CL_STARTINFO;
		msg->start_info.name = ddproto_unpacker_get_string(unpacker);
		msg->start_info.clan = ddproto_unpacker_get_string(unpacker);
		msg->start_info.country = ddproto_unpacker_get_int(unpacker);
		msg->start_info.skin = ddproto_unpacker_get_string(unpacker);
		msg->start_info.use_custom_color = ddproto_unpacker_get_bool(unpacker);
		msg->start_info.color_body = ddproto_unpacker_get_int(unpacker);
		msg->start_info.color_feet = ddproto_unpacker_get_int(unpacker);
		break;
	default:
		return DDPROTO_ERR_UNKNOWN_MESSAGE;
	}

	return unpacker->err;
}

static DDProtoError decode_system_message(DDProtoChunk *chunk, DDProtoMessageId msg_id, DDProtoUnpacker *unpacker) {
	DDProtoGenericMessage *msg = &chunk->payload.msg;
	switch(msg_id) {
	case DDPROTO_MSG_INFO:
		msg->info.version = ddproto_unpacker_get_string(unpacker);
		msg->info.password = ddproto_unpacker_get_string(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_INFO;
		break;
	case DDPROTO_MSG_MAP_CHANGE:
		msg->map_change.name = ddproto_unpacker_get_string(unpacker);
		msg->map_change.crc = ddproto_unpacker_get_int(unpacker);
		msg->map_change.size = ddproto_unpacker_get_int(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_MAP_CHANGE;
		break;
	case DDPROTO_MSG_MAP_DATA:
		msg->map_data.last = ddproto_unpacker_get_bool(unpacker);
		msg->map_data.map_crc = ddproto_unpacker_get_int(unpacker);
		msg->map_data.chunk = ddproto_unpacker_get_int(unpacker);
		msg->map_data.chunk_size = ddproto_unpacker_get_int(unpacker);
		msg->map_data.data = ddproto_unpacker_get_raw(unpacker, msg->map_data.chunk_size);
		chunk->payload.kind = DDPROTO_MSG_KIND_MAP_CHANGE;
		break;
	case DDPROTO_MSG_SNAPSINGLE:
		msg->snap_single.game_tick = ddproto_unpacker_get_int(unpacker);
		msg->snap_single.delta_tick = ddproto_unpacker_get_int(unpacker);
		msg->snap_single.crc = ddproto_unpacker_get_int(unpacker);
		msg->snap_single.part_size = ddproto_unpacker_get_int(unpacker);
		DDProtoError err = ddproto_decode_snapshot(unpacker, &msg->snap_single.snapshot);
		if(err != DDPROTO_ERR_NONE) {
			return err;
		}
		chunk->payload.kind = DDPROTO_MSG_KIND_SNAPSINGLE;
		break;
	case DDPROTO_MSG_CON_READY:
		chunk->payload.kind = DDPROTO_MSG_KIND_CON_READY;
		break;
	case DDPROTO_MSG_INPUTTIMING:
		msg->input_timing.intended_tick = ddproto_unpacker_get_int(unpacker);
		msg->input_timing.time_left = ddproto_unpacker_get_int(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_INPUTTIMING;
		break;
	case DDPROTO_MSG_RCON_AUTH_STATUS:
		msg->rcon_auth_status.authed = ddproto_unpacker_get_bool(unpacker);
		msg->rcon_auth_status.cmdlist = ddproto_unpacker_get_bool(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_RCON_AUTH_STATUS;
		break;
	case DDPROTO_MSG_RCON_LINE:
		msg->rcon_line.line = ddproto_unpacker_get_string(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_RCON_LINE;
		break;
	case DDPROTO_MSG_READY:
		chunk->payload.kind = DDPROTO_MSG_KIND_READY;
		break;
	case DDPROTO_MSG_ENTERGAME:
		chunk->payload.kind = DDPROTO_MSG_KIND_ENTERGAME;
		break;
	case DDPROTO_MSG_INPUT:
		msg->input.ack_game_tick = ddproto_unpacker_get_int(unpacker);
		msg->input.prediction_tick = ddproto_unpacker_get_int(unpacker);
		msg->input.size = ddproto_unpacker_get_int(unpacker);
		msg->input.direction = ddproto_unpacker_get_int(unpacker);
		msg->input.target_x = ddproto_unpacker_get_int(unpacker);
		msg->input.target_y = ddproto_unpacker_get_int(unpacker);
		msg->input.jump = ddproto_unpacker_get_int(unpacker);
		msg->input.fire = ddproto_unpacker_get_int(unpacker);
		msg->input.hook = ddproto_unpacker_get_int(unpacker);
		msg->input.player_flags = ddproto_unpacker_get_int(unpacker);
		msg->input.wanted_weapon = ddproto_unpacker_get_int(unpacker);
		msg->input.next_weapon = ddproto_unpacker_get_int(unpacker);
		msg->input.prev_weapon = ddproto_unpacker_get_int(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_INPUT;
		break;
	case DDPROTO_MSG_RCON_CMD:
		msg->rcon_cmd.command = ddproto_unpacker_get_string(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_RCON_CMD;
		break;
	case DDPROTO_MSG_RCON_AUTH:
		msg->rcon_auth.name = ddproto_unpacker_get_string(unpacker);
		msg->rcon_auth.password = ddproto_unpacker_get_string(unpacker);
		msg->rcon_auth.send_rcon_cmds = ddproto_unpacker_get_bool(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_RCON_AUTH;
		break;
	case DDPROTO_MSG_REQUEST_MAP_DATA:
		msg->request_map_data.chunk = ddproto_unpacker_get_int(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_REQUEST_MAP_DATA;
		break;
	case DDPROTO_MSG_RCON_CMD_ADD:
		msg->rcon_cmd_add.name = ddproto_unpacker_get_string(unpacker);
		msg->rcon_cmd_add.help = ddproto_unpacker_get_string(unpacker);
		msg->rcon_cmd_add.params = ddproto_unpacker_get_string(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_RCON_CMD_ADD;
		break;
	case DDPROTO_MSG_RCON_CMD_REM:
		msg->rcon_cmd_rem.name = ddproto_unpacker_get_string(unpacker);
		chunk->payload.kind = DDPROTO_MSG_KIND_RCON_CMD_REM;
		break;
	default:
		return DDPROTO_ERR_UNKNOWN_MESSAGE;
	}

	return unpacker->err;
}

DDProtoError ddproto_decode_message(DDProtoChunk *chunk, const uint8_t *buf) {
	DDProtoUnpacker unpacker;
	ddproto_unpacker_init(&unpacker, buf, chunk->header.size);
	int32_t msg_and_sys = ddproto_unpacker_get_int(&unpacker);
	bool sys = msg_and_sys & 1;
	DDProtoMessageId msg_id = msg_and_sys >> 1;

	DDProtoError err = DDPROTO_ERR_NONE;
	if(sys) {
		err = decode_system_message(chunk, msg_id, &unpacker);
	} else {
		err = decode_game_message(chunk, msg_id, &unpacker);
	}

	if(err == DDPROTO_ERR_UNKNOWN_MESSAGE) {
		chunk->payload.msg.unknown.len = chunk->header.size;
		chunk->payload.msg.unknown.buf = buf;
		chunk->payload.kind = DDPROTO_MSG_KIND_UNKNOWN;
	}

	return err;
}

size_t ddproto_encode_message(DDProtoChunk *chunk, uint8_t *buf, DDProtoError *err) {
	DDProtoPacker packer;
	ddproto_packer_init_msg(&packer, chunk->payload.kind);

	DDProtoGenericMessage *msg = &chunk->payload.msg;

	switch(chunk->payload.kind) {
	case DDPROTO_MSG_KIND_UNKNOWN:
		memcpy(buf, chunk->payload.msg.unknown.buf, chunk->payload.msg.unknown.len);
		return msg->unknown.len;
	case DDPROTO_MSG_KIND_INFO:
		ddproto_packer_add_string(&packer, msg->info.version);
		ddproto_packer_add_string(&packer, msg->info.password);
		break;
	case DDPROTO_MSG_KIND_MAP_CHANGE:
		ddproto_packer_add_string(&packer, msg->map_change.name);
		ddproto_packer_add_int(&packer, msg->map_change.crc);
		ddproto_packer_add_int(&packer, msg->map_change.size);
		break;
	case DDPROTO_MSG_KIND_MAP_DATA:
		ddproto_packer_add_int(&packer, msg->map_data.last);
		ddproto_packer_add_int(&packer, msg->map_data.map_crc);
		ddproto_packer_add_int(&packer, msg->map_data.chunk);
		ddproto_packer_add_int(&packer, msg->map_data.chunk_size);
		ddproto_packer_add_raw(&packer, msg->map_data.data, msg->map_data.chunk_size);
		break;
	case DDPROTO_MSG_KIND_CON_READY:
	case DDPROTO_MSG_KIND_READY:
	case DDPROTO_MSG_KIND_ENTERGAME:
	case DDPROTO_MSG_KIND_SV_READYTOENTER:
	case DDPROTO_MSG_KIND_SV_VOTECLEAROPTIONS:
		break;
	case DDPROTO_MSG_KIND_INPUTTIMING:
		ddproto_packer_add_int(&packer, msg->input_timing.intended_tick);
		ddproto_packer_add_int(&packer, msg->input_timing.time_left);
		break;
	case DDPROTO_MSG_KIND_RCON_AUTH_STATUS:
		ddproto_packer_add_int(&packer, msg->rcon_auth_status.authed);
		ddproto_packer_add_int(&packer, msg->rcon_auth_status.cmdlist);
		break;
	case DDPROTO_MSG_KIND_RCON_LINE:
		ddproto_packer_add_string(&packer, msg->rcon_line.line);
		break;
	case DDPROTO_MSG_KIND_INPUT:
		ddproto_packer_add_int(&packer, msg->input.ack_game_tick);
		ddproto_packer_add_int(&packer, msg->input.prediction_tick);
		ddproto_packer_add_int(&packer, msg->input.size);
		ddproto_packer_add_int(&packer, msg->input.direction);
		ddproto_packer_add_int(&packer, msg->input.target_x);
		ddproto_packer_add_int(&packer, msg->input.target_y);
		ddproto_packer_add_int(&packer, msg->input.jump);
		ddproto_packer_add_int(&packer, msg->input.fire);
		ddproto_packer_add_int(&packer, msg->input.hook);
		ddproto_packer_add_int(&packer, msg->input.player_flags);
		ddproto_packer_add_int(&packer, msg->input.wanted_weapon);
		ddproto_packer_add_int(&packer, msg->input.next_weapon);
		ddproto_packer_add_int(&packer, msg->input.prev_weapon);
		break;
	case DDPROTO_MSG_KIND_RCON_CMD:
		ddproto_packer_add_string(&packer, msg->rcon_cmd.command);
		break;
	case DDPROTO_MSG_KIND_RCON_AUTH:
		ddproto_packer_add_string(&packer, msg->rcon_auth.name);
		ddproto_packer_add_string(&packer, msg->rcon_auth.password);
		ddproto_packer_add_int(&packer, msg->rcon_auth.send_rcon_cmds);
		break;
	case DDPROTO_MSG_KIND_REQUEST_MAP_DATA:
		ddproto_packer_add_int(&packer, msg->request_map_data.chunk);
		break;
	case DDPROTO_MSG_KIND_RCON_CMD_ADD:
		ddproto_packer_add_string(&packer, msg->rcon_cmd_add.name);
		ddproto_packer_add_string(&packer, msg->rcon_cmd_add.help);
		ddproto_packer_add_string(&packer, msg->rcon_cmd_add.params);
		break;
	case DDPROTO_MSG_KIND_RCON_CMD_REM:
		ddproto_packer_add_string(&packer, msg->rcon_cmd_rem.name);
		break;
	case DDPROTO_MSG_KIND_SV_MOTD:
		ddproto_packer_add_string(&packer, msg->motd.message);
		break;
	case DDPROTO_MSG_KIND_SV_BROADCAST:
		ddproto_packer_add_string(&packer, msg->broadcast.message);
		break;
	case DDPROTO_MSG_KIND_SV_CHAT:
		ddproto_packer_add_int(&packer, msg->chat.team);
		ddproto_packer_add_int(&packer, msg->chat.client_id);
		ddproto_packer_add_string(&packer, msg->chat.message);
		break;
	case DDPROTO_MSG_KIND_SV_KILLMSG:
		ddproto_packer_add_int(&packer, msg->kill_msg.killer_id);
		ddproto_packer_add_int(&packer, msg->kill_msg.victim_id);
		ddproto_packer_add_int(&packer, msg->kill_msg.weapon);
		ddproto_packer_add_int(&packer, msg->kill_msg.mode_special);
		break;
	case DDPROTO_MSG_KIND_SV_SOUNDGLOBAL:
		ddproto_packer_add_int(&packer, msg->sound_global.sound_id);
		break;
	case DDPROTO_MSG_KIND_SV_TUNEPARAMS:
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.ground_control_speed * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.ground_control_accel * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.ground_friction * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.ground_jump_impulse * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.air_jump_impulse * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.air_control_speed * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.air_control_accel * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.air_friction * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.hook_length * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.hook_fire_speed * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.hook_drag_accel * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.hook_drag_speed * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.gravity * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.velramp_start * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.velramp_range * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.velramp_curvature * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.gun_curvature * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.gun_speed * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.gun_lifetime * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.shotgun_curvature * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.shotgun_speed * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.shotgun_speeddiff * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.shotgun_lifetime * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.grenade_curvature * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.grenade_speed * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.grenade_lifetime * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.laser_reach * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.laser_bounce_delay * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.laser_bounce_num * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.laser_bounce_cost * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.laser_damage * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.player_collision * 100));
		ddproto_packer_add_int(&packer, (int32_t)(msg->tune_params.player_hooking * 100));
		break;
	case DDPROTO_MSG_KIND_SV_WEAPONPICKUP:
		ddproto_packer_add_int(&packer, msg->weapon_pickup.weapon);
		break;
	case DDPROTO_MSG_KIND_SV_EMOTICON:
		ddproto_packer_add_int(&packer, msg->emoticon.client_id);
		ddproto_packer_add_int(&packer, msg->emoticon.emoticon);
		break;
	case DDPROTO_MSG_KIND_SV_VOTEOPTIONLISTADD:
		ddproto_packer_add_int(&packer, msg->vote_option_list_add.num_options);
		for(int32_t i = 0; i < (sizeof(msg->vote_option_list_add.descriptions) / sizeof(const char *)); i++) {
			if(i > msg->vote_option_list_add.num_options) {
				ddproto_packer_add_string(&packer, "");
				continue;
			}
			ddproto_packer_add_string(&packer, msg->vote_option_list_add.descriptions[i]);
		}
		break;
	case DDPROTO_MSG_KIND_SV_VOTEOPTIONADD:
		ddproto_packer_add_string(&packer, msg->vote_option_add.description);
		break;
	case DDPROTO_MSG_KIND_SV_VOTEOPTIONREMOVE:
		ddproto_packer_add_string(&packer, msg->vote_option_remove.description);
		break;
	case DDPROTO_MSG_KIND_SV_VOTESET:
		ddproto_packer_add_int(&packer, msg->vote_set.timeout);
		ddproto_packer_add_string(&packer, msg->vote_set.description);
		ddproto_packer_add_string(&packer, msg->vote_set.reason);
		break;
	case DDPROTO_MSG_KIND_CL_SAY:
		ddproto_packer_add_int(&packer, msg->chat.team);
		ddproto_packer_add_string(&packer, msg->chat.message);
		break;
	case DDPROTO_MSG_KIND_CL_STARTINFO:
		ddproto_packer_add_string(&packer, msg->start_info.name);
		ddproto_packer_add_string(&packer, msg->start_info.clan);
		ddproto_packer_add_int(&packer, (int32_t)msg->start_info.country);
		ddproto_packer_add_string(&packer, msg->start_info.skin);
		ddproto_packer_add_int(&packer, msg->start_info.use_custom_color);
		ddproto_packer_add_int(&packer, (int32_t)msg->start_info.color_body);
		ddproto_packer_add_int(&packer, (int32_t)msg->start_info.color_feet);
		break;
	case DDPROTO_MSG_KIND_SNAP:
	case DDPROTO_MSG_KIND_SNAPEMPTY:
	case DDPROTO_MSG_KIND_SNAPSINGLE:
	case DDPROTO_MSG_KIND_SNAPSMALL:
	default:
		if(err != NULL) {
			*err = DDPROTO_ERR_UNKNOWN_MESSAGE;
		}
	}

	if(err != NULL && packer.err != DDPROTO_ERR_NONE) {
		*err = packer.err;

		return 0;
	}

	memcpy(buf, ddproto_packer_data(&packer), ddproto_packer_size(&packer));

	return ddproto_packer_size(&packer);
}

DDProtoMessage ddproto_build_msg_info(const char *password) {
	return (DDProtoMessage){
		.kind = DDPROTO_MSG_KIND_INFO,
		.msg = {
			.info = {
				.version = "0.6 626fce9a778df4d4",
				.password = password,
			},
		},
	};
}
