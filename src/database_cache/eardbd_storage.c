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

//
extern char input_buffer[SZ_BUFF_BIG];
extern char extra_buffer[SZ_BUFF_BIG];

// Mirroring
extern int master_iam; // Master is who speaks
extern int server_iam;
extern int mirror_iam;

// Pipeline
extern int forked;

// Time
extern struct timeval timeout_slct;
extern time_t time_slct;

// Storage
extern uint per_appsm;
extern uint per_appsn;
extern uint per_appsl;
extern uint per_loops;
extern uint per_enrgy;
extern uint per_aggrs;
extern uint per_evnts;

extern periodic_aggregation_t *aggrs;
extern periodic_metric_t *enrgy;
extern application_t *appsm;
extern application_t *appsn;
extern application_t *appsl;
extern ear_event_t *evnts;
extern loop_t *loops;

extern ulong max_aggrs;
extern ulong max_enrgy;
extern ulong max_appsm;
extern ulong max_appsn;
extern ulong max_appsl;
extern ulong max_evnts;
extern ulong max_loops;

// Metrics
extern time_t glb_time1[MAX_TYPES];
extern time_t glb_time2[MAX_TYPES];
extern time_t ins_time1[MAX_TYPES];
extern time_t ins_time2[MAX_TYPES];
extern size_t typ_sizof[MAX_TYPES];
extern char  *sam_iname[MAX_TYPES];
extern uint   sam_index[MAX_TYPES];
extern ulong  sam_inmax[MAX_TYPES];
extern uint   sam_recvd[MAX_TYPES];

// Verbosity
extern char *str_who[2];
extern int verbosity;

/*
 *
 * Reset
 *
 */

static void reset_aggregations()
{
	if (max_aggrs < sam_inmax[i_aggrs] && aggrs[max_aggrs].n_samples > 0) {
		memcpy (aggrs, &aggrs[max_aggrs], sizeof(periodic_aggregation_t));
    } else {
        init_periodic_aggregation(aggrs);
    }

    max_aggrs = 0;
}

void reset_indexes()
{
    max_appsm = 0;
    max_appsn = 0;
    max_appsl = 0;
    max_enrgy = 0;
    max_evnts = 0;
    max_loops = 0;
}

void reset_all()
{
	// Generic reset
	reset_indexes();

    // Specific resets
    reset_aggregations();
}

/*
 *
 * Insert
 *
 */
#if 0
	#define db_batch_insert_applications(a, b);
	#define db_batch_insert_applications_no_mpi(a, b);
	#define db_batch_insert_loops(a, b);
	#define db_batch_insert_periodic_metrics(a, b);
	#define db_batch_insert_periodic_aggregations(a, b);
	#define db_batch_insert_ear_event(a, b);
#endif

static void insert_apps_mpi()
{
	if (per_appsm == 0 || max_appsm <= 0) {
		return;
	}

	stats_account_insert_start(i_appsm);
	db_batch_insert_applications(appsm, max_appsm);
	stats_account_insert_stop(i_appsm, max_appsm);

	// Reset
	max_appsm = 0;
}

static void insert_apps_non_mpi()
{
	time_t time_start;

	if (per_appsn == 0 || max_appsn <= 0) {
		return;
	}

	stats_account_insert_start(i_appsn);
	db_batch_insert_applications_no_mpi(appsn, max_appsn);
	stats_account_insert_stop(i_appsn, max_appsn);

	// Reset
	max_appsn = 0;
}

static void insert_apps_learning()
{
	time_t time_start;

	if (per_appsl == 0 || max_appsl <= 0) {
		return;
	}

	stats_account_insert_start(i_appsl);
	//db_batch_insert_applications(appsl, max_appsl);
	db_batch_insert_applications_learning(appsl, max_appsl);

	stats_account_insert_stop(i_appsl, max_appsl);

	// Reset
	max_appsl = 0;
}

static void insert_loops()
{
	time_t time_start;

	if (per_loops == 0 || max_loops <= 0) {
		return;
	}

	stats_account_insert_start(i_loops);
	db_batch_insert_loops(loops, max_loops);
	stats_account_insert_stop(i_loops, max_loops);

	// Reset
	max_loops = 0;
}

static void insert_energy()
{
	time_t time_start;

	if (per_enrgy == 0 || max_enrgy <= 0) {
		return;
	}

	stats_account_insert_start(i_enrgy);
	db_batch_insert_periodic_metrics(enrgy, max_enrgy);
	stats_account_insert_stop(i_enrgy, max_enrgy);

	// Reset
	max_enrgy = 0;
}

static void insert_aggregations()
{
	time_t time_start;

	if (per_aggrs == 0 || max_aggrs <= 0) {
		return;
	}

	stats_account_insert_start(i_aggrs);
	db_batch_insert_periodic_aggregations(aggrs, max_aggrs);
	stats_account_insert_stop(i_aggrs, max_aggrs);

	// Reset
	reset_aggregations();
}

static void insert_events()
{
	time_t time_start;

	if (per_evnts == 0 || max_evnts <= 0) {
		return;
	}

	stats_account_insert_start(i_evnts);
	db_batch_insert_ear_event(evnts, max_evnts);
	stats_account_insert_stop(i_evnts, max_evnts);

	// Reset
	max_evnts = 0;
}

void insert_hub(uint option, uint reason)
{
	verwho1("looking for possible DB insertion (type 0x%x, reason 0x%x)", option, reason);

	if (sync_option_m(option, SYNC_APPSM, SYNC_ALL)) {
		insert_apps_mpi();
	}
	if (sync_option_m(option, SYNC_APPSN, SYNC_ALL)) {
		insert_apps_non_mpi();
	}
	if (sync_option_m(option, SYNC_APPSL, SYNC_ALL)) {
		insert_apps_learning();
	}
	if (sync_option_m(option, SYNC_ENRGY, SYNC_ALL)) {
		insert_energy();
	}
	if (sync_option_m(option, SYNC_AGGRS, SYNC_ALL)) {
		insert_aggregations();
	}
	if (sync_option_m(option, SYNC_EVNTS, SYNC_ALL)) {
		insert_events();
	}
	if (sync_option_m(option, SYNC_LOOPS, SYNC_ALL)) {
		insert_loops();
	}

	stats_print();
	stats_reset();
}

/*
 *
 * Storage management
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
		} else if (state_fail(sync_question(opt))) {
			insert_hub(opt, RES_OVER);
		}
	}
}

void storage_sample_receive(int fd, packet_header_t *header, char *content)
{
	state_t state;

	if (header->content_type == CONTENT_TYPE_APM)
	{
		application_t *app = (application_t *) content;

		if (app->is_learning)
		{
			storage_sample_add((char *) &appsl[max_appsl], sam_inmax[i_appsl], &max_appsl,
							   content, sizeof(application_t), SYNC_APPSL);
		}
		else if (app->is_mpi)
		{
			storage_sample_add((char *) &appsm[max_appsm], sam_inmax[i_appsm], &max_appsm,
							   content, sizeof(application_t), SYNC_APPSM);
		}
		else
		{
			storage_sample_add((char *) &appsn[max_appsn], sam_inmax[i_appsn], &max_appsn,
							   content, sizeof(application_t), SYNC_APPSN);
		}
	} else if (header->content_type == CONTENT_TYPE_EVE)
	{
		storage_sample_add((char *) &evnts[max_evnts], sam_inmax[i_evnts], &max_evnts,
						   content, sizeof(ear_event_t), SYNC_EVNTS);
	}
	else if (header->content_type == CONTENT_TYPE_LOO)
	{
		storage_sample_add((char *) &loops[max_loops], sam_inmax[i_loops], &max_loops,
						   content, sizeof(loop_t), SYNC_LOOPS);
	}
	else if (header->content_type == CONTENT_TYPE_PER)
	{
		periodic_metric_t *met = (periodic_metric_t *) content;

		// Add sample to the aggregation
		add_periodic_aggregation(&aggrs[max_aggrs], met->DC_energy, met->start_time, met->end_time);

		// Add sample to the energy array
		storage_sample_add((char *) &enrgy[max_enrgy], sam_inmax[i_enrgy], &max_enrgy,
						   content, sizeof(periodic_metric_t), SYNC_ENRGY);
	}
	else if (header->content_type == CONTENT_TYPE_QST)
	{
		sync_qst_t *q = (sync_qst_t *) content;

		// Passing the question option
		insert_hub(q->sync_option, RES_SYNC);

		// Answering the mirror question
		sync_answer(fd);

		// In case it is a full sync the sync time is resetted before the answer
		// with a very small offset (5 second is enough)
		if (sync_option(q->sync_option, SYNC_ALL))
		{
			/// We get the timeout passed since the 'time_slct' got its value
			// and added to 'timeout_insr', because when 'time_slct' would be
			// substracted from both 'timeout_insr' and 'timeout_aggr', it
			// also be substracted the passed time.
			time_t timeout_passed = time_slct - timeout_slct.tv_sec;

			time_reset_timeout_insr(timeout_passed + 5);
		}
	}
}
