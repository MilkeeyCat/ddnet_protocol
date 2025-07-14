# Token

## Syntax

```C
typedef uint32_t Token;
```

4 byte security token used in packet headers
to avoid ip spoofing

See https://chillerdragon.github.io/teeworlds-protocol/06/fundamentals.html#tokens

# TOKEN_MAGIC

## Syntax

```C
extern const Token TOKEN_MAGIC;
```

unsigned 32 bit integer
that holds the magic value "TKEN"
when interpreted as ASCII

```
0x544b454e
  T K E N
```

it is used to inform the peer that we support
the security token protocol extension
this is done for backwards compatibility
because the 0.6 release of teeworlds
had no security tokens
and we (ddnet) are 0.6 compatible

# TOKEN_NONE

## Syntax

```C
extern const Token TOKEN_NONE;
```

the token value that represents the absence of a token
sent during the token handshake as a placeholder

# read_token

## Syntax

```C
Token read_token(const uint8_t *buf);
```

reads the first 4 bytes of `buf`
and converts it into `Token` which
is one uint32_t

the data in `buf` is expected to be in network endianness

# write_token

## Syntax

```C
void write_token(Token token, uint8_t *buf);
```

writes 4 bytes token into `buf`

