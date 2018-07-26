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

#ifndef EAR_SOCKETS_H
#define EAR_SOCKETS_H

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <common/sizes.h>
#include <common/states.h>
#include <common/types/generic.h>

#define BACKLOG				10
#define TCP					SOCK_STREAM
#define UDP					SOCK_DGRAM

/* types */
typedef struct socket {
	char host_dst[SZ_NAME_SHORT];
	struct addrinfo *info;
	char *host;
	uint protocol;
	uint port;
	int fd;
} socket_t;

typedef struct packet_header {
	char host_src[SZ_NAME_SHORT]; // Filled in sockets_send()
	time_t timestamp; // Filled in sockets_send()
	size_t content_size; // Filled in sockets_send()
	uchar content_type;
	uchar data_extra1;
	uchar data_extra2;
	uchar data_extra3;
}  packet_header_t;

#define PACKET_HEADER(buffer) \
	(packet_header_t *) buffer;

#define PACKET_CONTENT(buffer) \
	(void *) &buffer[sizeof(packet_header_t)];

/* functions */
state_t sockets_header_clean(packet_header_t *header);

state_t sockets_init(socket_t *socket, char *host, uint port, uint protocol);

state_t sockets_dispose(socket_t *socket);

state_t sockets_clean(socket_t *socket);

state_t sockets_socket(socket_t *socket);

state_t sockets_bind(socket_t *socket);

state_t sockets_listen(socket_t *socket);

state_t sockets_accept(int fd_req, int *fd_cli);

state_t sockets_connect(socket_t *socket);

state_t sockets_disconnect(int *fd);

state_t sockets_send(socket_t *socket, packet_header_t *header, char *content);

state_t sockets_receive(int fd, packet_header_t *header, char *buffer, ssize_t size_buffer);

state_t sockets_set_timeout(int fd, struct timeval time);

state_t sockets_header_clean(packet_header_t *header);

state_t sockets_header_update(packet_header_t *header);

void sockets_print_socket(socket_t *socket);

void sockets_print_sockaddr(struct sockaddr *host_addr);

#endif //EAR_SOCKETS_H
