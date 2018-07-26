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
#include <database_cache/eardbd_api.h>

static socket_t sock_main;
static socket_t sock_mirr;
static packet_header_t header;
static int _connected = -1;

/*
 *
 * API
 *
 */

static int is_connected(socket_t *socket)
{
	int connected = 0;
	connected |= socket->fd >= 0 && socket->protocol == TCP;
	//connected |= socket->fd
	return 0;
}

static state_t _packet_send(uint content_type, char *content, ssize_t content_size)
{
	state_t s;

	// Main packet
	header.content_type = content_type;
	header.data_extra1 = 0; // Mirroring
	header.data_extra2 = 0; // Its ok

	//header->content_size = sizeof(packet_header_t) + content_size;
	//header->content_type = content_type;
	//header->data_extra = header->data_extra;


	s = sockets_send(&sock_main, &header, content, content_size);

	if (state_fail(s)) {
		eardbd_disconnect();
		return s;
	}

	// Sending to mirror
	if (sock_mirr.fd == -1) {
		return EAR_SUCCESS;
	}

	header.data_extra = 1; // Mirroring
	s = sockets_send(&sock_mirr, &header, content, content_size);

	if (state_fail(s)) {
		eardbd_disconnect();
		return s;
	}

	return EAR_SUCCESS;
}

state_t eardbd_send_application(application_t *app)
{
	return _packet_send(CONTENT_TYPE_APP, (char *) app, sizeof(application_t));
}

state_t eardbd_send_periodic_metric(periodic_metric_t *met)
{
	return _packet_send(CONTENT_TYPE_PER, (char *) met, sizeof(periodic_metric_t));
}

state_t eardbd_send_event(ear_event_t *eve)
{
	return _packet_send(CONTENT_TYPE_EVE, (char *) eve, sizeof(ear_event_t));
}

state_t eardbd_send_loop(loop_t *loop)
{
	return _packet_send(CONTENT_TYPE_LOO, (char *) loop, sizeof(loop_t));
}

state_t eardbd_ping()
{
	char ping[] = "ping";
	return _packet_send(CONTENT_TYPE_PIN, (char *) ping, sizeof(ping));
}

static state_t _eardbd_socket(socket_t *socket, char *host, uint port, uint protocol)
{
	state_t s;
	s = sockets_init(&sock_main, host, port, protocol);

	if (state_fail(s)) {
		return s;
	}

	s = sockets_socket(&sock_main);

	if (protocol == TCP)
	{
		s = sockets_connect(&sock_main);
		printf("api = %d (%s) %p\n", s, state_error, &state_error);

		if (state_fail(s)) {
			return s;
		}
	}

	return EAR_SUCCESS;
}

state_t eardbd_connect(char *host_main, char *host_mirror, uint port, uint protocol)
{
	state_t s;

	if (host_main == NULL) {
		return EAR_ERROR;
	}

	// Resetting type data
	sockets_clean(&sock_main);
	sockets_clean(&sock_mirr);

	// Connecting to main
	s = _eardbd_socket(&sock_main, host_main, port, protocol);

	if (state_fail(s)) {
		eardbd_disconnect();
		return s;
	}

	if (host_mirror == NULL) {
		return EAR_SUCCESS;
	}

	s = _eardbd_socket(&sock_mirr, host_mirror, port, protocol);

	if (state_fail(s)) {
		eardbd_disconnect();
		return s;
	}

	return EAR_SUCCESS;
}

state_t eardbd_disconnect()
{
	printf("eardbd_disconnect\n");
	sockets_dispose(&sock_main);
	sockets_dispose(&sock_mirr);
	return EAR_SUCCESS;
}
