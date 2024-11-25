# Unpacker

## Syntax

```C
typedef struct {
	Error err;
	uint8_t *buf_end;
	uint8_t *buf;
} Unpacker;
```

State for the unpacker
holds the data to be unpacked
and keeps track of how much data was unpacked
and also tracks if errors occured

See also `unpacker_new` and `unpacker_get_int`

# unpacker_new

## Syntax

```C
Unpacker unpacker_new(uint8_t *buf, size_t len);
```

returns a new `Unpacker` instance
it keeps track of how much data was already unpacked

```C
uint8_t bytes[] = {0x05, 0x01, 0x02};
Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
unpacker_get_int(&unpacker); // => 5
unpacker_get_int(&unpacker); // => 1
unpacker_get_int(&unpacker); // => 2
unpacker.err; // => Error::ERR_NONE
```

# unpacker_get_int

## Syntax

```C
int32_t unpacker_get_int(Unpacker *state);
```

Use `unpacker_new` to get the value for `Unpacker *state`
it returns the next integer in the unpacker data
and also progresses the internal unpacker state to point to the next element

```C
uint8_t bytes[] = {0x05};
Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
unpacker_get_int(&unpacker); // => 5
unpacker.err; // => Error::ERR_NONE
```

# unpacker_get_bool

## Syntax

```C
bool unpacker_get_bool(Unpacker *state);
```

Use `unpacker_new` to get the value for `Unpacker *state`
it returns the next boolean in the unpacker data
and also progresses the internal unpacker state to point to the next element

Might set the state->err to `ERR_INVALID_BOOL`

```C
uint8_t bytes[] = {0x00, 0x01, 0xcc};
Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));

unpacker_get_bool(&unpacker) // => false
unpacker_get_bool(&unpacker) // => true
unpacker_get_bool(&unpacker) // => false (invalid boolean)
unpacker.err; // => Error::ERR_INVALID_BOOL
```

# unpacker_get_raw

## Syntax

```C
const uint8_t *unpacker_get_raw(Unpacker *state, size_t len);
```

Use `unpacker_new` to get the value for `Unpacker *state`
it returns the next `len` amount of bytes in the unpacker data
and also progresses the internal unpacker state to point to the next element

```C
uint8_t bytes[] = {0x05};
Unpacker unpacker = unpacker_new(bytes, sizeof(bytes));
unpacker_get_raw(&unpacker, 1); // => 0x05
unpacker.err; // => Error::ERR_NONE
```

