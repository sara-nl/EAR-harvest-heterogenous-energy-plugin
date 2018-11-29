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
#include <database_cache/eardbd.h>
#include <database_cache/eardbd_body.h>
#include <database_cache/eardbd_sync.h>
#include <database_cache/eardbd_signals.h>
#include <database_cache/eardbd_storage.h>

// Configuration
extern cluster_conf_t conf_clus;

// Buffers
extern packet_header_t input_header;
extern char input_buffer[SZ_BUFF_BIG];
extern char extra_buffer[SZ_BUFF_BIG];

// Sockets
extern socket_t *smets_srv;
extern socket_t *smets_mir;
extern socket_t *ssync_srv;
extern socket_t *ssync_mir;

// Descriptors
extern struct sockaddr_storage addr_cli;
extern fd_set fds_metr_tcp;
extern fd_set fds_sync_tcp;
extern fd_set fds_incoming;
extern fd_set fds_incoming;
extern fd_set fds_active;
extern int fd_cli;
extern int fd_min;
extern int fd_max;

// Mirroring
extern process_data_t proc_data_srv;
extern process_data_t proc_data_mir;
extern pid_t server_pid;
extern pid_t mirror_pid;
extern pid_t others_pid;
extern int master_iam; // Master is who speaks
extern int server_iam;
extern int mirror_iam;
extern int server_too;
extern int mirror_too;

// Data
extern time_t glb_time1[MAX_TYPES];
extern time_t glb_time2[MAX_TYPES];
extern time_t ins_time1[MAX_TYPES];
extern time_t ins_time2[MAX_TYPES];
extern size_t typ_sizof[MAX_TYPES];
extern uint   typ_prcnt[MAX_TYPES];
extern uint   typ_index[MAX_TYPES];
extern char  *typ_alloc[MAX_TYPES];
extern char  *sam_iname[MAX_TYPES];
extern ulong  sam_inmax[MAX_TYPES];
extern ulong  sam_index[MAX_TYPES];
extern uint   sam_recvd[MAX_TYPES];
extern uint   soc_accpt;
extern uint   soc_discn;
extern uint   soc_unkwn;
extern uint   soc_tmout;

// Times (alarms)
extern struct timeval timeout_insr;
extern struct timeval timeout_aggr;
extern struct timeval timeout_slct;
extern time_t time_insr;
extern time_t time_aggr;
extern time_t time_slct;

// State machine
extern int reconfiguring;
extern int listening;
extern int releasing;
extern int exitting;
extern int updating;
extern int dreaming;
extern int veteran;
extern int forked;

// Extras
extern sigset_t sigset;

// Verbosity
extern char *str_who[2];
extern int verbosity;

/*
 *
 *
 *
 */

static void body_alarm(struct timeval *timeout_slct)
{
	if (timeout_slct->tv_sec == 0 && timeout_slct->tv_usec == 0)
	{
		time_substract_timeouts();

		if (timeout_aggr.tv_sec == 0)
		{
			peraggr_t *p = (peraggr_t *) typ_alloc[i_aggrs];
			peraggr_t *q = (peraggr_t *) &p[sam_index[i_aggrs]];

			verwho1("completed the aggregation number %lu with energy %lu",
					sam_index[i_aggrs], q->DC_energy);

			// Aggregation time done, so new aggregation incoming
			storage_sample_add(NULL, sam_inmax[i_aggrs], &sam_index[i_aggrs], NULL, 0, SYNC_AGGRS);

			// Initializing the new element
			q = (peraggr_t *) &p[sam_index[i_aggrs]];

			if (sam_inmax[i_aggrs] > 0) {
				init_periodic_aggregation(q);
			}

			//
			time_reset_timeout_aggr();
		}

		if (timeout_insr.tv_sec == 0)
		{
			// Synchronizing with the MAIN
			if (mirror_iam)
			{
				sync_ans_t answer;

				//TODO: MPKFA
				// Asking the question

				// In case of fail the mirror have to insert the data
				if(state_fail(sync_question(SYNC_ALL, veteran, &answer)))
				{
					insert_hub(SYNC_ALL, RES_TIME);
				// In case I'm veteran and the server is not
				} else if (!answer.veteran && veteran)
				{
					insert_hub(SYNC_ALL, RES_TIME);
				// In case of the answer is received just clear the data
				} else {
					reset_all();
				}
			} else {
				// Server normal insertion
				insert_hub(SYNC_ALL, RES_TIME);
			}

			// If server the time reset is do it after the insert
			time_reset_timeout_insr(0);

			// I'm a veteran
			veteran = 1;
		}

		time_reset_timeout_slct();
	}
}

static int body_new_connection(int fd)
{
	int nc;

	nc  = !mirror_iam && (fd == smets_srv->fd || fd == ssync_srv->fd);
	nc |=  mirror_iam && (fd == smets_mir->fd);

	return nc;
}

static void body_connections()
{
	state_t s;
	int i;

	for(i = fd_min; i <= fd_max && listening && !updating; i++)
	{
		if (listening && FD_ISSET(i, &fds_incoming)) // we got one!!
		{
			// Handle new connections (just for TCP)
			if (body_new_connection(i))
			{
				do {
					s = sockets_accept(i, &fd_cli, &addr_cli);

					if (state_ok(s))
					{
						FD_SET(fd_cli, &fds_active);

						if (fd_cli > fd_max) {
							fd_max = fd_cli;
						}
						if (fd_cli < fd_min) {
							fd_min = fd_cli;
						}

						soc_accpt += 1;
					}
				} while(state_ok(s));
				// Handle data transfers
			}
			else
			{
				s = sockets_receive(i, &input_header, input_buffer, sizeof(input_buffer), 0);

				if (state_ok(s))
				{
					//
					storage_sample_receive(i, &input_header, input_buffer);
				}
				else
				{
					if (state_is(s, EAR_SOCK_DISCONNECTED)) {
						//verwho1("disconnected from socket %d (num: %d, str: %s)",
						//		i, intern_error_num, intern_error_str);
						soc_discn += 1;
					} if (state_is(s, EAR_SOCK_TIMEOUT)) {
						sockets_get_address_fd(i, extra_buffer, SZ_BUFF_BIG);
						//verwho1("PANIC, disconnected from socket %d and node %s (num: %d, str: %s)",
						//		i, extra_buffer, intern_error_num, intern_error_str);
						soc_tmout += 1;
					} else {
						//verwho1("on reception (num: %d, str: %s), disconnecting from socket %d",
						//		intern_error_num, intern_error_str, i);
						soc_unkwn += 1;
					}

					sockets_close_fd(i);
					FD_CLR(i, &fds_active);
				}
			}
		} // FD_ISSET
	}
}

static void body_insert()
{

}

void body()
{
	int s;

	if (listening) {
		printml1("phase 7: listening (processing every %lu s)", time_insr);
	}

	// BODY
	while(listening)
	{
		fds_incoming = fds_active;

		if ((s = select(fd_max + 1, &fds_incoming, NULL, NULL, &timeout_slct)) == -1)
		{
			if (listening && !updating)
			{
				error("during select (%s)", strerror(errno));
			}
		}

		body_alarm(&timeout_slct);

		body_connections();

		body_insert();

		updating = 0;
	}
}

void dream()
{
	while (dreaming) {
		sigsuspend(&sigset);
	}
}

void release()
{
	int i;

	// Socket closing
	for(i = fd_max; i >= fd_min && !listening; --i) {
		close(i);
	}

	// Cleaning sockets
	sockets_dispose(smets_srv);
	sockets_dispose(smets_mir);
	sockets_dispose(ssync_srv);
	sockets_dispose(ssync_mir);

	// Freeing data
	free_cluster_conf(&conf_clus);

	for (i = 0; i < MAX_TYPES; ++i)
	{
		if (typ_alloc[i] != NULL)
		{
			free(typ_alloc[i]);
			typ_alloc[i] = NULL;
		}
	}

	// Reconfiguring
	if (reconfiguring)
	{
		sleep(5);
		reconfiguring = 0;
	}

	// Process PID cleaning
	if (server_iam && server_too) {
		process_pid_file_clean(&proc_data_srv);
	}
	if (mirror_iam && mirror_too) {
		process_pid_file_clean(&proc_data_mir);
	}

	// End releasing
	releasing = 0;
}
