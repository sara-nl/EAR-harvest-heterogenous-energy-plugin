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
#include <sys/time.h>
#include <sys/types.h>
#include <database_cache/sockets.h>
#include <database_cache/eardbd.h>

#define mets_len 4096
#define lops_len 4096
#define eves_len 4096
#define apps_len 1024

static char buffer_pck[MAX_PACKET_SIZE()];
static char buffer_gen[PATH_MAX];

static periodic_aggregation_t aggr;
static periodic_metric_t mets[mets_len];
static application_t apps[apps_len];
static ear_event_t eves[apps_len];
static loop_t lops[apps_len];

static uint mets_i;
static uint apps_i;
static uint eves_i;
static uint lops_i;

int EAR_VERBOSE_LEVEL=1;

/*
 * Signal processing
 */

/*
 * Data storing/loading
 */

static void db_store_events()
{
	if (eves_i <= 0) {
		return;
	}

    verbose("Trying to insert in DB %d event samples", eves_i);
    //db_batch_insert_ear_event(eves, eves_i);
}

static void db_store_loops()
{
   if (lops_i <= 0) {
        return; 
    }

    verbose("Trying to insert in DB %d loop samples", lops_i);
    //db_batch_insert_loops(lops, lops_i);
}

static void db_store_periodic_metrics()
{
	if (mets_i <= 0) {
		return;
	}

	verbose("Trying to insert in DB %d periodic metric samples", mets_i);
	//db_batch_insert_periodic_metrics(mets, mets_i);
}

static void db_store_periodic_aggregation()
{
	if (aggr.n_samples <= 0) {
		return;
	}

	verbose("Trying to insert in DB an aggregation of %d samples", aggr.n_samples);
	//db_insert_periodic_aggregation(&aggr);
}

static void db_store_applications()
{
	if (apps_i <= 0) {
		return;
	}

	verbose("Trying to insert in DB %d applications samples", apps_i);
	//db_batch_insert_applications(apps, apps_i);
}

/*
 * Data processing
 */

static void make_periodic_aggregation(periodic_metric_t *met)
{
	add_periodic_aggregation(&aggr, met->DC_energy, met->start_time, met->end_time);
}

static void process_timeout_data()
{
	verbose("Finished aggregation, consumed %lu energy (?J) from %lu to %lu,",
			aggr.DC_energy, aggr.start_time, aggr.end_time);

	db_store_periodic_aggregation();
	init_periodic_aggregation(&aggr);

	db_store_periodic_metrics();
	mets_i = 0;

	db_store_applications();
	apps_i = 0;
	
	db_store_events();
	eves_i = 0;

	db_store_loops();
	lops_i = 0;
}

static void process_incoming_data(int fd, char *buffer, ssize_t size)
{
	packet_header_t *header;
	void *content;
	char *type;

	header = P_HEADER(buffer);
	content = P_CONTENT(buffer);

	printf("CONTENT TAIP %u\n", header->content_type);

	if (header->content_type == CONTENT_TYPE_APP) {
		type = "application_t";

		memcpy (&apps[apps_i], content, sizeof(application_t));
		apps_i += 1;

		if (apps_i == apps_len)
		{
			db_store_applications();
			apps_i = 0;
		}
	}
	else if (header->content_type == CONTENT_TYPE_PER) {
		type = "periodic_metric_t";

		memcpy (&mets[mets_i], content, sizeof(periodic_metric_t));
		make_periodic_aggregation(&mets[mets_i]);
		mets_i += 1;

		if (mets_i == mets_len) {
			db_store_periodic_metrics();
			mets_i = 0;
		}
	} else if (header->content_type == CONTENT_TYPE_EVE) {
		type = "ear_event_t";

		memcpy (&eves[eves_i], content, sizeof(ear_event_t));
		eves_i += 1;

		if (eves_i == eves_len) {
			db_store_events();
			eves_i = 0;
		}
	} else if (header->content_type == CONTENT_TYPE_LOO) {
		type = "loop_t";

		memcpy (&lops[lops_i], content, sizeof(loop_t));
		lops_i += 1;

		if (lops_i == eves_len) {
			db_store_loops();
			lops_i = 0;
		}
	} else {
		type = "unknown";
	}

	verbose("received an object type '%s' from the socket %d of size %ld", type, fd, size);
}

/*
 * Server managing
 */

/*
 * Main
 */

void usage(int argc, char **argv)
{
	if (0)
	{
		printf("Usage: %s\n", argv[0]);
		exit(1);
	}
}

int main(int argc, char **argv)
{
	cluster_conf_t conf_clus;
	struct timeval timeout;

	long merge_time;
	float mb_apps;
	float mb_mets;
	float mb_lops;
	float mb_eves;
	ssize_t size;
	state_t state1;
	state_t state2;
	state_t state3;
	int i;

	char *mirror_main_hostname;
	int mirror_me;

	socket_t sockets[3];
	fd_set fds_incoming;
	fd_set fds_active;
	int fd_max;
	int fd_cli;

	socket_t *sock_metr_tcp = &sockets[0];
	socket_t *sock_metr_udp = &sockets[1];
	socket_t *sock_sync_tcp = &sockets[2];

	//
	usage(argc, argv);

	//
	verbose("phase 1: starting");

	mb_apps = (double) (sizeof(application_t)     * apps_len) / 1000000.0;
	mb_mets = (double) (sizeof(periodic_metric_t) * mets_len) / 1000000.0;

	verbose("reserving %0.2f MBytes for applications (%ld)", mb_apps, sizeof(application_t));
	verbose("reserving %0.2f MBytes for power metrics (%ld)", mb_mets, sizeof(periodic_metric_t));
	verbose("reserving %lu bytes for packet buffer", sizeof(buffer_pck));

	// Configuration file (TODO)

	#if 0
	if (get_ear_conf_path(buffer_gen) == EAR_ERROR) {
		error("Error getting ear.conf path");
	}

	verbose("Reading '%s' configuration file", buffer_gen);
	read_cluster_conf(buffer_gen, &conf_clus);
	#else
	conf_clus.db_manager.aggr_time = 3000;
	conf_clus.db_manager.tcp_port = 4711;
	conf_clus.db_manager.udp_port = 4712;
	#endif

	// Format
	merge_time = (long) conf_clus.db_manager.aggr_time;

	timeout.tv_sec = merge_time;
	timeout.tv_usec = 0L;

	FD_ZERO(&fds_incoming);
	FD_ZERO(&fds_active);

	// Opening socket
	sockets_init(sock_metr_tcp, NULL, conf_clus.db_manager.tcp_port, TCP);
	sockets_init(sock_metr_udp, NULL, conf_clus.db_manager.udp_port, UDP);
	sockets_init(sock_sync_tcp, NULL, 4713, TCP);

	state1 = sockets_socket(sock_metr_tcp);
	state2 = sockets_socket(sock_metr_udp);
	state3 = sockets_socket(sock_sync_tcp);

	verbose ("opened metrics socket %d for TCP packets on port %u", sock_metr_tcp->fd, sock_metr_tcp->port);
	verbose ("opened metrics socket %d for UDP packets on port %u", sock_metr_udp->fd, sock_metr_udp->port);
	verbose ("opened sync socket %d for TCP packets on port %u", sock_sync_tcp->fd, sock_sync_tcp->port);

	if (state_fail(state1) || state_fail(state2) || state_fail(state3)) {
		error("while creating sockets (%s)", state_error);
	}

	// Binding socket
	state1 = sockets_bind(sock_metr_tcp);
	state2 = sockets_bind(sock_metr_udp);
	state3 = sockets_bind(sock_sync_tcp);

	if (state_fail(state1) || state_fail(state2) || state_fail(state3)) {
		error("while binding sockets (%s)", state_error);
	}

	// Listening socket
	state1 = sockets_listen(sock_metr_tcp);
	state3 = sockets_listen(sock_sync_tcp);

	if (state_fail(state1) || state_fail(state3)) {
		error("while listening sockets (%s)", state_error);
	}

	// Add the listener to the ready set
	FD_SET(sock_metr_tcp->fd, &fds_active);
	FD_SET(sock_metr_udp->fd, &fds_active);
	FD_SET(sock_sync_tcp->fd, &fds_active);

	// Keep track of the biggest file descriptor
	fd_max = sock_sync_tcp->fd;

	if (sock_metr_tcp->fd > fd_max) {
		fd_max = sock_metr_tcp->fd;
	}
	if (sock_metr_udp->fd > fd_max) {
		fd_max = sock_metr_udp->fd;
	}

	verbose("phase 2: listening (processing every %lu s)", merge_time);

	while(1)
	{
		fds_incoming = fds_active;

		if (select(fd_max + 1, &fds_incoming, NULL, NULL, &timeout) == -1) {
			error("select");
		}

		// If timeout, data processing
		if (timeout.tv_sec == 0 && timeout.tv_usec == 0)
		{
			process_timeout_data();
		
			timeout.tv_sec = merge_time;
			timeout.tv_usec = 0L;
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fd_max; i++)
		{
			if (FD_ISSET(i, &fds_incoming)) // we got one!!
			{
				// Handle new connections
				if (i == sock_metr_tcp->fd)
				{
					sockets_accept(i, &fd_cli);

					if (fd_cli != -1)
					{
						FD_SET(fd_cli, &fds_active);

						if (fd_cli > fd_max) {
							fd_max = fd_cli;
						}
					}
				// Handle data transfers
				} else {
					state1 = sockets_receive(i, buffer_pck, sizeof(buffer_pck), &size);

					if (state_ok(state1) && size > 0) {
						process_incoming_data(i, buffer_pck, size);
					} else {
						FD_CLR(i, &fds_active);
						close(i);
					}
				}
			}
		}
	}

	return 0;
}
