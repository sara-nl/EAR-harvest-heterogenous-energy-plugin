/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING
*/

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/sockets.h>

static char output_buffer[SZ_BUFF_BIG];
static state_t s;

state_t sockets_accept(int fd_req, int *fd_cli)
{
	struct sockaddr_storage cli_addr;
	socklen_t size = sizeof (cli_addr);

	*fd_cli = accept(fd_req, (struct sockaddr *) &cli_addr, &size);

	if (*fd_cli == -1) {
		state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_header_clean(packet_header_t *header)
{
	memset((void *) header, 0, sizeof(packet_header_t));
}

state_t sockets_header_update(packet_header_t *header)
{
	if (strlen(header->host_src) == 0) {
		gethostname(header->host_src, sizeof(header->host_src));
	}

	header->timestamp = time(NULL);
}

state_t sockets_listen(socket_t *socket)
{
	if (socket->protocol == UDP) {
		state_return_msg(EAR_BAD_ARGUMENT, 0, "UDP port can't be listened");
	}

	// Listening the port
	int r = listen(socket->fd, BACKLOG);

	if (r < 0) {
		state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_bind(socket_t *socket)
{
	// Assign to the socket the address and port
	int r = bind(socket->fd, socket->info->ai_addr, socket->info->ai_addrlen);

	if (r < 0) {
		state_return_msg(EAR_SOCK_OP_ERROR, errno, (char *) strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_socket(socket_t *sock)
{
	struct addrinfo hints;
	char c_port[16];
	int r;

	// Format
	sprintf(c_port, "%u", sock->port);
	memset(&hints, 0, sizeof (hints));

	//
	hints.ai_socktype = sock->protocol; // TCP stream sockets
	hints.ai_family = AF_UNSPEC;		// Don't care IPv4 or IPv6

	if (sock->host == NULL) {
		hints.ai_flags = AI_PASSIVE;    // Fill in my IP for me
	}

	if ((r = getaddrinfo(sock->host, c_port, &hints, &sock->info)) != 0) {
		state_return_msg(EAR_ADDR_NOT_FOUND, errno, (char *) gai_strerror(r));
	}

	//
	sock->fd = socket(sock->info->ai_family, sock->info->ai_socktype, sock->info->ai_protocol);

	if (sock->fd < 0) {
		state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_init(socket_t *socket, char *host, uint port, uint protocol)
{
	state_t s;

	if (protocol != TCP && protocol != UDP) {
		state_return_msg(EAR_BAD_ARGUMENT, 0, "protocol does not exist");
	}

	if (host != NULL) {
		socket->host = strcpy(socket->host_dst, host);
	} else {
		socket->host = NULL;
	}

	socket->fd = -1;
	socket->port = port;
	socket->info = NULL;
	socket->protocol = protocol;

	state_return(EAR_SUCCESS);
}

state_t sockets_dispose(socket_t *socket)
{
	sockets_close_fd(socket->fd);

	if (socket->info != NULL) {
        freeaddrinfo(socket->info);
        socket->info = NULL;
    }

	return sockets_clean(socket);
}

state_t sockets_clean(socket_t *socket)
{
	memset((void *) socket, 0, sizeof(socket_t));	
    socket->fd = -1;

    state_return(EAR_SUCCESS);
}

state_t sockets_connect(socket_t *socket)
{
	int r;

	// Assign to the socket the address and port
	r = connect(socket->fd, socket->info->ai_addr, socket->info->ai_addrlen);

	if (r < 0) {
		state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_close(socket_t *socket)
{
	state_t s;

	s = sockets_close_fd(socket->fd);
	socket->fd = -1;

	state_return(s);
}

state_t sockets_close_fd(int fd)
{
	if (fd > 0) {
		int r = close(fd);
	}

	state_return(EAR_SUCCESS);
}

// Send:
//
// Errors:
//	Returning EAR_BAD_ARGUMENT
//	 - Header or buffer pointers are NULL
//	Returning EAR_SOCK_DISCONNECTED
// 	 - On ENETUNREACH, ENETDOWN, ECONNRESET, ENOTCONN, EPIPE and EDESTADDRREQ
//	Returning EAR_NOT_READY
//	 - On EAGAIN
//	Returning EAR_ERROR
//	 - On EACCES, EBADF, ENOTSOCK, EOPNOTSUPP, EINTR, EIO and ENOBUFS

state_t sockets_send(socket_t *socket, packet_header_t *header, char *content)
{
	packet_header_t *output_header;
	char *output_content;
	ssize_t bytes_sent;
	ssize_t bytes_expc;
	ssize_t bytes_left;
	state_t s;
	int n;

	if (header == NULL || content == NULL) {
		state_return_msg(EAR_BAD_ARGUMENT, 0, "passing parameter can't be NULL");
	}

	// Output
	output_header = PACKET_HEADER(output_buffer);
	output_content = PACKET_CONTENT(output_buffer);

	// Header process
	memcpy(output_header, header, sizeof(packet_header_t));

	// Content process
	memcpy (output_content, content, header->content_size);

	// Sizes
	bytes_expc = sizeof(packet_header_t) + header->content_size;
	bytes_left = bytes_expc;
	bytes_sent = 0;

	// Sending
	while(bytes_sent < bytes_expc)
	{
		if (socket->protocol == TCP) {
			bytes_sent += send(socket->fd, output_buffer + bytes_sent, bytes_left, 0);
		} else {
			bytes_sent += send(socket->fd, output_buffer + bytes_sent, bytes_left, 0);
			//bytes_sent += sendto(socket->fd, output_buffer + bytes_sent, bytes_left, 0, socket->info->ai_addr, socket->info->ai_addrlen);
		}

		if (bytes_sent < 0)
		{
			switch (bytes_sent) {
				case ENETUNREACH:
				case ENETDOWN:
				case ECONNRESET:
				case ENOTCONN:
				case EPIPE:
				case EDESTADDRREQ:
					state_return_msg(EAR_SOCK_DISCONNECTED, errno, strerror(errno));
				case EAGAIN:
					state_return_msg(EAR_NOT_READY, errno, strerror(errno));
				default:
					state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
			}
		}

		bytes_left -= bytes_sent;
	}

	state_return(EAR_SUCCESS);
}

// Receive:
//
// Errors:
//	Returning EAR_BAD_ARGUMENT
//	 - File descriptor less than 0
//	 - Header or buffer pointers are NULL
//	 - Buffer size is less than the packet content size
//	Returning EAR_SOCK_DISCONNECTED
//	 - Received 0 bytes (meaning disconnection)
//	 - On ECONNRESET, ENOTCONN and ETIMEDOUT
//	Returning EAR_NOT_READY
//	 - On EAGAIN and EINVAL
//	Returning EAR_ERROR
//	 - On EBADF, ENOTSOCK, EOPNOTSUPP, EINTR, EIO, ENOBUFS and ENOMEM

state_t sockets_receive(int fd, packet_header_t *header, char *buffer, ssize_t size_buffer)
{
	ssize_t bytes_recv;
	ssize_t bytes_expc;
	ssize_t bytes_left;
	char *bytes_buff;
	int i = 0;

	if (fd < 0) {
		state_return_msg(EAR_BAD_ARGUMENT, 0, "invalid file descriptor");
	}

	if (header == NULL || buffer == NULL) {
		state_return_msg(EAR_BAD_ARGUMENT, 0, "passing parameter can't be NULL");
	}

	// Receiving header
	bytes_buff = (char *) header;
	bytes_expc = sizeof(packet_header_t);
	bytes_left = bytes_expc;
	bytes_recv = 0;

	while (i < 2)
	{
		while (bytes_recv < bytes_expc)
		{
			bytes_recv += recv(fd, (void *) &bytes_buff[bytes_recv], bytes_left, 0);

			if (bytes_recv <= 0)
			{
				switch (bytes_recv) {
					case 0:
						state_return_msg(EAR_SOCK_DISCONNECTED, 0, "disconnected from socket");
					case ENOTCONN:
					case ETIMEDOUT:
					case ECONNRESET:
						state_return_msg(EAR_SOCK_DISCONNECTED, errno, strerror(errno));
					case EINVAL:
					case EAGAIN:
						state_return_msg(EAR_NOT_READY, errno, strerror(errno));
					default:
						state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
				}
			}

			bytes_left -= bytes_recv;
		}

		bytes_buff = buffer;
		bytes_expc = header->content_size;
		bytes_left = bytes_expc;
		bytes_recv = 0;
		i++;

		if (bytes_expc > size_buffer) {
			printf("buffer to small for the received object %ld %ld\n", bytes_expc, size_buffer);
			state_return_msg(EAR_BAD_ARGUMENT, 0, "buffer to small for the received object");
		}
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_get_timeout(int fd, time_t *timeout)
{
	struct timeval tv;
	socklen_t length;

	if (getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, &length) != 0)
	{
		state_return_msg(EAR_ERROR, errno, strerror(errno));
		*timeout = 0;
	}

	*timeout = (time_t) tv.tv_sec;
	state_return(EAR_SUCCESS);
}

state_t sockets_set_timeout(int fd, time_t timeout)
{
	struct timeval tv;

	tv.tv_sec  = timeout;
	tv.tv_usec = 0;

	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) timeout, sizeof(struct timeval)) != 0) {
		state_return_msg(EAR_ERROR, errno, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

void sockets_print_socket(socket_t *socket)
{
	printf("socket (%d, %u, %u, '%s')\n", socket->fd, socket->port, socket->protocol, socket->host);
}

void sockets_print_sockaddr(struct sockaddr *host_addr)
{
	char buffer[512];
	char *ip_version;
	void *address;
	int port;

	// IPv4
	if (host_addr->sa_family == AF_INET)
	{
		struct sockaddr_in *ipv4 = (struct sockaddr_in *) host_addr;
		address = &(ipv4->sin_addr);
		port = (int) ntohs(ipv4->sin_port);
		ip_version = "IPv4";
	}
		// IPv6
	else
	{
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) host_addr;
		address = &(ipv6->sin6_addr);
		ip_version = "IPv6";
	}

	// convert the IP to a string and print it
	inet_ntop(host_addr->sa_family, address, buffer, INET6_ADDRSTRLEN);

	if (buffer[0] != ':') {
		printf("%s:%d", buffer, port);
	}
}
