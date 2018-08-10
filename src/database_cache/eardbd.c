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

#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <database_cache/eardbd.h>

int EAR_VERBOSE_LEVEL = 1;

// Configuration
static cluster_conf_t conf_clus;

// Extras
static pid_t server_pid;
static pid_t mirror_pid;
static float alloc;

// Sockets
static socket_t sockets[4];
static socket_t *smets_srv = &sockets[0];
static socket_t *smets_mir = &sockets[1];
static socket_t *ssync_srv = &sockets[2];
static socket_t *ssync_mir = &sockets[3];

// Descriptors
static fd_set fds_metr_tcp;
static fd_set fds_sync_tcp;
static fd_set fds_incoming;
static fd_set fds_incoming;
static fd_set fds_active;

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
static char server_host[SZ_NAME_MEDIUM]; // server host if mirror
static char mirror_host[SZ_NAME_MEDIUM]; // mirror host if server
static int  mirror_iam;
static int  mirror_too;
static int  server_iam;
static int  server_too;
static int  master_iam;

// Pipeline
static int reconfiguring;
static int listening;
static int releasing;
static int exitting;
static int waiting;
static int forked;
static int isle;

// Synchronization
static packet_header_t sync_ans_header;
static packet_header_t sync_qst_header;
static sync_qst_t sync_qst_content;
static sync_ans_t sync_ans_content;

// Data warehouse
static ulong len_appsl;
static ulong len_appsm;
static ulong len_appsn;
static ulong len_enrgy;
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
static ulong i_enrgy;
static ulong i_appsm;
static ulong i_appsn;
static ulong i_appsl;
static ulong i_evnts;
static ulong i_loops;

// Strings
static char *str_who[2] = { "server", "mirror" };
static char *str_soc[2] = { "listen", "closed" };

#define line "---------------------------------------------------------------\n"
#define col1 "\x1b[35m"
#define col2 "\x1b[0m"

#define verbose0(format) \
	fprintf(stderr, "EARDBD %s, %s \n", str_who[mirror_iam], format);

#define verbose1(format, ...) \
	fprintf(stderr, "EARDBD %s, " format "\n", str_who[mirror_iam], __VA_ARGS__);
	
#define verbose3(...) \
	if (!forked || master_iam) { \
		fprintf(stderr, "EARDBD, " __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	}

#define verline1(...) \
	if (!forked || master_iam) { \
		fprintf(stderr, col1 line "EARDBD, " __VA_ARGS__); \
		fprintf(stderr, col2 "\n"); \
	}

#define verline0() \
		fprintf(stderr, col1 line col2);

#define error(...) \
	fprintf(stderr, "EARDBD ERROR, " __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(1);

// Nomenclature:
// 	- Server: main buffer of the gathered metrics. Inserts buffered metrics in
//	the database.
// 	- Mirror: the secondary buffer of the gather metrics. Inserts the buffered
//	metrics in the database if the server is offline.
//	- Master: if in a node there is a server and a mirror, the master is the
//	server. If the server is not enabled because the admin wants the node to
//	be just a mirror, the mirror is the master.

/*
 *
 * Synchronization main/mirror
 *
 */

static int sync_question(uint sync_option)
{
	time_t timeout_old;
	state_t s;

	verbose1("synchronization started: asking the question (%d)", sync_option);
	sync_qst_content.sync_option = sync_option;

	// Preparing packet
	sockets_header_update(&sync_qst_header);

	// Synchronization pipeline
	s = sockets_socket(ssync_mir);

	if (state_fail(s)) {
		verbose1("failed to create client socket to MAIN (%d, inum: %d, istr: %s)",
				 s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_connect(ssync_mir);

	if (state_fail(s)) {
		verbose1("failed to connect to MAIN (%d, inum: %d, istr: %s)",
				 s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_send(ssync_mir, &sync_qst_header, (char *) &sync_qst_content);

	if (state_fail(s)) {
		verbose1("failed to send to MAIN (%d, num: %d, str: %s)",
				 s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	// Setting new timeout
	sockets_get_timeout(ssync_mir->fd, &timeout_old);
	sockets_set_timeout(ssync_mir->fd, 10);

	// Transferring
	s = sockets_receive(ssync_mir->fd, &sync_ans_header, (char *) &sync_ans_content, sizeof(sync_ans_t));

	// Recovering old timeout
	sockets_set_timeout(ssync_mir->fd, timeout_old);

	if (state_fail(s)) {
		verbose1("failed to receive from MAIN (%d, num: %d, str: %s)",
				 s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_close(ssync_mir);

	verbose0("synchronization completed correctly");

	return EAR_SUCCESS;
}

static int sync_answer(int fd)
{
	socket_t sync_ans_socket;
	state_t s;

	verbose0("synchronization started: answering the question");

	// Socket
	sockets_clean(&sync_ans_socket);
	sync_ans_socket.protocol = TCP;
	sync_ans_socket.fd = fd;

	// Header
	sockets_header_update(&sync_ans_header);

	s = sockets_send(&sync_ans_socket, &sync_ans_header, (char *) &sync_ans_content);

	if (state_fail(s)) {
		verbose1("Failed to send to MIRROR (%d, num: %d, str: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	verbose0("synchronization completed correctly");

	return EAR_SUCCESS;
}

/*
 *
 * Data storing
 *
 */

static void db_store_events()
{
	float percent = (float) (i_evnts) / (float) (len_evnts);
	verbose1("inserting: %lu/%lu (%0.2f) samples of events",
			 i_evnts, len_evnts, percent);

	if (i_evnts <= 0) {
		return;
	}

	verbose1("inserting in DB %lu event samples", i_evnts);
	//db_batch_insert_ear_event(evnts[i_evnts]);
	i_evnts = 0;
}

static void db_store_loops()
{
	float percent = (float) (i_loops) / (float) (len_loops);
	verbose1("inserting: %lu/%lu (%0.2f) samples of loops",
			 i_loops, len_loops, percent);

	if (i_loops <= 0) {
		return;
	}

	//db_batch_insert_loops(loops[i_loops]);
	i_loops = 0;
}

static void db_store_periodic_metrics()
{
	float percent = (float) (i_enrgy) / (float) (len_enrgy);
	verbose1("inserting: %lu/%lu (%0.2f) samples of energy monitoring data",
			 i_enrgy, len_enrgy, percent);

	if (i_enrgy <= 0) {
		return;
	}

	//db_batch_insert_periodic_metrics(enrgy[i_enrgy]);
	i_enrgy = 0;
}

static void db_store_periodic_aggregation()
{
	float percent = (float) (i_appsl) / (float) (len_appsl);
	verbose1("inserting: %d samples energy monitoring data (from %lu to %lu, consuming %lu mJ)",
			 aggr.n_samples, aggr.start_time, aggr.end_time, aggr.DC_energy);

	if (aggr.n_samples <= 0) {
		return;
	}

	//db_insert_periodic_aggregation(&aggr);
	init_periodic_aggregation(&aggr);
}

static void db_store_applications_mpi()
{
	float percent = (float) (i_appsm) / (float) (len_appsm);
	verbose1("inserting: %lu/%lu (%0.2f) samples of mpi applications",
			 i_appsm, len_appsm, percent);

	if (i_appsm <= 0) {
		return;
	}

	//db_batch_insert_applications(appsm[i_appsm]);
	i_appsm = 0;
}

static void db_store_applications()
{
	float percent = (float) (i_appsn) / (float) (len_appsn);
	verbose1("inserting: %lu/%lu (%0.2f) samples of non-mpi applications",
			 i_appsn, len_appsn, percent);

	if (i_appsn <= 0) {
		return;
	}

	//db_batch_insert_applications_no_mpi(appsn, i_appsn);
	i_appsn = 0;
}

static void db_store_applications_learning()
{
	float percent = (float) (i_appsl) / (float) (len_appsl);
	verbose1("inserting: %lu/%lu (%0.2f) samples of learning applications",
			 i_appsl, len_appsl, percent);

	if (i_appsl <= 0) {
		return;
	}

	//db_batch_insert_applications(appsl[i_appsl]);
	i_appsl = 0;
}

/*
 * Data processing
 */

static void make_periodic_aggregation(periodic_aggregation_t *aggr, periodic_metric_t *met)
{
	add_periodic_aggregation(aggr, met->DC_energy, met->start_time, met->end_time);
}

static void process_reset_insert_time()
{
	// Refresh insert time
	timeout_insr.tv_sec = time_insr;
	timeout_insr.tv_usec = 0L;

	// Set when was the last insert time
	time_insr_last = time(NULL);
}

static void process_reset_indexes()
{
	i_appsm = 0;
	i_appsn = 0;
	i_appsl = 0;
	i_enrgy = 0;
	i_evnts = 0;
	i_loops = 0;
}

static void process_insert(uint option, uint reason)
{
	verline0();
	verbose1("inserting reason: %u", reason);

	if (sync_option(option, SYNC_ALL))
	{
		// Time to aggregate
		db_store_periodic_aggregation();

		// Resetting time
		process_reset_insert_time();
	}
	if (sync_option(option, SYNC_APPSM)) {
		db_store_applications_mpi();
	}
	if (sync_option(option, SYNC_APPSN)) {
		db_store_applications();
	}
	if (sync_option(option, SYNC_APPSL)) {
		db_store_applications_learning();
	}
	if (sync_option(option, SYNC_ENRGY)) {
		db_store_events();
	}
	if (sync_option(option, SYNC_EVNTS)) {
		db_store_events();
	}
	if (sync_option(option, SYNC_LOOPS)) {
		db_store_loops();
	}
	if (sync_option(option, SYNC_RESET)) {
		process_reset_indexes();
		process_reset_insert_time();
	}
}

/*
 *
 * Incoming data
 *
 */

static void sample(char *buf, ulong len, ulong *idx, char *cnt, size_t siz, uint opt)
{
	memcpy (&buf[*idx], cnt, siz);
	*idx += 1;

	if (*idx == len)
	{
		if(server_iam) {
			process_insert(opt, RES_OVER);
		} else if (state_fail(sync_question(opt))) {
			process_insert(opt, RES_OVER);
		}
		*idx = 0;
	}
}

static void incoming_data_process(int fd, packet_header_t *header, char *content)
{
	state_t state;

	if (header->content_type == CONTENT_TYPE_APP)
	{
		application_t *app = (application_t *) content;

		if (app->is_learning) {
			sample((char *) appsl, len_appsl, &i_appsl, content, sizeof(application_t), SYNC_APPSL);
		} else if (app->is_mpi) {
			sample((char *) appsm, len_appsm, &i_appsm, content, sizeof(application_t), SYNC_APPSM);
		} else {
			sample((char *) appsn, len_appsn, &i_appsn, content, sizeof(application_t), SYNC_APPSN);
		}
	} else if (header->content_type == CONTENT_TYPE_EVE) {
		sample((char *) evnts, len_evnts, &i_evnts, content, sizeof(ear_event_t), SYNC_EVNTS);
	} else if (header->content_type == CONTENT_TYPE_LOO) {
		sample((char *) loops, len_loops, &i_loops, content, sizeof(loop_t), SYNC_LOOPS);
	} else if (header->content_type == CONTENT_TYPE_PER) {
		make_periodic_aggregation(&aggr, (periodic_metric_t *) content);
		sample((char *) enrgy, len_enrgy, &i_enrgy, content, sizeof(periodic_metric_t), SYNC_ENRGY);
	} else if (header->content_type == CONTENT_TYPE_QST)
	{
		sync_qst_t *q = (sync_qst_t *) content;
		process_insert(q->sync_option, RES_SYNC);
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

	verbose1("received '%s' packet (m: %d) from host '%s' (socket: %d)",
			type, header->data_extra1, header->host_src, fd);
}

static int incoming_new_connection(int fd)
{
	int nc;

	nc  = !mirror_iam && (fd == smets_srv->fd || fd == ssync_srv->fd);
	nc |=  mirror_iam && (fd == smets_mir->fd);

	return nc;
}

/*
 *
 * Signals
 *
 */

static void release_sockets()
{
	// Cleaning sockets
	sockets_dispose(smets_srv);
	sockets_dispose(smets_mir);
	sockets_dispose(ssync_srv);
	sockets_dispose(smets_srv);
}

static void release_resources()
{
	release_sockets();

	// Freeing data
	if (appsm != NULL)
	{
		free(appsm);
		free(appsn);
		free(appsl);
		free(enrgy);
		free(evnts);
		free(loops);

		appsm = NULL;
		appsn = NULL;
		appsl = NULL;
		enrgy = NULL;
		evnts = NULL;
		loops = NULL;
	}

	process_reset_indexes();

	free_cluster_conf(&conf_clus);
}

static void signal_handler(int signal)
{
	int propagating = 0;

	// Case exit
	if (signal == SIGTERM || signal == SIGINT && !exitting)
	{
		verbose1("signal SIGTERM/SIGINT received on %s, exitting", str_who[mirror_iam]);

		propagating = 1;
		listening   = 0;
		releasing   = 1;
		exitting    = 1;
	}

	// Case reconfigure
	if (signal == SIGHUP && !reconfiguring)
	{
		verbose1("signal SIGHUP received on %s, reconfiguring", str_who[mirror_iam]);

		propagating   = 1;
		listening     = 0;
		reconfiguring = server_iam;
		releasing     = 1;
		exitting      = mirror_iam;
	}

	// Propagate signals
	if (propagating && server_iam && mirror_too && (mirror_pid > 0)) {
		kill(mirror_pid, signal);
	}
	if (propagating && mirror_iam && server_too && (server_pid > 0)) {
		kill(server_pid, signal);
	}
}

/*
 *
 * Init
 *
 */

static void init_general_configuration(int argc, char **argv, cluster_conf_t *conf_clus)
{
	node_island_t *is;
	int i, j, k, found;
	char *p;

	// Init values (who am I? Ok I'm the server, which is also the master)
	mirror_iam = 0;
	mirror_too = 0;
	server_iam = 1;
	server_too = 1;
	master_iam = 0;
	forked     = 0;
	server_pid = getpid();
	mirror_pid = 0;

	// Configuration
#if 1
	if (get_ear_conf_path(extra_buffer) == EAR_ERROR) {
		error("while getting ear.conf path");
	}

	verbose1("reading '%s' configuration file", extra_buffer);
	read_cluster_conf(extra_buffer, conf_clus);

	// Database
	init_db_helper(&conf_clus->database);

	// Mirror finding
	gethostname(mirror_host, SZ_NAME_MEDIUM);

	for (i = 0; i < conf_clus->num_islands && !found; ++i)
	{
		is = &conf_clus->islands[i];

		for (k = 0; k < is->num_ranges && !found; k++)
		{
			p = is->db_ips[is->ranges[i].db_ip];

			if (!server_too && p != NULL && (strncmp(p, mirror_host, strlen(mirror_host)) == 0)){
				server_too = 1;
			}

        	if (!mirror_too && is->ranges[i].sec_ip >= 0)
			{
				p = is->backup_ips[is->ranges[i].sec_ip];

				if (p != NULL && (strncmp(p, mirror_host, strlen(mirror_host)) == 0))
				{
					strcpy(server_host, is->db_ips[is->ranges[i].db_ip]);
					mirror_too = 1;
				}
			}

			found = server_too && mirror_too;
		}
	}

	verbose1("'%s' '%s', '%u' '%u'", server_host, mirror_host, server_too, mirror_too);
#else
	conf_clus->db_manager.aggr_time = 30;
	conf_clus->db_manager.tcp_port = 4711;
	conf_clus->db_manager.udp_port = 4712;
	conf_clus->db_manager.mem_size = 100;

	server_too = atoi(argv[1]);
	mirror_too = atoi(argv[2]);
	strcpy(server_host, argv[3]);
#endif
	// Allocation
	alloc = (float) conf_clus->db_manager.mem_size;

	// Times
	time_insr = (long) conf_clus->db_manager.aggr_time;

	timeout_insr.tv_sec  = time_insr;
	timeout_insr.tv_usec = 0L;
	timeout_sync.tv_sec  = 10;
	timeout_sync.tv_usec = 0L;

	// Verbose
	verbose1("enabled cache server: %s",      server_too ? "OK": "NO");
	verbose1("enabled cache mirror: %s (%s)", mirror_too ? "OK" : "NO", server_host);

	if (!server_too && !mirror_too) {
		error("this node is not a server nor mirror");
	}
}

static void init_sockets(int argc, char **argv, cluster_conf_t *conf_clus)
{
	state_t s1 = EAR_SUCCESS;
	state_t s2 = EAR_SUCCESS;
	state_t s3 = EAR_SUCCESS;

	// Cleaning socket sets
	FD_ZERO(&fds_incoming);
	FD_ZERO(&fds_active);

	// Cleaning sockets
	sockets_clean(smets_srv);
	sockets_clean(smets_mir);
	sockets_clean(ssync_srv);
	sockets_clean(smets_srv);

	// Setting data
	sockets_init(smets_srv, NULL, 4711, TCP);
	sockets_init(ssync_srv, NULL, 4713, TCP);
	sockets_init(smets_mir, NULL, 4712, TCP);
	sockets_init(ssync_mir, server_host, 4713, TCP);

	// Opening server socket
	if (server_too) s1 = sockets_socket(smets_srv);
	if (server_too) s2 = sockets_socket(ssync_srv);
	if (mirror_too) s3 = sockets_socket(smets_mir);

	if (state_fail(s1) || state_fail(s2) || state_fail(s3)) {
		error("while creating sockets (%s)", intern_error_str);
	}

	// Binding socket
	if (server_too) s1 = sockets_bind(smets_srv);
	if (server_too) s2 = sockets_bind(ssync_srv);
	if (mirror_too) s3 = sockets_bind(smets_mir);

	if (state_fail(s1) || state_fail(s2) || state_fail(s3)) {
		error("while binding sockets (%s) %d %d %d", intern_error_str, s1, s2, s3);
	}

	// Listening socket
	if (server_too) s1 = sockets_listen(smets_srv);
	if (server_too) s2 = sockets_listen(ssync_srv);
	if (mirror_too) s3 = sockets_listen(smets_mir);

	if (state_fail(s1) || state_fail(s2) || state_fail(s3)) {
		error("while listening sockets (%s)", intern_error_str);
	}

	int fd1 = smets_srv->fd;
	int fd2 = smets_mir->fd;
	int fd3 = ssync_srv->fd;
	int fd4 = ssync_mir->fd;

	// Summary
	verbose3("type          \tport\tprot\tstat  \tfd");
	verbose3("----          \t----\t----\t----  \t--");
	verbose3("server metrics\t%d  \tTCP \t%s\t%d", smets_srv->port, str_soc[fd1 == -1], fd1);
	verbose3("mirror metrics\t%d  \tTCP \t%s\t%d", smets_mir->port, str_soc[fd2 == -1], fd2);
	verbose3("server sync   \t%d  \tTCP \t%s\t%d", ssync_srv->port, str_soc[fd3 == -1], fd3);
	verbose3("mirror sync   \t%d  \tTCP \t%s\t%d", ssync_mir->port, str_soc[fd4 == -1], fd4);
	verbose3("TIP! mirror sync socket opens and closes intermittently");
}

static void init_fork(int argc, char **argv, cluster_conf_t *conf_clus)
{
	if (mirror_too)
	{
		mirror_pid = fork();

		if (mirror_pid == 0)
		{
			// Reconfiguring
			mirror_iam = 1;
			server_iam = 0;

			mirror_pid = getpid();
			sleep(1);
		} else if (mirror_pid > 0) {
		} else {
			error("error while forking, terminating the program");
		}
	}

	forked = 1;
	master_iam = (server_iam && server_too) || (mirror_iam && !server_too);

	verbose3("cache server pid: %d", server_pid);
	verbose3("cache mirror pid: %d", mirror_pid);
}

static void init_sockets_mirror(int argc, char **argv, cluster_conf_t *conf_clus)
{
	// Destroying main sockets
	sockets_dispose(smets_srv);
	sockets_dispose(ssync_srv);

	// Keep track of the biggest file descriptor
	fd_max = smets_mir->fd;

	FD_SET(smets_mir->fd, &fds_active);
}

static void init_sockets_main(int argc, char **argv, cluster_conf_t *conf_clus)
{
	// Destroying mirror soockets
	sockets_dispose(smets_mir);

	// Keep track of the biggest file descriptor
	fd_max = ssync_srv->fd;

	if (smets_srv->fd > fd_max) {
		fd_max = smets_srv->fd;
	}

	FD_SET(smets_srv->fd, &fds_active);
	FD_SET(ssync_srv->fd, &fds_active);
}

static void init_signals()
{
	struct sigaction action;
	sigset_t set;

	// Blocking all signals except PIPE, TERM, INT and HUP
	sigfillset(&set);
	sigdelset(&set, SIGPIPE);
	sigdelset(&set, SIGTERM);
	sigdelset(&set, SIGINT);
	sigdelset(&set, SIGHUP);

	sigprocmask(SIG_SETMASK, &set, NULL);

	// Editing signals individually
	sigfillset(&action.sa_mask);
	action.sa_handler = signal_handler;
	action.sa_flags = 0;

	if (sigaction(SIGPIPE, &action, NULL) < 0) {
		verbose1("sigaction error on signal %d (%s)", SIGPIPE, strerror(errno));
	}
	if (sigaction(SIGTERM, &action, NULL) < 0) {
		verbose1("sigaction error on signal %d (%s)", SIGTERM, strerror(errno));
	}
	if (sigaction(SIGINT, &action, NULL) < 0) {
		verbose1("sigaction error on signal %d (%s)", SIGINT, strerror(errno));
	}
	if (sigaction(SIGHUP, &action, NULL) < 0) {
		verbose1("sigaction error on signal %d (%s)", SIGHUP, strerror(errno));
	}
}

static ulong get_allocation_elements(float percent, size_t size)
{
	float megabytes_blk = ceilf(alloc * percent);
	float megabytes_one = ((float) size) / 1000000.0;
	ulong elements = (ulong) (megabytes_blk / megabytes_one);

	if (elements < 100) {
		return 100;
	}

	return elements;
}

static void init_process_configuration(int argc, char **argv, cluster_conf_t *conf_clus)
{
	// Single process configuration
	listening = (server_iam && server_too) || (mirror_iam);

	// is not a listening socket?
	waiting   = (server_iam && !listening);
	releasing = waiting;

	// Activating signal catch
	init_signals();

	if (waiting) {
		return;
	}

	// Socket closing
	if (mirror_iam) {
		init_sockets_mirror(argc, argv, conf_clus);
	} else {
		init_sockets_main(argc, argv, conf_clus);
	}

	float len_appsl_pc = 0.05;
	float len_appsm_pc = 0.40;
	float len_appsn_pc = 0.20;
	float len_enrgy_pc = 0.05;
	float len_evnts_pc = 0.05;
	float len_loops_pc = 0.25;

	// Data allocation
	len_appsl = get_allocation_elements(len_appsl_pc, sizeof(application_t));
	len_appsm = get_allocation_elements(len_appsm_pc, sizeof(application_t));
	len_appsn = get_allocation_elements(len_appsn_pc, sizeof(application_t));
	len_enrgy = get_allocation_elements(len_enrgy_pc, sizeof(periodic_metric_t));
	len_evnts = get_allocation_elements(len_evnts_pc, sizeof(ear_event_t));
	len_loops = get_allocation_elements(len_loops_pc, sizeof(loop_t));

	// Raw data
	ulong b_enrgy = sizeof(periodic_metric_t) * len_enrgy;
	ulong b_appsl = sizeof(application_t) * len_appsl;
	ulong b_appsm = sizeof(application_t) * len_appsm;
	ulong b_appsn = sizeof(application_t) * len_appsn;
	ulong b_evnts = sizeof(ear_event_t) * len_evnts;
	ulong b_loops = sizeof(loop_t) * len_loops;

	appsm = malloc(b_appsm);
	appsl = malloc(b_appsl);
	appsn = malloc(b_appsn);
	enrgy = malloc(b_enrgy);
	evnts = malloc(b_evnts);
	loops = malloc(b_loops);

	float mb_appsl = (double) (b_appsl) / 1000000.0;
	float mb_appsm = (double) (b_appsm) / 1000000.0;
	float mb_appsn = (double) (b_appsn) / 1000000.0;
	float mb_enrgy = (double) (b_enrgy) / 1000000.0;
	float mb_loops = (double) (b_loops) / 1000000.0;
	float mb_evnts = (double) (b_evnts) / 1000000.0;
	float mb_total = mb_appsl + mb_appsm + mb_appsn + mb_enrgy + mb_loops + mb_evnts;

	// Synchronization headers
	sockets_header_clean(&sync_ans_header);
	sockets_header_clean(&sync_qst_header);

	sync_ans_header.content_type = CONTENT_TYPE_ANS;
	sync_ans_header.content_size = sizeof(sync_ans_t);
	sync_qst_header.content_type = CONTENT_TYPE_QST;
	sync_qst_header.content_size = sizeof(sync_qst_t);

	// Summary
	verbose3("type         \tmemory   \tsample\t\tElems\t%%");
	verbose3("----         \t------   \t------\t\t-----\t----");
	verbose3("mpi apps     \t%0.2f MBs\t%lu Bs\t\t%lu\t%0.2f", mb_appsm, sizeof(application_t), len_appsm, len_appsm_pc);
	verbose3("non-mpi apps \t%0.2f MBs\t%lu Bs\t\t%lu\t%0.2f", mb_appsn, sizeof(application_t), len_appsn, len_appsn_pc);
	verbose3("learn apps   \t%0.2f MBs\t%lu Bs\t\t%lu\t%0.2f", mb_appsl, sizeof(application_t), len_appsl, len_appsl_pc);
	verbose3("pwr metrics  \t%0.2f MBs\t%lu Bs\t\t%lu\t%0.2f", mb_enrgy, sizeof(periodic_metric_t), len_enrgy, len_enrgy_pc);
	verbose3("app loops    \t%0.2f MBs\t%lu Bs\t\t%lu\t%0.2f", mb_loops, sizeof(loop_t), len_loops, len_loops_pc);
	verbose3("events       \t%0.2f MBs\t%lu Bs\t\t%lu\t%0.2f", mb_evnts, sizeof(ear_event_t), len_evnts, len_evnts_pc);
	verbose3("TOTAL        \t%0.2f MBs", mb_total);
}

static void pipeline()
{
	state_t s1;
	state_t s2;
	state_t s3;
	int s;
	int i;

	while(listening)
	{
		fds_incoming = fds_active;

		if ((s = select(fd_max + 1, &fds_incoming, NULL, NULL, &timeout_insr)) == -1) {
			if (listening) {
				error("during select (%s)", intern_error_str);
			}
		}

		// If timeout_insr, data processing
		if (timeout_insr.tv_sec == 0 && timeout_insr.tv_usec == 0)
		{
			// Inserts data and updates timeouts
			// Synchronizing with the MAIN
			if (mirror_iam)
			{
				if(state_fail(sync_question(SYNC_ALL))) {
					process_insert(SYNC_ALL, RES_TIME);
				} else {
					process_insert(SYNC_RESET, RES_TIME);
				}
			} else {
				process_insert(SYNC_ALL, RES_TIME);
			}
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fd_max && listening; i++)
		{
			if (FD_ISSET(i, &fds_incoming)) // we got one!!
			{
				// Handle new connections (just for TCP)
				if (incoming_new_connection(i))
				{
					s1 = sockets_accept(i, &fd_cli);

					if (state_ok(s1))
					{
						verbose1("accepted connection from socket %d", fd_cli);

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
							verbose1("disconnected from socket %d (num: %d, str: %s)",
									i, intern_error_num, intern_error_str);
						} else {
							verbose1("on reception (num: %d, str: %s), disconnecting from socket %d",
									intern_error_num, intern_error_str, i);
						}

						sockets_close_fd(i);
						FD_CLR(i, &fds_active);
					}
				}
			}
		}
	}


	if (waiting) {
		waitpid(mirror_pid, NULL, 0);
	}

	if (releasing) {
		release_resources();
		releasing = 0;
	}

	if (reconfiguring) {
		sleep(5);
		reconfiguring = 0;
	}
}

void usage(int argc, char **argv) {}

int main(int argc, char **argv)
{
	//
	usage(argc, argv);

	while(!exitting)
	{
		//
		verline1("phase 1: general configuration");
		init_general_configuration(argc, argv, &conf_clus);

		//
		verline1("phase 2: sockets initialization");
		init_sockets(argc, argv, &conf_clus);

		//
		verline1("phase 3: processes fork");
		init_fork(argc, argv, &conf_clus);

		//
		verline1("phase 4: process configuration & allocation");
		init_process_configuration(argc, argv, &conf_clus);

		//
		verline1("phase 5: listening (processing every %lu s)", time_insr);
		pipeline();
	}

	return 0;
}
