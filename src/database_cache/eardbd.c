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
#include <database_cache/eardbd_storage.h>
#include <common/database/db_helper.h>

int EAR_VERBOSE_LEVEL = 1;

// Configuration
static cluster_conf_t conf_clus;

// Extras
static pid_t server_pid;
static pid_t mirror_pid;
static pid_t others_pid;
static float alloc;
static int isle;

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

int fd_min;
int fd_max;
int fd_cli;

// Times
static struct timeval timeout_insr;
static struct timeval timeout_aggr;
static struct timeval timeout_slct;
static time_t time_insr;
static time_t time_aggr;
static time_t time_slct;

// Input buffers
static packet_header_t input_header;
static char input_buffer[SZ_BUFF_BIG];
static char extra_buffer[SZ_BUFF_BIG];

// Mirroring
static char master_host[SZ_NAME_MEDIUM]; // This node name
static char server_host[SZ_NAME_MEDIUM]; // If i'm mirror, which is the server?
static int server_port;
static int mirror_port;
static int synchr_port;
static int server_too;
static int mirror_too;
static int others_too; // Just if other process exists
int master_iam; // Master is who speaks
int server_iam;
int mirror_iam;

// Pipeline
static int reconfiguring;
static int listening;
static int releasing;
static int exitting;
static int waiting;
int forked;

// Synchronization
static packet_header_t sync_ans_header;
static packet_header_t sync_qst_header;
static sync_qst_t sync_qst_content;
static sync_ans_t sync_ans_content;

// Data warehouse
uint per_appsm = 40;
uint per_appsn = 20;
uint per_appsl = 05;
uint per_loops = 24;
uint per_enrgy = 05;
uint per_aggrs = 01;
uint per_evnts = 05;

ulong len_aggrs;
ulong len_appsl;
ulong len_appsm;
ulong len_appsn;
ulong len_enrgy;
ulong len_evnts;
ulong len_loops;

periodic_aggregation_t *aggrs;
periodic_metric_t *enrgy;
    application_t *appsm;
    application_t *appsn;
    application_t *appsl;
      ear_event_t *evnts;
           loop_t *loops;

ulong i_aggrs;
ulong i_enrgy;
ulong i_appsm;
ulong i_appsn;
ulong i_appsl;
ulong i_evnts;
ulong i_loops;

// Strings
char *str_who[2] = { "server", "mirror" };

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

int sync_question(uint sync_option)
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
	s = sockets_receive(ssync_mir->fd, &sync_ans_header,
		(char *) &sync_ans_content, sizeof(sync_ans_t));

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

int sync_answer(int fd)
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
 * Time
 *
 */

void time_substract_timeouts()
{
	timeout_insr.tv_sec -= time_slct;
	timeout_aggr.tv_sec -= time_slct;
}

void time_reset_timeout_insr(time_t offset_insr)
{
	// Refresh insert time
	timeout_insr.tv_sec  = time_insr + offset_insr;
	timeout_insr.tv_usec = 0L;
}

void time_reset_timeout_aggr()
{
	// Refresh aggregation timeout
	timeout_aggr.tv_sec  = time_aggr;
	timeout_aggr.tv_usec = 0L;
}

void time_reset_timeout_slct()
{
	// Refresh select time
	time_slct = timeout_aggr.tv_sec;

	if (timeout_insr.tv_sec < timeout_aggr.tv_sec) {
		time_slct = timeout_insr.tv_sec;
	}

	timeout_slct.tv_sec  = time_slct;
	timeout_slct.tv_usec = 0L;
}

/*
 *
 * Net
 *
 */

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
		free(aggrs);
		free(evnts);
		free(loops);

		appsm = NULL;
		appsn = NULL;
		appsl = NULL;
		enrgy = NULL;
		aggrs = NULL;
		evnts = NULL;
		loops = NULL;
	}

	reset_indexes();

	free_cluster_conf(&conf_clus);
}

//static void signal_handler(int signal)
static void signal_handler(int signal, siginfo_t *info, void *context)
{
	int propagating = 0;

	// Case exit
	if (signal == SIGTERM || signal == SIGINT && !exitting)
	{
		verbose1("signal SIGTERM/SIGINT received on %s, exitting", str_who[mirror_iam]);

		propagating = others_pid > 0 && info->si_pid != others_pid;
		listening   = 0;
		releasing   = 1;
		exitting    = 1;
	}

	// Case reconfigure
	if (signal == SIGHUP && !reconfiguring)
	{
		verbose1("signal SIGHUP received on %s, reconfiguring", str_who[mirror_iam]);

		propagating   = others_pid > 0 && info->si_pid != others_pid;
		listening     = 0;
		reconfiguring = server_iam;
		releasing     = 1;
		exitting      = mirror_iam;
	}

	// Propagate signals
	if (propagating)
	{
		kill(others_pid, signal);

		if (server_iam) {
			waitpid(mirror_pid, NULL, 0);
		}
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

	// Cleaning 0 (who am I? Ok I'm the server, which is also the master for now)
	mirror_iam = 0;
	mirror_too = 0;
	server_iam = 1;
	server_too = 0;
	master_iam = 1;
	forked     = 0;
	server_pid = getpid();
	mirror_pid = 0;
	others_pid = 0;

	// Configuration
#if 1
	if (get_ear_conf_path(extra_buffer) == EAR_ERROR) {
		error("while getting ear.conf path");
	}

	verbose3("reading '%s' configuration file", extra_buffer);
	read_cluster_conf(extra_buffer, conf_clus);

	// Database
	init_db_helper(&conf_clus->database);

	// Mirror finding
	gethostname(master_host, SZ_NAME_MEDIUM);
	found = 0;

	for (i = 0; i < conf_clus->num_islands && !found; ++i)
	{
		is = &conf_clus->islands[i];
		//verbose3("i %d", i);

		for (k = 0; k < is->num_ranges && !found; k++)
		{
			p = is->db_ips[is->ranges[k].db_ip];
			//verbose3("r %s", p);

			if (!server_too && p != NULL && (strncmp(p, master_host, strlen(master_host)) == 0)){
				//verbose3("db_ip %s", p);
				server_too = 1;
			}

        	if (!mirror_too && is->ranges[k].sec_ip >= 0)
			{
				p = is->backup_ips[is->ranges[k].sec_ip];

				if (p != NULL && (strncmp(p, master_host, strlen(master_host)) == 0))
				{
					//verbose3("db_ip_sec %s", p);
					strcpy(server_host, is->db_ips[is->ranges[k].db_ip]);
					mirror_too = 1;
				}
			}

			found = server_too && mirror_too;
		}
	}
#else
	conf_clus->db_manager.tcp_port      = 4711;
	conf_clus->db_manager.sec_tcp_port  = 4712;
	conf_clus->db_manager.sync_tcp_port = 4713;
	conf_clus->db_manager.mem_size      = 100;
	conf_clus->db_manager.aggr_time     = 10;
	conf_clus->db_manager.insr_time     = 20;

	server_too = atoi(argv[1]);
	mirror_too = atoi(argv[2]);
	strcpy(server_host, argv[3]);
#endif
	// Ports
	server_port = conf_clus->db_manager.tcp_port;
    mirror_port = conf_clus->db_manager.sec_tcp_port;
    synchr_port = conf_clus->db_manager.sync_tcp_port;

	// Allocation
	alloc = (float) conf_clus->db_manager.mem_size;

	// Server & mirro verbosity
	verbose3("enabled cache server: %s",      server_too ? "OK": "NO");
	verbose3("enabled cache mirror: %s (%s)", mirror_too ? "OK" : "NO", server_host);

	if (!server_too && !mirror_too) {
		error("this node is not configured as a server nor mirror");
	}
}

static void init_time_configuration(int argc, char **argv, cluster_conf_t *conf_clus)
{
	// Times
	time_insr = (time_t) conf_clus->db_manager.insr_time;
	time_aggr = (time_t) conf_clus->db_manager.aggr_time;

	if (time_insr == 0) {
		verbose3("insert time can't be 0, using 300 seconds (default)");
		time_insr = 300;
	}
	if (time_aggr == 0) {
		verbose3("aggregation time can't be 0, using 60 seconds (default)");
		time_aggr = 60;
	}

	// Looking for multiple value
	time_reset_timeout_insr(0);
	time_reset_timeout_aggr();
	time_reset_timeout_slct();

	// Times verbosity
	verbose3("insertion time:   %lu seconds", time_insr);
	verbose3("aggregation time: %lu seconds", time_aggr);
}

static void init_sockets(int argc, char **argv, cluster_conf_t *conf_clus)
{
	state_t s1 = EAR_SUCCESS;
	state_t s2 = EAR_SUCCESS;
	state_t s3 = EAR_SUCCESS;
	time_t timeout;
	int binded;

	// Cleaning socket sets
	FD_ZERO(&fds_incoming);
	FD_ZERO(&fds_active);

	// Cleaning sockets
	sockets_clean(smets_srv);
	sockets_clean(smets_mir);
	sockets_clean(ssync_srv);
	sockets_clean(smets_srv);

	// Setting data
	sockets_init(smets_srv, NULL, server_port, TCP);
	sockets_init(ssync_srv, NULL, synchr_port, TCP);
	sockets_init(smets_mir, NULL, mirror_port, TCP);
	sockets_init(ssync_mir, server_host, synchr_port, TCP);

	// Opening server socket
	if (server_too) s1 = sockets_socket(smets_srv);
	if (server_too) s2 = sockets_socket(ssync_srv);
	if (mirror_too) s3 = sockets_socket(smets_mir);

	if (state_fail(s1) || state_fail(s2) || state_fail(s3)) {
		error("while creating sockets (%s)", intern_error_str);
	}

	if (server_too) s1 = EAR_NOT_READY;
    if (server_too) s2 = EAR_NOT_READY;
    if (mirror_too) s3 = EAR_NOT_READY;
	timeout = time(NULL) + 61;

	// Binding socket
	do {
		if (server_too && state_fail(s1)) s1 = sockets_bind(smets_srv);
		if (server_too && state_fail(s2)) s2 = sockets_bind(ssync_srv);
		if (mirror_too && state_fail(s3)) s3 = sockets_bind(smets_mir);
		binded = state_ok(s1) && state_ok(s2) && state_ok(s3);
	}
	while (!binded && time(NULL) < timeout && !sleep(10));

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

	// Verbosity
	char *str_sta[2] = { "listen", "closed" };

	int fd1 = smets_srv->fd;
	int fd2 = smets_mir->fd;
	int fd3 = ssync_srv->fd;
	int fd4 = ssync_mir->fd;

	// Summary
	tprintf_init(stderr, "18 8 7 10 8");

	tprintf("type||port||prot||stat||fd");
	tprintf("----||----||----||----||--");
	tprintf("server metrics||%d||TCP||%s||%d", smets_srv->port, str_sta[fd1 == -1], fd1);
	tprintf("mirror metrics||%d||TCP||%s||%d", smets_mir->port, str_sta[fd2 == -1], fd2);
	tprintf("server sync||%d||TCP||%s||%d", ssync_srv->port, str_sta[fd3 == -1], fd3);
	tprintf("mirror sync||%d||TCP||%s||%d", ssync_mir->port, str_sta[fd4 == -1], fd4);
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
			others_pid = server_pid;
			sleep(1);
		}
		else if (mirror_pid > 0) {
			others_pid = mirror_pid;
		} else {
			error("error while forking, terminating the program");
		}
	}

	forked = 1;
	master_iam = (server_iam && server_too) || (mirror_iam && !server_too);

	// Verbosity
	char *str_sta[2] = { "(just sleeps)", "" };

	verbose3("cache server pid: %d %s", server_pid, str_sta[server_too]);
	verbose3("cache mirror pid: %d", mirror_pid);
}

static void init_sockets_mirror(int argc, char **argv, cluster_conf_t *conf_clus)
{
	// Destroying main sockets
	sockets_dispose(smets_srv);
	sockets_dispose(ssync_srv);

	// Keep track of the biggest file descriptor
	fd_max = smets_mir->fd;
	fd_min = smets_mir->fd;

	FD_SET(smets_mir->fd, &fds_active);
}

static void init_sockets_main(int argc, char **argv, cluster_conf_t *conf_clus)
{
	// Destroying mirror soockets
	sockets_dispose(smets_mir);

	// Keep track of the biggest file descriptor
	fd_max = ssync_srv->fd;
	fd_min = ssync_srv->fd;

	if (smets_srv->fd > fd_max) {
		fd_max = smets_srv->fd;
	}
	if (smets_srv->fd < fd_min) {
		fd_min = smets_srv->fd;
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
	sigdelset(&set, SIGTERM);
	sigdelset(&set, SIGINT);
	sigdelset(&set, SIGHUP);

	sigprocmask(SIG_SETMASK, &set, NULL);

	// Editing signals individually
	sigfillset(&action.sa_mask);

	action.sa_handler = NULL;
	action.sa_sigaction = signal_handler;
	action.sa_flags = SA_SIGINFO;

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
	uint per_total = 0;
	int i;

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

	for (i = 0; i < EARDBD_TYPES; ++i) {
		per_total += conf_clus->db_manager.mem_size_types[i];
	}

	if (per_total > 90 && per_total < 110) {
		per_appsm = conf_clus->db_manager.mem_size_types[0];
		per_appsn = conf_clus->db_manager.mem_size_types[1];
		per_appsl = conf_clus->db_manager.mem_size_types[2];
		per_loops = conf_clus->db_manager.mem_size_types[3];
		per_enrgy = conf_clus->db_manager.mem_size_types[4];
		per_aggrs = conf_clus->db_manager.mem_size_types[5];
		per_evnts = conf_clus->db_manager.mem_size_types[6];
	} else {
		per_appsm = 40;
		per_appsn = 20;
		per_appsl = 05;
		per_loops = 24;
		per_enrgy = 05;
		per_aggrs = 01;
		per_evnts = 05;	
	}

	float len_appsm_pc = ((float) per_appsm) / 100.0;
	float len_appsn_pc = ((float) per_appsn) / 100.0;
	float len_appsl_pc = ((float) per_appsl) / 100.0;
	float len_loops_pc = ((float) per_loops) / 100.0;
	float len_enrgy_pc = ((float) per_enrgy) / 100.0;
	float len_aggrs_pc = ((float) per_aggrs) / 100.0;
	float len_evnts_pc = ((float) per_evnts) / 100.0;

	// Data allocation
	len_aggrs = get_allocation_elements(len_aggrs_pc, sizeof(periodic_aggregation_t));
	len_appsl = get_allocation_elements(len_appsl_pc, sizeof(application_t));
	len_appsm = get_allocation_elements(len_appsm_pc, sizeof(application_t));
	len_appsn = get_allocation_elements(len_appsn_pc, sizeof(application_t));
	len_enrgy = get_allocation_elements(len_enrgy_pc, sizeof(periodic_metric_t));
	len_evnts = get_allocation_elements(len_evnts_pc, sizeof(ear_event_t));
	len_loops = get_allocation_elements(len_loops_pc, sizeof(loop_t));

	// Raw data
	ulong b_aggrs = sizeof(periodic_aggregation_t) * len_aggrs;
	ulong b_enrgy = sizeof(periodic_metric_t) * len_enrgy;
	ulong b_appsl = sizeof(application_t) * len_appsl;
	ulong b_appsm = sizeof(application_t) * len_appsm;
	ulong b_appsn = sizeof(application_t) * len_appsn;
	ulong b_evnts = sizeof(ear_event_t) * len_evnts;
	ulong b_loops = sizeof(loop_t) * len_loops;

	appsm = malloc(b_appsm);
	appsn = malloc(b_appsn);
	appsl = malloc(b_appsl);
	loops = malloc(b_loops);
	enrgy = malloc(b_enrgy);
	aggrs = malloc(b_aggrs);
	evnts = malloc(b_evnts);

	memset(appsm, 0, b_appsm);
	memset(appsn, 0, b_appsn);
	memset(appsl, 0, b_appsl);
	memset(loops, 0, b_loops);
	memset(enrgy, 0, b_enrgy);
	memset(aggrs, 0, b_aggrs);
	memset(evnts, 0, b_evnts);

	float mb_aggrs = (double) (b_aggrs) / 1000000.0;
	float mb_appsl = (double) (b_appsl) / 1000000.0;
	float mb_appsm = (double) (b_appsm) / 1000000.0;
	float mb_appsn = (double) (b_appsn) / 1000000.0;
	float mb_enrgy = (double) (b_enrgy) / 1000000.0;
	float mb_loops = (double) (b_loops) / 1000000.0;
	float mb_evnts = (double) (b_evnts) / 1000000.0;
	float mb_total = mb_aggrs + mb_appsl + mb_appsm + mb_appsn +
					 mb_enrgy + mb_loops + mb_evnts;

	// Synchronization headers
	sockets_header_clean(&sync_ans_header);
	sockets_header_clean(&sync_qst_header);

	sync_ans_header.content_type = CONTENT_TYPE_ANS;
	sync_ans_header.content_size = sizeof(sync_ans_t);
	sync_qst_header.content_type = CONTENT_TYPE_QST;
	sync_qst_header.content_size = sizeof(sync_qst_t);

	// Verbose
	if(!master_iam) {
		return;
	}

	tprintf_init(stderr, "15 15 11 9 8");

	// Summary
	tprintf("type||memory||sample||elems||%%");
	tprintf("----||------||------||-----||----");

	tprintf("mpi apps||%0.2f MBs||%lu Bs||%lu||%0.2f", mb_appsm, sizeof(application_t), len_appsm, len_appsm_pc);
	tprintf("non-mpi apps||%0.2f MBs||%lu Bs||%lu||%0.2f", mb_appsn, sizeof(application_t), len_appsn, len_appsn_pc);
	tprintf("learn apps||%0.2f MBs||%lu Bs||%lu||%0.2f", mb_appsl, sizeof(application_t), len_appsl, len_appsl_pc);
	tprintf("pwr metrics||%0.2f MBs||%lu Bs||%lu||%0.2f", mb_enrgy, sizeof(periodic_metric_t), len_enrgy, len_enrgy_pc);
	tprintf("app loops||%0.2f MBs||%lu Bs||%lu||%0.2f", mb_loops, sizeof(loop_t), len_loops, len_loops_pc);
	tprintf("events||%0.2f MBs||%lu Bs||%lu||%0.2f", mb_evnts, sizeof(ear_event_t), len_evnts, len_evnts_pc);
	tprintf("aggregations||%0.2f MBs||%lu Bs||%lu||%0.2f", mb_aggrs, sizeof(periodic_aggregation_t), len_aggrs, len_aggrs_pc);

	tprintf("TOTAL||%0.2f MBs", mb_total);
	verbose3("TIP! this allocated space is per process server/mirror");
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

		if ((s = select(fd_max + 1, &fds_incoming, NULL, NULL, &timeout_slct)) == -1) {
			if (listening) {
				error("during select (%s)", intern_error_str);
			}
		}

		// If timeout_insr, data processing
		if (timeout_slct.tv_sec == 0 && timeout_slct.tv_usec == 0)
		{
			time_substract_timeouts();

			if (timeout_aggr.tv_sec == 0)
			{
				verbose3("completed the aggregation number %u with energy %lu", i_aggrs, aggrs[i_aggrs].DC_energy);
				
				// Aggregation time done, so new aggregation incoming
				storage_sample_add(NULL, len_aggrs, &i_aggrs, NULL, 0, SYNC_AGGRS);

				// Initializing the new element
				if (len_aggrs > 0) {
					init_periodic_aggregation(&aggrs[i_aggrs]);
				}

				//
				time_reset_timeout_aggr();
			}

			if (timeout_insr.tv_sec == 0)
			{
				// Synchronizing with the MAIN
				if (mirror_iam)
				{
					// Asking the question
					if(state_fail(sync_question(SYNC_ALL))) {
						// In case of fail the mirror have to insert the data
						insert_hub(SYNC_ALL, RES_TIME);
					} else {
						// In case of the answer is received the mirror just have to clear the data
						insert_hub(SYNC_RESET, RES_TIME);
					}
				} else
				{
					// Server normal insertion
					insert_hub(SYNC_ALL, RES_TIME);
				}

				// If server the time reset is do it after the insert
				time_reset_timeout_insr(0);
			}

			time_reset_timeout_slct();
		}

		// run through the existing connections looking for data to read
		for(i = fd_min; i <= fd_max && listening; i++)
		{
			if (listening && FD_ISSET(i, &fds_incoming)) // we got one!!
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
						if (fd_cli < fd_min) {
							fd_min = fd_cli;
						}
					}
					// Handle data transfers
				}
				else
				{
					s1 = sockets_receive(i, &input_header, input_buffer, sizeof(input_buffer));

					if (state_ok(s1))
					{
						storage_sample_announce(i, &input_header, input_buffer);
						storage_sample_receive(i, &input_header, input_buffer);
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
			} // FD_ISSET
		}

		// In case the listening is finished, the sockets are closed	
		for(i = fd_max; i >= fd_min && !listening; --i)
		{
			close(i);
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

		verline1("phase 2: time configuration");
		init_time_configuration(argc, argv, &conf_clus);

		//
		verline1("phase 3: sockets initialization");
		init_sockets(argc, argv, &conf_clus);

		//
		verline1("phase 4: processes fork");
		init_fork(argc, argv, &conf_clus);

		//
		verline1("phase 5: process configuration & allocation");
		init_process_configuration(argc, argv, &conf_clus);

		//
		verline1("phase 6: listening (processing every %lu s)", time_insr);
		pipeline();
	}

	return 0;
}
