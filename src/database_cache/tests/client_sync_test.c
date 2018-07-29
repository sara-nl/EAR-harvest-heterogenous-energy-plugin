#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <common/states.h>
#include <database_cache/eardbd_api.h>

void usage(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage: %s IP/Hostname\n", argv[0]);
		exit(1);
	}
}

/*
 *
 */

int main(int argc, char **argv)
{
	sync_qst_t question;
	packet_header_t header;
	char buffer[4096];
	socket_t socket;
	state_t s;

	//
	usage(argc, argv);

	//
	sockets_clean(&socket);

	s = sockets_init(&socket, argv[1], 4713, TCP);

	s = sockets_socket(&socket);

	if (state_fail(s)) printf("ERROR, while creating socket (%s)\n", intern_error_str);
	else printf("created socket\n");

	s = sockets_connect(&socket);

	if (state_fail(s)) printf("ERROR, while connecting socket (%s)\n", intern_error_str);
	else printf("connected\n");

	sleep(2);

	// Main packet
	header.content_type = CONTENT_TYPE_QST;
	header.content_size = sizeof(sync_qst_t);
	header.data_extra1 = 0; // Mirroring

	s = sockets_send(&socket, &header, (char *) &question);

	if (state_fail(s)) printf("ERROR, while sending (%s)\n", intern_error_str);
	else printf("sended question\n");

	//
	//struct timeval tv;
	//tv.tv_sec = 10;
	//tv.tv_usec = 0;
	//setsockopt(socket.fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof tv);

	//
	s = sockets_receive(socket.fd, &header, buffer, 4096);

	if (state_fail(s)) printf("ERROR, while receiving (%s)\n", intern_error_str);
	else printf("received type %d\n", header.content_type);


	sockets_disconnect(&socket.fd);

	return 0;
}
