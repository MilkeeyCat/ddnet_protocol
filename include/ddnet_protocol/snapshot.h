#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "errors.h"
#include "packer.h"
#include "snap_items.h"

/// @attention This is not the snap item id that is sent over the network.
typedef enum {
	// objects
	DDPROTO_ITEM_KIND_UNKNOWN,
	DDPROTO_ITEM_KIND_PROJECTILE,
	DDPROTO_ITEM_KIND_LASER,
	DDPROTO_ITEM_KIND_PICKUP,
	DDPROTO_ITEM_KIND_FLAG,
	DDPROTO_ITEM_KIND_GAME_INFO,
	DDPROTO_ITEM_KIND_GAME_DATA,
	DDPROTO_ITEM_KIND_CHARACTER,
	DDPROTO_ITEM_KIND_CLIENT_INFO,
	DDPROTO_ITEM_KIND_PLAYER_INFO,
	DDPROTO_ITEM_KIND_SPECTATOR_INFO,

	// events
	DDPROTO_ITEM_KIND_EXPLOSION,
	DDPROTO_ITEM_KIND_SPAWN,
	DDPROTO_ITEM_KIND_HAMMER_HIT,
	DDPROTO_ITEM_KIND_DEATH,
	DDPROTO_ITEM_KIND_SOUND_GLOBAL,
	DDPROTO_ITEM_KIND_SOUND_WORLD,
	DDPROTO_ITEM_KIND_DAMAGE_INDICATOR,
} DDProtoSnapItemKind;

/// The type id that is used in the snapshot payload to identify the type of an
/// item.
typedef enum {
	// objects
	DDPROTO_ITEM_PROJECTILE = 2,
	DDPROTO_ITEM_LASER = 3,
	DDPROTO_ITEM_PICKUP = 4,
	DDPROTO_ITEM_FLAG = 5,
	DDPROTO_ITEM_GAME_INFO = 6,
	DDPROTO_ITEM_GAME_DATA = 7,
	DDPROTO_ITEM_CHARACTER = 9,
	DDPROTO_ITEM_PLAYER_INFO = 10,
	DDPROTO_ITEM_CLIENT_INFO = 11,
	DDPROTO_ITEM_SPECTATOR_INFO = 12,

	// events
	/// This type id is unused. Event common is a meta event. That builds the
	/// base of all events. Event common as is will never be placed alone in the
	/// snapshot so type id 13 is unused.
	DDPROTO_ITEM_EVENT_COMMON = 13,
	DDPROTO_ITEM_EXPLOSION = 14,
	DDPROTO_ITEM_SPAWN = 15,
	DDPROTO_ITEM_HAMMER_HIT = 16,
	DDPROTO_ITEM_DEATH = 17,
	DDPROTO_ITEM_SOUND_GLOBAL = 18,
	DDPROTO_ITEM_SOUND_WORLD = 19,
	DDPROTO_ITEM_DAMAGE_INDICATOR = 20,
} DDProtoSnapItemTypeId;

/// Union abstracting away any kind of game or system message. Check the @ref
/// DDProtoMessageKind to know which one to use.
typedef union {
	/// ddnet_protocol specific item to represent a unknown snapshot item. This
	/// item kind does not exist in the reference implementation.
	DDProtoObjUnknown unknown;

	// objects
	DDProtoObjProjectile projectile;
	DDProtoObjLaser laser;
	DDProtoObjPickup pickup;
	DDProtoObjFlag flag;
	DDProtoObjGameInfo game_info;
	DDProtoObjGameData game_data;
	DDProtoObjCharacter character;
	DDProtoObjPlayerInfo player_info;
	DDProtoObjClientInfo client_info;
	DDProtoObjSpectatorInfo spectator_info;

	// events
	/// Meta event that is never sent as is.
	DDProtoEventCommon common;
	DDProtoEventExplosion explosion;
	DDProtoEventSpawn spawn;
	DDProtoEventHammerHit hammer_hit;
	DDProtoEventDeath death;
	DDProtoEventSoundGlobal sound_global;
	DDProtoEventSoundWorld sound_world;
	DDProtoEventDamageIndicator damage_indicator;
} DDProtoGenericSnapItem;

/// To access the item struct check the @ref DDProtoSnapItem.kind and access the
/// @ref DDProtoSnapItem.item union accordingly.
typedef struct {
	DDProtoSnapItemKind kind;
	DDProtoGenericSnapItem item;
} DDProtoSnapItem;

typedef struct {
	struct {
		int32_t *data;
		size_t len;
	} removed_keys;
	struct {
		// should be either `NULL` or point to memory of size
		// `items.len * sizeof(DDProtoSnapItem)`
		DDProtoSnapItem *data;

		// should be either `0` or match the allocated size of `items.data` in
		// `sizeof(DDProtoSnapItem)` otherwise you might run into segfaults
		size_t len;
	} items;
} DDProtoSnapshot;

/// Consumes data from the unpacker and writes the parsed item to the output
/// parameter `item`.
DDProtoError ddproto_decode_snap_item(DDProtoUnpacker *unpacker, DDProtoSnapItem *item);

/// Frees the memory allocated for the snap items.
void ddproto_free_snapshot(DDProtoSnapshot *snap);

/// Given a unpacker holding data beginning with a snapshot payload this parses
/// the snapshot header and item deltas. Beginning of snapshot payload is
/// defined as the data field of the system net message that holds the snap. So
/// the first byte of the unpackers data is the start of the amount of removed
/// items.
///
/// It writes the parsed snapshot to the `snap` output parameter. And reads from
/// the `unpacker` by consuming its data.
///
/// This function allocates memory for the snap items. You need to free it by
/// calling @ref ddproto_free_snapshot. You do not need to free it if the
/// snapshot is part of a packet and you already call @ref ddproto_free_packet.
DDProtoError ddproto_decode_snapshot(DDProtoUnpacker *unpacker, DDProtoSnapshot *snap);

#ifdef __cplusplus
}
#endif
