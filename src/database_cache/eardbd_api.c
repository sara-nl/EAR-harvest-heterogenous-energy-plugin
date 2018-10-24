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

static socket_t server_sock;
static socket_t mirror_sock;
static packet_header_t header;

#define verbose(...) \
	fprintf(stderr, "EARDBD, " __VA_ARGS__); \
	fprintf(stderr, "\n");

/*
 *
 * Intern functions
 *
 */

static state_t _packet_send(uint content_type, char *content, ssize_t content_size)
{
	state_t server_state;
	state_t mirror_state;

	if (server_sock.fd == -1 && mirror_sock.fd == -1) {
		state_return_msg(EAR_DBD_ERROR_BOTH, 0, "API not initialized");
	}

	// Main packet
	sockets_header_update(&header);
	header.content_type = content_type;
	header.content_size = content_size;
	header.data_extra1 = 0; // Mirroring
	header.data_extra2 = 0; // Its ok

	server_state = sockets_send(&server_sock, &header, content);

	// If the sending fails
	if (state_fail(server_state)) {
		verbose("Failed to send to MAIN EARDBD (num: %d, inum: %d, istr: %s)",
				server_state, intern_error_num, intern_error_str);
	}

	// Sending to mirror
	if (mirror_sock.fd == -1) {
		if (state_fail(server_state)) {
			state_return(EAR_DBD_ERROR_MAIN);
		}
		state_return(server_state);
	}

	header.data_extra1 = 1; // Mirroring
	header.data_extra2 = state_fail(server_state); // It's ok?
	mirror_state = sockets_send(&mirror_sock, &header, content);

	if (state_fail(mirror_state))
	{
		verbose("Failed to send to MIRROR EARDBD (num: %d, inum: %d, istr: %s)",
				mirror_state, intern_error_num, intern_error_str);

		if (state_fail(server_state)) {
			state_return(EAR_DBD_ERROR_BOTH);
		}
		state_return(EAR_DBD_ERROR_MIRR);
	}

	state_return(EAR_SUCCESS);
}

static state_t _eardbd_prepare_socket(socket_t *socket, char *host, uint port, uint protocol)
{
	state_t s;

	s = sockets_init(socket, host, port, protocol);

	if (state_fail(s)) {
		return s;
	}

	s = sockets_socket(socket);

	if (state_fail(s)) {
		return s;
	}

	if (protocol == TCP)
	{
		s = sockets_connect(socket);

		if (state_fail(s)) {
			return s;
		}
	}

	return EAR_SUCCESS;
}

static void _eardbd_disconnect(socket_t *socket)
{
    sockets_dispose(socket);
}

/*
 *
 * API functions
 *
 */

state_t eardbd_send_application(application_t *app)
{
	return _packet_send(CONTENT_TYPE_APM, (char *) app, sizeof(application_t));
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

state_t eardbd_connect(cluster_conf_t *conf, my_node_conf_t *node)
{
	state_t server_state;
	state_t mirror_state;
	char *server_host;
	char *mirror_host;
	uint server_port;
	uint mirror_port;

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

	if (server_host == NULL) {
		return EAR_ERROR;
	}

	// Resetting type data
	sockets_clean(&server_sock);
	sockets_clean(&mirror_sock);

	// Connecting to main
	server_state = _eardbd_prepare_socket(&server_sock, server_host, server_port, TCP);

	if (state_fail(server_state) && mirror_host == NULL) {
		_eardbd_disconnect(&server_sock);
		return EAR_DBD_ERROR_MAIN;
	}

	if (mirror_host == NULL) {
		return EAR_SUCCESS;
	}

	mirror_state = _eardbd_prepare_socket(&mirror_sock, mirror_host, mirror_port, TCP);

	if (state_fail(mirror_state))
	{
		_eardbd_disconnect(&mirror_sock);

		if (state_fail(server_state)) {
			return EAR_DBD_ERROR_BOTH;
		}
		return EAR_DBD_ERROR_MIRR;
	}

	return EAR_SUCCESS;
}

state_t eardbd_reconnect(cluster_conf_t *conf, my_node_conf_t *node)
{
	eardbd_disconnect();
	return eardbd_connect(conf, node);
}

state_t eardbd_disconnect()
{
	_eardbd_disconnect(&server_sock);
	_eardbd_disconnect(&mirror_sock);

	return EAR_SUCCESS;
}
