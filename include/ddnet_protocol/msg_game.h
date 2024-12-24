#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

typedef struct {
	const char *name;
	const char *clan;
	size_t country;
	const char *skin;
	bool use_custom_color;
	size_t color_body;
	size_t color_feet;
} MsgClStartInfo;

#ifdef __cplusplus
}
#endif
