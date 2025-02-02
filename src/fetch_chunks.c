#include "fetch_chunks.h"
#include "errors.h"
#include "message.h"
#include "packet.h"

Error fetch_chunks(uint8_t *buf, size_t len, PacketHeader *header, OnChunk callback, void *ctx) {
	uint8_t *end = buf + len;
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
			return ERR_END_OF_BUFFER;
		}

		ChunkHeader chunk_header;
		buf += decode_chunk_header(buf, &chunk_header);

		space = end - buf;
		if(space < chunk_header.size) {
			return ERR_END_OF_BUFFER;
		}

		Chunk chunk;
		chunk.header = chunk_header;
		Error chunk_err = decode_message(&chunk, buf);
		callback(ctx, &chunk);
		num_chunks++;
		// unknown message ids are not a fatal error in teeworlds
		if(chunk_err != ERR_NONE && chunk_err != ERR_UNKNOWN_MESSAGE) {
			return chunk_err;
		}
		buf += chunk_header.size;
	}

	if(num_chunks < header->num_chunks) {
		return ERR_END_OF_BUFFER;
	}

	size_t space = end - buf;
	// missing ddnet security token
	// this is an error in the ddnet protocol
	// but expected in the teeworlds protocol
	//
	// because pure teeworlds is not in scope we throw an error
	// https://github.com/MilkeeyCat/ddnet_protocol/issues/48
	if(space < 4) {
		return ERR_MISSING_DDNET_SECURITY_TOKEN;
	}
	if(space > 4) {
		// we did already parse the expected amount of chunks
		// and the ddnet security token
		// but there are still bytes left!
		return ERR_REMAINING_BYTES_IN_BUFFER;
	}

	header->token = read_token(buf);

	return ERR_NONE;
}
