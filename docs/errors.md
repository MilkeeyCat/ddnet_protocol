# DDNetError

## Syntax

```C
typedef enum {
#define X(err) err,
	DDNET_ERRORS
#undef X
} DDNetError;
```

Generic error enum, holds all kinds of errors returned by different
functions.

# ddnet_error_str

## Syntax

```C
const char *ddnet_error_str(DDNetError err);
```

Turns error enum into human readable string.

