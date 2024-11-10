#pragma once

#ifdef BLOAT
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#else
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef long unsigned int size_t;

extern void *malloc(size_t size);
extern char *strncpy(char *dest, const char *src, size_t len);
#ifndef strlen
extern size_t strlen(const char *str);
#endif
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
