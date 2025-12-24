# DDNetChatTeam

## Syntax

```C
typedef enum {
	// Chat message sent to public chat.
	DDNET_CHAT_PUBLIC,
	// Chat message sent to team chat. This can be scoped to TEAM_RED, TEAM_BLUE
	// and TEAM_SPECTATORS based on which team the sender is in but it can also
	// be scoped to ddrace teams.
	DDNET_CHAT_TEAM,
	// This value is a ddnet extension. This is sent from the server to the
	// client to display an outgoing ddnet whisper message. In this case the
	// client_id field on the `DDNetMsgSvChat` struct is the recipient not the
	// sender. Because the sender knows his own name.
	DDNET_CHAT_WHISPER_SEND,
	// This value is a ddnet extension. This is sent from the server to the
	// client to display received whisper messages differently in the chat.
	DDNET_CHAT_WHISPER_RECEIVE,
} DDNetChatTeam;
```

Possible values of the team field in the chat message sent by the server.

# DDNetWeapon

## Syntax

```C
typedef enum {
	// Team switching and similar.
	DDNET_WEAPON_GAME = -3,
	// Console kill command (not self damage).
	DDNET_WEAPON_SELF = -2,
	// Kill tiles or out of world.
	DDNET_WEAPON_WORLD = -1,
	DDNET_WEAPON_HAMMER = 0,
	DDNET_WEAPON_GUN = 1,
	DDNET_WEAPON_SHOTGUN = 2,
	DDNET_WEAPON_GRENADE = 3,
	DDNET_WEAPON_LASER = 4,
	DDNET_WEAPON_NINJA = 5,
} DDNetWeapon;
```

Regular and special weapons. See also `DDNetPickupWeapon` for regular weapons
only.

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

Regular weapons only. See also `DDNetWeapon` for special weapons. These
weapons can be picked up. Kill messages can contain more weapons.

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

Emoticons are displayed above the tee.

# DDNetMsgSvMotd

## Syntax

```C
typedef struct {
	const char *message;
} DDNetMsgSvMotd;
```

Message of the day. Sent by the server and displayed in the middle of the
screen with transparent background on the client side.

# DDNetMsgSvBroadcast

## Syntax

```C
typedef struct {
	const char *message;
} DDNetMsgSvBroadcast;
```

Sent by the server and displayed in the middle of the screen as white text.

# DDNetMsgSvChat

## Syntax

```C
typedef struct {
	DDNetChatTeam team;
	// Client id of the message author. Can be -1 if the message was sent by the
	// server can be the message recipient if the team is
	// `DDNET_CHAT_WHISPER_SEND`.
	int32_t client_id;
	// Chat message.
	const char *message;
} DDNetMsgSvChat;
```

sent by the server

# DDNetMsgSvKillMsg

## Syntax

```C
typedef struct {
	// Client id of the player that caused the kill can be the same as the
	// victim_id on suicide.
	int32_t killer_id;
	// Client id of the player that got killed.
	int32_t victim_id;
	DDNetWeapon weapon;
	// Set to 1 if the victim had the flag in a ctf gametype.
	int32_t mode_special;
} DDNetMsgSvKillMsg;
```

sent by the server

# DDNetMsgSvSoundGlobal

## Syntax

```C
typedef struct {
	int32_t sound_id;
} DDNetMsgSvSoundGlobal;
```

sent by the server

# DDNetMsgSvTuneParams

## Syntax

```C
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
```

sent by the server

# DDNetMsgSvWeaponPickup

## Syntax

```C
typedef struct {
	DDNetPickupWeapon weapon;
} DDNetMsgSvWeaponPickup;
```

sent by the server

# DDNetMsgSvEmoticon

## Syntax

```C
typedef struct {
	// Id of the player that the emoticon is attached to.
	int32_t client_id;
	DDNetEmoticon emoticon;
} DDNetMsgSvEmoticon;
```

sent by the server

# DDNetMsgSvVoteOptionListAdd

## Syntax

```C
typedef struct {
	// The next field `descriptions` has a fixed size of 14 but not all strings
	// might be set. So `num_options` determines how many values starting from
	// index 0 are valid in `descriptions`.
	int32_t num_options;
	// Only access index 0-num_options.
	const char *descriptions[14];
} DDNetMsgSvVoteOptionListAdd;
```

sent by the server

# DDNetMsgSvVoteOptionAdd

## Syntax

```C
typedef struct {
	const char *description;
} DDNetMsgSvVoteOptionAdd;
```

sent by the server
See also `DDNetMsgSvVoteOptionListAdd` for multiple vote options at once.

# DDNetMsgSvVoteOptionRemove

## Syntax

```C
typedef struct {
	const char *description;
} DDNetMsgSvVoteOptionRemove;
```

sent by the server

# DDNetMsgSvVoteSet

## Syntax

```C
typedef struct {
	int32_t timeout;
	const char *description;
	const char *reason;
} DDNetMsgSvVoteSet;
```

sent by the server

# DDNetMsgClSay

## Syntax

```C
typedef struct {
	DDNetChatTeam team;
	const char *message;
} DDNetMsgClSay;
```

sent by the client

# DDNetMsgClStartInfo

## Syntax

```C
typedef struct {
	const char *name;
	const char *clan;
	size_t country;
	const char *skin;
	bool use_custom_color;
	size_t color_body;
	size_t color_feet;
} DDNetMsgClStartInfo;
```

sent by the client

