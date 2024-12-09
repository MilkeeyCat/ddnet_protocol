#include "packet_normal.h"
#include "errors.h"
#include "message.h"
#include "packet.h"

PacketNormal *decode_normal(uint8_t *buf, size_t len, PacketHeader *header, Error *err) {
	PacketNormal *packet = malloc(sizeof(PacketNormal));

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
			*err = ERR_END_OF_BUFFER;
			free(packet);
			return NULL;
		}

		ChunkHeader chunk_header = decode_chunk_header(&buf);

		space = end - buf;
		if(space < chunk_header.size) {
			*err = ERR_END_OF_BUFFER;
			free(packet);
			return NULL;
		}

		Chunk chunk;
		packet->chunks[num_chunks].header = chunk_header;
		Error chunk_err = decode_message(&packet->chunks[num_chunks], buf);
		num_chunks++;
		// unknown message ids are not a fatal error in teeworlds
		if(chunk_err != ERR_NONE && chunk_err != ERR_UNKNOWN_MESSAGE) {
			*err = chunk_err;
			free(packet);
			return NULL;
		}
		buf += chunk_header.size;
	}

	if(num_chunks < header->num_chunks) {
		*err = ERR_END_OF_BUFFER;
		free(packet);
		return NULL;
	}

	size_t space = end - buf;
	// missing ddnet security token
	// this is an error in the ddnet protocol
	// but expected in the teeworlds protocol
	//
	// because pure teeworlds is not in scope we throw an error
	// https://github.com/MilkeeyCat/ddnet_protocol/issues/48
	if(space < 4) {
		*err = ERR_MISSING_DDNET_SECURITY_TOKEN;
		free(packet);
		return NULL;
	}
	if(space > 4) {
		// we did already parse the expected amount of chunks
		// and the ddnet security token
		// but there are still bytes left!
		*err = ERR_REMAINING_BYTES_IN_BUFFER;
		free(packet);
		return NULL;
	}

	header->token = read_token(buf);
	return packet;
}
