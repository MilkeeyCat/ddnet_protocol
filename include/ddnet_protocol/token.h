#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

// 4 byte security token used in packet headers
// to avoid ip spoofing
//
// See https://chillerdragon.github.io/teeworlds-protocol/06/fundamentals.html#tokens
typedef uint32_t Token;

// unsigned 32 bit integer
// that holds the magic value "TKEN"
// when interpreted as ASCII
//
// ```
// 0x544b454e
//   T K E N
// ```
//
// it is used to inform the peer that we support
// the security token protocol extension
// this is done for backwards compatibility
// because the 0.6 release of teeworlds
// had no security tokens
// and we (ddnet) are 0.6 compatible
extern const Token TOKEN_MAGIC;

// reads the first 4 bytes of `buf`
// and converts it into `Token` which
// is one uint32_t
//
// the data in `buf` is expected to be in network endianness
Token read_token(const uint8_t *buf);

// writes 4 bytes token into `buf`
void write_token(Token token, uint8_t *buf);

#ifdef __cplusplus
}
#endif
