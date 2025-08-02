#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "session.h"
#include "token.h"

// minimum size in bytes required for a valid packet header
#define DDNET_PACKET_HEADER_SIZE 3

// maximum amount of total packet size
#define DDNET_MAX_PACKET_SIZE 1400

// internal enum for packet types
// not sent over the network
typedef enum {
	// Connection less packet.
	// This is for master server communication (server browser).
	// And not used for anything gameplay relevant.
	DDNET_PACKET_CONNLESS,

	// Control packets are the lowest layer of the protocol.
	// They handle connect, disconnect and keep alive.
	DDNET_PACKET_CONTROL,

	// Normal packets can contain multiple game and system
	// messages in their payload.
	// These messages contain all the gameplay relevant information.
	DDNET_PACKET_NORMAL,
} DDNetPacketKind;

// packet flags
// used for the packet header
// multiple flags can be combined
//
// but if control is set compression should not be set
typedef enum {
	// Indicating that the packet is a control packet (See the `DDNetControlMessage` struct)
	// Can not be mixed with the DDNET_PACKET_FLAG_COMPRESSION!
	DDNET_PACKET_FLAG_CONTROL = 1 << 2,

	// Indicating that the packet is a connection less packet.
	DDNET_PACKET_FLAG_CONNLESS = 1 << 3,

	// Requesting a resend from the peer.
	DDNET_PACKET_FLAG_RESEND = 1 << 4,

	// Indicating that the packet payload is huffman compressed (see `ddnet_huffman_decompress()`)
	// Can not be mixed with the DDNET_PACKET_FLAG_CONTROL!
	DDNET_PACKET_FLAG_COMPRESSION = 1 << 5,
} DDNetPacketFlag;

// Teeworlds packet header.
// Parsed version of the first 3 bytes in
// the udp payload.
// Plus the security token.
//
// Example:
//
// ```C
// DDNetPacketHeader header;
// header.flags = DDNET_PACKET_FLAG_CONTROL | DDNET_PACKET_FLAG_RESEND;
// header.ack = 10;
// header.num_chunks = 0; // control packets have no chunks
// header.token = DDNET_TOKEN_MAGIC;
// ```
typedef struct {
	// Bit flags from the `PacketFlag` enum
	// multiple flags can be set at once.
	//
	// But control packets should never have the compression flag set.
	uint16_t flags;

	// Acknowledged sequence number.
	// Telling the receiver how many vital messages were successfully received.
	uint16_t ack;

	// Number of chunks in the packet payload.
	// One chunk contains one net message.
	// If it is a control packet the number of chunks should
	// be always zero.
	//
	// Should be kept in sync with `DDNetPacket`s `packet.chunks.len`
	uint8_t num_chunks;

	// DDNet security token. Is a 4 byte random integer
	// to avoid spoofing.
	// The token is placed at the end of the packet payload.
	// But conceptually it belongs into the header.
	DDNetToken token;
} DDNetPacketHeader;

// Type of control packet
typedef enum {
	DDNET_CTRL_MSG_KEEPALIVE,
	DDNET_CTRL_MSG_CONNECT,
	DDNET_CTRL_MSG_CONNECTACCEPT,
	DDNET_CTRL_MSG_ACCEPT,
	DDNET_CTRL_MSG_CLOSE,
} DDNetControlMessageKind;

// Payload of control packets
typedef struct {
	DDNetControlMessageKind kind;
	const char *reason; // Can be set if msg_kind == DDNET_CTRL_MSG_CLOSE
} DDNetControlMessage;

// Holds information about on full ddnet packet
typedef struct {
	DDNetPacketKind kind;
	DDNetPacketHeader header;

	uint8_t *payload;
	size_t payload_len;

	// The parsed packet payload
	// Check `kind` to know which field in the union to access
	union {
		DDNetControlMessage control;
		struct {
			// should be either `NULL`
			// or point to memory of size `chunks.len * sizeof(DDNetChunk)`
			DDNetChunk *data;

			// should be either `0`
			// or match the allocated size of `chunks.data` in `sizeof(DDNetChunk)`
			// otherwise you might run into segfaults
			//
			// should match `header.num_chunks` or is a protocol issue
			// and the peer might not understand you correctly
			size_t len;
		} chunks;
	};
} DDNetPacket;

// Unpacks packet header and fills the `PacketHeader` struct.
//
// Warning it does not set the `token` because this one is at the end of
// the payload.
// So it is the responsibility of the payload unpacker to parse the token.
// https://github.com/MilkeeyCat/ddnet_protocol/issues/54
DDNetPacketHeader ddnet_decode_packet_header(const uint8_t *buf);

// Given a `PacketHeader` as input it writes 3 bytes into `buf`
DDNetError ddnet_encode_packet_header(const DDNetPacketHeader *header, uint8_t *buf);

// Extract and decompress packet payload.
// Given a full raw packet as `full_data`
// It will extract only the payload into `payload` and return the size of the payload.
size_t ddnet_get_packet_payload(DDNetPacketHeader *header, const uint8_t *full_data, size_t full_len, uint8_t *payload, size_t payload_len, DDNetError *err);

// Given a pointer to the beginning of a udp payload
// this determines the type of packet.
//
// It returns `NULL` on error. Check the `err` value for more details.
// Or a pointer to newly allocated memory that holds the parsed packet struct.
// It is your responsibility to free it using `ddnet_free_packet()`
DDNetPacket ddnet_decode_packet(const uint8_t *buf, size_t len, DDNetError *err);

// Given a `DDNetPacket` struct it will encode a full udp payload
// the output is written into `buf` which has to be at least `len` big
// And returns the amount of written bytes
size_t ddnet_encode_packet(const DDNetPacket *packet, uint8_t *buf, size_t len, DDNetError *err);

// Convenience function to initialize a `packet` struct.
// Creates a normal ddnet packet. If you need a connless or control packet
// You have to build it by hand.
// Fills the passed `packet` struct based on the messages and session passed in.
// It will read and write to the `session` struct passed in.
//
// The ``messages`` will be copied into the ``packet``.
// new memory will be allocated for that operation.
// It is your responsibility to free it using `ddnet_free_packet()`
DDNetError ddnet_build_packet(DDNetPacket *packet, const DDNetMessage messages[], uint8_t messages_len, DDNetSession *session);

// Frees a packet struct and all of its fields
DDNetError ddnet_free_packet(DDNetPacket *packet);

#ifdef __cplusplus
}
#endif
