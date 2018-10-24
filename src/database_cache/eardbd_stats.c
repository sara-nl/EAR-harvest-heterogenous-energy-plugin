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

#include <stdlib.h>
#include <database_cache/eardbd.h>

extern time_t glb_time1[MAX_TYPES];
extern time_t glb_time2[MAX_TYPES];
extern time_t ins_time1[MAX_TYPES];
extern time_t ins_time2[MAX_TYPES];
extern size_t typ_sizof[MAX_TYPES];
extern char  *sam_iname[MAX_TYPES];
extern uint   sam_index[MAX_TYPES];
extern ulong  sam_inmax[MAX_TYPES];
extern uint   sam_recvd[MAX_TYPES];
extern uint   soc_accpt;
extern uint   soc_discn;
extern uint   soc_unkwn;
extern uint   soc_tmout;

/*
 *
 *
 *
 */

void stats_print()
{
	float gtime;
	float itime;
	float alloc;
	float prcnt;
	float block;
	int i;

	verline0();

	//
	tprintf_init(stderr, "15 13 9 10 10");

	//
	tprintf("sample||recv/alloc||%%||t. insr||t. recv");
	tprintf("----||----------||--||-------||-------");

	for (i = 0; i < MAX_TYPES; ++i)
	{
		itime = ((float) difftime(ins_time2[i], ins_time1[i]));
		gtime = ((float) difftime(glb_time2[i], glb_time1[i]));
		alloc = ((float) (typ_sizof[i] * sam_inmax[i]) / 1000000.0);
		prcnt = ((float) (sam_index[i]) / (float) (sam_inmax[i])) * 100.0f;
		block = ((float) (sam_index[i] * typ_sizof[i]) / 1000000.0);

		tprintf("%s||%u/%lu||%2.2f||%0.2fs||%0.2fs",
				sam_iname[i], sam_index[i], sam_inmax[i],
				prcnt, itime, gtime);
	}

	tprintf_init(stderr, "15 13 9 10 10");

	// Unknowns
	//tprintf("unknown||%u/--||--||--||--", soc_unkwn);
	//tprintf("timeout||%u/--||--||--||--", soc_tmout);

	//
	fprintf(stderr, "--\n");
	fprintf(stderr, "accept. sockets: %u\n", soc_accpt);
	fprintf(stderr, "discon. sockets: %u\n", soc_discn);
	fprintf(stderr, "timeout sockets: %u\n", soc_tmout);
	fprintf(stderr, "unknown samples: %u\n", soc_tmout);

	verline0();
}

void stats_reset()
{
	int i;

	for (i = 0; i < MAX_TYPES; ++i)
	{
		glb_time1[i] = 0;
		glb_time2[i] = 0;
		ins_time1[i] = 0;
		ins_time2[i] = 0;
		sam_recvd[i] = 0;
		sam_index[i] = 0;
	}

	soc_accpt = 0;
	soc_discn = 0;
	soc_unkwn = 0;
	soc_tmout = 0;
}

/*
 *
 * Insert
 *
 */

void stats_account_insert_start(uint i)
{
	time(&ins_time1[i]);
}

void stats_account_insert_stop(uint i, uint max)
{
	//
	time(&ins_time2[i]);
	sam_index[i] = max;
}

/*
 *
 * Type managing
 *
 */

static int stats_type_extract(packet_header_t *header, char *content)
{
	application_t *app;
	uint type;

	//
	type = header->content_type;

	if (type != CONTENT_TYPE_APM) {
		return type;
	}

	//
	app = (application_t *) content;

	if (app->is_learning) {
		return CONTENT_TYPE_APL;
	} else if (app->is_mpi) {
		return CONTENT_TYPE_APM;
	} else {
		return CONTENT_TYPE_APN;
	}
}

static int stats_type_index(uint type, char *name)
{
	switch (type)
	{
		case CONTENT_TYPE_APM:
			name = sam_iname[i_appsm];
			return i_appsm;
		case CONTENT_TYPE_APN:
			name = sam_iname[i_appsn];
			return i_appsn;
		case CONTENT_TYPE_APL:
			name = sam_iname[i_appsl];
			return i_appsl;
		case CONTENT_TYPE_PER:
			name = sam_iname[i_enrgy];
			return i_enrgy;
		case CONTENT_TYPE_EVE:
			name = sam_iname[i_evnts];
			return i_evnts;
		case CONTENT_TYPE_LOO:
			name = sam_iname[i_loops];
			return i_loops;
		case CONTENT_TYPE_AGG:
			name = sam_iname[i_aggrs];
			return i_aggrs;
		case CONTENT_TYPE_QST:
			name = "sync_question";
			return -1;
		case CONTENT_TYPE_ANS:
			name = "sync_answer";
			return -1;
		case CONTENT_TYPE_PIN:
			name = "ping";
			return -1;
		default:
			name = "unknown";
			return -1;
	}
}

/*
 *
 *
 *
 */

void stats_sample_account(int fd, packet_header_t *header, char *content)
{
	time_t aux;
	char *name;
	uint index;
	uint print;
	uint type;

	type  = stats_type_extract(header, content);
	index = stats_type_index(type, name);

	//
	if (sam_recvd[index] == 0)
	{
		time(&glb_time1[index]);
	}

	//
	time(&glb_time2[index]);

	//
	sam_recvd[index] += 1;
	
	/*if (print)
	{
		sockets_get_address_fd(fd, extra_buffer, SZ_NAME_MEDIUM);

		verwho1("received '%s' packet from host '%s' (socket: %d)",
				type, extra_buffer, fd);
	}*/
}