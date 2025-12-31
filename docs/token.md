# DDProtoToken

## Syntax

```C
typedef uint32_t DDProtoToken;
```

4 byte security token used in packet headers to avoid ip spoofing.

See https://chillerdragon.github.io/teeworlds-protocol/06/fundamentals.html#tokens.

# DDPROTO_TOKEN_MAGIC

## Syntax

```C
extern const DDProtoToken DDPROTO_TOKEN_MAGIC;
```

Unsigned 32 bit integer that holds the magic value "TKEN" when interpreted as
ASCII.

```
0x544b454e
  T K E N
```

It is used to inform the peer that we support the security token protocol
extension this is done for backwards compatibility because the 0.6 release of
teeworlds had no security tokens and we (ddnet) are 0.6 compatible.

# DDPROTO_TOKEN_NONE

## Syntax

```C
extern const DDProtoToken DDPROTO_TOKEN_NONE;
```

The token value that represents the absence of a token sent during the token
handshake as a placeholder.

# ddproto_read_token

## Syntax

```C
DDProtoToken ddproto_read_token(const uint8_t *buf);
```

Reads the first 4 bytes of `buf` and converts it into `Token` which is one
uint32_t.

The data in `buf` is expected to be in network endianness.

# ddproto_write_token

## Syntax

```C
void ddproto_write_token(DDProtoToken token, uint8_t *buf);
```

Writes 4 bytes token into `buf`.

