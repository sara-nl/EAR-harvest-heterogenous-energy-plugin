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

#include <database_cache/eardbd.h>

//
extern int master_iam; // Master is who speaks
extern int server_iam;
extern int mirror_iam;

// Sockets
extern socket_t *smets_srv;
extern socket_t *smets_mir;
extern socket_t *ssync_srv;
extern socket_t *ssync_mir;


// Synchronization
extern packet_header_t sync_ans_header;
extern packet_header_t sync_qst_header;
extern sync_qst_t sync_qst_content;
extern sync_ans_t sync_ans_content;

//
extern struct timeval timeout_insr;
extern struct timeval timeout_aggr;
extern struct timeval timeout_slct;
extern time_t time_insr;
extern time_t time_aggr;
extern time_t time_slct;

//
extern time_t glb_time1[MAX_TYPES];
extern time_t glb_time2[MAX_TYPES];
extern time_t ins_time1[MAX_TYPES];
extern time_t ins_time2[MAX_TYPES];
extern size_t typ_sizof[MAX_TYPES];
extern uint   sam_index[MAX_TYPES];
extern char  *sam_iname[MAX_TYPES];
extern ulong  sam_inmax[MAX_TYPES];
extern uint   sam_recvd[MAX_TYPES];
extern uint   soc_accpt;
extern uint   soc_discn;
extern uint   soc_unkwn;
extern uint   soc_tmout;

// Strings
extern char *str_who[2];
extern int verbosity0;

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



/*
 *
 * Synchronization main/mirror
 *
 */

int sync_question(uint sync_option)
{
	time_t timeout_old;
	state_t s;

	verwho1("synchronization started: asking the question (%d)", sync_option);
	sync_qst_content.sync_option = sync_option;

	// Preparing packet
	sockets_header_update(&sync_qst_header);

	// Synchronization pipeline
	s = sockets_socket(ssync_mir);

	if (state_fail(s)) {
		verwho1("failed to create client socket to MAIN (%d, inum: %d, istr: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_connect(ssync_mir);

	if (state_fail(s)) {
		verwho1("failed to connect to MAIN (%d, inum: %d, istr: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_send(ssync_mir, &sync_qst_header, (char *) &sync_qst_content);

	if (state_fail(s)) {
		verwho1("failed to send to MAIN (%d, num: %d, str: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	// Setting new timeout
	sockets_timeout_get(ssync_mir->fd, &timeout_old);
	sockets_timeout_set(ssync_mir->fd, 10);

	// Transferring
	s = sockets_receive(ssync_mir->fd, &sync_ans_header,
						(char *) &sync_ans_content, sizeof(sync_ans_t), 1);

	// Recovering old timeout
	sockets_timeout_set(ssync_mir->fd, timeout_old);

	if (state_fail(s)) {
		verwho1("failed to receive from MAIN (%d, num: %d, str: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	s = sockets_close(ssync_mir);

	verwho0("synchronization completed correctly");

	return EAR_SUCCESS;
}

int sync_answer(int fd)
{
	socket_t sync_ans_socket;
	state_t s;

	verwho0("synchronization started: answering the question");

	// Socket
	sockets_clean(&sync_ans_socket);
	sync_ans_socket.protocol = TCP;
	sync_ans_socket.fd = fd;

	// Header
	sockets_header_update(&sync_ans_header);

	s = sockets_send(&sync_ans_socket, &sync_ans_header, (char *) &sync_ans_content);

	if (state_fail(s)) {
		verwho1("Failed to send to MIRROR (%d, num: %d, str: %s)",
				s, intern_error_num, intern_error_str);
		return EAR_ERROR;
	}

	verwho0("synchronization completed correctly");

	return EAR_SUCCESS;
}
