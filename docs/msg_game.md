# DDProtoChatTeam

## Syntax

```C
typedef enum {
	// Chat message sent to public chat.
	DDPROTO_CHAT_PUBLIC,
	// Chat message sent to team chat. This can be scoped to TEAM_RED, TEAM_BLUE
	// and TEAM_SPECTATORS based on which team the sender is in but it can also
	// be scoped to ddrace teams.
	DDPROTO_CHAT_TEAM,
	// This value is a ddnet extension. This is sent from the server to the
	// client to display an outgoing ddnet whisper message. In this case the
	// client_id field on the `DDProtoMsgSvChat` struct is the recipient not the
	// sender. Because the sender knows his own name.
	DDPROTO_CHAT_WHISPER_SEND,
	// This value is a ddnet extension. This is sent from the server to the
	// client to display received whisper messages differently in the chat.
	DDPROTO_CHAT_WHISPER_RECEIVE,
} DDProtoChatTeam;
```

Possible values of the team field in the chat message sent by the server.

# DDProtoWeapon

## Syntax

```C
typedef enum {
	// Team switching and similar.
	DDPROTO_WEAPON_GAME = -3,
	// Console kill command (not self damage).
	DDPROTO_WEAPON_SELF = -2,
	// Kill tiles or out of world.
	DDPROTO_WEAPON_WORLD = -1,
	DDPROTO_WEAPON_HAMMER = 0,
	DDPROTO_WEAPON_GUN = 1,
	DDPROTO_WEAPON_SHOTGUN = 2,
	DDPROTO_WEAPON_GRENADE = 3,
	DDPROTO_WEAPON_LASER = 4,
	DDPROTO_WEAPON_NINJA = 5,
} DDProtoWeapon;
```

Regular and special weapons. See also `DDProtoPickupWeapon` for regular weapons
only.

# DDProtoPickupWeapon

## Syntax

```C
typedef enum {
	DDPROTO_PICKUP_WEAPON_HAMMER = 0,
	DDPROTO_PICKUP_WEAPON_GUN = 1,
	DDPROTO_PICKUP_WEAPON_SHOTGUN = 2,
	DDPROTO_PICKUP_WEAPON_GRENADE = 3,
	DDPROTO_PICKUP_WEAPON_LASER = 4,
	DDPROTO_PICKUP_WEAPON_NINJA = 5,
} DDProtoPickupWeapon;
```

Regular weapons only. See also `DDProtoWeapon` for special weapons. These
weapons can be picked up. Kill messages can contain more weapons.

# DDProtoEmoticon

## Syntax

```C
typedef enum {
	DDPROTO_EMOTICON_OOP,
	DDPROTO_EMOTICON_EXCLAMATION,
	DDPROTO_EMOTICON_HEARTS,
	DDPROTO_EMOTICON_DROP,
	DDPROTO_EMOTICON_DOTDOT,
	DDPROTO_EMOTICON_MUSIC,
	DDPROTO_EMOTICON_SORRY,
	DDPROTO_EMOTICON_GHOST,
	DDPROTO_EMOTICON_SUSHI,
	DDPROTO_EMOTICON_SPLATTEE,
	DDPROTO_EMOTICON_DEVILTEE,
	DDPROTO_EMOTICON_ZOMG,
	DDPROTO_EMOTICON_ZZZ,
	DDPROTO_EMOTICON_WTF,
	DDPROTO_EMOTICON_EYES,
	DDPROTO_EMOTICON_QUESTION,
} DDProtoEmoticon;
```

Emoticons are displayed above the tee.

# DDProtoMsgSvMotd

## Syntax

```C
typedef struct {
	const char *message;
} DDProtoMsgSvMotd;
```

Message of the day. Sent by the server and displayed in the middle of the
screen with transparent background on the client side.

# DDProtoMsgSvBroadcast

## Syntax

```C
typedef struct {
	const char *message;
} DDProtoMsgSvBroadcast;
```

Sent by the server and displayed in the middle of the screen as white text.

# DDProtoMsgSvChat

## Syntax

```C
typedef struct {
	DDProtoChatTeam team;
	// Client id of the message author. Can be -1 if the message was sent by the
	// server can be the message recipient if the team is
	// `DDPROTO_CHAT_WHISPER_SEND`.
	int32_t client_id;
	// Chat message.
	const char *message;
} DDProtoMsgSvChat;
```

sent by the server

# DDProtoMsgSvKillMsg

## Syntax

```C
typedef struct {
	// Client id of the player that caused the kill can be the same as the
	// victim_id on suicide.
	int32_t killer_id;
	// Client id of the player that got killed.
	int32_t victim_id;
	DDProtoWeapon weapon;
	// Set to 1 if the victim had the flag in a ctf gametype.
	int32_t mode_special;
} DDProtoMsgSvKillMsg;
```

sent by the server

# DDProtoMsgSvSoundGlobal

## Syntax

```C
typedef struct {
	int32_t sound_id;
} DDProtoMsgSvSoundGlobal;
```

sent by the server

# DDProtoMsgSvTuneParams

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
} DDProtoMsgSvTuneParams;
```

sent by the server

# DDProtoMsgSvWeaponPickup

## Syntax

```C
typedef struct {
	DDProtoPickupWeapon weapon;
} DDProtoMsgSvWeaponPickup;
```

sent by the server

# DDProtoMsgSvEmoticon

## Syntax

```C
typedef struct {
	// Id of the player that the emoticon is attached to.
	int32_t client_id;
	DDProtoEmoticon emoticon;
} DDProtoMsgSvEmoticon;
```

sent by the server

# DDProtoMsgSvVoteOptionListAdd

## Syntax

```C
typedef struct {
	// The next field `descriptions` has a fixed size of 14 but not all strings
	// might be set. So `num_options` determines how many values starting from
	// index 0 are valid in `descriptions`.
	int32_t num_options;
	// Only access index 0-num_options.
	const char *descriptions[14];
} DDProtoMsgSvVoteOptionListAdd;
```

sent by the server

# DDProtoMsgSvVoteOptionAdd

## Syntax

```C
typedef struct {
	const char *description;
} DDProtoMsgSvVoteOptionAdd;
```

sent by the server
See also `DDProtoMsgSvVoteOptionListAdd` for multiple vote options at once.

# DDProtoMsgSvVoteOptionRemove

## Syntax

```C
typedef struct {
	const char *description;
} DDProtoMsgSvVoteOptionRemove;
```

sent by the server

# DDProtoMsgSvVoteSet

## Syntax

```C
typedef struct {
	int32_t timeout;
	const char *description;
	const char *reason;
} DDProtoMsgSvVoteSet;
```

sent by the server

# DDProtoMsgClSay

## Syntax

```C
typedef struct {
	DDProtoChatTeam team;
	const char *message;
} DDProtoMsgClSay;
```

sent by the client

# DDProtoMsgClStartInfo

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
} DDProtoMsgClStartInfo;
```

sent by the client

