#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/huffman.h>
#include <ddnet_protocol/message.h>
#include <ddnet_protocol/msg_game.h>
#include <ddnet_protocol/packet.h>
#include <ddnet_protocol/session.h>

typedef struct {
	int32_t socket;
	struct sockaddr_in addr;
	struct sockaddr_in server_addr;
	DDNetSession session;
	time_t last_send;
	bool request_resend;
} TwClient;

ssize_t twclient_send(TwClient *client, const uint8_t *data, size_t len) {
	client->last_send = time(NULL);
	return sendto(client->socket, data, len, 0, (const struct sockaddr *)&client->server_addr, sizeof(client->server_addr));
}

ssize_t twclient_recv(TwClient *client, uint8_t *buf, size_t buf_len) {
	struct sockaddr_in peer_addr;
	socklen_t len = sizeof(peer_addr);
	errno = 0;
	ssize_t bytes = recvfrom(
		client->socket,
		buf,
		buf_len,
		0,
		(struct sockaddr *)&peer_addr,
		&len);
	if(bytes < 0) {
		if(errno == EWOULDBLOCK) {
			return 0;
		}
		fprintf(stderr, "network error: %s\n", strerror(errno));
		return -1;
	}
	return bytes;
}

void twclient_init_udp(TwClient *client) {
	client->socket = socket(AF_INET, SOCK_DGRAM, 0);
	client->addr.sin_family = AF_INET;
	client->addr.sin_port = 0;
	uint8_t own_ip[4] = {127, 0, 0, 1};
	memcpy(&client->addr.sin_addr.s_addr, own_ip, 4);
	int32_t err = bind(client->socket, (struct sockaddr *)&client->addr, sizeof(client->addr));
	if(err) {
		fprintf(stderr, "failed to bind network socket: %s\n", strerror(errno));
		exit(1);
	}
	int32_t broadcast = 1;
	setsockopt(client->socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
	int32_t recvsize = 65536;
	setsockopt(client->socket, SOL_SOCKET, SO_RCVBUF, &recvsize, sizeof(recvsize));
	fcntl(client->socket, F_SETFL, fcntl(client->socket, F_GETFL, 0) | O_NONBLOCK);
}

void twclient_init_ddnet(TwClient *client) {
	client->session.ack = 0;
	client->session.sequence = 0;
	client->session.peer_ack = 0;
	client->session.token = DDNET_TOKEN_NONE;
	client->last_send = 0;
}

void twclient_init(TwClient *client) {
	twclient_init_udp(client);
	twclient_init_ddnet(client);
}

void twclient_send_packet(TwClient *client, DDNetPacket *packet) {
	packet->header.token = client->session.token;
	if(client->request_resend) {
		packet->header.flags |= DDNET_PACKET_FLAG_RESEND;
		client->request_resend = false;
	}
	uint8_t buf[DDNET_MAX_PACKET_SIZE];
	DDNetError err = DDNET_ERR_NONE;
	size_t len = ddnet_encode_packet(packet, buf, sizeof(buf), &err);
	if(err != DDNET_ERR_NONE) {
		fprintf(stderr, "encode error=%d\n", err);
		return;
	}
	twclient_send(client, buf, len);
}

void twclient_connect(TwClient *client, const char *server_ip, uint16_t server_port) {
	client->server_addr.sin_family = AF_INET;
	if(inet_pton(AF_INET, server_ip, &(client->server_addr.sin_addr)) != 1) {
		fprintf(stderr, "invalid server ip '%s' (should not contain port)\n", server_ip);
		exit(1);
	}

	client->server_addr.sin_port = htons(server_port);

	printf("connecting to %s:%d ...\n", server_ip, server_port);
	twclient_init_ddnet(client);

	DDNetPacket packet = {
		.header = {
			.flags = DDNET_PACKET_FLAG_CONTROL,
		},
		.kind = DDNET_PACKET_CONTROL,
		.control = {.kind = DDNET_CTRL_MSG_CONNECT}};
	twclient_send_packet(client, &packet);
}

void twclient_ack_accept(TwClient *client) {
	DDNetPacket packet = {
		.header = {
			.flags = DDNET_PACKET_FLAG_CONTROL,
		},
		.kind = DDNET_PACKET_CONTROL,
		.control = {.kind = DDNET_CTRL_MSG_ACCEPT}};
	twclient_send_packet(client, &packet);
}

void twclient_disconnect(TwClient *client, const char *reason) {
	DDNetPacket packet = {
		.header = {
			.flags = DDNET_PACKET_FLAG_CONTROL,
			.token = client->session.token,
			.ack = client->session.ack,
		},
		.kind = DDNET_PACKET_CONTROL,
		.control = {
			.kind = DDNET_CTRL_MSG_CLOSE,
			.reason = reason,
		}};
	twclient_send_packet(client, &packet);
}

void twclient_keep_alive(TwClient *client) {
	DDNetPacket packet = {
		.header = {
			.flags = DDNET_PACKET_FLAG_CONTROL,
			.token = client->session.token,
			.ack = client->session.ack,
		},
		.kind = DDNET_PACKET_CONTROL,
		.control = {
			.kind = DDNET_CTRL_MSG_KEEPALIVE,
		}};
	twclient_send_packet(client, &packet);
}

void twclient_send_info(TwClient *client) {
	DDNetPacket packet = {};
	DDNetMessage msg = ddnet_build_msg_info("");
	DDNetError err = ddnet_build_packet(&packet, &msg, 1, &client->session);
	twclient_send_packet(client, &packet);
}

void twclient_send_ready(TwClient *client) {
	DDNetPacket packet = {};
	DDNetMessage msg = {.kind = DDNET_MSG_KIND_READY};
	DDNetError err = ddnet_build_packet(&packet, &msg, 1, &client->session);
	twclient_send_packet(client, &packet);
}

void twclient_send_start_info(TwClient *client) {
	DDNetPacket packet = {};
	DDNetMessage msg = {
		.kind = DDNET_MSG_KIND_CL_STARTINFO,
		.msg = {
			.start_info = {
				.name = "client.c",
				.clan = "",
				.skin = "greensward",
			}}};
	DDNetError err = ddnet_build_packet(&packet, &msg, 1, &client->session);
	twclient_send_packet(client, &packet);
}

void twclient_send_enter_game(TwClient *client) {
	DDNetPacket packet = {};
	DDNetMessage msg = {.kind = DDNET_MSG_KIND_ENTERGAME};
	DDNetError err = ddnet_build_packet(&packet, &msg, 1, &client->session);
	twclient_send_packet(client, &packet);
}

void twclient_on_control_packet(TwClient *client, DDNetPacket *packet) {
	switch(packet->control.kind) {
	case DDNET_CTRL_MSG_CONNECT:
	case DDNET_CTRL_MSG_KEEPALIVE:
	case DDNET_CTRL_MSG_ACCEPT:
		break;
	case DDNET_CTRL_MSG_CONNECTACCEPT:
		printf("got ddnet security token %d\n", packet->header.token);
		client->session.token = packet->header.token;
		twclient_ack_accept(client);
		twclient_send_info(client);
		break;
	case DDNET_CTRL_MSG_CLOSE:
		if(packet->control.reason && packet->control.reason[0]) {
			printf("connection closed (%s).\n", packet->control.reason);
		} else {
			printf("connection closed.\n");
		}
		exit(0);
		break;
	};
}

void twclient_on_map_change(TwClient *client, DDNetMsgMapChange *map_change) {
	printf("got map change: %s\n", map_change->name);
	twclient_send_ready(client);
}

void twclient_on_broadcast(TwClient *client, DDNetMsgSvBroadcast *msg) {
	if(msg->message[0] == '\0') {
		return;
	}
	printf("[broadcast] %s\n", msg->message);
}

void twclient_on_motd(TwClient *client, DDNetMsgSvMotd *msg) {
	if(msg->message[0] == '\0') {
		return;
	}
	printf("[motd] %s\n", msg->message);
}

void twclient_on_chat(TwClient *client, DDNetMsgSvChat *msg) {
	if(msg->message[0] == '\0') {
		return;
	}
	printf("[chat] %s\n", msg->message);
}

void twclient_on_chunk(TwClient *client, DDNetChunk *chunk) {
	if(chunk->header.flags & DDNET_CHUNK_FLAG_VITAL) {
		if(chunk->header.sequence == (client->session.ack + 1) % DDNET_MAX_SEQUENCE) {
			// in sequence
			client->session.ack = (client->session.ack + 1) % DDNET_MAX_SEQUENCE;
		} else {
			if(ddnet_seq_in_backroom(chunk->header.sequence, client->session.ack)) {
				// dropping known chunk
				return;
			}
			client->request_resend = true;
		}
	}

	// union hack all payloads point to the same address anyways
	void *msg_payload = &chunk->payload.msg.unknown;

	switch(chunk->payload.kind) {
	case DDNET_MSG_KIND_MAP_CHANGE:
		twclient_on_map_change(client, msg_payload);
		return;
	case DDNET_MSG_KIND_SV_BROADCAST:
		twclient_on_broadcast(client, msg_payload);
		return;
	case DDNET_MSG_KIND_SV_MOTD:
		twclient_on_motd(client, msg_payload);
		return;
	case DDNET_MSG_KIND_SV_CHAT:
		twclient_on_chat(client, msg_payload);
		return;
	case DDNET_MSG_KIND_CON_READY:
		twclient_send_start_info(client);
		return;
	case DDNET_MSG_KIND_SV_READYTOENTER:
		twclient_send_enter_game(client);
		return;
	case DDNET_MSG_KIND_SNAP:
	case DDNET_MSG_KIND_SNAPEMPTY:
	case DDNET_MSG_KIND_SNAPSINGLE:
	case DDNET_MSG_KIND_SNAPSMALL:
	case DDNET_MSG_KIND_INPUTTIMING:
	case DDNET_MSG_KIND_RCON_AUTH_STATUS:
	case DDNET_MSG_KIND_RCON_LINE:
	case DDNET_MSG_KIND_READY:
	case DDNET_MSG_KIND_INPUT:
	case DDNET_MSG_KIND_RCON_AUTH:
	case DDNET_MSG_KIND_REQUEST_MAP_DATA:
	case DDNET_MSG_KIND_PING:
	case DDNET_MSG_KIND_PING_REPLY:
	case DDNET_MSG_KIND_RCON_CMD_ADD:
	case DDNET_MSG_KIND_RCON_CMD_REM:
	case DDNET_MSG_KIND_INFO:
	case DDNET_MSG_KIND_MAP_DATA:
	case DDNET_MSG_KIND_RCON_CMD:
	case DDNET_MSG_KIND_CL_STARTINFO:
	case DDNET_MSG_KIND_UNKNOWN:
	case DDNET_MSG_KIND_ENTERGAME:
	case DDNET_MSG_KIND_SV_KILLMSG:
	case DDNET_MSG_KIND_SV_SOUNDGLOBAL:
	case DDNET_MSG_KIND_SV_TUNEPARAMS:
	case DDNET_MSG_KIND_SV_WEAPONPICKUP:
	case DDNET_MSG_KIND_CL_SAY:
	case DDNET_MSG_KIND_SV_EMOTICON:
	case DDNET_MSG_KIND_SV_VOTECLEAROPTIONS:
	case DDNET_MSG_KIND_SV_VOTEOPTIONLISTADD:
	case DDNET_MSG_KIND_SV_VOTEOPTIONADD:
	case DDNET_MSG_KIND_SV_VOTEOPTIONREMOVE:
	case DDNET_MSG_KIND_SV_VOTESET:
		return;
	};

	fprintf(stderr, "unknown chunk kind %d\n", chunk->payload.kind);
}

void twclient_on_normal_packet(TwClient *client, DDNetPacket *packet) {
	for(size_t i = 0; i < packet->chunks.len; i++) {
		DDNetChunk *chunk = &packet->chunks.data[i];
		twclient_on_chunk(client, chunk);
	}
}

void twclient_on_network_data(TwClient *client, uint8_t *buf, size_t len) {
	DDNetError err = DDNET_ERR_NONE;
	DDNetPacket packet = ddnet_decode_packet(buf, len, &err);
	if(err != DDNET_ERR_NONE) {
		// TODO: depends on https://github.com/MilkeeyCat/ddnet_protocol/issues/156
		if(packet.kind == DDNET_PACKET_CONNLESS) {
			return;
		}
		fprintf(stderr, "packet decode error %d\n", err);
		exit(1);
		return;
	}

	switch(packet.kind) {
	case DDNET_PACKET_CONTROL:
		twclient_on_control_packet(client, &packet);
		break;
	case DDNET_PACKET_NORMAL:
		twclient_on_normal_packet(client, &packet);
		break;
	case DDNET_PACKET_CONNLESS:
		break;
	};

	ddnet_free_packet(&packet);
}

static volatile bool got_sigint = false;

void sigint_handler(int sig) {
	got_sigint = true;
}

int main(int argc, char *argv[]) {
	TwClient client = {};

	const char *server_ip = "127.0.0.1";
	uint16_t server_port = 8303;

	for(size_t i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			puts("usage: client [server_ip] [server_port]");
			exit(0);
		}
		if(i == 1) {
			server_ip = argv[i];
		} else if(i == 2) {
			server_port = atoi(argv[i]);
		}
	}

	twclient_init(&client);
	twclient_connect(&client, server_ip, server_port);

	signal(SIGINT, sigint_handler);

	while(1) {
		// be nice to the cpu
		usleep(200);

		if(got_sigint) {
			twclient_disconnect(&client, "");
			break;
		}

		time_t sent_delta = time(NULL) - client.last_send;
		if(sent_delta > 1) {
			twclient_keep_alive(&client);
		}

		uint8_t buf[DDNET_MAX_PACKET_SIZE];
		ssize_t len = twclient_recv(&client, buf, sizeof(buf));
		if(len < 1) {
			continue;
		}

		twclient_on_network_data(&client, buf, len);
	}
}
