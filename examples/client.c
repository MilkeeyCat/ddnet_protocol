#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include <ddnet_protocol/huffman.h>
#include <ddnet_protocol/packet.h>

typedef struct {
	int socket;
	struct sockaddr_in addr;
	struct sockaddr_in server_addr;
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
		// this is spamming idk why
		if(errno == 11) {
			return 0;
		}
		fprintf(stderr, "network error: %s\n", strerror(errno));
		return -1;
	}
	if(bytes == 0) {
		return 0;
	}

	printf("got %ld bytes\n", bytes);
	return bytes;
}

void twclient_init(TwClient *client) {
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

void twclient_connect(TwClient *client) {
	client->server_addr.sin_family = AF_INET;
	client->server_addr.sin_port = htons(8303);
	unsigned char dst_ip[4] = {127, 0, 0, 1};
	memcpy(&client->server_addr.sin_addr.s_addr, dst_ip, 4);

	uint8_t connect_msg[512] = {0};
	twclient_send(client, connect_msg, sizeof(connect_msg));
}

int main() {
	TwClient client = {};
	twclient_init(&client);

	twclient_connect(&client);

	while(1) {
		uint8_t buf[MAX_PACKET_SIZE];
		ssize_t len = twclient_recv(&client, buf, sizeof(buf));
		if(len < 1) {
			continue;
		}

	}
}
