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
		return ERR_UNKNOWN_MESSAGE;
	}

	return unpacker->err;
}

static Error decode_system_message(Chunk *chunk, MessageId msg_id, Unpacker *unpacker) {
	switch(msg_id) {
	case MSG_RCON_CMD:
		chunk->msg.rcon_cmd.command = unpacker_get_string(unpacker);
		chunk->kind = CHUNK_KIND_RCON_CMD;
		break;
	default:
		return ERR_UNKNOWN_MESSAGE;
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
