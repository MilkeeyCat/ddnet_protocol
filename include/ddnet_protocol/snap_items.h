#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "limits.h"
#include "msg_game.h"

// Gamemode flags. These flags can be combined. And they are used by the
// `DDNetObjGameInfo` snapshot item.
typedef enum {
	// If this flag is set the current gamemode has vanilla teams. Meaning there
	// is `DDNET_TEAM_RED` and `DDNET_TEAM_BLUE`. If the flag is not set the
	// `DDNET_TEAM_GAME` will be used for all players.
	DDNET_GAMEFLAG_TEAMS = 1 << 0,

	// If this flag is set the current gamemode has vanilla flags. A flag being
	// a flag in the world that comes from the capture the flag gametype (ctf).
	DDNET_GAMEFLAG_FLAGS = 1 << 1,
} DDNetGameFlag;

// The current gamestate. Only one of these flags can be used. This flag is used
// by the `DDNetObjGameInfo` snapshot item.
typedef enum {
	// The world is paused and the scoreboard is displayed.
	DDNET_GAMESTATEFLAG_GAMEOVER = 1 << 0,

	// The round will end on the next score.
	DDNET_GAMESTATEFLAG_SUDDENDEATH = 1 << 1,

	// The world is paused.
	DDNET_GAMESTATEFLAG_PAUSED = 1 << 2,
} DDNetGameStateFlag;

// This enum represents teeworlds vanilla teams. These teams are different than
// ddrace teams that were added by ddnet.
typedef enum {
	DDNET_TEAM_SPECTATORS = -1,
	DDNET_TEAM_GAME = 0,
	DDNET_TEAM_RED = 0,
	DDNET_TEAM_BLUE = 1,
	DDNET_NUM_TEAMS = 2,
} DDNetTeam;

// Used to determine a pickup type.
typedef enum {
	DDNET_POWERUP_HEALTH = 0,
	DDNET_POWERUP_ARMOR = 1,
	DDNET_POWERUP_WEAPON = 2,
	DDNET_POWERUP_NINJA = 3
} DDNetPowerup;

// Used by the flag carrier fields in the `DDNetObjGameData` snap item. Values
// from 0 to max clients is a client id. And if it is not a client id it is one
// of these 3 negative values.
typedef enum {
	DDNET_FLAG_MISSING = -3,
	DDNET_FLAG_ATSTAND = -2,
	DDNET_FLAG_TAKEN = -1,
} DDNetFlag;

// Special values for spectator id used by `DDNetObjSpectatorInfo`
typedef enum {
	DDNET_SPEC_FREEVIEW = -1,
	DDNET_SPEC_FOLLOW = -2,
} DDNetSpec;

// Holds the raw data of a snap item unknown to the ddnet_protocol library.
typedef struct
{
	int32_t type_id;
	int32_t id;

	// The size is also sent over the network. The is the amount of packed
	// integers in the `data` field. Warning! This is not the amount of bytes in
	// `data`!
	int32_t size;

	// `data` are the raw packed integers.
	const uint8_t *data;

	// This value is not sent over the network! It is the size of the `data` in
	// bytes.
	size_t data_len;
} DDNetObjUnknown;

// Flying weapon bullet.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
	int32_t vel_x;
	int32_t vel_y;
	DDNetPickupWeapon type;
	int32_t start_tick;
} DDNetObjProjectile;

// A blue laser ray shot by the rifle weapon.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
	int32_t from_x;
	int32_t from_y;
	int32_t start_tick;
} DDNetObjLaser;

// A pickup is a collectable weapon, armor or shield in the world.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
	// Be careful to not confuse this with the generic snap item field
	// `type_id`. The `type` is pickup specific and determines if it is a
	// weapon, ninja, health or armor.
	DDNetPowerup type;
	// The `subtype` should only be used if the `type` is set to
	// `DDNET_POWERUP_WEAPON` otherwise `subtype` should be set to `0` by the
	// sender and ignored by the receiver.
	DDNetPickupWeapon subtype;
} DDNetObjPickup;

// This represents a flag from the capture the flag mode.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
	// The flag will be red if set to `DDNET_TEAM_RED`.
	// The flag will be blue if set to `DDNET_TEAM_BLUE`.
	// No other values are allowed.
	DDNetTeam team;
} DDNetObjFlag;

// Info about the current game state and score. This object should only exist
// once per snapshot.
typedef struct
{
	int32_t type_id;
	int32_t id;

	DDNetGameFlag game_flags;
	DDNetGameStateFlag game_state_flags;
	int32_t round_start_tick;
	int32_t warmup_timer;
	int32_t score_limit;
	int32_t time_limit;
	int32_t round_num;
	int32_t round_current;
} DDNetObjGameInfo;

// Similar to `DDNetObjGameInfo` but more optional.
// This snap item is only expected to be included in the snapshot if the
// `DDNetObjGameInfo` `game_flags` are set.
//
// It contains flag and team score data which is not used by all modes.
typedef struct
{
	int32_t type_id;
	int32_t id;

	// The current score of the red team. Will be displayed in the scoreboard.
	// Needs the `DDNetObjGameInfo.game_flags` set to `DDNET_GAMEFLAG_TEAMS` to
	// work properly.
	int32_t teamscore_red;

	// The current score of the blue team. Will be displayed in the scoreboard.
	// Needs the `DDNetObjGameInfo.game_flags` set to `DDNET_GAMEFLAG_TEAMS` to
	// work properly.
	int32_t teamscore_blue;

	// Client Id of the current flag holder. Is expected to be `-1` if the red flag
	// is not being carried by anyone. See the `DDNetFlag` enum for all special values.
	// Needs the `DDNetObjGameInfo.game_flags` set to `DDNET_GAMEFLAG_FLAGS`
	// to work properly.
	// Client Id of the current flag holder. Is expected to be `-1` if the red
	// flag is not being carried by anyone. See the `DDNetFlag` enum for all
	// special values. Needs the `DDNetObjGameInfo.game_flags` set to
	// `DDNET_GAMEFLAG_FLAGS` to work properly.
	// TODO: which type to use for that field? Could use `DDNetFlag` but that
	// does not clearly communicate it also contains client ids.
	int32_t flag_carrier_red;

	// Client Id of the current flag holder. Is expected to be `-1` if the blue
	// flag is not being carried by anyone. See the `DDNetFlag` enum for all
	// special values. Needs the `DDNetObjGameInfo.game_flags` set to
	// `DDNET_GAMEFLAG_FLAGS` to work properly.
	int32_t flag_carrier_blue;
} DDNetObjGameData;

// This is never sent over the network on its own. It is always used as a field
// in the `DDNetObjCharacter` object.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t tick;
	int32_t x;
	int32_t y;
	int32_t vel_x;
	int32_t vel_y;
	int32_t angle;
	int32_t direction;
	int32_t jumped;
	int32_t hooked_player;
	int32_t hook_state;
	int32_t hook_tick;
	int32_t hook_x;
	int32_t hook_y;
	int32_t hook_dx;
	int32_t hook_dy;
} DDNetObjCharacterCore;

// Represents a tee in the world. Its position and other state.
typedef struct
{
	int32_t type_id;
	int32_t id;

	DDNetObjCharacterCore core;
	int32_t player_flags;
	int32_t health;
	int32_t armor;
	int32_t ammo_count;
	int32_t weapon;
	int32_t emote;
	int32_t attack_tick;
} DDNetObjCharacter;

// Score, team and latency.
typedef struct
{
	int32_t type_id;
	int32_t id;

	// This is set to true if this player is the same player as the snapshot
	// receiver. So the client can use this snap item to figure out its own
	// client id.
	bool local;
	int32_t client_id;
	DDNetTeam team;

	// The amount of points a player made, will be displayed as is in the
	// scoreboard. Unless it is a ddrace server that uses time score then a time
	// should be displayed. This score used to also hold times as negative
	// numbers so shorter times would be sorted higher by vanilla clients. But
	// nowerdays modern ddnet also sends the timescore in dedicated snapshot
	// item extensions. These are not supported yet by the ddnet_protocol
	// library.
	int32_t score;

	// This is used to display the ping of players in the scoreboard.
	int32_t latency;
} DDNetObjPlayerInfo;

// Skin, name, clan and country info.
typedef struct
{
	int32_t type_id;
	int32_t id;

	char name[DDNET_MAX_SKIN_LENGTH];
	char clan[DDNET_MAX_CLAN_LENGTH];
	int32_t country;
	char skin[DDNET_MAX_SKIN_LENGTH];
	bool use_custom_color;
	int32_t color_body;
	int32_t color_feet;
} DDNetObjClientInfo;

// Only sent to spectators if the server forces the camera position. Its either
// set to the x/y position or following a client id.
typedef struct
{
	int32_t type_id;
	int32_t id;

	// Can be the client id of a player that the camera should follow. Or one of
	// the magic values in the `DDNetSpec` enum.
	DDNetSpec spectator_id;
	int32_t x;
	int32_t y;
} DDNetObjSpectatorInfo;

// Meta event. Never actually sent over the network.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
} DDNetEventCommon;

// Grenade explosion animation.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
} DDNetEventExplosion;

// Purple steam spawn animation.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
} DDNetEventSpawn;

// Hammer hit animation that looks like a white splash.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
} DDNetEventHammerHit;

// Bursting tee animation. The color of the particles depends on the skin color
// of the tee that died. So one has to look into `client_id` and the stored skin
// info.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
	// Client id of the tee that died.
	int32_t client_id;
} DDNetEventDeath;

// Plays a sound.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
	int32_t sound_id;
} DDNetEventSoundGlobal;

// Plays a sound.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
	int32_t sound_id;
} DDNetEventSoundWorld;

// Displays rotating yellow damage indicator star.
typedef struct
{
	int32_t type_id;
	int32_t id;

	int32_t x;
	int32_t y;
	int32_t angle;
} DDNetEventDamageIndicator;

#ifdef __cplusplus
}
#endif
