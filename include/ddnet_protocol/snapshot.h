#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"
#include "packer.h"
#include "snap_items.h"

// Be careful this is not the snap item id
// that is sent over the network!
typedef enum {
	DDNET_ITEM_KIND_UNKNOWN,
	DDNET_ITEM_KIND_GAME_INFO,
	DDNET_ITEM_KIND_GAME_DATA,
	DDNET_ITEM_KIND_CHARACTER,
	DDNET_ITEM_KIND_CLIENT_INFO,
	DDNET_ITEM_KIND_PLAYER_INFO,
} DDNetSnapItemKind;

// the type id that is used in the snapshot
// payload to identify the type of an item.
typedef enum {
	DDNET_ITEM_GAME_INFO = 6,
	DDNET_ITEM_GAME_DATA = 7,
	DDNET_ITEM_CHARACTER = 9,
	DDNET_ITEM_PLAYER_INFO = 10,
	DDNET_ITEM_CLIENT_INFO = 11,
} DDNetItemTypeId;

// Union abstracting away any kind of game or system message
// Check the DDNetMessageKind to know which one to use
typedef union {
	// ddnet_protocol specific item to represent
	// a unknown snapshot item
	// this item kind does not exist in the reference implementation
	DDNetObjUnknown unknown;

	// objects
	DDNetObjGameInfo game_info;
	DDNetObjGameData game_data;
	DDNetObjCharacter character;
	DDNetObjPlayerInfo player_info;
	DDNetObjClientInfo client_info;

	// events
	// ...
} DDNetGenericSnapItem;

// To access the item struct check the `kind`
// and access the `item` union accordingly.
typedef struct {
	DDNetSnapItemKind kind;
	DDNetGenericSnapItem item;
} DDNetSnapItem;

typedef struct {
	struct {
		int32_t *data;
		size_t len;
	} removed_keys;
	struct {
		// should be either `NULL`
		// or point to memory of size `items.len * sizeof(DDNetSnapItem)`
		DDNetSnapItem *data;

		// should be either `0`
		// or match the allocated size of `items.data` in `sizeof(DDNetSnapItem)`
		// otherwise you might run into segfaults
		size_t len;
	} items;
} DDNetSnapshot;

// Consumes data from the unpacker and writes
// the parsed item to the output parameter `item`.
DDNetError ddnet_decode_snap_item(DDNetUnpacker *unpacker, DDNetSnapItem *item);

// frees the memory allocated for the snap items
void ddnet_free_snapshot(DDNetSnapshot *snap);

// Given a unpacker holding data beginning with a snapshot payload
// this parses the snapshot header and item deltas.
// Beginning of snapshot payload is defined as the
// data field of the system net message that holds the snap.
// So the first byte of the unpackers data is the start of the amount of removed items.
//
// It writes the parsed snapshot to the `snap` output parameter.
// And reads from the `unpacker` by consuming its data.
//
// This function allocates memory for the snap items.
// You need to free it by calling `ddnet_free_snapshot()`
// You do not need to free it if the snapshot is part of a packet
// and you already call `ddnet_free_packet()`
DDNetError ddnet_decode_snapshot(DDNetUnpacker *unpacker, DDNetSnapshot *snap);

#ifdef __cplusplus
}
#endif
