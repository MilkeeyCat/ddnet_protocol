# DDNetSession

## Syntax

```C
typedef struct {
	// Acknowledged sequence number.
	// Tracking how many vital messages were successfully received.
	// This value should be included in the packet header.
	uint16_t ack;
	// Sequence number.
	// Tracking how many vital messages were sent.
	// This value should be included in every messages chunk header.
	// The first vital message sent should have sequence number 1
	// in its chunk header.
	uint16_t sequence;
	// Acknowledge sequence number of the other party.
	// This is the ``ack`` of the other peer.
	// This value should never be sent. It is being received from the
	// packet header the peer sent to us.
	// And we can use it to compare it to our ``sequence``
	// to make sure no messages were lost.
	uint16_t peer_ack;
	// DDNet security token. Is a 4 byte random integer
	// to avoid spoofing.
	// The token is placed at the end of the packet payload.
	// But conceptually it belongs into the packet header.
	// The token is determined by the server and both
	// client and server use the same value for one session.
	Token token;
} DDNetSession;
```

State of a ddnet network connection.

Holds ack/seq numbers that make up the teeworlds
reliability layer on top of udp.
And the ddnet security token against spoofing.

Every connection needs one of this struct on both sides.
The struct will be used in the same way on the server and the client side.

This struct is meant to be passed as mutable pointer to encode and decode methods.
Where its sequence numbers can be read and written to based on the amount of
vital messages packed and unpacked.

