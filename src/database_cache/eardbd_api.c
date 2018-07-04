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
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <database_cache/eardbd_api.h>
#include <common/states.h>

static char buffer_gen[4096];
static char buffer_pck[MAX_PACKET_SIZE()];

static struct addrinfo *srv_info;

int _protocol = -1;
int connected = -1;
int fd_srv    = -1;

/*
 * Generic functions
 */

static void print_sockaddr(struct sockaddr *host_addr)
{
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
	inet_ntop(host_addr->sa_family, address, buffer_gen, INET6_ADDRSTRLEN);

	if (buffer_gen[0] != ':') {
		verbose("%s:%d", buffer_gen, port);
	}
}

static int _send(int fd, void *object, size_t size)
{
	size_t bytes_left = size;
	size_t bytes_sent = 0;
	int n;

	while(bytes_sent < size)
	{
		if (_protocol == TCP) {
			verbose("sending by TCP an object of size %lu by socket %d", bytes_left, fd);
			n = send(fd, object + bytes_sent, bytes_left, 0);
		} else {
			verbose("sending UDP an object of size %lu using socket %d", bytes_left, fd);
			n = sendto(fd, object + bytes_sent, bytes_left, 0, srv_info->ai_addr, srv_info->ai_addrlen);
		}

		if (n == -1) {
			error("sending (%s)", strerror(errno));
			return EAR_ERROR;
		}
		verbose("sended");

		bytes_sent += n;
		bytes_left -= n;
	}

	return EAR_SUCCESS;
}

static int _socket(char *host, unsigned int port, int protocol)
{
	static struct addrinfo hints;
	char c_port[16];
	int status;

	// Format
	sprintf(c_port, "%u", port);
	memset(&hints, 0, sizeof hints);
	
	hints.ai_socktype = protocol;
	hints.ai_family = AF_UNSPEC; // Don't care IPv4 or IPv6

	if ((status = getaddrinfo(host, c_port, &hints, &srv_info)) != 0)
	{
		error("getaddrinfo error (%s)", gai_strerror(status));
		return EAR_ERROR;
	}

	fd_srv = socket(srv_info->ai_family, srv_info->ai_socktype, srv_info->ai_protocol);

	if (fd_srv < 0) {
		return EAR_ERROR;
	}

	return EAR_SUCCESS;
}

static int _disconnect()
{
	if (fd_srv > 0) {
		close(fd_srv);
	}

	connected = -1;
	fd_srv    = -1;

	return EAR_SUCCESS;
}

static int _connect(char *host, unsigned int port)
{
	int status;

	status = _socket(host, port, TCP);

	if (status < 0) {
		error("opening socket");
		return EAR_ERROR;
	}

	verbose("openned socket");

	// Assign to the socket the address and port
	status = connect(fd_srv, srv_info->ai_addr, srv_info->ai_addrlen);

	if (status < 0) {
		error("conecting socket");
		return EAR_ERROR;
	}

	verbose("connected");

	freeaddrinfo(srv_info);
	connected = 1;

	print_sockaddr(srv_info->ai_addr);

	return EAR_SUCCESS;
}

/*
 * API
 */
#define CONNECTION_TEST(pass_text) \
	if (fd_srv == -1 || _protocol == -1 || (_protocol == TCP && connected == -1)) { \
		return EAR_ERROR; \
	} \

static size_t prepare_packet(char *buffer, unsigned char type, void *object, size_t object_size)
{
	packet_header_t *header = P_HEADER(buffer);
	void *content = P_CONTENT(buffer);

	header->content_type = type;
	memcpy (content, object, sizeof(object_size));

	return sizeof(packet_header_t) + object_size;
}

int eardbd_send_application(application_t *app)
{
	unsigned char type = CONTENT_TYPE_APP;
	size_t size_pck;

	CONNECTION_TEST();
	verbose("sending application %lu", sizeof(application_t));

	size_pck = prepare_packet(buffer_pck, type, (void *) app, sizeof(application_t));
	return _send(fd_srv, (void *) buffer_pck, size_pck);
}

int eardbd_send_periodic_metric(periodic_metric_t *met)
{
	unsigned char type = CONTENT_TYPE_PER;
	size_t size_pck;

	CONNECTION_TEST("sending application");

	size_pck = prepare_packet(buffer_pck, type, (void *) met, sizeof(periodic_metric_t));
	return _send(fd_srv, (void *) buffer_pck, size_pck);
}

int eardbd_send_event(ear_event_t *eve)
{
	unsigned char type = CONTENT_TYPE_EVE;
	size_t size_pck;

	CONNECTION_TEST();
    verbose("sending event");

	size_pck = prepare_packet(buffer_pck, type, (void *) eve, sizeof(ear_event_t));
	return _send(fd_srv, (void *) buffer_pck, size_pck);
}

int eardbd_send_loop(loop_t *loop)
{
	unsigned char type = CONTENT_TYPE_LOO;
	size_t size_pck;

    CONNECTION_TEST();
    verbose("sending loop");

	size_pck = prepare_packet(buffer_pck, type, (void *) loop, sizeof(loop_t));
	return _send(fd_srv, (void *) buffer_pck, size_pck);
}

int eardbd_ping()
{
	char ping[] = "ping";

	CONNECTION_TEST();
	verbose("sending ping");

	return _send(fd_srv, ping, sizeof(ping));
}

int eardbd_is_connected()
{
	CONNECTION_TEST();
	return EAR_SUCCESS;
}

int eardbd_connect(char *host, unsigned int port, int protocol)
{
	int status;

	if (protocol == TCP) {
		status = _connect(host, port);
		//status = _connect(host_aux, port);
	} else if (protocol == UDP) {
		status = _socket(host, port, UDP);
		//status = _socket(host_aux, port, UDP);
	} else {
		error("unknown protocol");
		return EAR_ERROR;
	}

	if (status < EAR_SUCCESS) {
		_disconnect();
		return EAR_ERROR;
	}

	_protocol = protocol;
	verbose("using %d fd", fd_srv);

	return EAR_SUCCESS;
}

int eardbd_disconnect()
{
	return _disconnect();
}
