#pragma once

#include "packet.h"
#include "token.h"

typedef enum {
	CTRL_MSG_KEEPALIVE,
	CTRL_MSG_CONNECT,
	CTRL_MSG_CONNECTACCEPT,
	CTRL_MSG_ACCEPT,
	CTRL_MSG_CLOSE,
} ControlMessageKind;

typedef struct {
	PacketKind _;
	ControlMessageKind kind;
	Token token;
	char *reason; // Can be set if msg_kind == CTRL_MSG_CLOSE
} PacketControl;

PacketControl *decode_control(uint8_t *buf, size_t len, Error *err);
