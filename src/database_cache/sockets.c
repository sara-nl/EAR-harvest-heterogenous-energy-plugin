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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <database_cache/sockets.h>

static state_t s;

state_t sockets_accept(int fd_req, int *fd_cli)
{
	struct sockaddr_storage cli_addr;
	socklen_t size = sizeof (cli_addr);

	*fd_cli = accept(fd_req, (struct sockaddr *) &cli_addr, &size);

	if (*fd_cli == -1) {
		state_return_msg(EAR_SOCK_ACCEPT_ERROR, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_send(socket_t *socket, char *buffer, ssize_t size)
{
	size_t bytes_left = size;
	size_t bytes_sent = 0;
	int n;

	while(bytes_sent < size)
	{
		if (socket->protocol == TCP) {
			n = send(socket->fd, buffer + bytes_sent, bytes_left, 0);
		} else {
			n = sendto(socket->fd, buffer + bytes_sent, bytes_left, 0, socket->info->ai_addr, socket->info->ai_addrlen);
		}

		if (n == -1) {
			state_return_msg(EAR_SOCK_SEND_ERROR, strerror(errno));
		}

		bytes_sent += n;
		bytes_left -= n;
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_receive(int fd, char *buffer, ssize_t size_buffer, ssize_t *size_read)
{
	*size_read = recv(fd, buffer, size_buffer, 0);

	// Handle data from a client
	if (*size_read <= 0)
	{
		if (*size_read == 0) {
			state_return_msg(EAR_SUCCESS, "disconnected from socket");
		} else {
			state_return_msg(EAR_SOCK_RECV_ERROR, strerror(errno));
		}
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_listen(socket_t *socket)
{
	if (socket->protocol == UDP) {
		state_return_msg(EAR_SOCK_LISTEN_ERROR, "UDP port can't be listened");
	}

	// Listening the port
	int r = listen(socket->fd, BACKLOG);

	if (r < 0) {
		state_return_msg(EAR_SOCK_LISTEN_ERROR, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_bind(socket_t *socket)
{
	// Assign to the socket the address and port
	int r = bind(socket->fd, socket->info->ai_addr, socket->info->ai_addrlen);

	if (r < 0) {
		state_return_msg(EAR_SOCK_BIND_ERROR, (char *) strerror(errno));
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
		state_return_msg(EAR_ADDR_NOT_FOUND, (char *) gai_strerror(r));
	}

	//
	sock->fd = socket(sock->info->ai_family, sock->info->ai_socktype, sock->info->ai_protocol);

	if (sock->fd < 0) {
		state_return_msg(EAR_SOCK_CREAT_ERROR, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_init(socket_t *socket, char *host, uint port, uint protocol)
{
	state_t s;

	if (protocol != TCP && protocol != UDP) {
		state_return_msg(EAR_SOCK_BAD_PROTOCOL, "protocol does not exist");
	}

	if (host != NULL) {
		socket->host = strcpy(socket->hostname, host);
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
	if (socket->fd >= 0) {
		sockets_disconnect(&socket->fd);
	}

	if (socket->info != NULL) {
		freeaddrinfo(socket->info);
		socket->info = NULL;
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_connect(socket_t *socket)
{
	int r;

	// Assign to the socket the address and port
	r = connect(socket->fd, socket->info->ai_addr, socket->info->ai_addrlen);

	if (r < 0) {
		printf("str %s\n", strerror(errno));
		state_return_msg(EAR_SOCK_CONN_ERROR, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t sockets_disconnect(int *fd)
{
	if (*fd > 0) {
		close(*fd);
	}
	*fd = -1;

	state_return(EAR_SUCCESS);
}

state_t sockets_clean(socket_t *socket)
{
	socket->hostname[0] = '\0';
 	socket->info = NULL;
	socket->host = NULL;
	socket->protocol = 0;
	socket->port = 0;
	socket->fd = -1;

	return EAR_SUCCESS;
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
