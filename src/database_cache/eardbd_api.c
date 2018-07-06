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
#include <database_cache/sockets.h>
#include <database_cache/eardbd_api.h>

static socket_t sock_main;
static socket_t sock_mirr;
static int connected = -1;

static char buffer_gen[4096];
static char buffer_pck[MAX_PACKET_SIZE()];

/*
 * API
 */

#define CONNECTION_TEST(pass_text) \
	if (sock_main.fd == -1 || sock_main.protocol == -1 || (sock_main.protocol == TCP && connected == -1)) { \
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

	//CONNECTION_TEST();
	verbose("sending application (size: %lu)", sizeof(application_t));

	size_pck = prepare_packet(buffer_pck, type, (void *) app, sizeof(application_t));
	return sockets_send(&sock_main, (void *) buffer_pck, size_pck);
}

int eardbd_send_periodic_metric(periodic_metric_t *met)
{
	unsigned char type = CONTENT_TYPE_PER;
	size_t size_pck;

	//CONNECTION_TEST();
	verbose("sending periodic metric (size: %lu) %d", sizeof(periodic_metric_t), type);

	size_pck = prepare_packet(buffer_pck, type, (void *) met, sizeof(periodic_metric_t));
	return sockets_send(&sock_main, (void *) buffer_pck, size_pck);
}

int eardbd_send_event(ear_event_t *eve)
{
	unsigned char type = CONTENT_TYPE_EVE;
	size_t size_pck;

	CONNECTION_TEST();
    verbose("sending event");

	size_pck = prepare_packet(buffer_pck, type, (void *) eve, sizeof(ear_event_t));
	return sockets_send(&sock_main, (void *) buffer_pck, size_pck);
}

int eardbd_send_loop(loop_t *loop)
{
	unsigned char type = CONTENT_TYPE_LOO;
	size_t size_pck;

    CONNECTION_TEST();
    verbose("sending loop");

	size_pck = prepare_packet(buffer_pck, type, (void *) loop, sizeof(loop_t));
	return sockets_send(&sock_main, (void *) buffer_pck, size_pck);
}

int eardbd_ping()
{
	char ping[] = "ping";

	CONNECTION_TEST();
	verbose("sending ping");

	return sockets_send(&sock_main, ping, sizeof(ping));
}

int eardbd_is_connected()
{
	CONNECTION_TEST();
	return EAR_SUCCESS;
}

int eardbd_connect(char *host, uint port, uint protocol)
{
	state_t r;

	r = sockets_init(&sock_main, host, port, protocol);

	if (state_fail(r)) {
		sockets_dispose(&sock_main);
		return r;
	}

	r = sockets_socket(&sock_main);

	if (protocol == TCP)
	{
		r = sockets_connect(&sock_main);

		if (state_fail(r)) {
			sockets_dispose(&sock_main);
			return r;
		}
	}

	return r;
}

int eardbd_disconnect()
{
	return sockets_dispose(&sock_main);
}
