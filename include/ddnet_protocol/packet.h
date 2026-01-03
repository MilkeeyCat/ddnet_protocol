#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"
#include "common.h"
#include "errors.h"
#include "session.h"
#include "token.h"

/// Minimum size in bytes required for a valid packet header.
#define DDPROTO_PACKET_HEADER_SIZE 3

/// Maximum amount of total packet size.
#define DDPROTO_MAX_PACKET_SIZE 1400

/// Internal enum for packet types. Not sent over the network.
typedef enum {
	/// Connection less packet.
	/// This is for master server communication (server browser). And not used
	/// for anything gameplay relevant.
	DDPROTO_PACKET_CONNLESS,

	/// Control packets are the lowest layer of the protocol. They handle
	/// connect, disconnect and keep alive.
	DDPROTO_PACKET_CONTROL,

	/// Normal packets can contain multiple game and system messages in their
	/// payload. These messages contain all the gameplay relevant information.
	DDPROTO_PACKET_NORMAL,
} DDProtoPacketKind;

/// @brief Packet flags.
///
/// Used for the packet headerm. Multiple flags can be combined but if control
/// is set, compression should not be set.
typedef enum {
	/// Indicating that the packet is a control packet(see @ref
	/// DDProtoControlMessage).
	/// Can not be mixed with the @ref DDPROTO_PACKET_FLAG_COMPRESSION.
	DDPROTO_PACKET_FLAG_CONTROL = 1 << 2,

	/// Indicating that the packet is a connection less packet.
	DDPROTO_PACKET_FLAG_CONNLESS = 1 << 3,

	/// Requesting a resend from the peer.
	DDPROTO_PACKET_FLAG_RESEND = 1 << 4,

	/// Indicating that the packet payload is huffman compressed(see @ref
	/// ddproto_huffman_decompress).
	/// Can not be mixed with the @ref DDPROTO_PACKET_FLAG_CONTROL.
	DDPROTO_PACKET_FLAG_COMPRESSION = 1 << 5,
} DDProtoPacketFlag;

/// @brief Teeworlds packet header.
///
/// Parsed version of the first 3 bytes in the udp payload. Plus the security
/// token.
///
/// Example:
///
/// ```C
/// DDProtoPacketHeader header;
/// header.flags = DDPROTO_PACKET_FLAG_CONTROL | DDPROTO_PACKET_FLAG_RESEND;
/// header.ack = 10;
/// header.num_chunks = 0; // control packets have no chunks
/// header.token = DDPROTO_TOKEN_MAGIC;
/// ```
typedef struct {
	/// Bit flags from the @ref DDProtoPacketFlag. Multiple flags can be set at
	/// once.
	///
	/// But control packets should never have the compression flag set.
	uint16_t flags;

	/// @brief Acknowledged sequence number.
	///
	/// Telling the receiver how many vital messages were successfully received.
	uint16_t ack;

	/// @brief Number of chunks in the packet payload.
	///
	/// One chunk contains one net message. If it is a control packet the number
	/// of chunks should be always zero.
	///
	/// Should be kept in sync with @ref DDProtoPacket.chunks.len.
	uint8_t num_chunks;

	/// @brief DDNet security token.
	///
	/// 4 byte random integer to avoid spoofing. The token is placed at the end
	/// of the packet payload. But conceptually it belongs into the header.
	DDProtoToken token;
} DDProtoPacketHeader;

/// Type of control packet.
typedef enum {
	DDPROTO_CTRL_MSG_KEEPALIVE,
	DDPROTO_CTRL_MSG_CONNECT,
	DDPROTO_CTRL_MSG_CONNECTACCEPT,
	DDPROTO_CTRL_MSG_ACCEPT,
	DDPROTO_CTRL_MSG_CLOSE,
} DDProtoControlMessageKind;

/// Payload of control packets.
typedef struct {
	DDProtoControlMessageKind kind;
	const char *reason; // can be set if msg_kind == `DDPROTO_CTRL_MSG_CLOSE`
} DDProtoControlMessage;

/// Holds information about on full ddnet packet.
typedef struct {
	DDProtoPacketKind kind;
	DDProtoPacketHeader header;

	uint8_t *payload;
	size_t payload_len;

	/// @brief The parsed packet payload.
	///
	/// Check @ref DDProtoPacket.kind to know which field in the union to
	/// access.
	union {
		DDProtoControlMessage control;
		struct {
			/// Should be either `NULL` or point to memory of size
			/// `chunks.len * sizeof(DDProtoChunk)`.
			DDProtoChunk *data;

			/// Should be either 0 or match the allocated size of @ref
			/// DDProtoPacket.chunks.data in `sizeof(DDProtoChunk)` otherwise
			/// you might run into segfaults.
			///
			/// Should match @ref DDProtoPacketHeader.num_chunks or is a
			/// protocol issue and the peer might not understand you correctly.
			size_t len;
		} chunks;
	};
} DDProtoPacket;

/// @brief Unpacks packet header and fills the @ref DDProtoPacketHeader struct.
///
/// @attention It does not set the `token` because this one is at the end of the
/// payload. So it is the responsibility of the payload unpacker to parse the
/// token.
/// https://github.com/MilkeeyCat/ddnet_protocol/issues/54
DDProtoPacketHeader ddproto_decode_packet_header(const uint8_t *buf);

/// Given a @ref DDProtoPacketHeader as input it writes 3 bytes into `buf`.
DDProtoError ddproto_encode_packet_header(const DDProtoPacketHeader *header, uint8_t *buf);

/// @brief Extract and decompress packet payload.
///
/// Given a full raw packet as `full_data` It will extract only the payload into
/// `payload` and return the size of the payload.
size_t ddproto_get_packet_payload(DDProtoPacketHeader *header, const uint8_t *full_data, size_t full_len, uint8_t *payload, size_t payload_len, DDProtoError *err);

/// @brief Given a pointer to the beginning of a udp payload this determines the
/// type of packet.
///
/// It returns `NULL` on error. Check the `err` value for more details. Or a
/// pointer to newly allocated memory that holds the parsed packet struct. It is
/// your responsibility to free it using @ref ddproto_free_packet.
DDProtoPacket ddproto_decode_packet(const uint8_t *buf, size_t len, DDProtoError *err);

/// Given a @ref DDProtoPacket struct it will encode a full udp payload the
/// output is written into `buf` which has to be at least `len` big. And
/// returns the amount of written bytes.
size_t ddproto_encode_packet(const DDProtoPacket *packet, uint8_t *buf, size_t len, DDProtoError *err);

/// @brief Convenience function to initialize a `packet` struct.
///
/// Creates a normal ddnet packet. If you need a connless or control packet. You
/// have to build it by hand. Fills the passed `packet` struct based on the
/// messages and session passed in. It will read and write to the `session`
/// struct passed in.
///
/// The `messages` will be copied into the `packet`. New memory will be
/// allocated for that operation. It is your responsibility to free it using
/// @ref ddproto_free_packet.
DDProtoError ddproto_build_packet(DDProtoPacket *packet, const DDProtoMessage messages[], uint8_t messages_len, DDProtoSession *session);

/// Frees a packet struct and all of its fields.
DDProtoError ddproto_free_packet(DDProtoPacket *packet);

#ifdef __cplusplus
}
#endif
