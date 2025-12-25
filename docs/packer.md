# ddnet_str_sanitize_cc

## Syntax

```C
void ddnet_str_sanitize_cc(char *string);
```

Replaces all characters below ASCII 32 with whitespace.

# ddnet_str_sanitize

## Syntax

```C
void ddnet_str_sanitize(char *string);
```

Replaces all characters below ASCII 32 except \r, \n and \t with whitespace.

# ddnet_str_clean_whitespaces

## Syntax

```C
void ddnet_str_clean_whitespaces(char *string);
```

Removes leading and trailing spaces and limits the use of multiple spaces.

# DDNetStringSanitize

## Syntax

```C
typedef enum {
	DDNET_STRING_SANITIZE_NONE = 0,
	DDNET_STRING_SANITIZE = 1 << 0,
	DDNET_STRING_SANITIZE_CC = 1 << 1,
	DDNET_STRING_SKIP_START_WHITESPACES = 1 << 2,
} DDNetStringSanitize;
```

Used by `ddnet_unpacker_get_string_sanitized()` to strip unwanted characters
from the strings received from the peer.

# DDNetUnpacker

## Syntax

```C
typedef struct {
	DDNetError err;
	const uint8_t *buf_end;
	const uint8_t *buf;
} DDNetUnpacker;
```

State for the unpacker.
Holds the data to be unpacked and keeps track of how much data was unpacked
and also tracks if errors occurred.

See also `ddnet_unpacker_init` and `ddnet_unpacker_get_int`.

# DDNET_PACKER_BUFFER_SIZE

## Syntax

```C
#define DDNET_PACKER_BUFFER_SIZE (1024 * 2)
```

Maximum output and storage size in bytes used by the `DDNetPacker`.

# DDNetPacker

## Syntax

```C
typedef struct {
	DDNetError err;
	uint8_t *current;
	uint8_t *end;
	uint8_t buf[DDNET_PACKER_BUFFER_SIZE];
} DDNetPacker;
```

State for the packer.
Holds the currently packed data and also tracks errors.

# ddnet_packer_init

## Syntax

```C
void ddnet_packer_init(DDNetPacker *packer);
```

Initializes a new packer struct.
See also `ddnet_packer_init_msg()` if you want to send a net message.

# ddnet_packer_size

## Syntax

```C
size_t ddnet_packer_size(DDNetPacker *packer);
```

Get the size in bytes of the currently packed data.
See also `ddnet_packer_data()`.

# ddnet_packer_remaining_size

## Syntax

```C
size_t ddnet_packer_remaining_size(DDNetPacker *packer);
```

Amount of free bytes in the output buffer.
The packer can pack a maximum of `DDNET_PACKER_BUFFER_SIZE` bytes.

# ddnet_packer_data

## Syntax

```C
uint8_t *ddnet_packer_data(DDNetPacker *packer);
```

Use in combination with `ddnet_packer_size()`.

# ddnet_packer_add_int

## Syntax

```C
DDNetError ddnet_packer_add_int(DDNetPacker *packer, int32_t value);
```

Packs `value` as teeworlds varint.
Call `ddnet_packer_data()` to receive the full packed data.

# ddnet_packer_add_string

## Syntax

```C
DDNetError ddnet_packer_add_string(DDNetPacker *packer, const char *value);
```

Packs `value` as plain null terminated C string.
Call `ddnet_packer_data()` to receive the full packed data.

# ddnet_packer_add_raw

## Syntax

```C
DDNetError ddnet_packer_add_raw(DDNetPacker *packer, const uint8_t *data, size_t size);
```

Packs `data` as raw data.
Call `ddnet_packer_data()` to receive the full packed data.

# ddnet_unpacker_init

## Syntax

```C
void ddnet_unpacker_init(DDNetUnpacker *unpacker, const uint8_t *buf, size_t len);
```

Returns a new `DDNetUnpacker` instance.
It keeps track of how much data was already unpacked.

```C
uint8_t bytes[] = {0x05, 0x01, 0x02};
DDNetUnpacker unpacker;
ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
ddnet_unpacker_get_int(&unpacker); // => 5
ddnet_unpacker_get_int(&unpacker); // => 1
ddnet_unpacker_get_int(&unpacker); // => 2
unpacker.err; // => DDNetError::DDNET_ERR_NONE
```

# ddnet_unpacker_remaining_size

## Syntax

```C
size_t ddnet_unpacker_remaining_size(DDNetUnpacker *unpacker);
```

Amount of bytes that have not yet been unpacked.

# ddnet_unpacker_get_int

## Syntax

```C
int32_t ddnet_unpacker_get_int(DDNetUnpacker *unpacker);
```

Use `ddnet_unpacker_init()` to get the value for `DDNetUnpacker *unpacker`.
It returns the next integer in the unpacker data and also progresses the
internal unpacker state to point to the next element.

```C
uint8_t bytes[] = {0x05};
DDNetUnpacker unpacker;
ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
ddnet_unpacker_get_int(&unpacker); // => 5
unpacker.err; // => DDNetError::DDNET_ERR_NONE
```

# ddnet_unpacker_get_string

## Syntax

```C
const char *ddnet_unpacker_get_string(DDNetUnpacker *unpacker);
```

Use `ddnet_unpacker_init()` to get the value for `DDNetUnpacker *unpacker`.
It returns the next null terminated string in the unpacker data and also
progresses the internal unpacker state to point to the next element.

Applies `STRING_SANITIZE` by default. If you want a string without
sanitization use:

```C
uint8_t bytes[] = {'f', 'o', 0x03, 'o', 0x00};
DDNetUnpacker unpacker;
ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
ddnet_unpacker_get_string_sanitized(&unpacker, DDNET_STRING_SANITIZE_NONE);
```

# ddnet_unpacker_get_string_sanitized

## Syntax

```C
const char *ddnet_unpacker_get_string_sanitized(DDNetUnpacker *unpacker, DDNetStringSanitize sanitize);
```

Use `ddnet_unpacker_init` to get the value for `DDNetUnpacker *unpacker`.
It returns the next null terminated string in the unpacker data and also
progresses the internal unpacker state to point to the next element.

```C
uint8_t bytes[] = {'f', 'o', 'o', 0x00};
DDNetUnpacker unpacker;
ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));

ddnet_unpacker_get_string_sanitized(&unpacker, DDNET_STRING_SANITIZE_CC); // => foo
unpacker.err; // =>  DDNetError::DDNET_ERR_NONE
```

# ddnet_unpacker_get_bool

## Syntax

```C
bool ddnet_unpacker_get_bool(DDNetUnpacker *unpacker);
```

Use `ddnet_unpacker_init` to get the value for `DDNetUnpacker *unpacker`.
It returns the next boolean in the unpacker data
and also progresses the internal unpacker state to point to the next element

Might set the unpacker->err to `DDNET_ERR_INVALID_BOOL`

```C
uint8_t bytes[] = {0x00, 0x01, 0xcc};
DDNetUnpacker unpacker;
ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));

ddnet_unpacker_get_bool(&unpacker); // => false
ddnet_unpacker_get_bool(&unpacker); // => true
ddnet_unpacker_get_bool(&unpacker); // => false (invalid boolean)
unpacker.err; // => DDNetError::DDNET_ERR_INVALID_BOOL
```

# ddnet_unpacker_get_raw

## Syntax

```C
const uint8_t *ddnet_unpacker_get_raw(DDNetUnpacker *unpacker, size_t len);
```

Use `ddnet_unpacker_init` to get the value for `DDNetUnpacker *unpacker`.
It returns the next `len` amount of bytes in the unpacker data and also
progresses the internal unpacker state to point to the next element.

```C
uint8_t bytes[] = {0x05};
DDNetUnpacker unpacker;
ddnet_unpacker_init(&unpacker, bytes, sizeof(bytes));
ddnet_unpacker_get_raw(&unpacker, 1); // => 0x05
unpacker.err; // => DDNetError::DDNET_ERR_NONE
```

