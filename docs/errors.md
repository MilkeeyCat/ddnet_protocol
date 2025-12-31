# DDProtoError

## Syntax

```C
typedef enum {
#define X(err) err,
	DDPROTO_ERRORS
#undef X
} DDProtoError;
```

Generic error enum, holds all kinds of errors returned by different
functions.

# ddproto_error_str

## Syntax

```C
const char *ddproto_error_str(DDProtoError err);
```

Turns error enum into human readable string.

