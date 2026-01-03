#include <ddnet_protocol/fetch_chunks.h>

#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/packet.h>

size_t ddproto_fetch_chunks(const uint8_t *buf, size_t len, DDProtoPacketHeader *header, OnDDProtoChunk callback, void *ctx, DDProtoError *err) {
	const uint8_t *start = buf;
	const uint8_t *end = buf + len;
	uint8_t num_chunks = 0;

	while(true) {
		if(num_chunks == header->num_chunks) {
			break;
		}

		size_t space = end - buf;
		// technically there could be a non vital
		// chunk that holds a net message without payload
		// that would be 3 bytes long
		// 2 bytes chunk header and 1 byte message id
		//
		// but in the ddnet, 0.6 and 0.7 protocol no such message exists
		// so we can assume that it is an invalid message
		if(space < 4) {
			if(err) {
				*err = DDPROTO_ERR_END_OF_BUFFER;
			}

			return 0;
		}

		DDProtoChunkHeader chunk_header;
		buf += ddproto_decode_chunk_header(buf, &chunk_header);

		space = end - buf;
		if(space < chunk_header.size) {
			if(err) {
				*err = DDPROTO_ERR_END_OF_BUFFER;
			}

			return 0;
		}

		DDProtoChunk chunk;
		chunk.header = chunk_header;
		DDProtoError chunk_err = ddproto_decode_message(&chunk, buf);
		callback(ctx, &chunk);
		num_chunks++;
		// unknown message ids are not a fatal error in teeworlds
		if(chunk_err != DDPROTO_ERR_NONE && chunk_err != DDPROTO_ERR_UNKNOWN_MESSAGE) {
			if(err) {
				*err = chunk_err;
			}

			return 0;
		}
		buf += chunk_header.size;
	}

	if(num_chunks < header->num_chunks) {
		if(err) {
			*err = DDPROTO_ERR_END_OF_BUFFER;
		}

		return 0;
	}

	return buf - start;
}
