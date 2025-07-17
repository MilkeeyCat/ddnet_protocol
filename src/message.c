#include "message.h"
#include "chunk.h"
#include "errors.h"
#include "msg_system.h"
#include "packer.h"

static DDNetError decode_game_message(DDNetChunk *chunk, MessageId msg_id, Unpacker *unpacker) {
	DDNetGenericMessage *msg = &chunk->payload.msg;
	switch(msg_id) {
	case MSG_CL_STARTINFO:
		chunk->payload.kind = DDNET_MSG_KIND_CL_STARTINFO;
		msg->start_info.name = unpacker_get_string(unpacker);
		msg->start_info.clan = unpacker_get_string(unpacker);
		msg->start_info.country = unpacker_get_int(unpacker);
		msg->start_info.skin = unpacker_get_string(unpacker);
		msg->start_info.use_custom_color = unpacker_get_bool(unpacker);
		msg->start_info.color_body = unpacker_get_int(unpacker);
		msg->start_info.color_feet = unpacker_get_int(unpacker);
		break;
	default:
		return DDNET_ERR_UNKNOWN_MESSAGE;
	}

	return unpacker->err;
}

static DDNetError decode_system_message(DDNetChunk *chunk, MessageId msg_id, Unpacker *unpacker) {
	DDNetGenericMessage *msg = &chunk->payload.msg;
	switch(msg_id) {
	case MSG_INFO:
		msg->info.version = unpacker_get_string(unpacker);
		msg->info.password = unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_INFO;
		break;
	case MSG_MAP_CHANGE:
		msg->map_change.name = unpacker_get_string(unpacker);
		msg->map_change.crc = unpacker_get_int(unpacker);
		msg->map_change.size = unpacker_get_int(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_MAP_CHANGE;
		break;
	case MSG_MAP_DATA:
		msg->map_data.last = unpacker_get_bool(unpacker);
		msg->map_data.map_crc = unpacker_get_int(unpacker);
		msg->map_data.chunk = unpacker_get_int(unpacker);
		msg->map_data.chunk_size = unpacker_get_int(unpacker);
		msg->map_data.data = unpacker_get_raw(unpacker, msg->map_data.chunk_size);
		chunk->payload.kind = DDNET_MSG_KIND_MAP_CHANGE;
		break;
	case MSG_CON_READY:
		chunk->payload.kind = DDNET_MSG_KIND_CON_READY;
		break;
	case MSG_INPUTTIMING:
		msg->input_timing.intended_tick = unpacker_get_int(unpacker);
		msg->input_timing.time_left = unpacker_get_int(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_INPUTTIMING;
		break;
	case MSG_RCON_AUTH_STATUS:
		msg->rcon_auth_status.authed = unpacker_get_bool(unpacker);
		msg->rcon_auth_status.cmdlist = unpacker_get_bool(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_AUTH_STATUS;
		break;
	case MSG_RCON_LINE:
		msg->rcon_line.line = unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_LINE;
		break;
	case MSG_READY:
		chunk->payload.kind = DDNET_MSG_KIND_READY;
		break;
	case MSG_ENTERGAME:
		chunk->payload.kind = DDNET_MSG_KIND_ENTERGAME;
		break;
	case MSG_INPUT:
		msg->input.ack_game_tick = unpacker_get_int(unpacker);
		msg->input.prediction_tick = unpacker_get_int(unpacker);
		msg->input.size = unpacker_get_int(unpacker);
		msg->input.direction = unpacker_get_int(unpacker);
		msg->input.target_x = unpacker_get_int(unpacker);
		msg->input.target_y = unpacker_get_int(unpacker);
		msg->input.jump = unpacker_get_int(unpacker);
		msg->input.fire = unpacker_get_int(unpacker);
		msg->input.hook = unpacker_get_int(unpacker);
		msg->input.player_flags = unpacker_get_int(unpacker);
		msg->input.wanted_weapon = unpacker_get_int(unpacker);
		msg->input.next_weapon = unpacker_get_int(unpacker);
		msg->input.prev_weapon = unpacker_get_int(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_INPUT;
		break;
	case MSG_RCON_CMD:
		msg->rcon_cmd.command = unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_CMD;
		break;
	case MSG_RCON_AUTH:
		msg->rcon_auth.name = unpacker_get_string(unpacker);
		msg->rcon_auth.password = unpacker_get_string(unpacker);
		msg->rcon_auth.send_rcon_cmds = unpacker_get_bool(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_AUTH;
		break;
	case MSG_REQUEST_MAP_DATA:
		msg->request_map_data.chunk = unpacker_get_int(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_REQUEST_MAP_DATA;
		break;
	case MSG_RCON_CMD_ADD:
		msg->rcon_cmd_add.name = unpacker_get_string(unpacker);
		msg->rcon_cmd_add.help = unpacker_get_string(unpacker);
		msg->rcon_cmd_add.params = unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_CMD_ADD;
		break;
	case MSG_RCON_CMD_REM:
		msg->rcon_cmd_rem.name = unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_CMD_REM;
		break;
	default:
		return DDNET_ERR_UNKNOWN_MESSAGE;
	}

	return unpacker->err;
}

DDNetError decode_message(DDNetChunk *chunk, uint8_t *buf) {
	Unpacker unpacker;
	unpacker_init(&unpacker, buf, chunk->header.size);
	int32_t msg_and_sys = unpacker_get_int(&unpacker);
	bool sys = msg_and_sys & 1;
	MessageId msg_id = msg_and_sys >> 1;

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

size_t encode_message(DDNetChunk *chunk, uint8_t *buf, DDNetError *err) {
	Packer packer;
	packer_init_msg(&packer, chunk->payload.kind);

	DDNetGenericMessage *msg = &chunk->payload.msg;

	switch(chunk->payload.kind) {
	case DDNET_MSG_KIND_UNKNOWN:
		memcpy(buf, chunk->payload.msg.unknown.buf, chunk->payload.msg.unknown.len);
		return msg->unknown.len;
	case DDNET_MSG_KIND_INFO:
		packer_add_string(&packer, msg->info.version);
		packer_add_string(&packer, msg->info.password);
		break;
	case DDNET_MSG_KIND_MAP_CHANGE:
		packer_add_string(&packer, msg->map_change.name);
		packer_add_int(&packer, msg->map_change.crc);
		packer_add_int(&packer, msg->map_change.size);
		break;
	case DDNET_MSG_KIND_MAP_DATA:
		packer_add_int(&packer, msg->map_data.last);
		packer_add_int(&packer, msg->map_data.map_crc);
		packer_add_int(&packer, msg->map_data.chunk);
		packer_add_int(&packer, msg->map_data.chunk_size);
		packer_add_raw(&packer, msg->map_data.data, msg->map_data.chunk_size);
		break;
	case DDNET_MSG_KIND_CON_READY:
	case DDNET_MSG_KIND_READY:
	case DDNET_MSG_KIND_ENTERGAME:
		break;
	case DDNET_MSG_KIND_CL_STARTINFO:
		packer_add_string(&packer, msg->start_info.name);
		packer_add_string(&packer, msg->start_info.clan);
		packer_add_int(&packer, (int32_t)msg->start_info.country);
		packer_add_string(&packer, msg->start_info.skin);
		packer_add_int(&packer, msg->start_info.use_custom_color);
		packer_add_int(&packer, (int32_t)msg->start_info.color_body);
		packer_add_int(&packer, (int32_t)msg->start_info.color_feet);
		break;
	case DDNET_MSG_KIND_INPUTTIMING:
		packer_add_int(&packer, msg->input_timing.intended_tick);
		packer_add_int(&packer, msg->input_timing.time_left);
		break;
	case DDNET_MSG_KIND_RCON_AUTH_STATUS:
		packer_add_int(&packer, msg->rcon_auth_status.authed);
		packer_add_int(&packer, msg->rcon_auth_status.cmdlist);
		break;
	case DDNET_MSG_KIND_RCON_LINE:
		packer_add_string(&packer, msg->rcon_line.line);
		break;
	case DDNET_MSG_KIND_INPUT:
		packer_add_int(&packer, msg->input.ack_game_tick);
		packer_add_int(&packer, msg->input.prediction_tick);
		packer_add_int(&packer, msg->input.size);
		packer_add_int(&packer, msg->input.direction);
		packer_add_int(&packer, msg->input.target_x);
		packer_add_int(&packer, msg->input.target_y);
		packer_add_int(&packer, msg->input.jump);
		packer_add_int(&packer, msg->input.fire);
		packer_add_int(&packer, msg->input.hook);
		packer_add_int(&packer, msg->input.player_flags);
		packer_add_int(&packer, msg->input.wanted_weapon);
		packer_add_int(&packer, msg->input.next_weapon);
		packer_add_int(&packer, msg->input.prev_weapon);
		break;
	case DDNET_MSG_KIND_RCON_CMD:
		packer_add_string(&packer, msg->rcon_cmd.command);
		break;
	case DDNET_MSG_KIND_RCON_AUTH:
		packer_add_string(&packer, msg->rcon_auth.name);
		packer_add_string(&packer, msg->rcon_auth.password);
		packer_add_int(&packer, msg->rcon_auth.send_rcon_cmds);
		break;
	case DDNET_MSG_KIND_REQUEST_MAP_DATA:
		packer_add_int(&packer, msg->request_map_data.chunk);
		break;
	case DDNET_MSG_KIND_RCON_CMD_ADD:
		packer_add_string(&packer, msg->rcon_cmd_add.name);
		packer_add_string(&packer, msg->rcon_cmd_add.help);
		packer_add_string(&packer, msg->rcon_cmd_add.params);
		break;
	case DDNET_MSG_KIND_RCON_CMD_REM:
		packer_add_string(&packer, msg->rcon_cmd_rem.name);
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

	memcpy(buf, packer_data(&packer), packer_size(&packer));

	return packer_size(&packer);
}

DDNetMessage ddnet_build_msg_info(const char *password) {
	return (DDNetMessage){
		.kind = DDNET_MSG_KIND_INFO,
		.msg = {
			.info = {
				.version = "0.6 626fce9a778df4d4",
				.password = password}}};
}
