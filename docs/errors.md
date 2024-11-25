# Error

## Syntax

```C
typedef enum {
	ERR_NONE,
	ERR_INVALID_PACKET,
	ERR_INVALID_CONTROL_MESSAGE,
	ERR_INVALID_TOKEN_MAGIC,
	ERR_EMPTY_BUFFER,
	ERR_INVALID_BOOL,
} Error;
```

generic error enum
holds all kind of errors returned
by different functions

