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
	PacketHeader header;
	ControlMessageKind kind;
	char *reason; // Can be set if msg_kind == CTRL_MSG_CLOSE
} PacketControl;

// Given a buffer containing the packet payload without packet header.
// It will extract one control message.
PacketControl *decode_control(uint8_t *buf, size_t len, PacketHeader header, Error *err);
