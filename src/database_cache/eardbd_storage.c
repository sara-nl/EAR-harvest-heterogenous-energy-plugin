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

// Mirroring
extern int master_iam; // Master is who speaks
extern int server_iam;
extern int mirror_iam;

// Pipeline
extern int forked;


// Storage
extern uint per_appsm;
extern uint per_appsn;
extern uint per_appsl;
extern uint per_loops;
extern uint per_enrgy;
extern uint per_aggrs;
extern uint per_evnts;

extern ulong len_aggrs;
extern ulong len_appsl;
extern ulong len_appsm;
extern ulong len_appsn;
extern ulong len_enrgy;
extern ulong len_evnts;
extern ulong len_loops;

extern periodic_aggregation_t *aggrs;
extern periodic_metric_t *enrgy;
extern application_t *appsm;
extern application_t *appsn;
extern application_t *appsl;
extern ear_event_t *evnts;
extern loop_t *loops;

extern ulong i_aggrs;
extern ulong i_enrgy;
extern ulong i_appsm;
extern ulong i_appsn;
extern ulong i_appsl;
extern ulong i_evnts;
extern ulong i_loops;

// Verbosity
extern char *str_who[2];

/*
 *
 * Insert
 *
 */

static void insert_apps_mpi()
{
	if (per_appsm == 0) {
		return;
	}

	float percent = (float) (i_appsm) / (float) (len_appsm);
	verbose1("%lu/%lu (%0.2f) samples of mpi applications",
			 i_appsm, len_appsm, percent);

	if (i_appsm <= 0) {
		return;
	}

	db_batch_insert_applications(appsm, i_appsm);
	i_appsm = 0;
}

static void insert_apps_non_mpi()
{
	if (per_appsn == 0) {
		return;
	}

	float percent = (float) (i_appsn) / (float) (len_appsn);
	verbose1("%lu/%lu (%0.2f) samples of non-mpi applications",
			 i_appsn, len_appsn, percent);

	if (i_appsn <= 0) {
		return;
	}

	db_batch_insert_applications_no_mpi(appsn, i_appsn);
	i_appsn = 0;
}

static void insert_apps_learning()
{
	if (per_appsl == 0) {
		return;
	}

	float percent = (float) (i_appsl) / (float) (len_appsl);
	verbose1("%lu/%lu (%0.2f) samples of learning applications",
			 i_appsl, len_appsl, percent);

	if (i_appsl <= 0) {
		return;
	}

	db_batch_insert_applications(appsl, i_appsl);
	i_appsl = 0;
}

static void insert_loops()
{
	if (per_loops == 0) {
		return;
	}

	float percent = (float) (i_loops) / (float) (len_loops);
	verbose1("%lu/%lu (%0.2f) samples of loops",
			 i_loops, len_loops, percent);

	if (i_loops <= 0) {
		return;
	}

	db_batch_insert_loops(loops, i_loops);
	i_loops = 0;
}

static void insert_energy()
{
	if (per_enrgy == 0) {
		return;
	}

	float percent = (float) (i_enrgy) / (float) (len_enrgy);
	verbose1("%lu/%lu (%0.2f) samples of energy monitoring data",
			 i_enrgy, len_enrgy, percent);

	if (i_enrgy <= 0) {
		return;
	}

	db_batch_insert_periodic_metrics(enrgy, i_enrgy);
	i_enrgy = 0;
}

static void insert_aggregations()
{
	if (per_aggrs == 0) {
		return;
	}

	float percent = (float) (i_aggrs) / (float) (len_aggrs);
	verbose1("%lu/%lu (%0.2f) samples of energy aggregations",
			 i_aggrs, len_aggrs, percent);

	if (i_aggrs <= 0) {
		return;
	}

	db_batch_insert_periodic_aggregations(aggrs, i_aggrs);

	if (i_aggrs < len_aggrs && aggrs[i_aggrs].n_samples > 0) {
		memcpy (aggrs, &aggrs[i_aggrs], sizeof(periodic_aggregation_t));
	} else {
		init_periodic_aggregation(aggrs);
	}

	i_aggrs = 0;
}

static void insert_events()
{
	if (per_evnts == 0) {
		return;
	}

	float percent = (float) (i_evnts) / (float) (len_evnts);
	verbose1("%lu/%lu (%0.2f) samples of events",
			 i_evnts, len_evnts, percent);

	if (i_evnts <= 0) {
		return;
	}

	db_batch_insert_ear_event(evnts, i_evnts);
	i_evnts = 0;
}

void insert_hub(uint option, uint reason)
{
	verline0();
	verbose1("looking for possible DB insertion (type 0x%x, reason 0x%x)", option, reason);

	if (sync_option(option, SYNC_AGGRS)) {
		insert_aggregations();
	}
	if (sync_option(option, SYNC_APPSM)) {
		insert_apps_mpi();
	}
	if (sync_option(option, SYNC_APPSN)) {
		insert_apps_non_mpi();
	}
	if (sync_option(option, SYNC_APPSL)) {
		insert_apps_learning();
	}
	if (sync_option(option, SYNC_ENRGY)) {
		insert_energy();
	}
	if (sync_option(option, SYNC_EVNTS)) {
		insert_events();
	}
	if (sync_option(option, SYNC_LOOPS)) {
		insert_loops();
	}
	if (sync_option(option, SYNC_RESET)) {
		storage_reset_indexes();
	}
}

/*
 *
 * Storage management
 *
 */

void storage_reset_indexes()
{
	i_aggrs = 0;
	i_appsm = 0;
	i_appsn = 0;
	i_appsl = 0;
	i_enrgy = 0;
	i_evnts = 0;
	i_loops = 0;
}

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
		*idx = 0;
	}
}

void storage_sample_receive(int fd, packet_header_t *header, char *content)
{
	state_t state;

	if (header->content_type == CONTENT_TYPE_APP)
	{
		application_t *app = (application_t *) content;

		if (app->is_learning)
		{
			storage_sample_add((char *) &appsl[i_appsl], len_appsl, &i_appsl,
				content, sizeof(application_t), SYNC_APPSL);
		}
		else if (app->is_mpi)
		{
			storage_sample_add((char *) &appsm[i_appsm], len_appsm, &i_appsm,
				content, sizeof(application_t), SYNC_APPSM);
		}
		else
		{
			storage_sample_add((char *) &appsn[i_appsn], len_appsn, &i_appsn,
				content, sizeof(application_t), SYNC_APPSN);
		}
	} else if (header->content_type == CONTENT_TYPE_EVE)
	{
		storage_sample_add((char *) &evnts[i_evnts], len_evnts, &i_evnts,
			content, sizeof(ear_event_t), SYNC_EVNTS);
	}
	else if (header->content_type == CONTENT_TYPE_LOO)
	{
		storage_sample_add((char *) &loops[i_loops], len_loops, &i_loops,
			content, sizeof(loop_t), SYNC_LOOPS);
	}
	else if (header->content_type == CONTENT_TYPE_PER)
	{
		periodic_metric_t *met = (periodic_metric_t *) content;

		// Add sample to the aggregation
		add_periodic_aggregation(&aggrs[i_aggrs], met->DC_energy, met->start_time, met->end_time);
		
		// Add sample to the energy array
		storage_sample_add((char *) &enrgy[i_enrgy], len_enrgy, &i_enrgy,
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
		// with a very small offset (1 second is enough)
		if (sync_option(q->sync_option, SYNC_ALL)) {
			time_reset_timeout_insr(1);
		}
	}
}

void storage_sample_announce(int fd, packet_header_t *header, char *content)
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
	} else if (header->content_type == CONTENT_TYPE_PIN) {
		type = "ping";
	} else {
		type = "unknown";
	}

	verbose1("received '%s' packet from host '%s' (socket: %d)",
			type, header->host_src, fd);
}
