#include "message.h"
#include "chunk.h"
#include "errors.h"
#include "msg_system.h"
#include "packer.h"

static DDNetError decode_game_message(DDNetChunk *chunk, DDNetMessageId msg_id, DDNetUnpacker *unpacker) {
	DDNetGenericMessage *msg = &chunk->payload.msg;
	switch(msg_id) {
	case DDNET_MSG_CL_STARTINFO:
		chunk->payload.kind = DDNET_MSG_KIND_CL_STARTINFO;
		msg->start_info.name = ddnet_unpacker_get_string(unpacker);
		msg->start_info.clan = ddnet_unpacker_get_string(unpacker);
		msg->start_info.country = ddnet_unpacker_get_int(unpacker);
		msg->start_info.skin = ddnet_unpacker_get_string(unpacker);
		msg->start_info.use_custom_color = ddnet_unpacker_get_bool(unpacker);
		msg->start_info.color_body = ddnet_unpacker_get_int(unpacker);
		msg->start_info.color_feet = ddnet_unpacker_get_int(unpacker);
		break;
	default:
		return DDNET_ERR_UNKNOWN_MESSAGE;
	}

	return unpacker->err;
}

static DDNetError decode_system_message(DDNetChunk *chunk, DDNetMessageId msg_id, DDNetUnpacker *unpacker) {
	DDNetGenericMessage *msg = &chunk->payload.msg;
	switch(msg_id) {
	case DDNET_MSG_INFO:
		msg->info.version = ddnet_unpacker_get_string(unpacker);
		msg->info.password = ddnet_unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_INFO;
		break;
	case DDNET_MSG_MAP_CHANGE:
		msg->map_change.name = ddnet_unpacker_get_string(unpacker);
		msg->map_change.crc = ddnet_unpacker_get_int(unpacker);
		msg->map_change.size = ddnet_unpacker_get_int(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_MAP_CHANGE;
		break;
	case DDNET_MSG_MAP_DATA:
		msg->map_data.last = ddnet_unpacker_get_bool(unpacker);
		msg->map_data.map_crc = ddnet_unpacker_get_int(unpacker);
		msg->map_data.chunk = ddnet_unpacker_get_int(unpacker);
		msg->map_data.chunk_size = ddnet_unpacker_get_int(unpacker);
		msg->map_data.data = ddnet_unpacker_get_raw(unpacker, msg->map_data.chunk_size);
		chunk->payload.kind = DDNET_MSG_KIND_MAP_CHANGE;
		break;
	case DDNET_MSG_CON_READY:
		chunk->payload.kind = DDNET_MSG_KIND_CON_READY;
		break;
	case DDNET_MSG_INPUTTIMING:
		msg->input_timing.intended_tick = ddnet_unpacker_get_int(unpacker);
		msg->input_timing.time_left = ddnet_unpacker_get_int(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_INPUTTIMING;
		break;
	case DDNET_MSG_RCON_AUTH_STATUS:
		msg->rcon_auth_status.authed = ddnet_unpacker_get_bool(unpacker);
		msg->rcon_auth_status.cmdlist = ddnet_unpacker_get_bool(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_AUTH_STATUS;
		break;
	case DDNET_MSG_RCON_LINE:
		msg->rcon_line.line = ddnet_unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_LINE;
		break;
	case DDNET_MSG_READY:
		chunk->payload.kind = DDNET_MSG_KIND_READY;
		break;
	case DDNET_MSG_ENTERGAME:
		chunk->payload.kind = DDNET_MSG_KIND_ENTERGAME;
		break;
	case DDNET_MSG_INPUT:
		msg->input.ack_game_tick = ddnet_unpacker_get_int(unpacker);
		msg->input.prediction_tick = ddnet_unpacker_get_int(unpacker);
		msg->input.size = ddnet_unpacker_get_int(unpacker);
		msg->input.direction = ddnet_unpacker_get_int(unpacker);
		msg->input.target_x = ddnet_unpacker_get_int(unpacker);
		msg->input.target_y = ddnet_unpacker_get_int(unpacker);
		msg->input.jump = ddnet_unpacker_get_int(unpacker);
		msg->input.fire = ddnet_unpacker_get_int(unpacker);
		msg->input.hook = ddnet_unpacker_get_int(unpacker);
		msg->input.player_flags = ddnet_unpacker_get_int(unpacker);
		msg->input.wanted_weapon = ddnet_unpacker_get_int(unpacker);
		msg->input.next_weapon = ddnet_unpacker_get_int(unpacker);
		msg->input.prev_weapon = ddnet_unpacker_get_int(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_INPUT;
		break;
	case DDNET_MSG_RCON_CMD:
		msg->rcon_cmd.command = ddnet_unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_CMD;
		break;
	case DDNET_MSG_RCON_AUTH:
		msg->rcon_auth.name = ddnet_unpacker_get_string(unpacker);
		msg->rcon_auth.password = ddnet_unpacker_get_string(unpacker);
		msg->rcon_auth.send_rcon_cmds = ddnet_unpacker_get_bool(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_AUTH;
		break;
	case DDNET_MSG_REQUEST_MAP_DATA:
		msg->request_map_data.chunk = ddnet_unpacker_get_int(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_REQUEST_MAP_DATA;
		break;
	case DDNET_MSG_RCON_CMD_ADD:
		msg->rcon_cmd_add.name = ddnet_unpacker_get_string(unpacker);
		msg->rcon_cmd_add.help = ddnet_unpacker_get_string(unpacker);
		msg->rcon_cmd_add.params = ddnet_unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_CMD_ADD;
		break;
	case DDNET_MSG_RCON_CMD_REM:
		msg->rcon_cmd_rem.name = ddnet_unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_CMD_REM;
		break;
	default:
		return DDNET_ERR_UNKNOWN_MESSAGE;
	}

	return unpacker->err;
}

DDNetError ddnet_decode_message(DDNetChunk *chunk, uint8_t *buf) {
	DDNetUnpacker unpacker;
	ddnet_unpacker_init(&unpacker, buf, chunk->header.size);
	int32_t msg_and_sys = ddnet_unpacker_get_int(&unpacker);
	bool sys = msg_and_sys & 1;
	DDNetMessageId msg_id = msg_and_sys >> 1;

	DDNetError err = DDNET_ERR_NONE;
	if(sys) {
		err = decode_system_message(chunk, msg_id, &unpacker);
	} else {
		err = decode_game_message(chunk, msg_id, &unpacker);
	}

	if(err == DDNET_ERR_UNKNOWN_MESSAGE) {
		chunk->payload.msg.unknown.len = chunk->header.size;
		chunk->payload.msg.unknown.buf = buf;
		chunk->payload.kind = DDNET_MSG_KIND_UNKNOWN;
	}

	return err;
}

size_t ddnet_encode_message(DDNetChunk *chunk, uint8_t *buf, DDNetError *err) {
	DDNetPacker packer;
	ddnet_packer_init_msg(&packer, chunk->payload.kind);

	DDNetGenericMessage *msg = &chunk->payload.msg;

	switch(chunk->payload.kind) {
	case DDNET_MSG_KIND_UNKNOWN:
		memcpy(buf, chunk->payload.msg.unknown.buf, chunk->payload.msg.unknown.len);
		return msg->unknown.len;
	case DDNET_MSG_KIND_INFO:
		ddnet_packer_add_string(&packer, msg->info.version);
		ddnet_packer_add_string(&packer, msg->info.password);
		break;
	case DDNET_MSG_KIND_MAP_CHANGE:
		ddnet_packer_add_string(&packer, msg->map_change.name);
		ddnet_packer_add_int(&packer, msg->map_change.crc);
		ddnet_packer_add_int(&packer, msg->map_change.size);
		break;
	case DDNET_MSG_KIND_MAP_DATA:
		ddnet_packer_add_int(&packer, msg->map_data.last);
		ddnet_packer_add_int(&packer, msg->map_data.map_crc);
		ddnet_packer_add_int(&packer, msg->map_data.chunk);
		ddnet_packer_add_int(&packer, msg->map_data.chunk_size);
		ddnet_packer_add_raw(&packer, msg->map_data.data, msg->map_data.chunk_size);
		break;
	case DDNET_MSG_KIND_CON_READY:
	case DDNET_MSG_KIND_READY:
	case DDNET_MSG_KIND_ENTERGAME:
		break;
	case DDNET_MSG_KIND_CL_STARTINFO:
		ddnet_packer_add_string(&packer, msg->start_info.name);
		ddnet_packer_add_string(&packer, msg->start_info.clan);
		ddnet_packer_add_int(&packer, (int32_t)msg->start_info.country);
		ddnet_packer_add_string(&packer, msg->start_info.skin);
		ddnet_packer_add_int(&packer, msg->start_info.use_custom_color);
		ddnet_packer_add_int(&packer, (int32_t)msg->start_info.color_body);
		ddnet_packer_add_int(&packer, (int32_t)msg->start_info.color_feet);
		break;
	case DDNET_MSG_KIND_INPUTTIMING:
		ddnet_packer_add_int(&packer, msg->input_timing.intended_tick);
		ddnet_packer_add_int(&packer, msg->input_timing.time_left);
		break;
	case DDNET_MSG_KIND_RCON_AUTH_STATUS:
		ddnet_packer_add_int(&packer, msg->rcon_auth_status.authed);
		ddnet_packer_add_int(&packer, msg->rcon_auth_status.cmdlist);
		break;
	case DDNET_MSG_KIND_RCON_LINE:
		ddnet_packer_add_string(&packer, msg->rcon_line.line);
		break;
	case DDNET_MSG_KIND_INPUT:
		ddnet_packer_add_int(&packer, msg->input.ack_game_tick);
		ddnet_packer_add_int(&packer, msg->input.prediction_tick);
		ddnet_packer_add_int(&packer, msg->input.size);
		ddnet_packer_add_int(&packer, msg->input.direction);
		ddnet_packer_add_int(&packer, msg->input.target_x);
		ddnet_packer_add_int(&packer, msg->input.target_y);
		ddnet_packer_add_int(&packer, msg->input.jump);
		ddnet_packer_add_int(&packer, msg->input.fire);
		ddnet_packer_add_int(&packer, msg->input.hook);
		ddnet_packer_add_int(&packer, msg->input.player_flags);
		ddnet_packer_add_int(&packer, msg->input.wanted_weapon);
		ddnet_packer_add_int(&packer, msg->input.next_weapon);
		ddnet_packer_add_int(&packer, msg->input.prev_weapon);
		break;
	case DDNET_MSG_KIND_RCON_CMD:
		ddnet_packer_add_string(&packer, msg->rcon_cmd.command);
		break;
	case DDNET_MSG_KIND_RCON_AUTH:
		ddnet_packer_add_string(&packer, msg->rcon_auth.name);
		ddnet_packer_add_string(&packer, msg->rcon_auth.password);
		ddnet_packer_add_int(&packer, msg->rcon_auth.send_rcon_cmds);
		break;
	case DDNET_MSG_KIND_REQUEST_MAP_DATA:
		ddnet_packer_add_int(&packer, msg->request_map_data.chunk);
		break;
	case DDNET_MSG_KIND_RCON_CMD_ADD:
		ddnet_packer_add_string(&packer, msg->rcon_cmd_add.name);
		ddnet_packer_add_string(&packer, msg->rcon_cmd_add.help);
		ddnet_packer_add_string(&packer, msg->rcon_cmd_add.params);
		break;
	case DDNET_MSG_KIND_RCON_CMD_REM:
		ddnet_packer_add_string(&packer, msg->rcon_cmd_rem.name);
		break;
	case DDNET_MSG_KIND_SNAP:
	case DDNET_MSG_KIND_SNAPEMPTY:
	case DDNET_MSG_KIND_SNAPSINGLE:
	case DDNET_MSG_KIND_SNAPSMALL:
	default:
		if(err != NULL) {
			*err = DDNET_ERR_UNKNOWN_MESSAGE;
		}
	}

	if(err != NULL && packer.err != DDNET_ERR_NONE) {
		*err = packer.err;

		return 0;
	}

	memcpy(buf, ddnet_packer_data(&packer), ddnet_packer_size(&packer));

	return ddnet_packer_size(&packer);
}

DDNetMessage ddnet_build_msg_info(const char *password) {
	return (DDNetMessage){
		.kind = DDNET_MSG_KIND_INFO,
		.msg = {
			.info = {
				.version = "0.6 626fce9a778df4d4",
				.password = password,
			},
		},
	};
}
