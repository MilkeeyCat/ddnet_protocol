#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"
#include "snapshot.h"

typedef struct {
	uint8_t data[DDPROTO_SNAPSHOT_MAX_SIZE];
	int32_t offsets[DDPROTO_SNAPSHOT_MAX_ITEMS];
	int32_t num_items;
	int32_t data_size;
} DDProtoSnapBuilder;

/// Initializes a new builder instance. Call this before using it.
void ddproto_snap_builder_init(DDProtoSnapBuilder *builder);

/// Call ddproto_snap_builder_init() on the builder instance first. Before using
/// this function. It will allocate the space for a new snap item and return a
/// pointer to the memory which you then can fill with the snap item properties.
///
/// The item_size is the size in bytes that will be allocated for the item.
/// Returns NULL on error and writes details to the err parameter.
///
/// Example:
///
/// ```C
/// DDProtoSnapBuilder builder = {};
/// DDProtoError err = DDPROTO_ERR_NONE;
/// ddproto_snap_builder_init(&builder);
/// DDProtoEventExplosion *explosion = ddproto_snap_builder_new_item(&builder, DDPROTO_ITEM_EXPLOSION, 0, sizeof(DDProtoEventExplosion), &err);
/// explosion->x = 10;
/// explosion->y = 10;
/// ```
void *ddproto_snap_builder_new_item(DDProtoSnapBuilder *builder, int32_t item_type, int32_t item_id, int32_t item_size, DDProtoError *err);

/// Writes the final snapshot to the output parameter snapshot. And returns the
/// size in bytes that was written.
int32_t ddproto_snap_builder_finish(DDProtoSnapBuilder *builder, DDProtoSnapshot *snap, DDProtoError *err);

#ifdef __cplusplus
}
#endif
