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
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <common/process.h>
#include <database_cache/eardbd.h>

int EAR_VERBOSE_LEVEL = 1;

// Configuration
cluster_conf_t conf_clus;

// Input buffers
packet_header_t input_header;
char input_buffer[SZ_BUFF_BIG];
char extra_buffer[SZ_BUFF_BIG];

// Extras
static float alloc;
static int isle;

// Sockets
static socket_t sockets[4];
socket_t *smets_srv = &sockets[0];
socket_t *smets_mir = &sockets[1];
socket_t *ssync_srv = &sockets[2];
socket_t *ssync_mir = &sockets[3];

// Descriptors
struct sockaddr_storage addr_cli;
fd_set fds_metr_tcp;
fd_set fds_sync_tcp;
fd_set fds_incoming;
fd_set fds_incoming;
fd_set fds_active;
int fd_cli;
int fd_min;
int fd_max;

// Nomenclature:
// 	- Server: main buffer of the gathered metrics. Inserts buffered metrics in
//	the database.
// 	- Mirror: the secondary buffer of the gather metrics. Inserts the buffered
//	metrics in the database if the server is offline.
//	- Master: if in a node there is a server and a mirror, the master is the
//	server. If the server is not enabled because the admin wants the node to
//	be just a mirror, the mirror is the master.

// Mirroring
static char master_host[SZ_NAME_MEDIUM]; // This node name
static char server_host[SZ_NAME_MEDIUM]; // If i'm mirror, which is the server?
process_data_t proc_data_srv;
process_data_t proc_data_mir;
pid_t server_pid;
pid_t mirror_pid;
pid_t others_pid;
static int server_port;
static int mirror_port;
static int synchr_port;
static int server_too;
static int mirror_too;
static int others_too; // Just if other process exists
int master_iam; // Master is who speaks
int server_iam;
int mirror_iam;

// Synchronization
packet_header_t sync_ans_header;
packet_header_t sync_qst_header;
sync_qst_t sync_qst_content;
sync_ans_t sync_ans_content;

// Data
time_t glb_time1[MAX_TYPES];
time_t glb_time2[MAX_TYPES];
time_t ins_time1[MAX_TYPES];
time_t ins_time2[MAX_TYPES];
size_t typ_sizof[MAX_TYPES];
uint   typ_prcnt[MAX_TYPES];
uint   typ_index[MAX_TYPES];
char  *typ_alloc[MAX_TYPES];
char  *sam_iname[MAX_TYPES];
ulong  sam_inmax[MAX_TYPES];
ulong  sam_index[MAX_TYPES];
uint   sam_recvd[MAX_TYPES];
uint   soc_accpt;
uint   soc_discn;
uint   soc_unkwn;
uint   soc_tmout;

// Times (alarms)
struct timeval timeout_insr;
struct timeval timeout_aggr;
struct timeval timeout_slct;
time_t time_insr;
time_t time_aggr;
time_t time_slct;

// State machine
int reconfiguring;
int listening;
int releasing;
int exitting;
int updating;
int veteran;
int waiting;
int forked;

// Strings
char *str_who[2] = { "server", "mirror" };
int verbosity = 0;

/*
 *
 * Init
 *
 */

static void init_general_configuration(int argc, char **argv, cluster_conf_t *conf_clus)
{
	node_island_t *is;
	int i, j, k, found;
	char *p, *hl, *hs;

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
#if 0
	if (get_ear_conf_path(extra_buffer) == EAR_ERROR) {
		error("while getting ear.conf path");
	}

	vermast1("reading '%s' configuration file", extra_buffer);
	read_cluster_conf(extra_buffer, conf_clus);

	// Database
	init_db_helper(&conf_clus->database);

	// Mirror finding
	hl = master_host;  // Long host pointer
	hs = extra_buffer; // Short host pointer

	gethostname(hl, SZ_NAME_MEDIUM);
	strncpy(hs, hl, SZ_NAME_MEDIUM);
	found = 0;

	// Finding the short form 
	if ((p = strchr(hs, '.')) != NULL) {
		p[0] = '\0';
	}

	for (i = 0; i < conf_clus->num_islands && !found; ++i)
	{
		is = &conf_clus->islands[i];
		//vermast1("i %d", i);

		for (k = 0; k < is->num_ranges && !found; k++)
		{
			p = is->db_ips[is->ranges[k].db_ip];
			//vermast1("r %s", p);

			if (!server_too && p != NULL &&
				((strncmp(p, hl, strlen(hl)) == 0) || (strncmp(p, hs, strlen(hs) == 0))))
			{
				//vermast1("db_ip %s", p);
				server_too = 1;
			}

        	if (!mirror_too && is->ranges[k].sec_ip >= 0)
			{
				p = is->backup_ips[is->ranges[k].sec_ip];

				if (p != NULL &&
					((strncmp(p, hl, strlen(hl)) == 0) || (strncmp(p, hs, strlen(hs) == 0))))
				{
					//vermast1("db_ip_sec %s", p);
					strcpy(server_host, is->db_ips[is->ranges[k].db_ip]);
					mirror_too = 1;
				}
			}

			found = server_too && mirror_too;
		}
	}
#else
	conf_clus->db_manager.tcp_port      = 4811;
	conf_clus->db_manager.sec_tcp_port  = 4812;
	conf_clus->db_manager.sync_tcp_port = 4813;
	conf_clus->db_manager.mem_size      = 1;
	conf_clus->db_manager.aggr_time     = 60;
	conf_clus->db_manager.insr_time     = atoi(argv[4]);

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

	// PID protection
	process_data_initialize(&proc_data_srv, "eardbd_server", "/home/xgomez/Desktop/git/EAR");
	process_data_initialize(&proc_data_mir, "eardbd_mirror", "/home/xgomez/Desktop/git/EAR");

	int server_xst = process_exists(&proc_data_srv);
	int mirror_xst = process_exists(&proc_data_mir);

	server_too = server_too && !server_xst;
	mirror_too = mirror_too && !mirror_xst;

	if (!server_xst) process_pid_file_clean(&proc_data_srv);
	if (!mirror_xst) process_pid_file_clean(&proc_data_mir);

	// Server & mirro verbosity
	vermast1("enabled cache server: %s",      server_too ? "OK": "NO");
	vermast1("enabled cache mirror: %s (%s)", mirror_too ? "OK" : "NO", server_host);

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
		vermast1("insert time can't be 0, using 300 seconds (default)");
		time_insr = 300;
	}
	if (time_aggr == 0) {
		vermast1("aggregation time can't be 0, using 60 seconds (default)");
		time_aggr = 60;
	}

	// Looking for multiple value
	time_reset_timeout_insr(0);
	time_reset_timeout_aggr();
	time_reset_timeout_slct();

	// Times verbosity
	vermast1("insertion time:   %lu seconds", time_insr);
	vermast1("aggregation time: %lu seconds", time_aggr);
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

	//
	sockets_nonblock_set(smets_srv->fd);
	sockets_nonblock_set(ssync_srv->fd);
	sockets_nonblock_set(smets_mir->fd);

	// Verbosity
	char *str_sta[2] = { "listen", "closed" };

	int fd1 = smets_srv->fd;
	int fd2 = smets_mir->fd;
	int fd3 = ssync_srv->fd;
	int fd4 = ssync_mir->fd;

	// Summary
	tprintf_init(stderr, STR_MODE_DEF, "18 8 7 10 8");

	tprintf("type||port||prot||stat||fd");
	tprintf("----||----||----||----||--");
	tprintf("server metrics||%d||TCP||%s||%d", smets_srv->port, str_sta[fd1 == -1], fd1);
	tprintf("mirror metrics||%d||TCP||%s||%d", smets_mir->port, str_sta[fd2 == -1], fd2);
	tprintf("server sync||%d||TCP||%s||%d", ssync_srv->port, str_sta[fd3 == -1], fd3);
	tprintf("mirror sync||%d||TCP||%s||%d", ssync_mir->port, str_sta[fd4 == -1], fd4);
	vermast1("TIP! mirror sync socket opens and closes intermittently");
}

static void init_fork(int argc, char **argv, cluster_conf_t *conf_clus)
{
	state_t state;

	// Fork
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

	vermast1("cache server pid: %d %s", server_pid, str_sta[server_too]);
	vermast1("cache mirror pid: %d", mirror_pid);
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

static void init_sockets_server(int argc, char **argv, cluster_conf_t *conf_clus)
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
	sigdelset(&set, SIGUSR1);
	sigdelset(&set, SIGUSR2);
	sigdelset(&set, SIGTERM);
	sigdelset(&set, SIGCHLD);
	sigdelset(&set, SIGINT);
	sigdelset(&set, SIGHUP);

	sigprocmask(SIG_SETMASK, &set, NULL);

	// Editing signals individually
	sigfillset(&action.sa_mask);

	action.sa_handler = NULL;
	action.sa_sigaction = signal_handler;
	action.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &action, NULL) < 0) { 
        verwho1("sigaction error on signal %d (%s)", SIGUSR1, strerror(errno));
    }
	if (sigaction(SIGUSR2, &action, NULL) < 0) {
		verwho1("sigaction error on signal %d (%s)", SIGUSR1, strerror(errno));
	}
	if (sigaction(SIGCHLD, &action, NULL) < 0) {
		verwho1("sigaction error on signal %d (%s)", SIGUSR1, strerror(errno));
	}
	if (sigaction(SIGTERM, &action, NULL) < 0) {
		verwho1("sigaction error on signal %d (%s)", SIGTERM, strerror(errno));
	}
	if (sigaction(SIGINT, &action, NULL) < 0) {
		verwho1("sigaction error on signal %d (%s)", SIGINT, strerror(errno));
	}
	if (sigaction(SIGHUP, &action, NULL) < 0) {
		verwho1("sigaction error on signal %d (%s)", SIGHUP, strerror(errno));
	}
}

static void init_types(int argc, char **argv, cluster_conf_t *conf_clus)
{
	// Types
	sam_iname[i_aggrs] = "energy aggrs";
	sam_iname[i_enrgy] = "energy reps";
	sam_iname[i_appsm] = "apps mpi";
	sam_iname[i_appsn] = "apps non-mpi";
	sam_iname[i_appsl] = "apps learn.";
	sam_iname[i_evnts] = "events";
	sam_iname[i_loops] = "loops";

	typ_sizof[i_aggrs] = sizeof(periodic_aggregation_t);
	typ_sizof[i_enrgy] = sizeof(periodic_metric_t);
	typ_sizof[i_appsm] = sizeof(application_t);
	typ_sizof[i_appsn] = sizeof(application_t);
	typ_sizof[i_appsl] = sizeof(application_t);
	typ_sizof[i_evnts] = sizeof(ear_event_t);
	typ_sizof[i_loops] = sizeof(loop_t);

	typ_prcnt[i_appsm] = 40;
	typ_prcnt[i_appsn] = 20;
	typ_prcnt[i_appsl] = 05;
	typ_prcnt[i_loops] = 24;
	typ_prcnt[i_enrgy] = 05;
	typ_prcnt[i_aggrs] = 01;
	typ_prcnt[i_evnts] = 05;
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
	updating  = 0;

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
		init_sockets_server(argc, argv, conf_clus);
	}

	for (i = 0; i < EARDBD_TYPES; ++i) {
		per_total += conf_clus->db_manager.mem_size_types[i];
	}

	if (per_total > 90 && per_total < 110) {
		typ_prcnt[i_appsm] = conf_clus->db_manager.mem_size_types[0];
		typ_prcnt[i_appsn] = conf_clus->db_manager.mem_size_types[1];
		typ_prcnt[i_appsl] = conf_clus->db_manager.mem_size_types[2];
		typ_prcnt[i_loops] = conf_clus->db_manager.mem_size_types[3];
		typ_prcnt[i_enrgy] = conf_clus->db_manager.mem_size_types[4];
		typ_prcnt[i_aggrs] = conf_clus->db_manager.mem_size_types[5];
		typ_prcnt[i_evnts] = conf_clus->db_manager.mem_size_types[6];
	}

	float lpc_appsm = ((float) typ_prcnt[i_appsm]) / 100.0;
	float lpc_appsn = ((float) typ_prcnt[i_appsn]) / 100.0;
	float lpc_appsl = ((float) typ_prcnt[i_appsl]) / 100.0;
	float lpc_loops = ((float) typ_prcnt[i_loops]) / 100.0;
	float lpc_enrgy = ((float) typ_prcnt[i_enrgy]) / 100.0;
	float lpc_aggrs = ((float) typ_prcnt[i_aggrs]) / 100.0;
	float lpc_evnts = ((float) typ_prcnt[i_evnts]) / 100.0;

	// Data allocation
	sam_inmax[i_aggrs] = get_allocation_elements(lpc_aggrs, typ_sizof[i_aggrs]);
	sam_inmax[i_appsl] = get_allocation_elements(lpc_appsl, typ_sizof[i_appsm]);
	sam_inmax[i_appsm] = get_allocation_elements(lpc_appsm, typ_sizof[i_appsm]);
	sam_inmax[i_appsn] = get_allocation_elements(lpc_appsn, typ_sizof[i_appsm]);
	sam_inmax[i_enrgy] = get_allocation_elements(lpc_enrgy, typ_sizof[i_enrgy]);
	sam_inmax[i_evnts] = get_allocation_elements(lpc_evnts, typ_sizof[i_evnts]);
	sam_inmax[i_loops] = get_allocation_elements(lpc_loops, typ_sizof[i_loops]);

	// Raw data
	ulong b_aggrs = typ_sizof[i_aggrs] * sam_inmax[i_aggrs];
	ulong b_enrgy = typ_sizof[i_enrgy] * sam_inmax[i_enrgy];
	ulong b_appsl = typ_sizof[i_appsm] * sam_inmax[i_appsl];
	ulong b_appsm = typ_sizof[i_appsm] * sam_inmax[i_appsm];
	ulong b_appsn = typ_sizof[i_appsm] * sam_inmax[i_appsn];
	ulong b_evnts = typ_sizof[i_evnts] * sam_inmax[i_evnts];
	ulong b_loops = typ_sizof[i_loops] * sam_inmax[i_loops];

	typ_alloc[i_appsm] = malloc(b_appsm);
	typ_alloc[i_appsn] = malloc(b_appsn);
	typ_alloc[i_appsl] = malloc(b_appsl);
	typ_alloc[i_loops] = malloc(b_loops);
	typ_alloc[i_enrgy] = malloc(b_enrgy);
	typ_alloc[i_aggrs] = malloc(b_aggrs);
	typ_alloc[i_evnts] = malloc(b_evnts);

	memset(typ_alloc[i_appsm], 0, b_appsm);
	memset(typ_alloc[i_appsn], 0, b_appsn);
	memset(typ_alloc[i_appsl], 0, b_appsl);
	memset(typ_alloc[i_loops], 0, b_loops);
	memset(typ_alloc[i_enrgy], 0, b_enrgy);
	memset(typ_alloc[i_aggrs], 0, b_aggrs);
	memset(typ_alloc[i_evnts], 0, b_evnts);

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

	tprintf_init(stderr, STR_MODE_DEF, "15 15 11 9 8");

	// Summary
	tprintf("type||memory||sample||elems||%%");
	tprintf("----||------||------||-----||----");

	tprintf("mpi apps||%0.2f MBs||%lu Bs||%lu||%0.2f",
			mb_appsm, typ_sizof[i_appsm], sam_inmax[i_appsm], lpc_appsm);
	tprintf("non-mpi apps||%0.2f MBs||%lu Bs||%lu||%0.2f",
			mb_appsn, typ_sizof[i_appsm], sam_inmax[i_appsn], lpc_appsn);
	tprintf("learn apps||%0.2f MBs||%lu Bs||%lu||%0.2f",
			mb_appsl, typ_sizof[i_appsm], sam_inmax[i_appsl], lpc_appsl);
	tprintf("pwr metrics||%0.2f MBs||%lu Bs||%lu||%0.2f",
			mb_enrgy, typ_sizof[i_enrgy], sam_inmax[i_enrgy], lpc_enrgy);
	tprintf("app loops||%0.2f MBs||%lu Bs||%lu||%0.2f",
			mb_loops, typ_sizof[i_loops], sam_inmax[i_loops], lpc_loops);
	tprintf("events||%0.2f MBs||%lu Bs||%lu||%0.2f",
			mb_evnts, typ_sizof[i_evnts], sam_inmax[i_evnts], lpc_evnts);
	tprintf("aggregations||%0.2f MBs||%lu Bs||%lu||%0.2f",
			mb_aggrs, typ_sizof[i_aggrs], sam_inmax[i_aggrs], lpc_aggrs);

	tprintf("TOTAL||%0.2f MBs", mb_total);
	vermast1("TIP! this allocated space is per process server/mirror");
}

static void init_pid_files(int argc, char **argv)
{
	// Process PID save file
	if (server_iam && server_too) {
		process_pid_file_save(&proc_data_srv);
	}

	if (mirror_iam && mirror_too) {
		process_update_pid(&proc_data_mir);
		process_pid_file_save(&proc_data_mir);
	}
}

/*
 *
 * Release
 *
 */

void release()
{
	// Process PID cleaning
	if (server_iam) process_pid_file_clean(&proc_data_srv);
	if (mirror_iam) process_pid_file_clean(&proc_data_mir);
}

/*
 *
 *
 *
 */

void usage(int argc, char **argv)
{
}

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
		verline1("phase 2: time configuration");
		init_time_configuration(argc, argv, &conf_clus);

		//
		verline1("phase 3: sockets initialization");
		init_sockets(argc, argv, &conf_clus);

		//
		verline1("phase 4: processes fork");
		init_fork(argc, argv, &conf_clus);

		//
		verline1("phase 5: types initialization");
		init_types(argc, argv, &conf_clus);

		//
		verline1("phase 6: process configuration & allocation");
		init_process_configuration(argc, argv, &conf_clus);

		//
		verline1("phase 7: PID files creation");
		init_pid_files(argc, argv);

		//
		verline1("phase 8: listening (processing every %lu s)", time_insr);
		body();
	}

	//
	release();

	return 0;
}
