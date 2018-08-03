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
#include <database_cache/eardbd.h>

static char buffer_pck[MAX_PACKET_SIZE()];
static char buffer_gen[PATH_MAX];

#define lops_len 128 * 512
#define mets_len 32 * 512
#define eves_len 32 * 512
#define apps_len 32 * 512

static periodic_aggregation_t aggr;
static application_t apps_mpi[apps_len];
static application_t apps_nor[apps_len];
static application_t apps_ler[apps_len];
static periodic_metric_t mets[mets_len];
static ear_event_t eves[eves_len];
static loop_t lops[lops_len];
static uint apps_mpi_i;
static uint apps_nor_i;
static uint apps_ler_i;
static uint mets_i;
static uint eves_i;
static uint lops_i;

int EAR_VERBOSE_LEVEL=1;

/*
 * Signal processing
 */

/*
 * Data storing/loading
 */

static void db_store_events(ear_event_t *eves, uint n_eves)
{
	if (n_eves <= 0) {
		return;
	}

	verbose("Trying to insert in DB %d event samples", n_eves);
	db_batch_insert_ear_event(eves, n_eves);
}

static void db_store_loops(loop_t *lops, uint n_lops)
{
	if (n_lops <= 0) {
		return;
	}

	verbose("Trying to insert in DB %d loop samples", n_lops);
	db_batch_insert_loops(lops, n_lops);
	//db_insert_loop(lops);
}

static void db_store_periodic_metrics(periodic_metric_t *mets, uint n_mets)
{
	if (n_mets <= 0) {
		return;
	}

	verbose("Trying to insert in DB %d periodic metric samples", n_mets);
	db_batch_insert_periodic_metrics(mets, n_mets);
}

static void db_store_periodic_aggregation()
{
	if (aggr.n_samples <= 0) {
		return;
	}

	verbose("Trying to insert in DB an aggregation of %d samples", aggr.n_samples);
	db_insert_periodic_aggregation(&aggr);
}

static void db_store_applications_mpi(application_t *apps, uint n_apps)
{
	if (n_apps == 0) {
		return;
	}

	verbose("Trying to insert in DB %d mpi application samples", n_apps);
	db_batch_insert_applications(apps, n_apps);
	//db_insert_application(apps);
}

static void db_store_applications(application_t *apps, uint n_apps)
{
	if (n_apps == 0) {
		return;
	}

	verbose("Trying to insert in DB %d non-mpi application samples", n_apps);
	db_batch_insert_applications_no_mpi(apps, n_apps);
	//db_insert_application(apps);
}

/*
 * Data processing
 */

static size_t extract_packet_size(char *buffer)
{
	packet_header_t *header;
	header = P_HEADER(buffer);

	return header->packet_size;
}

static void make_periodic_aggregation(periodic_metric_t *met)
{
	add_periodic_aggregation(&aggr, met->DC_energy, met->start_time, met->end_time);
}

static void process_timeout_data()
{
	verbose("Finished aggregation, consumed %lu energy (mJ) from %lu to %lu,",
			aggr.DC_energy, aggr.start_time, aggr.end_time);

	db_store_periodic_aggregation();
	init_periodic_aggregation(&aggr);

	db_store_periodic_metrics(mets, mets_i);
	mets_i = 0;

	db_store_applications_mpi(apps_ler, apps_ler_i);
	apps_ler_i = 0;

	db_store_applications_mpi(apps_mpi, apps_mpi_i);
	apps_mpi_i = 0;

	db_store_applications(apps_nor, apps_nor_i);
	apps_nor_i = 0;

	db_store_events(eves, eves_i);
	eves_i = 0;

	db_store_loops(lops, lops_i);
	lops_i = 0;
}

static void process_incoming_data(int fd, char *buffer, ssize_t size)
{
	packet_header_t *header;
	void *content;
	char *type;

	header = P_HEADER(buffer);
	content = P_CONTENT(buffer);

	if (header->content_type == CONTENT_TYPE_APP)
	{
		application_t *app = (application_t *) content;
		verbose("received an application %d from host %s", app->job.id, app->node_id);
		//report_application_data(app);

		if (app->is_learning)
		{
			type = "learning application_t";
			
			memcpy (&apps_ler[apps_ler_i], content, sizeof(application_t));
			apps_ler_i += 1;

			if (apps_ler_i == apps_len)
			{
				db_store_applications_mpi(apps_ler, apps_ler_i);
				apps_ler_i = 0;
			}
		} else if (app->is_mpi)
		{
			type = "mpi application_t";
			
			memcpy (&apps_mpi[apps_mpi_i], content, sizeof(application_t));
			apps_mpi_i += 1;

			if (apps_mpi_i == apps_len)
			{
				db_store_applications_mpi(apps_mpi, apps_mpi_i);
				apps_mpi_i = 0;
			}
		} else
		{
			type = "non-mpi application_t";
			
			memcpy (&apps_nor[apps_nor_i], content, sizeof(application_t));
			apps_nor_i += 1;

			if (apps_nor_i == apps_len)
			{
				db_store_applications(apps_nor, apps_nor_i);
				apps_nor_i = 0;
			}
		}
	}
	else if (header->content_type == CONTENT_TYPE_PER) {
		type = "periodic_metric_t";
		
		memcpy (&mets[mets_i], content, sizeof(periodic_metric_t));
		make_periodic_aggregation(&mets[mets_i]);
		mets_i += 1;

		if (mets_i == mets_len) {
			db_store_periodic_metrics(mets, mets_i);
			mets_i = 0;
		}
	} else if (header->content_type == CONTENT_TYPE_EVE) {
		type = "ear_event_t";
		
		memcpy (&eves[eves_i], content, sizeof(ear_event_t));
		eves_i += 1;

		if (eves_i == eves_len) {
			db_store_events(eves, eves_i);
			eves_i = 0;
		}
	} else if (header->content_type == CONTENT_TYPE_LOO) {
		type = "loop_t";

		memcpy (&lops[lops_i], content, sizeof(loop_t));

		verbose("%d %d %d '%s' %lu ", lops[lops_i].id.event, lops[lops_i].id.size, lops[lops_i].id.level,
			lops[lops_i].node_id, lops[lops_i].total_iterations);
		print_signature_fd(STDERR_FILENO, &lops[lops_i].signature, 1);
		lops_i += 1;

		if (lops_i == eves_len) {
			verbose("Inserting %u loop instances\n",lops_i);
			db_store_loops(lops, lops_i);
			lops_i = 0;
		}else{
			verbose("total loop_i=%u\n",lops_i);
		}
	} else {
		type = "unknown";
	}

	verbose("received a '%s' packet (%ld bytes), from the socket %d", type, size, fd);
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
	size_t accum_size;
	size_t packt_size;
	ssize_t recvd_size;
	int i;

	socket_t sockets[3];
	socket_t *sock_metr_tcp = &sockets[0];
	socket_t *sock_metr_udp = &sockets[1];
	socket_t *sock_sync_tcp = &sockets[2];
	fd_set fds_incoming;
	fd_set fds_active;
	int fd_max;
	int fd_cli;

	long merge_time;
	float mb_totl;
	float mb_apps;
	float mb_lops;
	float mb_eves;
	float mb_mets;

	state_t state1;
	state_t state2;
	state_t state3;

	//
	usage(argc, argv);

	//
	verbose("phase 1: starting");

	mb_apps = (double) (sizeof(application_t)     * apps_len) / 1000000.0;
	mb_mets = (double) (sizeof(periodic_metric_t) * mets_len) / 1000000.0;
	mb_lops = (double) (sizeof(loop_t)            * lops_len) / 1000000.0;
	mb_eves = (double) (sizeof(ear_event_t)       * mets_len) / 1000000.0;
	mb_totl = (mb_apps * 3) + mb_mets + mb_lops + mb_eves;

	verbose("reserving %0.2f MBytes for mpi applications (%lu per application)", mb_apps, sizeof(application_t));
	verbose("reserving %0.2f MBytes for learning applications (%lu per application)", mb_apps, sizeof(application_t));
	verbose("reserving %0.2f MBytes for non-mpi applications (%lu per application)", mb_apps, sizeof(application_t));
	verbose("reserving %0.2f MBytes for power metrics (%lu per power metric)", mb_mets, sizeof(periodic_metric_t));
	verbose("reserving %0.2f MBytes for loops (%lu per loop)", mb_lops, sizeof(loop_t));
	verbose("reserving %0.2f MBytes for events  (%lu per ear_event_t)", mb_eves, sizeof(ear_event_t));
	verbose("total memory allocated: %0.2f MBytes", mb_totl);

	#if 1
	// Configuration file (TODO)
	if (get_ear_conf_path(buffer_gen) == EAR_ERROR) {
		error("Error getting ear.conf path");
	}

	verbose("Reading '%s' configuration file", buffer_gen);
	read_cluster_conf(buffer_gen, &conf_clus);

	// Database
	init_db_helper(&conf_clus.database);
	#else
	conf_clus.db_manager.aggr_time = 60;
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
					state1 = sockets_accept(i, &fd_cli);

					if (state_ok(state1))
					{
						verbose("accepted connection from socket %d", fd_cli);

						FD_SET(fd_cli, &fds_active);

						if (fd_cli > fd_max) {
							fd_max = fd_cli;
						}
					}
				// Handle data transfers
				}
				else
				{
					state1 = sockets_receive(i, buffer_pck, sizeof(buffer_pck), &recvd_size);
					//verbose("1 recvd_size %lu", recvd_size);

					if (state_ok(state1) && recvd_size > 0)
					{
						packt_size = extract_packet_size(buffer_pck);
						accum_size = recvd_size;

						//verbose("2 packt_size %lu", packt_size);
						//verbose("3 accum_size %lu", accum_size);
						
						while (state_ok(state1) && recvd_size > 0 && accum_size < packt_size) {
							state1 = sockets_receive(i, &buffer_pck[accum_size], sizeof(buffer_pck), &recvd_size);
							accum_size += recvd_size;
							//verbose("4 recvd_size %lu", recvd_size);
							//verbose("5 accum_size %lu", accum_size);
						}
					}

					if (state_ok(state1) && recvd_size > 0 && accum_size == packt_size){
						process_incoming_data(i, buffer_pck, packt_size);
					} else {
						if (recvd_size == 0) {
							verbose("disconnected from socket %d", i);
						} else {
							error("on reception (%s), disconnecting from socket %d", strerror(errno), i);
						}						

						FD_CLR(i, &fds_active);
						close(i);
					}
				}
			}
		}
	}

	return 0;
}
