#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include <ddnet_protocol/chunk.h>
#include <ddnet_protocol/huffman.h>
#include <ddnet_protocol/errors.h>
#include <ddnet_protocol/packet.h>

// debug start
// TODO: remove debug section

void str_hex(char *dst, int dst_size, const void *data, int data_size)
{
	static const char hex[] = "0123456789ABCDEF";
	int data_index;
	int dst_index;
	for(data_index = 0, dst_index = 0; data_index < data_size && dst_index < dst_size - 3; data_index++)
	{
		dst[(ptrdiff_t)(data_index * 3)] = hex[((const unsigned char *)data)[data_index] >> 4];
		dst[(data_index * 3) + 1] = hex[((const unsigned char *)data)[data_index] & 0xf];
		dst[(data_index * 3) + 2] = ' ';
		dst_index += 3;
	}
	dst[dst_index] = '\0';
}

// debug end

typedef struct {
	int32_t socket;
	struct sockaddr_in addr;
	struct sockaddr_in server_addr;
	DDNetToken token;
	int32_t vital_msgs_sent;
	int32_t vital_msgs_received;
} TwClient;

ssize_t twclient_send(TwClient *client, const uint8_t *data, size_t len) {
	return sendto(client->socket, (const char *)data, len, 0, (const struct sockaddr *)&client->server_addr, sizeof(client->server_addr));
}

ssize_t twclient_recv(TwClient *client, unsigned char *buf, size_t buf_len) {
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
	if(bytes == 0) {
		return 0;
	}
	return bytes;
}

void twclient_init_udp(TwClient *client) {
	client->socket = socket(AF_INET, SOCK_DGRAM, 0);
	client->addr.sin_family = AF_INET;
	client->addr.sin_port = 0;
	char own_ip[4] = {127, 0, 0, 1};
	memcpy(&client->addr.sin_addr.s_addr, own_ip, 4);
	int32_t err = bind(client->socket, (struct sockaddr *)&client->addr, sizeof(client->addr));
	int32_t broadcast = 1;
	setsockopt(client->socket, SOL_SOCKET, SO_BROADCAST, (const char *)&broadcast, sizeof(broadcast));
	int32_t recvsize = 65536;
	setsockopt(client->socket, SOL_SOCKET, SO_RCVBUF, (char *)&recvsize, sizeof(recvsize));
	uint64_t mode = 1;
	ioctl(client->socket, FIONBIO, &mode);
}

void twclient_init_ddnet(TwClient *client) {
	client->token = DDNET_TOKEN_NONE;
	client->vital_msgs_sent = 0;
	client->vital_msgs_received = 0;
}

void twclient_init(TwClient *client) {
	twclient_init_udp(client);
	twclient_init_ddnet(client);
}

void twclient_send_packet(TwClient *client, DDNetPacket *packet) {
	packet->header.token = client->token;
	uint8_t buf[DDNET_MAX_PACKET_SIZE];
	DDNetError err = DDNET_ERR_NONE;
	size_t len = ddnet_encode_packet(packet, buf, sizeof(buf), &err);
	if(err != DDNET_ERR_NONE) {
		fprintf(stderr, "encode error=%d\n", err);
		return;
	}
	twclient_send(client, buf, len);
}

void twclient_connect(TwClient *client) {
	client->server_addr.sin_family = AF_INET;
	client->server_addr.sin_port = htons(8303);
	unsigned char dst_ip[4] = {127, 0, 0, 1};
	memcpy(&client->server_addr.sin_addr.s_addr, dst_ip, 4);

	twclient_init_ddnet(client);

	DDNetPacket packet = {
		.header = {
			.flags = DDNET_PACKET_FLAG_CONTROL,
		},
		.kind = DDNET_PACKET_CONTROL,
		.control = {
			.kind = DDNET_CTRL_MSG_CONNECT
		}
	};
	twclient_send_packet(client, &packet);
}

void twclient_ack_accept(TwClient *client) {
	DDNetPacket packet = {
		.header = {
			.flags = DDNET_PACKET_FLAG_CONTROL,
		},
		.kind = DDNET_PACKET_CONTROL,
		.control = {
			.kind = DDNET_CTRL_MSG_ACCEPT
		}
	};
	twclient_send_packet(client, &packet);
}

void twclient_send_info(TwClient *client) {
	DDNetPacket packet = {
		.header = { .num_chunks = 2 },
		.kind = DDNET_PACKET_NORMAL};
	packet.chunks.data = (DDNetChunk *)malloc(sizeof(DDNetChunk) * packet.header.num_chunks);
	packet.chunks.len = packet.header.num_chunks;

	client->vital_msgs_sent++;
	packet.chunks.data[0].payload.msg.info.password = "";
	packet.chunks.data[0].payload.msg.info.version = "0.6 626fce9a778df4d4";
	packet.chunks.data[0].header.flags = DDNET_CHUNK_FLAG_VITAL;
	packet.chunks.data[0].header.sequence = client->vital_msgs_sent;
	fill_chunk_header(&packet.chunks.data[0]);
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
		client->token = packet->header.token;
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
}

void twclient_on_chunk(TwClient *client, DDNetChunk *chunk) {
	switch(chunk->payload.kind) {
	case DDNET_MSG_KIND_MAP_CHANGE:
		twclient_on_map_change(client, &chunk->payload.msg.map_change);
		return;
	case DDNET_MSG_KIND_INFO:
	case DDNET_MSG_KIND_MAP_DATA:
	case DDNET_MSG_KIND_RCON_CMD:
	case DDNET_MSG_KIND_CL_STARTINFO:
	case DDNET_MSG_KIND_UNKNOWN:
	case DDNET_MSG_KIND_CON_READY:
	case DDNET_MSG_KIND_SNAP:
	case DDNET_MSG_KIND_SNAPEMPTY:
	case DDNET_MSG_KIND_SNAPSINGLE:
	case DDNET_MSG_KIND_SNAPSMALL:
	case DDNET_MSG_KIND_INPUTTIMING:
	case DDNET_MSG_KIND_RCON_AUTH_STATUS:
	case DDNET_MSG_KIND_RCON_LINE:
	case DDNET_MSG_KIND_READY:
	case DDNET_MSG_KIND_ENTERGAME:
	case DDNET_MSG_KIND_INPUT:
	case DDNET_MSG_KIND_RCON_AUTH:
	case DDNET_MSG_KIND_REQUEST_MAP_DATA:
	case DDNET_MSG_KIND_PING:
	case DDNET_MSG_KIND_PING_REPLY:
	case DDNET_MSG_KIND_RCON_CMD_ADD:
	case DDNET_MSG_KIND_RCON_CMD_REM:
		puts("ignoring boring chunk");
		return;
	};

	fprintf(stderr, "unknown chunk kind %d\n", chunk->payload.kind);
}

void twclient_on_normal_packet(TwClient *client, DDNetPacket *packet) {
	for(int i = 0; i < packet->chunks.len; i++) {
		DDNetChunk *chunk = &packet->chunks.data[i];
		twclient_on_chunk(client, chunk);
	}
}

void twclient_on_network_data(TwClient *client, uint8_t *buf, size_t len) {
	DDNetError err = DDNET_ERR_NONE;
	DDNetPacket packet = ddnet_decode_packet(buf, len, &err);
	if(err != DDNET_ERR_NONE) {
		fprintf(stderr, "packet decode error=%d\n", err);
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

int main() {
	TwClient client = {};
	twclient_init(&client);

	twclient_connect(&client);

	while(1) {
		uint8_t buf[DDNET_MAX_PACKET_SIZE];
		ssize_t len = twclient_recv(&client, buf, sizeof(buf));
		if(len < 1) {
			continue;
		}

		char hex[2048];
		str_hex(hex, sizeof(hex), buf, (int)len);
		printf("got %ld bytes: %s\n", len, hex);
		twclient_on_network_data(&client, buf, len);
	}
}
