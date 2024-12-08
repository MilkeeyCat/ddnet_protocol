#pragma once

#ifdef BLOAT
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
#ifndef _MSC_VER
typedef long unsigned int size_t;
#endif
#ifndef __cplusplus
typedef enum { false,
	true } bool;
#endif

extern void *memset(void *s, int c, size_t n);
extern void *malloc(size_t size);
extern void free(void *ptr);
extern char *strncpy(char *dest, const char *src, size_t len);
#ifndef strlen
extern size_t strlen(const char *str);
#endif
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
