# DDPROTO_PACKET_HEADER_SIZE

## Syntax

```C
#define DDPROTO_PACKET_HEADER_SIZE 3
```

Minimum size in bytes required for a valid packet header.

# DDPROTO_MAX_PACKET_SIZE

## Syntax

```C
#define DDPROTO_MAX_PACKET_SIZE 1400
```

Maximum amount of total packet size.

# DDProtoPacketKind

## Syntax

```C
typedef enum {
	// Connection less packet.
	// This is for master server communication (server browser). And not used
	// for anything gameplay relevant.
	DDPROTO_PACKET_CONNLESS,
	// Control packets are the lowest layer of the protocol. They handle
	// connect, disconnect and keep alive.
	DDPROTO_PACKET_CONTROL,
	// Normal packets can contain multiple game and system messages in their
	// payload. These messages contain all the gameplay relevant information.
	DDPROTO_PACKET_NORMAL,
} DDProtoPacketKind;
```

Internal enum for packet types. Not sent over the network.

# DDProtoPacketFlag

## Syntax

```C
typedef enum {
	// Indicating that the packet is a control packet (see
	// `DDProtoControlMessage` struct).
	// Can not be mixed with the `DDPROTO_PACKET_FLAG_COMPRESSION`!
	DDPROTO_PACKET_FLAG_CONTROL = 1 << 2,
	// Indicating that the packet is a connection less packet.
	DDPROTO_PACKET_FLAG_CONNLESS = 1 << 3,
	// Requesting a resend from the peer.
	DDPROTO_PACKET_FLAG_RESEND = 1 << 4,
	// Indicating that the packet payload is huffman compressed (see
	// `ddproto_huffman_decompress()`).
	// Can not be mixed with the `DDPROTO_PACKET_FLAG_CONTROL`!
	DDPROTO_PACKET_FLAG_COMPRESSION = 1 << 5,
} DDProtoPacketFlag;
```

Packet flags.
Used for the packet headerm. Multiple flags can be combined but if control is
set, compression should not be set.

# DDProtoPacketHeader

## Syntax

```C
typedef struct {
	// Bit flags from the `PacketFlag` enum. Multiple flags can be set at once.
	//
	// But control packets should never have the compression flag set.
	uint16_t flags;
	// Acknowledged sequence number.
	// Telling the receiver how many vital messages were successfully received.
	uint16_t ack;
	// Number of chunks in the packet payload.
	// One chunk contains one net message. If it is a control packet the number
	// of chunks should be always zero.
	//
	// Should be kept in sync with `DDProtoPacket`s `packet.chunks.len`.
	uint8_t num_chunks;
	// DDNet security token.
	// 4 byte random integer to avoid spoofing. The token is placed at the end
	// of the packet payload. But conceptually it belongs into the header.
	DDProtoToken token;
} DDProtoPacketHeader;
```

Teeworlds packet header.
Parsed version of the first 3 bytes in the udp payload. Plus the security
token.

Example:

```C
DDProtoPacketHeader header;
header.flags = DDPROTO_PACKET_FLAG_CONTROL | DDPROTO_PACKET_FLAG_RESEND;
header.ack = 10;
header.num_chunks = 0; // control packets have no chunks
header.token = DDPROTO_TOKEN_MAGIC;
```

# DDProtoControlMessageKind

## Syntax

```C
typedef enum {
	DDPROTO_CTRL_MSG_KEEPALIVE,
	DDPROTO_CTRL_MSG_CONNECT,
	DDPROTO_CTRL_MSG_CONNECTACCEPT,
	DDPROTO_CTRL_MSG_ACCEPT,
	DDPROTO_CTRL_MSG_CLOSE,
} DDProtoControlMessageKind;
```

Type of control packet.

# DDProtoControlMessage

## Syntax

```C
typedef struct {
	DDProtoControlMessageKind kind;
	const char *reason; // can be set if msg_kind == `DDPROTO_CTRL_MSG_CLOSE`
} DDProtoControlMessage;
```

Payload of control packets.

# DDProtoPacket

## Syntax

```C
typedef struct {
	DDProtoPacketKind kind;
	DDProtoPacketHeader header;
	uint8_t *payload;
	size_t payload_len;
	// The parsed packet payload.
	// Check `kind` to know which field in the union to access.
	union {
		DDProtoControlMessage control;
		struct {
			// Should be either `NULL` or point to memory of size
			// `chunks.len * sizeof(DDProtoChunk)`.
			DDProtoChunk *data;
			// Should be either `0` or match the allocated size of `chunks.data`
			// in `sizeof(DDProtoChunk)` otherwise you might run into segfaults.
			//
			// Should match `header.num_chunks` or is a protocol issue and the
			// peer might not understand you correctly.
			size_t len;
		} chunks;
	};
} DDProtoPacket;
```

Holds information about on full ddnet packet.

# ddproto_decode_packet_header

## Syntax

```C
DDProtoPacketHeader ddproto_decode_packet_header(const uint8_t *buf);
```

Unpacks packet header and fills the `PacketHeader` struct.

Warning it does not set the `token` because this one is at the end of the
payload. So it is the responsibility of the payload unpacker to parse the
token.
https://github.com/MilkeeyCat/ddnet_protocol/issues/54

# ddproto_encode_packet_header

## Syntax

```C
DDProtoError ddproto_encode_packet_header(const DDProtoPacketHeader *header, uint8_t *buf);
```

Given a `PacketHeader` as input it writes 3 bytes into `buf`.

# ddproto_get_packet_payload

## Syntax

```C
size_t ddproto_get_packet_payload(DDProtoPacketHeader *header, const uint8_t *full_data, size_t full_len, uint8_t *payload, size_t payload_len, DDProtoError *err);
```

Extract and decompress packet payload.
Given a full raw packet as `full_data` It will extract only the payload into
`payload` and return the size of the payload.

# ddproto_decode_packet

## Syntax

```C
DDProtoPacket ddproto_decode_packet(const uint8_t *buf, size_t len, DDProtoError *err);
```

Given a pointer to the beginning of a udp payload this determines the type
of packet.

It returns `NULL` on error. Check the `err` value for more details. Or a
pointer to newly allocated memory that holds the parsed packet struct. It is
your responsibility to free it using `ddproto_free_packet()`.

# ddproto_encode_packet

## Syntax

```C
size_t ddproto_encode_packet(const DDProtoPacket *packet, uint8_t *buf, size_t len, DDProtoError *err);
```

Given a `DDProtoPacket` struct it will encode a full udp payload the output is
written into `buf` which has to be at least `len` big. And returns the amount
of written bytes.

# ddproto_build_packet

## Syntax

```C
DDProtoError ddproto_build_packet(DDProtoPacket *packet, const DDProtoMessage messages[], uint8_t messages_len, DDProtoSession *session);
```

Convenience function to initialize a `packet` struct.
Creates a normal ddnet packet. If you need a connless or control packet. You
have to build it by hand. Fills the passed `packet` struct based on the
messages and session passed in. It will read and write to the `session`
struct passed in.

The `messages` will be copied into the `packet`. New memory will be allocated
for that operation. It is your responsibility to free it using
`ddproto_free_packet()`.

# ddproto_free_packet

## Syntax

```C
DDProtoError ddproto_free_packet(DDProtoPacket *packet);
```

Frees a packet struct and all of its fields.

