#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <common/sockets.h>

void fake_send(socket_t *socket, char *output_buffer, ssize_t output_bytes)
{
	packet_header_t *output_header = (packet_header_t *) output_buffer;
	ssize_t bytes_sent;
	ssize_t bytes_expc;
	ssize_t bytes_left;

	bytes_expc = output_bytes;
	bytes_left = bytes_expc;
	bytes_sent = 0;

	output_header->content_size = output_bytes + 100;

	while(bytes_sent < bytes_expc)
	{
		bytes_sent += send(socket->fd, output_buffer + bytes_sent, bytes_left, 0);

		if (bytes_sent < 0)	
			fprintf(stderr, "Error.\n");

		bytes_left -= bytes_sent;
	}
}

int main(int argc, char *argv[])
{
	char buffer[4096];
	socket_t server_sock;
	socket_t mirror_sock;
	state_t s;	

	sockets_clean(&server_sock);
	sockets_clean(&mirror_sock);

	s = sockets_init(&server_sock, "mgt2309", 4711, TCP);
	s = sockets_init(&mirror_sock, "mgt2309", 4712, TCP);
	s = sockets_socket(&server_sock);
	s = sockets_socket(&mirror_sock);
	s = sockets_connect(&server_sock);
	s = sockets_connect(&mirror_sock);

	//while(1) {
		fake_send(&server_sock, buffer, 2000);
		//fake_send(&mirror_sock, buffer, 2000);
		sleep(60);	
	//}

	return 0;
}
