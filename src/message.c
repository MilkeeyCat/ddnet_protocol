#include "message.h"
#include "chunk.h"
#include "errors.h"
#include "msg_system.h"
#include "packer.h"

static Error decode_game_message(Chunk *chunk, MessageId msg_id, Unpacker *unpacker) {
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
		return ERR_UNKNOWN_MESSAGE;
	}

	return unpacker->err;
}

static Error decode_system_message(Chunk *chunk, MessageId msg_id, Unpacker *unpacker) {
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
		msg->map_data.last = unpacker_get_int(unpacker);
		msg->map_data.map_crc = unpacker_get_int(unpacker);
		msg->map_data.chunk = unpacker_get_int(unpacker);
		msg->map_data.chunk_size = unpacker_get_int(unpacker);
		msg->map_data.data = unpacker_get_raw(unpacker, msg->map_data.chunk_size);
		chunk->payload.kind = DDNET_MSG_KIND_MAP_CHANGE;
		break;
	case MSG_CON_READY:
		chunk->payload.kind = DDNET_MSG_KIND_CON_READY;
		break;
	case MSG_RCON_CMD:
		msg->rcon_cmd.command = unpacker_get_string(unpacker);
		chunk->payload.kind = DDNET_MSG_KIND_RCON_CMD;
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

	Error err = ERR_NONE;
	if(sys) {
		err = decode_system_message(chunk, msg_id, &unpacker);
	} else {
		err = decode_game_message(chunk, msg_id, &unpacker);
	}

	if(err == ERR_UNKNOWN_MESSAGE) {
		chunk->payload.msg.unknown.len = chunk->header.size;
		chunk->payload.msg.unknown.buf = buf;
		chunk->payload.kind = DDNET_MSG_KIND_UNKNOWN;
	}

	return err;
}

size_t encode_message(Chunk *chunk, uint8_t *buf, Error *err) {
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
		break;
	case DDNET_MSG_KIND_RCON_CMD:
		packer_add_string(&packer, msg->rcon_cmd.command);
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
	case DDNET_MSG_KIND_SNAP:
	case DDNET_MSG_KIND_SNAPEMPTY:
	case DDNET_MSG_KIND_SNAPSINGLE:
	case DDNET_MSG_KIND_SNAPSMALL:
	case DDNET_MSG_KIND_INPUT:
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
