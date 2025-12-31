# ddproto_str_sanitize_cc

## Syntax

```C
void ddproto_str_sanitize_cc(char *string);
```

Replaces all characters below ASCII 32 with whitespace.

# ddproto_str_sanitize

## Syntax

```C
void ddproto_str_sanitize(char *string);
```

Replaces all characters below ASCII 32 except \r, \n and \t with whitespace.

# ddproto_str_clean_whitespaces

## Syntax

```C
void ddproto_str_clean_whitespaces(char *string);
```

Removes leading and trailing spaces and limits the use of multiple spaces.

# DDProtoStringSanitize

## Syntax

```C
typedef enum {
	DDPROTO_STRING_SANITIZE_NONE = 0,
	DDPROTO_STRING_SANITIZE = 1 << 0,
	DDPROTO_STRING_SANITIZE_CC = 1 << 1,
	DDPROTO_STRING_SKIP_START_WHITESPACES = 1 << 2,
} DDProtoStringSanitize;
```

Used by `ddproto_unpacker_get_string_sanitized()` to strip unwanted characters
from the strings received from the peer.

# DDProtoUnpacker

## Syntax

```C
typedef struct {
	DDProtoError err;
	const uint8_t *buf_end;
	const uint8_t *buf;
} DDProtoUnpacker;
```

State for the unpacker.
Holds the data to be unpacked and keeps track of how much data was unpacked
and also tracks if errors occurred.

See also `ddproto_unpacker_init` and `ddproto_unpacker_get_int`.

# DDPROTO_PACKER_BUFFER_SIZE

## Syntax

```C
#define DDPROTO_PACKER_BUFFER_SIZE (1024 * 2)
```

Maximum output and storage size in bytes used by the `DDProtoPacker`.

# DDProtoPacker

## Syntax

```C
typedef struct {
	DDProtoError err;
	uint8_t *current;
	uint8_t *end;
	uint8_t buf[DDPROTO_PACKER_BUFFER_SIZE];
} DDProtoPacker;
```

State for the packer.
Holds the currently packed data and also tracks errors.

# ddproto_packer_init

## Syntax

```C
void ddproto_packer_init(DDProtoPacker *packer);
```

Initializes a new packer struct.
See also `ddproto_packer_init_msg()` if you want to send a net message.

# ddproto_packer_size

## Syntax

```C
size_t ddproto_packer_size(DDProtoPacker *packer);
```

Get the size in bytes of the currently packed data.
See also `ddproto_packer_data()`.

# ddproto_packer_remaining_size

## Syntax

```C
size_t ddproto_packer_remaining_size(DDProtoPacker *packer);
```

Amount of free bytes in the output buffer.
The packer can pack a maximum of `DDPROTO_PACKER_BUFFER_SIZE` bytes.

# ddproto_packer_data

## Syntax

```C
uint8_t *ddproto_packer_data(DDProtoPacker *packer);
```

Use in combination with `ddproto_packer_size()`.

# ddproto_packer_add_int

## Syntax

```C
DDProtoError ddproto_packer_add_int(DDProtoPacker *packer, int32_t value);
```

Packs `value` as teeworlds varint.
Call `ddproto_packer_data()` to receive the full packed data.

# ddproto_packer_add_string

## Syntax

```C
DDProtoError ddproto_packer_add_string(DDProtoPacker *packer, const char *value);
```

Packs `value` as plain null terminated C string.
Call `ddproto_packer_data()` to receive the full packed data.

# ddproto_packer_add_raw

## Syntax

```C
DDProtoError ddproto_packer_add_raw(DDProtoPacker *packer, const uint8_t *data, size_t size);
```

Packs `data` as raw data.
Call `ddproto_packer_data()` to receive the full packed data.

# ddproto_unpacker_init

## Syntax

```C
void ddproto_unpacker_init(DDProtoUnpacker *unpacker, const uint8_t *buf, size_t len);
```

Returns a new `DDProtoUnpacker` instance.
It keeps track of how much data was already unpacked.

```C
uint8_t bytes[] = {0x05, 0x01, 0x02};
DDProtoUnpacker unpacker;
ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
ddproto_unpacker_get_int(&unpacker); // => 5
ddproto_unpacker_get_int(&unpacker); // => 1
ddproto_unpacker_get_int(&unpacker); // => 2
unpacker.err; // => DDProtoError::DDPROTO_ERR_NONE
```

# ddproto_unpacker_remaining_size

## Syntax

```C
size_t ddproto_unpacker_remaining_size(DDProtoUnpacker *unpacker);
```

Amount of bytes that have not yet been unpacked.

# ddproto_unpacker_get_int

## Syntax

```C
int32_t ddproto_unpacker_get_int(DDProtoUnpacker *unpacker);
```

Use `ddproto_unpacker_init()` to get the value for `DDProtoUnpacker *unpacker`.
It returns the next integer in the unpacker data and also progresses the
internal unpacker state to point to the next element.

```C
uint8_t bytes[] = {0x05};
DDProtoUnpacker unpacker;
ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
ddproto_unpacker_get_int(&unpacker); // => 5
unpacker.err; // => DDProtoError::DDPROTO_ERR_NONE
```

# ddproto_unpacker_get_string

## Syntax

```C
const char *ddproto_unpacker_get_string(DDProtoUnpacker *unpacker);
```

Use `ddproto_unpacker_init()` to get the value for `DDProtoUnpacker *unpacker`.
It returns the next null terminated string in the unpacker data and also
progresses the internal unpacker state to point to the next element.

Applies `STRING_SANITIZE` by default. If you want a string without
sanitization use:

```C
uint8_t bytes[] = {'f', 'o', 0x03, 'o', 0x00};
DDProtoUnpacker unpacker;
ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
ddproto_unpacker_get_string_sanitized(&unpacker, DDPROTO_STRING_SANITIZE_NONE);
```

# ddproto_unpacker_get_string_sanitized

## Syntax

```C
const char *ddproto_unpacker_get_string_sanitized(DDProtoUnpacker *unpacker, DDProtoStringSanitize sanitize);
```

Use `ddproto_unpacker_init` to get the value for `DDProtoUnpacker *unpacker`.
It returns the next null terminated string in the unpacker data and also
progresses the internal unpacker state to point to the next element.

```C
uint8_t bytes[] = {'f', 'o', 'o', 0x00};
DDProtoUnpacker unpacker;
ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

ddproto_unpacker_get_string_sanitized(&unpacker, DDPROTO_STRING_SANITIZE_CC); // => foo
unpacker.err; // =>  DDProtoError::DDPROTO_ERR_NONE
```

# ddproto_unpacker_get_bool

## Syntax

```C
bool ddproto_unpacker_get_bool(DDProtoUnpacker *unpacker);
```

Use `ddproto_unpacker_init` to get the value for `DDProtoUnpacker *unpacker`.
It returns the next boolean in the unpacker data
and also progresses the internal unpacker state to point to the next element

Might set the unpacker->err to `DDPROTO_ERR_INVALID_BOOL`

```C
uint8_t bytes[] = {0x00, 0x01, 0xcc};
DDProtoUnpacker unpacker;
ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));

ddproto_unpacker_get_bool(&unpacker); // => false
ddproto_unpacker_get_bool(&unpacker); // => true
ddproto_unpacker_get_bool(&unpacker); // => false (invalid boolean)
unpacker.err; // => DDProtoError::DDPROTO_ERR_INVALID_BOOL
```

# ddproto_unpacker_get_raw

## Syntax

```C
const uint8_t *ddproto_unpacker_get_raw(DDProtoUnpacker *unpacker, size_t len);
```

Use `ddproto_unpacker_init` to get the value for `DDProtoUnpacker *unpacker`.
It returns the next `len` amount of bytes in the unpacker data and also
progresses the internal unpacker state to point to the next element.

```C
uint8_t bytes[] = {0x05};
DDProtoUnpacker unpacker;
ddproto_unpacker_init(&unpacker, bytes, sizeof(bytes));
ddproto_unpacker_get_raw(&unpacker, 1); // => 0x05
unpacker.err; // => DDProtoError::DDPROTO_ERR_NONE
```

