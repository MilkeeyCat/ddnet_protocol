#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"
#include "packer.h"
#include "snap_items.h"

// Be careful this is not the snap item id that is sent over the network!
typedef enum {
	// objects
	DDNET_ITEM_KIND_UNKNOWN,
	DDNET_ITEM_KIND_PROJECTILE,
	DDNET_ITEM_KIND_LASER,
	DDNET_ITEM_KIND_PICKUP,
	DDNET_ITEM_KIND_FLAG,
	DDNET_ITEM_KIND_GAME_INFO,
	DDNET_ITEM_KIND_GAME_DATA,
	DDNET_ITEM_KIND_CHARACTER,
	DDNET_ITEM_KIND_CLIENT_INFO,
	DDNET_ITEM_KIND_PLAYER_INFO,
	DDNET_ITEM_KIND_SPECTATOR_INFO,

	// events
	DDNET_ITEM_KIND_EXPLOSION,
	DDNET_ITEM_KIND_SPAWN,
	DDNET_ITEM_KIND_HAMMER_HIT,
	DDNET_ITEM_KIND_DEATH,
	DDNET_ITEM_KIND_SOUND_GLOBAL,
	DDNET_ITEM_KIND_SOUND_WORLD,
	DDNET_ITEM_KIND_DAMAGE_INDICATOR,
} DDNetSnapItemKind;

// The type id that is used in the snapshot payload to identify the type of an
// item.
typedef enum {
	/**
	 * objects
	 */

	DDNET_ITEM_PROJECTILE = 2,
	DDNET_ITEM_LASER = 3,
	DDNET_ITEM_PICKUP = 4,
	DDNET_ITEM_FLAG = 5,
	DDNET_ITEM_GAME_INFO = 6,
	DDNET_ITEM_GAME_DATA = 7,
	DDNET_ITEM_CHARACTER = 9,
	DDNET_ITEM_PLAYER_INFO = 10,
	DDNET_ITEM_CLIENT_INFO = 11,
	DDNET_ITEM_SPECTATOR_INFO = 12,

	/**
	 * events
	 */

	// This type id is unused. Event common is a meta event. That builds the
	// base of all events. Event common as is will never be placed alone in the
	// snapshot so type id 13 is unused.
	DDNET_ITEM_EVENT_COMMON = 13,
	DDNET_ITEM_EXPLOSION = 14,
	DDNET_ITEM_SPAWN = 15,
	DDNET_ITEM_HAMMER_HIT = 16,
	DDNET_ITEM_DEATH = 17,
	DDNET_ITEM_SOUND_GLOBAL = 18,
	DDNET_ITEM_SOUND_WORLD = 19,
	DDNET_ITEM_DAMAGE_INDICATOR = 20,
} DDNetSnapItemTypeId;

// Union abstracting away any kind of game or system message. Check the
// `DDNetMessageKind` to know which one to use.
typedef union {
	// ddnet_protocol specific item to represent a unknown snapshot item. This
	// item kind does not exist in the reference implementation.
	DDNetObjUnknown unknown;

	/**
	 * objects
	 */

	DDNetObjProjectile projectile;
	DDNetObjLaser laser;
	DDNetObjPickup pickup;
	DDNetObjFlag flag;
	DDNetObjGameInfo game_info;
	DDNetObjGameData game_data;
	DDNetObjCharacter character;
	DDNetObjPlayerInfo player_info;
	DDNetObjClientInfo client_info;
	DDNetObjSpectatorInfo spectator_info;

	/**
	 * events
	 */

	// Meta event that is never sent as is.
	DDNetEventCommon common;
	DDNetEventExplosion explosion;
	DDNetEventSpawn spawn;
	DDNetEventHammerHit hammer_hit;
	DDNetEventDeath death;
	DDNetEventSoundGlobal sound_global;
	DDNetEventSoundWorld sound_world;
	DDNetEventDamageIndicator damage_indicator;
} DDNetGenericSnapItem;

// To access the item struct check the `kind` and access the `item` union
// accordingly.
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
		// should be either `NULL` or point to memory of size
		// `items.len * sizeof(DDNetSnapItem)`
		DDNetSnapItem *data;

		// should be either `0` or match the allocated size of `items.data` in
		// `sizeof(DDNetSnapItem)` otherwise you might run into segfaults
		size_t len;
	} items;
} DDNetSnapshot;

// Consumes data from the unpacker and writes the parsed item to the output
// parameter `item`.
DDNetError ddnet_decode_snap_item(DDNetUnpacker *unpacker, DDNetSnapItem *item);

// Frees the memory allocated for the snap items.
void ddnet_free_snapshot(DDNetSnapshot *snap);

// Given a unpacker holding data beginning with a snapshot payload this parses
// the snapshot header and item deltas. Beginning of snapshot payload is defined
// as the data field of the system net message that holds the snap. So the first
// byte of the unpackers data is the start of the amount of removed items.
//
// It writes the parsed snapshot to the `snap` output parameter. And reads from
// the `unpacker` by consuming its data.
//
// This function allocates memory for the snap items. You need to free it by
// calling `ddnet_free_snapshot()`. You do not need to free it if the snapshot
// is part of a packet and you already call `ddnet_free_packet()`.
DDNetError ddnet_decode_snapshot(DDNetUnpacker *unpacker, DDNetSnapshot *snap);

#ifdef __cplusplus
}
#endif
