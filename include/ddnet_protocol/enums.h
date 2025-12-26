#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Gamemode flags. These flags can be combined.
// And they are used by the `DDNetObjGameInfo` snapshot item.
typedef enum {
	// If this flag is set the current gamemode has vanilla teams.
	// Meaning there is `DDNET_TEAM_RED` and `DDNET_TEAM_BLUE`.
	// If the flag is not set the `DDNET_TEAM_GAME` will be used
	// for all players.
	DDNET_GAMEFLAG_TEAMS = 1 << 0,

	// If this flag is set the current gamemode has vanilla flags.
	// A flag being a flag in the world that comes from the
	// capture the flag gametype (ctf).
	DDNET_GAMEFLAG_FLAGS = 1 << 1,
} DDNetGameFlag;

// The current gamestate. Only one of these flags can be used.
// This flag is used by the `DDNetObjGameInfo` snapshot item.
typedef enum {
	// The world is paused and the scoreboard is displayed.
	DDNET_GAMESTATEFLAG_GAMEOVER = 1 << 0,

	// The round will end on the next score.
	DDNET_GAMESTATEFLAG_SUDDENDEATH = 1 << 1,

	// The world is paused.
	DDNET_GAMESTATEFLAG_PAUSED = 1 << 2,
} DDNetGameStateFlag;

// This enum represents teeworlds vanilla teams.
// These teams are different than ddrace teams that were added by ddnet.
typedef enum {
	DDNET_TEAM_SPECTATORS = -1,
	DDNET_TEAM_GAME = 0,
	DDNET_TEAM_RED = 0,
	DDNET_TEAM_BLUE = 1,
	DDNET_NUM_TEAMS = 2,
} DDNetTeam;

// possible values of the team field in the chat
// message sent by the server
typedef enum {
	// chat message sent to public chat
	DDNET_CHAT_PUBLIC,

	// chat message sent to team chat
	// this can be scoped to `DDNET_TEAM_RED`, `DDNET_TEAM_BLUE` and `DDNET_TEAM_SPECTATORS`
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

// emoticons are displayed above the tee
typedef enum {
	DDNET_EMOTICON_OOP,
	DDNET_EMOTICON_EXCLAMATION,
	DDNET_EMOTICON_HEARTS,
	DDNET_EMOTICON_DROP,
	DDNET_EMOTICON_DOTDOT,
	DDNET_EMOTICON_MUSIC,
	DDNET_EMOTICON_SORRY,
	DDNET_EMOTICON_GHOST,
	DDNET_EMOTICON_SUSHI,
	DDNET_EMOTICON_SPLATTEE,
	DDNET_EMOTICON_DEVILTEE,
	DDNET_EMOTICON_ZOMG,
	DDNET_EMOTICON_ZZZ,
	DDNET_EMOTICON_WTF,
	DDNET_EMOTICON_EYES,
	DDNET_EMOTICON_QUESTION,
} DDNetEmoticon;

#ifdef __cplusplus
}
#endif
