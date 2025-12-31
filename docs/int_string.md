# ddproto_str_length

## Syntax

```C
size_t ddproto_str_length(const char *str);
```

Get the length of a string in bytes.

# ddproto_ints_to_str

## Syntax

```C
DDProtoError ddproto_ints_to_str(const uint32_t *ints, size_t num_ints, char *str, size_t str_len);
```

Convert list of integers into a string.
Used by the snapshot which technically only consists of integers, but also holds strings.

# ddproto_str_to_ints

## Syntax

```C
DDProtoError ddproto_str_to_ints(uint32_t *ints, size_t num_ints, const char *str);
```

Convert string to a list of integers.
Used by the snapshot which technically only consists of integers, but also holds strings.
If the string is too big and does not fit into `num_ints` amount of integers the function
returns an error. If the string is so short that it does not need `num_ints` integers
to be represented it still fills all the integers with padding.

