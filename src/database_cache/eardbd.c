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
static socket_t *sock_main_srv_tcp = &sockets[0];
static socket_t *sock_mirr_srv_tcp = &sockets[1];
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

// Input buffers
static packet_header_t input_header;
//static char input_buffer[MAX_PACKET_SIZE()];
static char input_buffer[SZ_NAME_LARGE];
static char extra_buffer[SZ_NAME_LARGE];

// Mirroring
static char mirror_srv[SZ_NAME_MEDIUM];
static char mirror_cli[SZ_NAME_MEDIUM];
static int  mirror_iam;
static int  isle;

// Synchronization
static packet_header_t sync_ans_header;
static packet_header_t sync_qst_header;
static sync_qst_t sync_qst_content;
static sync_ans_t sync_ans_content;

// Data warehouse
static ulong len_enrgy;
static ulong len_appsx;
static ulong len_evnts;
static ulong len_loops;

static periodic_aggregation_t aggr;
static periodic_metric_t *enrgy;
static     application_t *appsm;
static     application_t *appsn;
static     application_t *appsl;
static       ear_event_t *evnts;
static            loop_t *loops;

// Indexes
static uint i_enrgy;
static uint i_appsm;
static uint i_appsn;
static uint i_appsl;
static uint i_evnts;
static uint i_loops;

/*
 *
 * Synchronization main/mirror
 *
 */

static int sync_question()
{
	state_t s;

	verbose("synchronization started: asking the question");
	
	// Preparing packet
	sockets_header_update(&sync_qst_header);
	
	// Synchronization pipeline
	s = sockets_socket(sock_sync_cli_tcp);

    if (state_fail(s)) {
        verbose("failed to create client socket to MAIN (%d, inum: %d, istr: %s)", s, intern_error_num, intern_error_str);
        return EAR_ERROR;
    }
	
	s = sockets_connect(sock_sync_cli_tcp);

	if (state_fail(s)) {
		verbose("failed to connect to MAIN (%d, inum: %d, istr: %s)", s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_send(sock_sync_cli_tcp, &sync_qst_header, (char *) &sync_qst_content);

	if (state_fail(s)) {
		verbose("failed to send to MAIN (%d, num: %d, str: %s)", s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_receive(sock_sync_cli_tcp->fd, &sync_ans_header, (char *) &sync_ans_content, sizeof(sync_ans_t));

	if (state_fail(s)) {
		verbose("failed to receive from MAIN (%d, num: %d, str: %s)", s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_close(sock_sync_cli_tcp);

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
 *
 * Data storing
 *
 */

static void db_store_events()
{
	if (i_evnts <= 0) {
		return;
	}

	verbose("inserting in DB %d event samples", i_evnts);
	//db_batch_insert_ear_event(evnts[i_evnts]);
	i_evnts = 0;
}

static void db_store_loops()
{
	if (i_loops <= 0) {
		return;
	}

	verbose("inserting in DB %d loop samples", i_loops);
	//db_batch_insert_loops(loops[i_loops]);
	i_loops = 0;
}

static void db_store_periodic_metrics()
{
	if (i_enrgy <= 0) {
		return;
	}

	verbose("inserting in DB %d periodic metric samples", i_enrgy);
	//db_batch_insert_periodic_metrics(enrgy[i_enrgy]);
	i_enrgy = 0;
}

static void db_store_periodic_aggregation()
{
	if (aggr.n_samples <= 0) {
		return;
	}

	verbose("insert in DB an aggregation of %d samples", aggr.n_samples);
	//db_insert_periodic_aggregation(&aggr);
	init_periodic_aggregation(&aggr);
}

static void db_store_applications_learning()
{
	if (i_appsl <= 0) {
		return;
	}

	verbose("inserting in DB %d learning mpi application samples", i_appsl);
	//db_batch_insert_applications(appsl[i_appsl]);
	i_appsl = 0;
}

static void db_store_applications_mpi()
{
	if (i_appsm <= 0) {
		return;
	}

	verbose("inserting in DB %d mpi application samples", i_appsm);
	//db_batch_insert_applications(appsm[i_appsm]);
	i_appsm = 0;
}

static void db_store_applications()
{
	if (i_appsn <= 0) {
		return;
	}

	verbose("inserting in DB %d non-mpi application samples", i_appsn);
	//db_batch_insert_applications_no_mpi(appsn, i_appsn);
	i_appsn = 0;
}

/*
 * Data processing
 */

static void make_periodic_aggregation(periodic_aggregation_t *aggr, periodic_metric_t *met)
{
	add_periodic_aggregation(aggr, met->DC_energy, met->start_time, met->end_time);
}

static void process_clean_all()
{
	i_enrgy = 0;
	i_appsm = 0;
	i_appsn = 0;
	i_appsl = 0;
	i_evnts = 0;
	i_loops = 0;
}

static void process_insert_all()
{
	db_store_periodic_aggregation();
	db_store_periodic_metrics();
	db_store_applications_mpi();
	db_store_applications_learning();
	db_store_applications_mpi();
	db_store_applications();
	db_store_events();
	db_store_loops();
}

static void process_insert()
{
	// Synchronizing with the MAIN
	/*if (mirror_iam)
	{
		if(state_fail(sync_question())) {
			process_insert_all();
		} else {
			process_clean_all();
		}
	}*/
	
	verbose("inserting main data, consumed %lu energy (mJ) from %lu to %lu",
		aggr.DC_energy, aggr.start_time, aggr.end_time);
	
	process_insert_all();

	// Refresh insert time
	timeout_insr.tv_sec = time_insr;
	timeout_insr.tv_usec = 0L;

	// Set when was the last insert time
	time_insr_last = time(NULL);
}

/*
 *
 * Incoming data
 * 
 */

static void incoming_data_process(int fd, packet_header_t *header, char *content)
{
	char *type;
	uint *j;

	if (header->content_type == CONTENT_TYPE_APP)
	{
		application_t *app = (application_t *) content;

		if (app->is_learning)
		{
			j = &i_appsl;

			memcpy (&appsl[*j], content, sizeof(application_t));
			*j += 1;

			if (*j == len_appsx) {
				//db_store_applications_learning(i);
				process_insert();
			}
		} else if (app->is_mpi)
		{
			j = &i_appsm;

			memcpy (&appsm[*j], content, sizeof(application_t));
			*j += 1;

			if (*j == len_appsx) {
				//db_store_applications_mpi(i);
				process_insert();
			}
		} else {
			j = &i_appsn;

			memcpy (&appsn[*j], content, sizeof(application_t));
			*j += 1;

			if (*j == len_appsx) {
				//db_store_applications(i);
				process_insert();
			}
		}
	} else if (header->content_type == CONTENT_TYPE_PER) {
		j = &i_enrgy;

		memcpy (&enrgy[*j], content, sizeof(periodic_metric_t));
		make_periodic_aggregation(&aggr, &enrgy[*j]);
		*j += 1;

		if (*j == len_enrgy) {
			//db_store_periodic_metrics(i);
			process_insert();
		}
	} else if (header->content_type == CONTENT_TYPE_EVE) {
		j = &i_evnts;

		memcpy (&evnts[*j], content, sizeof(ear_event_t));
		*j += 1;

		if (*j == len_evnts) {
			//db_store_events(i);
			process_insert();
		}
	} else if (header->content_type == CONTENT_TYPE_LOO) {
		j = &i_loops;

		memcpy (&loops[*j], content, sizeof(loop_t));
		*j += 1;

		if (*j == len_evnts) {
			//db_store_loops(i);
			process_insert();
		}
	} else if (header->content_type == CONTENT_TYPE_QST)
	{
		double ftime_insr_last = ((double) time(NULL)) - ((double) time_insr_last);
		double ftime_insr_sync = ((double) time_insr) * 0.1;

		if (ftime_insr_last > ftime_insr_sync) {
			process_insert();
		}

		sync_answer(fd);
	}
}

static void incoming_data_announce(int fd, packet_header_t *header, char *content)
{
	char *type;

	if (header->content_type == CONTENT_TYPE_APP)
	{
		application_t *app = (application_t *) content;

		if (app->is_learning) {
			type = "learning application_t";
		} else if (app->is_mpi) {
			type = "mpi application_t";
		} else {
			type = "non-mpi application_t";
		}
	} else if (header->content_type == CONTENT_TYPE_PER) {
		type = "periodic_metric_t";
	} else if (header->content_type == CONTENT_TYPE_EVE) {
		type = "ear_event_t";
	} else if (header->content_type == CONTENT_TYPE_LOO) {
		type = "loop_t";
	} else if (header->content_type == CONTENT_TYPE_QST) {
		type = "sync_question";
	} else {
		type = "unknown";
	}

	verbose("received '%s' packet (m: %d) from host '%s' (socket: %d)",
			type, header->data_extra1, header->host_src, fd);
}

static int incoming_new_connection(int fd)
{
	int nc;

	nc  = !mirror_iam && (fd == sock_main_srv_tcp->fd || fd == sock_sync_srv_tcp->fd);
	nc |=  mirror_iam && (fd == sock_mirr_srv_tcp->fd);

	return nc;
}

/*
 *
 * Main
 *
 */

static void init_configuration(int argc, char **argv, cluster_conf_t *conf_clus)
{
	node_island_t *is;
	int i, j, k;
	char *p;

	// Configuration
	#if 0
	if (get_ear_conf_path(extra_buffer) == EAR_ERROR) {
		error("Error getting ear.conf path");
	}

	verbose("Reading '%s' configuration file", extra_buffer);
	read_cluster_conf(extra_buffer, conf_clus);

	// Database
	init_db_helper(&conf_clus->database);

	// Mirror finding
	gethostname(mirror_cli, SZ_NAME_MEDIUM);

	for (i = 0; i < conf_clus->num_islands && !mirror_iam; ++i)
	{
		is = &conf_clus->islands[i];

		for (k = 0; k < is->num_ranges && !mirror_iam; k++)
		{
        	if (is->ranges[i].sec_ip >= 0)
			{
				p = is->backup_ips[is->ranges[i].sec_ip];

				if (strncmp(p, mirror_cli, strlen(mirror_cli)) == 0)
				{
					strcpy(mirror_srv, is->db_ips[is->ranges[i].db_ip]);
					mirror_iam = 1;
				}
			}
		}
	}
	#else
	conf_clus->db_manager.aggr_time = 10;
	conf_clus->db_manager.tcp_port = 4711;
	conf_clus->db_manager.udp_port = 4712;
	mirror_iam = atoi(argv[1]);
	strcpy(mirror_srv, argv[2]);
	#endif

	// Times
	time_insr = (long) conf_clus->db_manager.aggr_time;

	timeout_insr.tv_sec  = time_insr;
	timeout_insr.tv_usec = 0L;
	timeout_sync.tv_sec  = 30;
	timeout_sync.tv_usec = 0L;
}

static void init_data(int argc, char **argv, cluster_conf_t *conf_clus)
{
	len_enrgy = 32 * 512;
	len_appsx = 32 * 512;
	len_evnts = 32 * 512;
	len_loops = 128 * 512;

	// Raw data
	ulong b_enrgy = sizeof(periodic_metric_t) * len_enrgy;
	ulong b_appsx = sizeof(application_t) * len_appsx;
	ulong b_evnts = sizeof(ear_event_t) * len_evnts;
	ulong b_loops = sizeof(loop_t) * len_loops;

	appsm = malloc(b_appsx);
	appsn = malloc(b_appsx);
	appsl = malloc(b_appsx);
	enrgy = malloc(b_enrgy);
	evnts = malloc(b_evnts);
	loops = malloc(b_loops);

	float mb_appsx = (double) (b_appsx) / 1000000.0;
	float mb_enrgy = (double) (b_enrgy) / 1000000.0;
	float mb_loops = (double) (b_loops) / 1000000.0;
	float mb_evnts = (double) (b_evnts) / 1000000.0;
	float mb_total = (mb_appsx * 3) + mb_enrgy + mb_loops + mb_evnts;

	// Summary
	verbose("reserving %0.2f MBytes for mpi apps (%lu per obj)",
			mb_appsx, sizeof(application_t));
	verbose("reserving %0.2f MBytes for learning apps (%lu per obj)",
			mb_appsx, sizeof(application_t));
	verbose("reserving %0.2f MBytes for non-mpi apps (%lu per obj)",
			mb_appsx, sizeof(application_t));
	verbose("reserving %0.2f MBytes for power metrics (%lu per obj)",
			mb_enrgy, sizeof(periodic_metric_t));
	verbose("reserving %0.2f MBytes for events (%lu per obj)",
			mb_evnts, sizeof(ear_event_t));
	verbose("reserving %0.2f MBytes for loops (%lu per obj)",
			mb_loops, sizeof(loop_t));
	verbose("total memory allocated: %0.2f MBytes", mb_total);

	// Synchronization headers
	sockets_header_clean(&sync_ans_header);
	sockets_header_clean(&sync_qst_header);

	sync_ans_header.content_type = CONTENT_TYPE_ANS;
	sync_ans_header.content_size = sizeof(sync_ans_t);
	sync_qst_header.content_type = CONTENT_TYPE_QST;
	sync_qst_header.content_size = sizeof(sync_qst_t);
}

static void init_fork(int argc, char **argv, cluster_conf_t *conf_clus)
{
	pid_t pid;

	if (mirror_iam) {
		return;
	}

	pid = fork();

	if (pid == 0) {
		mirror_iam = 1;
	} else if (pid > 0) {
		mirror_iam = 0;
	} else {
		error("error while forking, terminating the program");
	}
}

static void init_sockets_mirror(int argc, char **argv, cluster_conf_t *conf_clus)
{
	state_t s1;
	state_t s2;

	// Opening server socket
	sockets_init(sock_mirr_srv_tcp,       NULL, 4712, TCP);
	sockets_init(sock_sync_cli_tcp, mirror_srv, 4714, TCP);

	s1 = sockets_socket(sock_mirr_srv_tcp);

	if (state_fail(s1) || state_fail(s2)) {
		error("while creating sockets (%s)", intern_error_str);
	}

	// Binding socket
	s1 = sockets_bind(sock_mirr_srv_tcp);

	if (state_fail(s1) || state_fail(s2)) {
		error("while binding sockets (%s)", intern_error_str);
	}

	// Listening socket
	s1 = sockets_listen(sock_mirr_srv_tcp);

	if (state_fail(s1)) {
		error("while listening sockets (%s)", intern_error_str);
	}

	// Add the listener to the ready set
	FD_SET(sock_mirr_srv_tcp->fd, &fds_active);

	//
	// Keep track of the biggest file descriptor
	fd_max = sock_mirr_srv_tcp->fd;

	// Summary
	verbose ("opened mirror socket %d for TCP packets on port %u",
			 sock_mirr_srv_tcp->fd, sock_mirr_srv_tcp->port);
}

static void init_sockets_main(int argc, char **argv, cluster_conf_t *conf_clus)
{
	state_t s1;
	state_t s2;

	// Opening server socket
	sockets_init(sock_main_srv_tcp, NULL, 4711, TCP);
	sockets_init(sock_sync_srv_tcp, NULL, 4713, TCP);

	s1 = sockets_socket(sock_main_srv_tcp);
	s2 = sockets_socket(sock_sync_srv_tcp);

	if (state_fail(s1) || state_fail(s2)) {
		error("while creating sockets (%s)", intern_error_str);
	}

	// Binding socket
	s1 = sockets_bind(sock_main_srv_tcp);
	s2 = sockets_bind(sock_sync_srv_tcp);

	if (state_fail(s1) || state_fail(s2)) {
		error("while binding sockets (%s)", intern_error_str);
	}

	// Listening socket
	s1 = sockets_listen(sock_main_srv_tcp);
	s2 = sockets_listen(sock_sync_srv_tcp);

	if (state_fail(s1) || state_fail(s2)) {
		error("while listening sockets (%s)", intern_error_str);
	}

	// Add the listener to the ready set
	FD_SET(sock_main_srv_tcp->fd, &fds_active);
	FD_SET(sock_sync_srv_tcp->fd, &fds_active);

	// Keep track of the biggest file descriptor
	fd_max = sock_sync_srv_tcp->fd;

	if (sock_main_srv_tcp->fd > fd_max) {
		fd_max = sock_main_srv_tcp->fd;
	}

	// Summary
	verbose ("opened metrics socket %d for TCP packets on port %u",
			 sock_main_srv_tcp->fd, sock_main_srv_tcp->port);
	verbose ("opened sync socket %d for TCP packets on port %u",
			 sock_sync_srv_tcp->fd, sock_sync_srv_tcp->port);
}

static void init_sockets(int argc, char **argv, cluster_conf_t *conf_clus)
{
	// Cleaning sockets
	sockets_clean(sock_main_srv_tcp);
	sockets_clean(sock_mirr_srv_tcp);
	sockets_clean(sock_sync_srv_tcp);
	sockets_clean(sock_main_srv_tcp);

	// Cleaning socket sets
	FD_ZERO(&fds_incoming);
	FD_ZERO(&fds_active);

	if (mirror_iam) {
		init_sockets_mirror(argc, argv, conf_clus);
	} else {
		init_sockets_main(argc, argv, conf_clus);
	}
}

void usage(int argc, char **argv) {}

int main(int argc, char **argv)
{
	cluster_conf_t conf_clus;
	state_t s1;
	state_t s2;
	state_t s3;
	int i;

	//
	usage(argc, argv);

	//
	verbose("phase 1: configuration initiallization");
	init_configuration(argc, argv, &conf_clus);

	//
	verbose("phase 2: processes fork");
	init_fork(argc, argv, &conf_clus);

	//
	verbose("phase 3: data allocation");
	init_data(argc, argv, &conf_clus);

	//
	verbose("phase 4: sockets initialization");
	init_sockets(argc, argv, &conf_clus);

	//
	verbose("phase 5: listening (processing every %lu s)", time_insr);

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
				if (incoming_new_connection(i))
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

					if (state_ok(s1))
					{
						incoming_data_announce(i, &input_header, input_buffer);
						incoming_data_process(i, &input_header, input_buffer);
					}
					else
					{
						if (state_is(s1, EAR_SOCK_DISCONNECTED)) {
							verbose("disconnected from socket %d (num: %d, str: %s)",
									i, intern_error_num, intern_error_str);
						} else {
							verbose("on reception (num: %d, str: %s), disconnecting from socket %d",
									intern_error_num, intern_error_str, i);
						}

						sockets_close_fd(i);
						FD_CLR(i, &fds_active);
					}
				}
			}
		}
	}

	return 0;
}
