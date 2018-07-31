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

int EAR_VERBOSE_LEVEL = 1;

// Sockets
static socket_t sockets[4];
static socket_t *sock_metr_srv_tcp = &sockets[0];
static socket_t *sock_metr_srv_udp = &sockets[1];
static socket_t *sock_sync_srv_tcp = &sockets[2];
static socket_t *sock_sync_cli_tcp = &sockets[3];

// Descriptors
fd_set fds_metr_tcp;
fd_set fds_sync_tcp;
fd_set fds_incoming;
fd_set fds_incoming;
fd_set fds_active;

int fd_max;
int fd_cli;

// Times
static struct timeval timeout_insr;
static struct timeval timeout_sync;
static time_t time_insr_last;
static ulong  time_insr;

/*
static time_t  tinsr_glbal[2];
static time_t *tmetr_enrgy[2];
static time_t *tmetr_appsm[2];
static time_t *tmetr_appsn[2];
static time_t *tmetr_appsl[2];
static time_t *tmetr_evnts[2];
static time_t *tmetr_loops[2];
*/

// Input buffers
static packet_header_t input_header;
//static char input_buffer[MAX_PACKET_SIZE()];
static char input_buffer[PATH_MAX];
static char extra_buffer[PATH_MAX];

// Mirror
static packet_header_t sync_ans_header;
static packet_header_t sync_qst_header;
static sync_qst_t sync_qst_content;
static sync_ans_t sync_ans_content;
static int im_mirror;

// Data warehouse
static periodic_aggregation_t aggr[2];
static periodic_metric_t *enrgy[2];
static     application_t *appsm[2];
static     application_t *appsn[2];
static     application_t *appsl[2];
static       ear_event_t *evnts[2];
static            loop_t *loops[2];

static ulong len_enrgy;
static ulong len_appsx;
static ulong len_evnts;
static ulong len_loops;

// Indexes
#define i_main 0
#define i_mirr 1

static uint i_enrgy[2];
static uint i_appsm[2];
static uint i_appsn[2];
static uint i_appsl[2];
static uint i_evnts[2];
static uint i_loops[2];

/*
 * Sync
 */
static int sync_question()
{
	state_t s;

	verbose("synchronization started: asking the question");
	
	// Preparing packet
	sockets_header_update(&sync_qst_header);
	
	// Synchronization pipeline
	s = sockets_connect(sock_sync_cli_tcp);

	if (state_fail(s)) {
		verbose("failed to connect to MAIN (%d, inum: %d, istr: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_send(sock_sync_cli_tcp, &sync_qst_header, (char *) &sync_qst_content);

	if (state_fail(s)) {
		verbose("failed to send to MAIN (%d, num: %d, str: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_receive(sock_sync_cli_tcp->fd, &sync_ans_header, (char *) &sync_ans_content, sizeof(sync_ans_t));

	if (state_fail(s)) {
		verbose("failed to receive from MAIN (%d, num: %d, str: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_disconnect(sock_sync_cli_tcp->fd, NULL);

	verbose("synchronization completed correctly");

	return EAR_SUCCESS;
}

static int sync_answer(int fd)
{
	socket_t sync_ans_socket;
	state_t s;

	verbose("synchronization started: answering the question");
	
	// Socket
	sockets_clean(&sync_ans_socket);
	sync_ans_socket.protocol = TCP;
	sync_ans_socket.fd = fd;

	// Header
	sockets_header_update(&sync_ans_header);

	s = sockets_send(&sync_ans_socket, &sync_ans_header, (char *) &sync_ans_content);

	if (state_fail(s)) {
		verbose("Failed to send to MIRROR (%d, num: %d, str: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	verbose("synchronization completed correctly");

	return EAR_SUCCESS;
}

/*
 * Data storing/loading
 */

static void db_store_events(uint mirror)
{
	uint  i = mirror;
	uint *j = &i_evnts[mirror];

	if (*j <= 0) {
		return;
	}

	verbose("trying to insert in DB %d event samples", *j);
	//db_batch_insert_ear_event(evnts[i], *j);
	*j = 0;
}

static void db_store_loops(uint mirror)
{
	uint  i = mirror;
	uint *j = &i_loops[mirror];

	if (*j <= 0) {
		return;
	}

	verbose("trying to insert in DB %d loop samples", *j);
	//db_batch_insert_loops(loops[i], *j);
	*j = 0;
}

static void db_store_periodic_metrics(uint mirror)
{
	uint  i = mirror;
	uint *j = &i_enrgy[mirror];

	if (*j <= 0) {
		return;
	}

	verbose("trying to insert in DB %d periodic metric samples", *j);
	//db_batch_insert_periodic_metrics(enrgy[i], *j);
	*j = 0;
}

static void db_store_periodic_aggregation(uint mirror)
{
	int i = mirror;

	if (aggr[i].n_samples <= 0) {
		return;
	}

	verbose("trying to insert in DB an aggregation of %d samples", aggr[i].n_samples);
	//db_insert_periodic_aggregation(&aggr[i]);
	init_periodic_aggregation(&aggr[i]);
}

static void db_store_applications_learning(uint mirror)
{
	uint  i = mirror;
	uint *j = &i_appsl[mirror];

	if (*j <= 0) {
		return;
	}

	verbose("trying to insert in DB %d mpi application samples", *j);
	//db_batch_insert_applications(appsl[i], *j);
	*j = 0;
}

static void db_store_applications_mpi(uint mirror)
{
	uint  i = mirror;
	uint *j = &i_appsm[mirror];

	if (*j <= 0) {
		return;
	}

	verbose("trying to insert in DB %d mpi application samples", *j);
	//db_batch_insert_applications(appsm[i], *j);
	*j = 0;
}

static void db_store_applications(uint mirror)
{
	uint  i = mirror;
	uint *j = &i_appsn[mirror];

	if (*j <= 0) {
		return;
	}

	verbose("trying to insert in DB %d non-mpi application samples", *j);
	//db_batch_insert_applications_no_mpi(appsn[i], *j);
	*j = 0;
}

/*
 * Data processing
 */

static void make_periodic_aggregation(periodic_aggregation_t *aggr, periodic_metric_t *met)
{
	add_periodic_aggregation(aggr, met->DC_energy, met->start_time, met->end_time);
}

static void process_clean_mirror(int mirror)
{
	i_enrgy[mirror] = 0;
	i_appsm[mirror] = 0;
	i_appsn[mirror] = 0;
	i_appsl[mirror] = 0;
	i_evnts[mirror] = 0;
	i_loops[mirror] = 0;
}

static void process_insert_mirror(int mirror)
{
	db_store_periodic_aggregation(mirror);
	db_store_periodic_metrics(mirror);
	db_store_applications_mpi(mirror);
	db_store_applications_learning(mirror);
	db_store_applications_mpi(mirror);
	db_store_applications(mirror);
	db_store_events(mirror);
	db_store_loops(mirror);
}

static void process_insert()
{
	verbose("inserting, consumed %lu energy (mJ) from %lu to %lu",
			aggr[i_main].DC_energy, aggr[i_main].start_time, aggr[i_main].end_time);

	process_insert_mirror(i_main);

	if (im_mirror)
	{
		if(state_fail(sync_question())) {
			process_insert_mirror(i_mirr);
		} else {
			process_clean_mirror(i_mirr);
		}
	}

	// Refresh insert time
	timeout_insr.tv_sec = time_insr;
	timeout_insr.tv_usec = 0L;

	// Set when was the last insert time
	time_insr_last = time(NULL);
}

static void process_incoming_data(int fd, packet_header_t *header, char *content)
{
	uint i = header->data_extra1 != 0;
	char *type;
	uint *j;

	if (header->content_type == CONTENT_TYPE_APP)
	{
		application_t *app = (application_t *) content;

		if (app->is_learning)
		{
			type = "learning application_t";
			j = &i_appsl[i];

			memcpy (&appsl[i][*j], content, sizeof(application_t));
			*j += 1;

			if (*j == len_appsx) {
				//db_store_applications_learning(i);
				process_insert();
			}
		} else if (app->is_mpi)
		{
			type = "mpi application_t";
			j = &i_appsm[i];

			memcpy (&appsm[i][*j], content, sizeof(application_t));
			*j += 1;

			if (*j == len_appsx) {
				//db_store_applications_mpi(i);
				process_insert();
			}
		} else
		{
			type = "non-mpi application_t";
			j = &i_appsn[i];

			memcpy (&appsn[i][*j], content, sizeof(application_t));
			*j += 1;

			if (*j == len_appsx && i == i_mirr) {
				process_insert();
			} else {
				db_store_applications(i);
			}
		}
	} else if (header->content_type == CONTENT_TYPE_PER) {
		type = "periodic_metric_t";
		j = &i_enrgy[i];

		memcpy (&enrgy[i][*j], content, sizeof(periodic_metric_t));
		make_periodic_aggregation(&aggr[i], &enrgy[i][*j]);
		*j += 1;

		if (*j == len_enrgy) {
			//db_store_periodic_metrics(i);
			process_insert();
		}
	} else if (header->content_type == CONTENT_TYPE_EVE) {
		type = "ear_event_t";
		j = &i_evnts[i];

		memcpy (&evnts[i][*j], content, sizeof(ear_event_t));
		*j += 1;

		if (*j == len_evnts && i == i_mirr) {
			//db_store_events(i);
			process_insert();
		}
	} else if (header->content_type == CONTENT_TYPE_LOO) {
		type = "loop_t";
		j = &i_loops[i];

		memcpy (&loops[i][*j], content, sizeof(loop_t));
		*j += 1;

		if (*j == len_evnts) {
			//db_store_loops(i);
			process_insert();
		}
	} else if (header->content_type == CONTENT_TYPE_QST)
	{
		type = "sync_question";

		double ftime_insr_last = (double) time(NULL) - (double) time_insr_last;
		double ftime_insr_sync = time_insr * 0.1;

		verbose("Received question from host: %s", header->host_src);
		verbose("Time passed since last insert: %0.2lf", ftime_insr_last);
		verbose("Threshold of synchronization: %0.2lf", ftime_insr_sync);

		if (time_insr_last > ftime_insr_sync) {
			process_insert();
		}

		sync_answer(fd);
	} else {
		type = "unknown";
	}

	verbose("received a '%s' packet (mirr: %d), from the host %s (socket: %d)", type, i, header->host_src, fd);
}

/*
 * Server managing
 */

/*
 * Main
 */

void data_init(int argc, char **argv, cluster_conf_t *conf_clus)
{
	len_enrgy = 32 * 512;
	len_appsx = 32 * 512;
	len_evnts = 32 * 512;
	len_loops = 128 * 512;

	// Raw data
	ulong b_enrgy = sizeof(periodic_metric_t) * len_enrgy * 2;
	ulong b_appsx = sizeof(application_t) * len_appsx * 2;
	ulong b_evnts = sizeof(ear_event_t) * len_evnts * 2;
	ulong b_loops = sizeof(loop_t) * len_loops * 2;

	appsm[i_main] = malloc(b_appsx);
	appsn[i_main] = malloc(b_appsx);
	appsl[i_main] = malloc(b_appsx);
	enrgy[i_main] = malloc(b_enrgy);
	evnts[i_main] = malloc(b_evnts);
	loops[i_main] = malloc(b_loops);

	enrgy[i_mirr] = &enrgy[i_main][len_enrgy];
	appsm[i_mirr] = &appsm[i_main][len_appsx];
	appsn[i_mirr] = &appsn[i_main][len_appsx];
	appsl[i_mirr] = &appsl[i_main][len_appsx];
	evnts[i_mirr] = &evnts[i_main][len_evnts];
	loops[i_mirr] = &loops[i_main][len_loops];

	float mb_appsx = (double) (b_appsx) / 1000000.0;
	float mb_enrgy = (double) (b_enrgy) / 1000000.0;
	float mb_loops = (double) (b_loops) / 1000000.0;
	float mb_evnts = (double) (b_evnts) / 1000000.0;
	float mb_total = (mb_appsx * 3) + mb_enrgy + mb_loops + mb_evnts;

	verbose("reserving %0.2f MBytes for mpi apps (%lu per app)", mb_appsx, sizeof(application_t));
	verbose("reserving %0.2f MBytes for learning apps (%lu per app)", mb_appsx, sizeof(application_t));
	verbose("reserving %0.2f MBytes for non-mpi apps (%lu per app)", mb_appsx, sizeof(application_t));
	verbose("reserving %0.2f MBytes for power metrics (%lu per metric)", mb_enrgy, sizeof(periodic_metric_t));
	verbose("reserving %0.2f MBytes for events (%lu per event)", mb_evnts, sizeof(ear_event_t));
	verbose("reserving %0.2f MBytes for loops (%lu per loop)", mb_loops, sizeof(loop_t));
	verbose("total memory allocated: %0.2f MBytes", mb_total);

	// Times
	/*
	ulong b_enrgy = sizeof(time_t) * len_enrgy * 2;
	ulong b_appsx = sizeof(time_t) * len_appsx * 2;
	ulong b_evnts = sizeof(time_t) * len_evnts * 2;
	ulong b_loops = sizeof(time_t) * len_loops * 2;

	tmetr_appsm[i_main] = malloc(b_appsx);
	tmetr_appsn[i_main] = malloc(b_appsx);
	tmetr_appsl[i_main] = malloc(b_appsx);
	tmetr_enrgy[i_main] = malloc(b_enrgy);
	tmetr_evnts[i_main] = malloc(b_evnts);
	tmetr_loops[i_main] = malloc(b_loops);

	tmetr_enrgy[i_mirr] = &enrgy[i_main][len_enrgy];
	tmetr_appsm[i_mirr] = &appsm[i_main][len_appsx];
	tmetr_appsn[i_mirr] = &appsn[i_main][len_appsx];
	tmetr_appsl[i_mirr] = &appsl[i_main][len_appsx];
	tmetr_evnts[i_mirr] = &evnts[i_main][len_evnts];
	tmetr_loops[i_mirr] = &loops[i_main][len_loops];
	*/

	#if 0
	// Configuration file (TODO)
	if (get_ear_conf_path(extra_buffer) == EAR_ERROR) {
		error("Error getting ear.conf path");
	}

	verbose("Reading '%s' configuration file", extra_buffer);
	read_cluster_conf(extra_buffer, &conf_clus);

	// Database
	init_db_helper(&conf_clus.database);
	#else
	conf_clus->db_manager.aggr_time = 10;
	conf_clus->db_manager.tcp_port = 4711;
	conf_clus->db_manager.udp_port = 4712;
	im_mirror = atoi(argv[1]);
	#endif

	// Times
	time_insr = (long) conf_clus->db_manager.aggr_time;

	timeout_insr.tv_sec  = time_insr;
	timeout_insr.tv_usec = 0L;
	timeout_sync.tv_sec  = 30;
	timeout_sync.tv_usec = 0L;

	// Synchronization
	sockets_header_clean(&sync_ans_header);
	sockets_header_clean(&sync_qst_header);

	sync_ans_header.content_type = CONTENT_TYPE_ANS;
	sync_ans_header.content_size = sizeof(sync_ans_t);
	sync_qst_header.content_type = CONTENT_TYPE_QST;
	sync_qst_header.content_size = sizeof(sync_qst_t);

	sockets_set_timeout(sock_sync_srv_tcp->fd, &timeout_sync);
}

void usage(int argc, char **argv)
{
}

int main(int argc, char **argv)
{
	cluster_conf_t conf_clus;
	state_t s1;
	state_t s2;
	state_t s3;
	state_t s4;
	int i;

	//
	usage(argc, argv);

	//
	verbose("phase 1: data initiallization space");

	data_init(argc, argv, &conf_clus);

	//
	verbose("phase 2: sockets");

	FD_ZERO(&fds_incoming);
	FD_ZERO(&fds_active);

	// Opening socket
	sockets_init(sock_metr_srv_tcp, NULL, conf_clus.db_manager.tcp_port, TCP);
	sockets_init(sock_metr_srv_udp, NULL, conf_clus.db_manager.udp_port, UDP);
	sockets_init(sock_sync_srv_tcp,    NULL, 4713, TCP);
	sockets_init(sock_sync_cli_tcp, argv[2], 4713, TCP);

	s1 = sockets_socket(sock_metr_srv_tcp);
	s2 = sockets_socket(sock_metr_srv_udp);
	s3 = sockets_socket(sock_sync_srv_tcp);
	s4 = sockets_socket(sock_sync_cli_tcp);

	if (state_fail(s1) || state_fail(s2) || state_fail(s3) || state_fail(s4)) {
		error("while creating sockets (%s)", intern_error_str);
	}

	verbose ("opened metrics socket %d for TCP packets on port %u", sock_metr_srv_tcp->fd, sock_metr_srv_tcp->port);
	verbose ("opened metrics socket %d for UDP packets on port %u", sock_metr_srv_udp->fd, sock_metr_srv_udp->port);
	verbose ("opened sync socket %d for TCP packets on port %u", sock_sync_srv_tcp->fd, sock_sync_srv_tcp->port);

	// Binding socket
	s1 = sockets_bind(sock_metr_srv_tcp);
	s2 = sockets_bind(sock_metr_srv_udp);
	s3 = sockets_bind(sock_sync_srv_tcp);

	if (state_fail(s1) || state_fail(s2) || state_fail(s3)) {
		error("while binding sockets (%s)", intern_error_str);
	}

	// Listening socket
	s1 = sockets_listen(sock_metr_srv_tcp);
	s3 = sockets_listen(sock_sync_srv_tcp);

	if (state_fail(s1) || state_fail(s3)) {
		error("while listening sockets (%s)", intern_error_str);
	}

	// Add the listener to the ready set
	FD_SET(sock_metr_srv_tcp->fd, &fds_active);
	FD_SET(sock_metr_srv_udp->fd, &fds_active);
	FD_SET(sock_sync_srv_tcp->fd, &fds_active);

	// Keep track of the biggest file descriptor
	fd_max = sock_sync_srv_tcp->fd;

	if (sock_metr_srv_tcp->fd > fd_max) {
		fd_max = sock_metr_srv_tcp->fd;
	}
	if (sock_metr_srv_udp->fd > fd_max) {
		fd_max = sock_metr_srv_udp->fd;
	}

	//
	verbose("phase 3: listening (processing every %lu s)", time_insr);

	while(1)
	{
		fds_incoming = fds_active;

		if (select(fd_max + 1, &fds_incoming, NULL, NULL, &timeout_insr) == -1) {
			error("select");
		}

		// If timeout_insr, data processing
		if (timeout_insr.tv_sec == 0 && timeout_insr.tv_usec == 0) {
			// Inserts data and updates timeouts
			process_insert();
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fd_max; i++)
		{
			if (FD_ISSET(i, &fds_incoming)) // we got one!!
			{
				// Handle new connections (just for TCP)
				if (i == sock_metr_srv_tcp->fd || i == sock_sync_srv_tcp->fd)
				{
					s1 = sockets_accept(i, &fd_cli);

					if (state_ok(s1))
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
					s1 = sockets_receive(i, &input_header, input_buffer, sizeof(input_buffer));

					if (state_ok(s1)) {
						process_incoming_data(i, &input_header, input_buffer);
					} else {
						if (state_is(s1, EAR_SOCK_DISCONNECTED)) {
							verbose("disconnected from socket %d (num: %d, str: %s)",
									i, intern_error_num, intern_error_str);
						} else {
							verbose("on reception (num: %d, str: %s), disconnecting from socket %d",
									intern_error_num, intern_error_str, i);
						}

						sockets_disconnect(i, &fds_active);
					}
				}
			}
		}
	}

	return 0;
}
