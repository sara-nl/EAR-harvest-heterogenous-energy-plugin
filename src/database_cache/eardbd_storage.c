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
#include <time.h>
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
#include <database_cache/eardbd_body.h>
#include <database_cache/eardbd_sync.h>
#include <database_cache/eardbd_signals.h>
#include <database_cache/eardbd_storage.h>

// Buffers
extern char input_buffer[SZ_BUFF_BIG];
extern char extra_buffer[SZ_BUFF_BIG];

// Mirroring
extern int master_iam; // Master is who speaks
extern int server_iam;
extern int mirror_iam;

// Time
extern struct timeval timeout_slct;
extern time_t time_slct;

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

// State machine
extern int reconfiguring;
extern int listening;
extern int releasing;
extern int exitting;
extern int updating;
extern int dreaming;
extern int veteran;
extern int forked;


// Verbosity
extern char *str_who[2];
extern int verbosity;

/*
 *
 * Time
 *
 */

void metrics_insert_start(uint i)
{
	time(&ins_time1[i]);
}

void metrics_insert_stop(uint i, ulong max)
{
	time(&ins_time2[i]);
}

void metrics_print()
{
	float gtime;
	float itime;
	float alloc;
	float prcnt;
	float block;
	int i;

	printl0();

	//
	tprintf_init(stderr, STR_MODE_DEF, "15 13 9 10 10");

	//
	tprintf("sample (%d)||recv/alloc||%%||t. insr||t. recv", mirror_iam);
	tprintf("-----------||----------||--||-------||-------");

	for (i = 0; i < MAX_TYPES; ++i)
	{
		itime = ((float) difftime(ins_time2[i], ins_time1[i]));
		gtime = ((float) difftime(glb_time2[i], glb_time1[i]));
		alloc = ((float) (typ_sizof[i] * sam_inmax[i]) / 1000000.0);
		block = ((float) (sam_recvd[i] * typ_sizof[i]) / 1000000.0);
		prcnt = 0.0f;

		if (sam_inmax[i] > 0.0f) {
			prcnt = ((float) (sam_recvd[i]) / (float) (sam_inmax[i])) * 100.0f;
		}

		tprintf("%s||%lu/%lu||%2.2f||%0.2fs||%0.2fs",
				sam_iname[i], sam_index[i], sam_inmax[i],
				prcnt, itime, gtime);
	}

	//
	fprintf(stderr, "--\n");
	fprintf(stderr, "accept. sockets: %u\n", soc_accpt);
	fprintf(stderr, "discon. sockets: %u\n", soc_discn);
	fprintf(stderr, "timeout sockets: %u\n", soc_tmout);
	fprintf(stderr, "unknown samples: %u\n", soc_tmout);

	printl0();

	soc_accpt = 0;
	soc_discn = 0;
	soc_unkwn = 0;
	soc_tmout = 0;
}

/*
 *
 * Reset
 *
 */

static void reset_aggregations()
{
	peraggr_t *p = (peraggr_t *) typ_alloc[i_aggrs];
	peraggr_t  q = (peraggr_t  ) p[sam_index[i_aggrs]];

	if (sam_index[i_aggrs] < sam_inmax[i_aggrs] && q.n_samples > 0)
	{
		memcpy (p, &q, sizeof(periodic_aggregation_t));
    } else {
        init_periodic_aggregation(p);
    }

    sam_index[i_aggrs] = 0;
}

static void reset_index(int index)
{
	glb_time1[index] = 0;
	glb_time2[index] = 0;
	ins_time1[index] = 0;
	ins_time2[index] = 0;
	sam_recvd[index] = 0;
	sam_index[index] = 0;
}

void reset_all()
{
	int i;

	// Specific resets
	reset_aggregations();

	// Generic reset
	for (i = 0; i < MAX_TYPES; ++i)
	{
		reset_index(i);
	}
}

/*
 *
 * Insert
 *
 */
#if 0
	#define db_batch_insert_applications(a, b);
	#define db_batch_insert_applications_no_mpi(a, b);
	#define db_batch_insert_applications_learning(a, b);
	#define db_batch_insert_loops(a, b);
	#define db_batch_insert_periodic_metrics(a, b);
	#define db_batch_insert_periodic_aggregations(a, b);
	#define db_batch_insert_ear_event(a, b);
#endif

static void insert_apps_mpi()
{
	if (typ_prcnt[i_appsm] == 0 || sam_index[i_appsm] <= 0) {
		return;
	}

	metrics_insert_start(i_appsm);
	db_batch_insert_applications(typ_alloc[i_appsm], sam_index[i_appsm]);
	metrics_insert_stop(i_appsm, sam_index[i_appsm]);
}

static void insert_apps_non_mpi()
{
	time_t time_start;

	if (typ_prcnt[i_appsn] == 0 || sam_index[i_appsn] <= 0) {
		return;
	}

	metrics_insert_start(i_appsn);
	db_batch_insert_applications_no_mpi(typ_alloc[i_appsn], sam_index[i_appsn]);
	metrics_insert_stop(i_appsn, sam_index[i_appsn]);
}

static void insert_apps_learning()
{
	time_t time_start;

	if (typ_prcnt[i_appsl] == 0 || sam_index[i_appsl] <= 0) {
		return;
	}

	metrics_insert_start(i_appsl);
	db_batch_insert_applications_learning(typ_alloc[i_appsl], sam_index[i_appsl]);
	metrics_insert_stop(i_appsl, sam_index[i_appsl]);
}

static void insert_loops()
{
	time_t time_start;

	if (typ_prcnt[i_loops] == 0 || sam_index[i_loops] <= 0) {
		return;
	}

	metrics_insert_start(i_loops);
	db_batch_insert_loops(typ_alloc[i_loops], sam_index[i_loops]);
	metrics_insert_stop(i_loops, sam_index[i_loops]);
}

static void insert_energy()
{
	time_t time_start;

	if (typ_prcnt[i_enrgy] == 0 || sam_index[i_enrgy] <= 0) {
		return;
	}

	metrics_insert_start(i_enrgy);
	db_batch_insert_periodic_metrics(typ_alloc[i_enrgy], sam_index[i_enrgy]);
	metrics_insert_stop(i_enrgy, sam_index[i_enrgy]);
}

static void insert_aggregations()
{
	time_t time_start;

	verwho1("Inserting aggregations %d",  sam_index[i_aggrs]);

	if (typ_prcnt[i_aggrs] == 0 || sam_index[i_aggrs] <= 0) {
		return;
	}

	metrics_insert_start(i_aggrs);
	db_batch_insert_periodic_aggregations(typ_alloc[i_aggrs], sam_index[i_aggrs]);
	metrics_insert_stop(i_aggrs, sam_index[i_aggrs]);
}

static void insert_events()
{
	time_t time_start;

	if (typ_prcnt[i_evnts] == 0 || sam_index[i_evnts] <= 0) {
		return;
	}

	metrics_insert_start(i_evnts);
	db_batch_insert_ear_event(typ_alloc[i_evnts], sam_index[i_evnts]);
	metrics_insert_stop(i_evnts, sam_index[i_evnts]);
}

void insert_hub(uint option, uint reason)
{
	verwho1("looking for possible DB insertion (type 0x%x, reason 0x%x)", option, reason);

	metrics_print();

	if (sync_option_m(option, SYNC_APPSM, SYNC_ALL))
	{
		//
		insert_apps_mpi();
		//
		reset_index(i_appsm);
	}
	if (sync_option_m(option, SYNC_APPSN, SYNC_ALL))
	{
		//
		insert_apps_non_mpi();
		//
		reset_index(i_appsn);
	}
	if (sync_option_m(option, SYNC_APPSL, SYNC_ALL))
	{
		//
		insert_apps_learning();
		//
		reset_index(i_appsl);
	}
	if (sync_option_m(option, SYNC_ENRGY, SYNC_ALL))
	{
		//
		insert_energy();
		//
		reset_index(i_enrgy);
	}
	if (sync_option_m(option, SYNC_AGGRS, SYNC_ALL))
	{
		//
		insert_aggregations();
		//
		reset_aggregations();
		reset_index(i_aggrs);
	}
	if (sync_option_m(option, SYNC_EVNTS, SYNC_ALL))
	{
		//
		insert_events();
		//
		reset_index(i_evnts);
	}
	if (sync_option_m(option, SYNC_LOOPS, SYNC_ALL))
	{
		//
		insert_loops();
		//
		reset_index(i_loops);
	}
}

/*
 *
 * Sample add
 *
 */

void storage_sample_add(char *buf, ulong len, ulong *idx, char *cnt, size_t siz, uint opt)
{
	if (len == 0) {
		return;
	}

	if (cnt != NULL) {
		memcpy (buf, cnt, siz);
	}

	*idx += 1;

	if (*idx == len)
	{
		if(server_iam) {
			insert_hub(opt, RES_OVER);
		} else if (state_fail(sync_question(opt, veteran, NULL))) {
			insert_hub(opt, RES_OVER);
		}
	}
}

/*
 *
 * Incoming data
 *
 */

static int storage_type_extract(packet_header_t *header, char *content)
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
	
	return -1;
}

static int storage_index_extract(int type, char **name)
{
	switch (type)
	{
		case CONTENT_TYPE_APM:
			*name = sam_iname[i_appsm];
			return i_appsm;
		case CONTENT_TYPE_APN:
			*name = sam_iname[i_appsn];
			return i_appsn;
		case CONTENT_TYPE_APL:
			*name = sam_iname[i_appsl];
			return i_appsl;
		case CONTENT_TYPE_PER:
			*name = sam_iname[i_enrgy];
			return i_enrgy;
		case CONTENT_TYPE_EVE:
			*name = sam_iname[i_evnts];
			return i_evnts;
		case CONTENT_TYPE_LOO:
			*name = sam_iname[i_loops];
			return i_loops;
		case CONTENT_TYPE_AGG:
			*name = sam_iname[i_aggrs];
			return i_aggrs;
		case CONTENT_TYPE_QST:
			*name = "sync_question";
			return -1;
		case CONTENT_TYPE_ANS:
			*name = "sync_answer";
			return -1;
		case CONTENT_TYPE_PIN:
			*name = "ping";
			return -1;
		default:
			*name = "unknown";
			return -1;
	}
}

void storage_sample_receive(int fd, packet_header_t *header, char *content)
{
	time_t aux;
	char *name;
	int index;
	int type;

	// Data extraction
	type = storage_type_extract(header, content);
	index = storage_index_extract(type, &name);

	fprintf(stderr, "RECEIVED %dt %di %s\n", type, index, name); 
	return;

	//TODO:
	// Currently, sam_index and sam_recv are the same. In the near future
	// a struct for every type has to be made. This type have to include
	// these things:
	//		- an index to the position of the empty value in the allocation
	//		- a maximum of the allocation
	//		- an ulong of the sizeof the type
	//		- a pointer to the allocation
	//		- a pointer to the MySQL insert function
	//		- a pointer to the string of the name of the type
	//		- a time_t of the insertion time
	//		- a value if the metric has overflow within a insert time
	//		- a value of the sync option

	// Storage
	if (type == CONTENT_TYPE_APM)
	{
		storage_sample_add((char *) &typ_alloc[i_appsm][sam_index[index]], sam_inmax[index],
		   &sam_index[index], content, typ_sizof[index], SYNC_APPSM);
	}
	else if (type == CONTENT_TYPE_APN)
	{
		storage_sample_add((char *) &typ_alloc[i_appsn][sam_index[index]], sam_inmax[index],
		   &sam_index[index], content, typ_sizof[index], SYNC_APPSN);
	}
	else if (type == CONTENT_TYPE_APL)
	{
		storage_sample_add((char *) &typ_alloc[i_appsl][sam_index[index]], sam_inmax[index],
			&sam_index[index], content, typ_sizof[index], SYNC_APPSL);
	}
	else if (type == CONTENT_TYPE_EVE)
	{
		storage_sample_add((char *) &typ_alloc[i_evnts][sam_index[index]], sam_inmax[index],
			&sam_index[index], content, typ_sizof[index], SYNC_EVNTS);
	}
	else if (type == CONTENT_TYPE_LOO)
	{
		storage_sample_add((char *) &typ_alloc[i_loops][sam_index[index]], sam_inmax[index],
			&sam_index[index], content, typ_sizof[index], SYNC_LOOPS);
	}
	else if (type == CONTENT_TYPE_PER)
	{
		peraggr_t *p = (peraggr_t *) typ_alloc[i_aggrs];
		peraggr_t *q = (peraggr_t *) &p[sam_index[i_aggrs]];
		periodic_metric_t *met = (periodic_metric_t *) content;

		// Verbosity mpkfa
		//if (verbosity == 1) {
		//	verwho1("RECEIVED %lu energy from %s", met->DC_energy, met->node_id);				
		//}

		// Add sample to the aggregation
		add_periodic_aggregation(q, met->DC_energy, met->start_time, met->end_time);

		// Add sample to the energy array
		storage_sample_add((char *) &typ_alloc[i_enrgy][sam_index[index]], sam_inmax[index],
			&sam_index[index], content, typ_sizof[index], SYNC_ENRGY);
	}
	else if (type == CONTENT_TYPE_QST)
	{
		sync_qst_t *question = (sync_qst_t *) content;

		if (veteran || !question->veteran) {
			//TODO: MPKFA
			// Passing the question option
			insert_hub(question->sync_option, RES_SYNC);
		}

		// Answering the mirror question
		sync_answer(fd, veteran);

		// In case it is a full sync the sync time is resetted before the answer
		// with a very small offset (5 second is enough)
		if (sync_option(question->sync_option, SYNC_ALL))
		{
			/// We get the timeout passed since the 'time_slct' got its value
			// and added to 'timeout_insr', because when 'time_slct' would be
			// substracted from both 'timeout_insr' and 'timeout_aggr', it
			// also be substracted the passed time.
			time_t timeout_passed = time_slct - timeout_slct.tv_sec;

			time_reset_timeout_insr(timeout_passed + 5);

			// I'm a veteran
			veteran = 1;
		}
	}

	// Metrics
	if (index == -1) {
		return;
	}
	
	if (sam_recvd[index] == 0)
	{
		time(&glb_time1[index]);
	}

	time(&glb_time2[index]);

	//
	sam_recvd[index] += 1;
}
