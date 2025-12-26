# DDNetTeam

## Syntax

```C
typedef enum {
	DDNET_TEAM_SPECTATORS = -1,
	DDNET_TEAM_RED,
	DDNET_TEAM_BLUE,
	DDNET_NUM_TEAMS,
} DDNetTeam;
```

This enum represents teeworlds vanilla teams.
These teams are different than ddrace teams that were added by ddnet.

# DDNetChatTeam

## Syntax

```C
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
```

possible values of the team field in the chat
message sent by the server

# DDNetWeapon

## Syntax

```C
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
```

regular and special weapons
see also `DDNetPickupWeapon` for regular weapons only

# DDNetPickupWeapon

## Syntax

```C
typedef enum {
	DDNET_PICKUP_WEAPON_HAMMER = 0,
	DDNET_PICKUP_WEAPON_GUN = 1,
	DDNET_PICKUP_WEAPON_SHOTGUN = 2,
	DDNET_PICKUP_WEAPON_GRENADE = 3,
	DDNET_PICKUP_WEAPON_LASER = 4,
	DDNET_PICKUP_WEAPON_NINJA = 5,
} DDNetPickupWeapon;
```

regular weapons only see also `DDNetWeapon` for special weapons
these weapons can be picked up
kill messages can contain more weapons

# DDNetEmoticon

## Syntax

```C
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
```

emoticons are displayed above the tee

