# PACKET_HEADER_SIZE

## Syntax

```C
#define PACKET_HEADER_SIZE 3
```

minimum size in bytes required for a valid packet header

# MAX_PACKET_SIZE

## Syntax

```C
#define MAX_PACKET_SIZE 1400
```

maximum amount of total packet size

# PacketKind

## Syntax

```C
typedef enum {
	// Connection less packet.
	// This is for master server communication (server browser).
	// And not used for anything gameplay relevant.
	PACKET_CONNLESS,
	// Control packets are the lowest layer of the protocol.
	// They handle connect, disconnect and keep alive.
	PACKET_CONTROL,
	// Normal packets can contain multiple game and system
	// messages in their payload.
	// These messages contain all the gameplay relevant information.
	PACKET_NORMAL,
} PacketKind;
```

internal enum for packet types
not sent over the network

# PacketFlag

## Syntax

```C
typedef enum {
	// Indicating that the packet is a control packet (See the `ControlMessage` struct)
	// Can not be mixed with the PACKET_FLAG_COMPRESSION!
	PACKET_FLAG_CONTROL = 1 << 2,
	// Indicating that the packet is a connection less packet.
	PACKET_FLAG_CONNLESS = 1 << 3,
	// Requesting a resend from the peer.
	PACKET_FLAG_RESEND = 1 << 4,
	// Indicating that the packet payload is huffman compressed (see `huffman_decompress()`)
	// Can not be mixed with the PACKET_FLAG_CONTROL!
	PACKET_FLAG_COMPRESSION = 1 << 5,
} PacketFlag;
```

packet flags
used for the packet header
multiple flags can be combined

but if control is set compression should not be set

# PacketHeader

## Syntax

```C
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
	uint8_t num_chunks;
	// DDNet security token. Is a 4 byte random integer
	// to avoid spoofing.
	// The token is placed at the end of the packet payload.
	// But conceptually it belongs into the header.
	Token token;
} PacketHeader;
```

Teeworlds packet header.
Parsed version of the first 3 bytes in
the udp payload.
Plus the security token.

Example:

```C
PacketHeader header;
header.flags = PACKET_FLAG_CONTROL | PACKET_FLAG_RESEND;
header.ack = 10;
header.num_chunks = 0; // control packets have no chunks
header.token = TOKEN_MAGIC;
```

# ControlMessageKind

## Syntax

```C
typedef enum {
	CTRL_MSG_KEEPALIVE,
	CTRL_MSG_CONNECT,
	CTRL_MSG_CONNECTACCEPT,
	CTRL_MSG_ACCEPT,
	CTRL_MSG_CLOSE,
} ControlMessageKind;
```

Type of control packet

# ControlMessage

## Syntax

```C
typedef struct {
	ControlMessageKind kind;
	const char *reason; // Can be set if msg_kind == CTRL_MSG_CLOSE
} ControlMessage;
```

Payload of control packets

# DDNetPacket

## Syntax

```C
typedef struct {
	PacketKind kind;
	PacketHeader header;
	uint8_t *payload;
	size_t payload_len;
	// The parsed packet payload
	// Check `kind` to know which field in the union to access
	union {
		ControlMessage control;
		struct {
			Chunk *data;
			size_t len;
		} chunks;
	};
} DDNetPacket;
```

Holds information about on full ddnet packet

# decode_packet_header

## Syntax

```C
PacketHeader decode_packet_header(const uint8_t *buf);
```

Unpacks packet header and fills the `PacketHeader` struct.

Warning it does not set the `token` because this one is at the end of
the payload.
So it is the responsibility of the payload unpacker to parse the token.
https://github.com/MilkeeyCat/ddnet_protocol/issues/54

# encode_packet_header

## Syntax

```C
DDNetError encode_packet_header(const PacketHeader *header, uint8_t *buf);
```

Given a `PacketHeader` as input it writes 3 bytes into `buf`

# get_packet_payload

## Syntax

```C
size_t get_packet_payload(PacketHeader *header, const uint8_t *full_data, size_t full_len, uint8_t *payload, size_t payload_len, DDNetError *err);
```

Extract and decompress packet payload.
Given a full raw packet as `full_data`
It will extract only the payload into `payload` and return the size of the payload.

# decode_packet

## Syntax

```C
DDNetPacket decode_packet(const uint8_t *buf, size_t len, DDNetError *err);
```

Given a pointer to the beginning of a udp payload
this determines the type of packet.

It returns `NULL` on error. Check the `err` value for more details.
Or a pointer to newly allocated memory that holds the parsed packet struct.
It is your responsibility to free it using `free_packet()`

# encode_packet

## Syntax

```C
size_t encode_packet(const DDNetPacket *packet, uint8_t *buf, size_t len, DDNetError *err);
```

Given a `Packet` struct it will encode a full udp payload
the output is written into `buf` which has to be at least `len` big
And returns the amount of written bytes

# free_packet

## Syntax

```C
DDNetError free_packet(DDNetPacket *packet);
```

Frees a packet struct and all of its fields

