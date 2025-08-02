#pragma once

#define KERNEL_SPACE 1
#define USER_SPACE 2

#ifdef __cplusplus
extern "C" {
#endif

#if CODE_SPACE == KERNEL_SPACE
// Non Linux users are in shambles
#include <linux/string.h>
#include <linux/types.h>

extern void *malloc(size_t size);
extern void free(void *ptr);
#elif CODE_SPACE == USER_SPACE
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#elif !defined(CODE_SPACE)
#error \
	"CODE_SPACE is not defined. Define it to `KERNEL_SPACE` or `USER_SPACE`."
#else
#error \
	"CODE_SPACE has invalid value, it must be `KERNEL_SPACE` or `USER_SPACE`."
#endif

#ifdef __cplusplus
}
#endif
