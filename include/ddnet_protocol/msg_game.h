#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

// possible values of the team field in the chat
// message sent by the server
typedef enum {
	// chat message sent to public chat
	DDNET_CHAT_PUBLIC,

	// chat message sent to team chat
	// this can be scoped to TEAM_RED, TEAM_BLUE and TEAM_SPECTATORS
	// beased on which team the sender is in
	// but it can also be scoped to ddrace teams
	DDNET_CHAT_TEAM,

	// this value is a ddnet extension
	// this is sent from the server to the client
	// to display an outgoing ddnet whisper message
	// in this case the client_id field on the DDNetMsgSvChat struct
	// is the recipient not the sender. Because the sender knows his own name.
	DDNET_CHAT_WHISPER_SEND,

	// this value is a ddnet extension
	// this is sent from the server to the client
	// to display received whisper messages differently in the chat.
	DDNET_CHAT_WHISPER_RECEIVE,
} DDNetChatTeam;

// regular and special weapons
// see also `DDNetPickupWeapon` for regular weapons only
typedef enum {
	// team switching and similar
	DDNET_WEAPON_GAME = -3,

	// console kill command (not self damage)
	DDNET_WEAPON_SELF = -2,

	// kill tiles or out of world
	DDNET_WEAPON_WORLD = -1,

	DDNET_WEAPON_HAMMER = 0,
	DDNET_WEAPON_GUN = 1,
	DDNET_WEAPON_SHOTGUN = 2,
	DDNET_WEAPON_GRENADE = 3,
	DDNET_WEAPON_LASER = 4,
	DDNET_WEAPON_NINJA = 5,
} DDNetWeapon;

// regular weapons only see also `DDNetWeapon` for special weapons
// these weapons can be picked up
// kill messages can contain more weapons
typedef enum {
	DDNET_PICKUP_WEAPON_HAMMER = 0,
	DDNET_PICKUP_WEAPON_GUN = 1,
	DDNET_PICKUP_WEAPON_SHOTGUN = 2,
	DDNET_PICKUP_WEAPON_GRENADE = 3,
	DDNET_PICKUP_WEAPON_LASER = 4,
	DDNET_PICKUP_WEAPON_NINJA = 5,
} DDNetPickupWeapon;

// message of the day
// sent by the server
// and displayed in the middle of the screen
// with transparent background on the client side
typedef struct {
	const char *message;
} DDNetMsgSvMotd;

// sent by the server
// and displayed in the middle of the screen as white text
typedef struct {
	const char *message;
} DDNetMsgSvBroadcast;

// sent by the server
typedef struct {
	DDNetChatTeam team;

	// client id of the message author
	// can be -1 if the message was sent by the server
	// can be the message recipient if the team is DDNET_CHAT_WHISPER_SEND
	int32_t client_id;

	// chat message
	const char *message;
} DDNetMsgSvChat;

// sent by the server
typedef struct {
	// client id of the player that caused the kill
	// can be the same as the victim_id on suicide
	int32_t killer_id;

	// client id of the player that got killed
	int32_t victim_id;

	DDNetWeapon weapon;

	// set to 1 if the victim had the flag in a ctf gametype
	int32_t mode_special;
} DDNetMsgSvKillMsg;

// sent by the server
typedef struct {
	int32_t sound_id;
} DDNetMsgSvSoundGlobal;

// sent by the server
typedef struct {
	float ground_control_speed;
	float ground_control_accel;
	float ground_friction;
	float ground_jump_impulse;
	float air_jump_impulse;
	float air_control_speed;
	float air_control_accel;
	float air_friction;
	float hook_length;
	float hook_fire_speed;
	float hook_drag_accel;
	float hook_drag_speed;
	float gravity;
	float velramp_start;
	float velramp_range;
	float velramp_curvature;
	float gun_curvature;
	float gun_speed;
	float gun_lifetime;
	float shotgun_curvature;
	float shotgun_speed;
	float shotgun_speeddiff;
	float shotgun_lifetime;
	float grenade_curvature;
	float grenade_speed;
	float grenade_lifetime;
	float laser_reach;
	float laser_bounce_delay;
	float laser_bounce_num;
	float laser_bounce_cost;
	float laser_damage;
	float player_collision;
	float player_hooking;
} DDNetMsgSvTuneParams;

// sent by the server
typedef struct {
	DDNetPickupWeapon weapon;
} DDNetMsgSvWeaponPickup;

// sent by the client
typedef struct {
	DDNetChatTeam team;
	const char *message;
} DDNetMsgClSay;

// sent by the client
typedef struct {
	const char *name;
	const char *clan;
	size_t country;
	const char *skin;
	bool use_custom_color;
	size_t color_body;
	size_t color_feet;
} DDNetMsgClStartInfo;

#ifdef __cplusplus
}
#endif
