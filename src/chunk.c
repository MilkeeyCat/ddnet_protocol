#include <ddnet_protocol/chunk.h>

#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/packet.h>

size_t ddproto_decode_chunk_header(const uint8_t *buf, DDProtoChunkHeader *header) {
	header->flags = buf[0] & 0b11000000;
	header->size = ((buf[0] & 0b00111111) << 4) | (buf[1] & ((1 << 4) - 1));
	header->sequence = 0;

	if(header->flags & DDPROTO_CHUNK_FLAG_VITAL) {
		header->sequence = ((buf[1] & (~((1 << 4) - 1))) << 2) | buf[2];

		return 3;
	}

	return 2;
}

size_t ddproto_encode_chunk_header(const DDProtoChunkHeader *header, uint8_t *buf) {
	buf[0] = header->flags & 0b11000000 | ((header->size >> 4) & 0b00111111);
	buf[1] = (header->size & 0b00001111);
	if(header->flags & DDPROTO_CHUNK_FLAG_VITAL) {
		buf[1] |= (header->sequence >> 2) & 0b11110000;
		buf[2] = header->sequence & 0xff;

		return 3;
	}

	return 2;
}

bool ddproto_is_vital_msg(DDProtoMessageKind kind) {
	return kind != DDPROTO_MSG_KIND_INPUT &&
	       kind != DDPROTO_MSG_KIND_SNAP &&
	       kind != DDPROTO_MSG_KIND_SNAPEMPTY &&
	       kind != DDPROTO_MSG_KIND_SNAPSINGLE &&
	       kind != DDPROTO_MSG_KIND_SNAPSMALL;
}

DDProtoError ddproto_fill_chunk_header(DDProtoChunk *chunk) {
	uint8_t chunk_payload[DDPROTO_MAX_PACKET_SIZE];
	DDProtoError encode_err = DDPROTO_ERR_NONE;
	size_t chunk_payload_len = ddproto_encode_message(chunk, chunk_payload, &encode_err);
	if(encode_err != DDPROTO_ERR_NONE) {
		return encode_err;
	}
	chunk->header.size = chunk_payload_len;
	chunk->header.flags = ddproto_is_vital_msg(chunk->payload.kind) ? DDPROTO_CHUNK_FLAG_VITAL : 0;
	return DDPROTO_ERR_NONE;
}
