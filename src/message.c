#include "message.h"
#include "chunk.h"
#include "errors.h"
#include "msg_system.h"
#include "packer.h"

static Error decode_game_message(Chunk *chunk, MessageId msg_id, Unpacker *unpacker) {
	switch(msg_id) {
	case MSG_CL_STARTINFO:
		chunk->kind = CHUNK_KIND_CL_STARTINFO;
		chunk->msg.start_info.name = unpacker_get_string(unpacker);
		chunk->msg.start_info.clan = unpacker_get_string(unpacker);
		chunk->msg.start_info.country = unpacker_get_int(unpacker);
		chunk->msg.start_info.skin = unpacker_get_string(unpacker);
		chunk->msg.start_info.use_custom_color = unpacker_get_bool(unpacker);
		chunk->msg.start_info.color_body = unpacker_get_int(unpacker);
		chunk->msg.start_info.color_feet = unpacker_get_int(unpacker);
		break;
	default:
		chunk->msg.raw.id = msg_id;
		chunk->msg.raw.kind = MESSAGE_GAME;
		chunk->msg.raw.data = unpacker->buf;
		chunk->kind = CHUNK_KIND_RAW;
	}

	return unpacker->err;
}

static Error decode_system_message(Chunk *chunk, MessageId msg_id, Unpacker *unpacker) {
	switch(msg_id) {
	case MSG_RCON_CMD:
		chunk->msg.rcon_cmd.command = unpacker_get_string(unpacker);
		chunk->kind = CHUNK_KIND_RCON_CMD;
		break;
	case MSG_INPUT:
		chunk->msg.input.ack = unpacker_get_int(unpacker);
		chunk->msg.input.prediction = unpacker_get_int(unpacker);
		chunk->msg.input.size = unpacker_get_int(unpacker);
		chunk->msg.input.direction = unpacker_get_int(unpacker);
		chunk->msg.input.target_x = unpacker_get_int(unpacker);
		chunk->msg.input.target_y = unpacker_get_int(unpacker);
		chunk->msg.input.jump = unpacker_get_int(unpacker);
		chunk->msg.input.fire = unpacker_get_int(unpacker);
		chunk->msg.input.hook = unpacker_get_int(unpacker);
		chunk->msg.input.player_flags = unpacker_get_int(unpacker);
		chunk->msg.input.wanted_weapon = unpacker_get_int(unpacker);
		chunk->msg.input.next_weapon = unpacker_get_int(unpacker);
		chunk->msg.input.prev_weapon = unpacker_get_int(unpacker);
		chunk->kind = CHUNK_KIND_INPUT;
		break;
	default:
		chunk->msg.raw.id = msg_id;
		chunk->msg.raw.kind = MESSAGE_SYSTEM;
		chunk->msg.raw.data = unpacker->buf;
		chunk->kind = CHUNK_KIND_RAW;
	}

	return unpacker->err;
}

Error decode_message(Chunk *chunk, uint8_t *buf) {
	Unpacker unpacker;
	unpacker_init(&unpacker, buf, chunk->header.size);
	int32_t msg_and_sys = unpacker_get_int(&unpacker);
	bool sys = msg_and_sys & 1;
	MessageId msg_id = msg_and_sys >> 1;

	if(sys) {
		return decode_system_message(chunk, msg_id, &unpacker);
	}
	return decode_game_message(chunk, msg_id, &unpacker);
}

size_t encode_message(Chunk *chunk, uint8_t *buf, Error *err) {
	Packer packer;
	if(chunk->kind == CHUNK_KIND_RAW) {
		packer_init(&packer);
		packer_add_int(&packer, (int32_t)((chunk->msg.raw.id << 1) | chunk->msg.raw.kind));
	} else {
		packer_init_msg(&packer, chunk->kind);
	}

	switch(chunk->kind) {
	case CHUNK_KIND_INPUT:
		packer_add_int(&packer, chunk->msg.input.ack);
		packer_add_int(&packer, chunk->msg.input.prediction);
		packer_add_int(&packer, chunk->msg.input.size);
		packer_add_int(&packer, chunk->msg.input.direction);
		packer_add_int(&packer, chunk->msg.input.target_x);
		packer_add_int(&packer, chunk->msg.input.target_y);
		packer_add_int(&packer, chunk->msg.input.jump);
		packer_add_int(&packer, chunk->msg.input.fire);
		packer_add_int(&packer, chunk->msg.input.hook);
		packer_add_int(&packer, chunk->msg.input.player_flags);
		packer_add_int(&packer, chunk->msg.input.wanted_weapon);
		packer_add_int(&packer, chunk->msg.input.next_weapon);
		packer_add_int(&packer, chunk->msg.input.prev_weapon);
		break;
	case CHUNK_KIND_RCON_CMD:
		packer_add_string(&packer, chunk->msg.rcon_cmd.command);
		break;
	case CHUNK_KIND_CL_STARTINFO:
		packer_add_string(&packer, chunk->msg.start_info.name);
		packer_add_string(&packer, chunk->msg.start_info.clan);
		packer_add_int(&packer, (int32_t)chunk->msg.start_info.country);
		packer_add_string(&packer, chunk->msg.start_info.skin);
		packer_add_int(&packer, chunk->msg.start_info.use_custom_color);
		packer_add_int(&packer, (int32_t)chunk->msg.start_info.color_body);
		packer_add_int(&packer, (int32_t)chunk->msg.start_info.color_feet);
		break;
	case CHUNK_KIND_RAW:
		memcpy(packer.current, chunk->msg.raw.data, chunk->header.size);
		packer.current += chunk->header.size - 1;
		break;
	default:
		if(err != NULL) {
			*err = ERR_UNKNOWN_MESSAGE;
		}
	}

	if(err != NULL && packer.err != ERR_NONE) {
		*err = packer.err;

		return 0;
	}

	memcpy(buf, packer_data(&packer), packer_size(&packer));

	return packer_size(&packer);
}
