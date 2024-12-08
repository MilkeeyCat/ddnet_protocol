# PACKET_MIN_HEADER_SIZE

## Syntax

```C
#define PACKET_MIN_HEADER_SIZE 3
```

minimum size in bytes required for a valid packet header

# PACKET_MAX_HEADER_SIZE

## Syntax

```C
#define PACKET_MAX_HEADER_SIZE 1400
```

maximum amount of total packet size

# PacketKind

## Syntax

```C
typedef enum {
	PACKET_CONNLESS,
	PACKET_CONTROL,
	PACKET_NORMAL,
} PacketKind;
```

internal enum for packet types
not sent over the network

# PacketFlag

## Syntax

```C
typedef enum {
	PACKET_FLAG_CONTROL = 1 << 2,
	PACKET_FLAG_CONNLESS = 1 << 3,
	PACKET_FLAG_RESEND = 1 << 4,
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
	uint16_t flags;
	uint16_t ack;
	uint8_t num_chunks;
	Token token;
} PacketHeader;
```

teeworlds packet header
parsed version of the first 7 bytes in
the udp payload

# decode_packet_header

## Syntax

```C
PacketHeader decode_packet_header(uint8_t *buf);
```

Unpacks packet header and fills the `PacketHeader` struct.

Warning it does not set the `token` because this one is at the end of
the payload.
So it is the responsibility of the payload unpacker to parse the token.
https://github.com/MilkeeyCat/ddnet_protocol/issues/54

# decode

## Syntax

```C
PacketKind *decode(uint8_t *buf, size_t len, Error *err);
```

Given a pointer to the beginning of a udp payload
this determins the type of packet.

It returns `NULL` on error. Check the `err` value for more details.
Or a pointer to newly allocated memory that holds the parsed packet struct.
It is your responsiblity to free that pointer!

