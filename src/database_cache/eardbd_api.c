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

#define verbose(...) \
        fprintf(stderr, "EARDBD, " __VA_ARGS__); \
        fprintf(stderr, "\n");

#if 1
#define debug(format, ...) \
        fprintf(stderr, "%s: " format "\n", __FUNCTION__, __VA_ARGS__);
#else
#define debug(format, ...)
#endif

static packet_header_t header;
static socket_t server_sock;
static socket_t mirror_sock;
static int enabled_server;
static int enabled_mirror;

/*
 *
 * Intern functions
 *
 */

static edb_state_t _send(uint content_type, char *content, ssize_t content_size)
{
	edb_state_t state;

	// If not initialized
	if (!enabled_server && !enabled_mirror) {
		edb_state_return_msg(state, EAR_NOT_INITIALIZED, "not initialized");

	}

	//
	sockets_header_update(&header);
	header.content_type = content_type;
	header.content_size = content_size;
	edb_state_init(state, EAR_SUCCESS);

	if (!enabled_server) {
		state.server = sockets_send(&server_sock, &header, content);
	}
	if (!enabled_mirror) {
		state.mirror = sockets_send(&mirror_sock, &header, content);
	}

	//
	debug("state.server %d, state.mirror %d", state.server, state.mirror);

	return state;
}

static state_t _connect(socket_t *socket, char *host, uint port, uint protocol)
{
	state_t s;

	//
	s = sockets_init(socket, host, port, protocol);

	if (state_fail(s)) {
		return s;
	}

	//
	s = sockets_socket(socket);

	if (state_fail(s)) {
		return s;
	}

	//
	if (protocol == TCP)
	{
		s = sockets_connect(socket);

		if (state_fail(s)) {
			return s;
		}
	}

	return EAR_SUCCESS;
}

static void _disconnect(socket_t *socket, int *enabled)
{
    sockets_dispose(socket);
    *enabled = 0;
}

/*
 *
 * API functions
 *
 */

edb_state_t eardbd_send_application(application_t *app)
{
	return _send(CONTENT_TYPE_APM, (char *) app, sizeof(application_t));
}

edb_state_t eardbd_send_periodic_metric(periodic_metric_t *met)
{
	return _send(CONTENT_TYPE_PER, (char *) met, sizeof(periodic_metric_t));
}

edb_state_t eardbd_send_event(ear_event_t *eve)
{
	return _send(CONTENT_TYPE_EVE, (char *) eve, sizeof(ear_event_t));
}

edb_state_t eardbd_send_loop(loop_t *loop)
{
	return _send(CONTENT_TYPE_LOO, (char *) loop, sizeof(loop_t));
}

edb_state_t eardbd_ping()
{
	char ping[] = "ping";
	return _send(CONTENT_TYPE_PIN, (char *) ping, sizeof(ping));
}

edb_state_t eardbd_connect(cluster_conf_t *conf, my_node_conf_t *node)
{
	edb_state_t state;
	char *server_host;
	char *mirror_host;
	uint server_port;
	uint mirror_port;

	if (enabled_server || enabled_mirror) {
		edb_state_return_msg(state, EAR_ALREADY_INITIALIZED, "it's already initialized");
	}

	enabled_server = (server_host == NULL) || (strlen(server_host) == 0);
	enabled_mirror = (mirror_host == NULL) || (strlen(mirror_host) == 0);

	//
	debug("enabled_server %d, enabled_mirror %d", enabled_server, enabled_mirror);

	// Neither server nor mirror enabled? Bad argument.
	if (!enabled_server && !enabled_mirror) {
		edb_state_return_msg(state, EAR_BAD_ARGUMENT, "server and mirror are not enabled");

	}

#if 1
	// Configuring hosts and ports
	server_host = node->db_ip;
	mirror_host = node->db_sec_ip;
	server_port = conf->db_manager.tcp_port;
	mirror_port = conf->db_manager.sec_tcp_port;
#else
	server_port = 4711;
	mirror_port = 4712;
	server_host = "E7450";
	mirror_host = "E7450";
#endif

	// Maybe it's not enabled, but it's ok.
	edb_state_init(state, EAR_SUCCESS);

	if (!enabled_server)
	{
		state.server = _connect(&server_sock, server_host, server_port, TCP);

		if (state_fail(state.server)) {
			_disconnect(&server_sock, &enabled_server);
		}
	}
	if (!enabled_mirror)
	{
		state.mirror = _connect(&mirror_sock, mirror_host, mirror_port, TCP);

		if (state_fail(state.mirror)) {
			_disconnect(&mirror_sock, &enabled_mirror);
		}
	}

	//
	debug("state.server %d, state.mirror %d", state.server, state.mirror);

	return state;
}

edb_state_t eardbd_reconnect(cluster_conf_t *conf, my_node_conf_t *node)
{
	eardbd_disconnect();

	return eardbd_connect(conf, node);
}

edb_state_t eardbd_disconnect()
{
	edb_state_t state;

	_disconnect(&server_sock, &enabled_server);
	_disconnect(&mirror_sock, &enabled_mirror);

	edb_state_return(state, EAR_SUCCESS);
}
