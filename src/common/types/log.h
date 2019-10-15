/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/

#ifndef _EAR_LOG_H
#define _EAR_LOG_H

#include <time.h>
#include <common/config.h>
#include <common/types/generic.h>
#include <common/types/job.h>

typedef struct ear_event {
	job_id jid;
    job_id step_id;
	char node_id[GENERIC_NAME];
	uint event;
	ulong freq;
	time_t timestamp;
} ear_event_t;

#define ENERGY_POLICY_NEW_FREQ	0
#define GLOBAL_ENERGY_POLICY	1
#define ENERGY_POLICY_FAILS		2
#define DYNAIS_OFF				3


/* EARD Init events */
#define PM_CREATION_ERROR			100
#define APP_API_CREATION_ERROR	101
#define DYN_CREATION_ERROR			102

#define EARD_INIT_ERROR 3
#define BASE_EARD_INIT_ERROR	100


/* EARD runtime events */
#define DC_POWER_ERROR		300
#define TEMP_ERROR				301
#define FREQ_ERROR				302
#define RAPL_ERROR				303
#define GBS_ERROR					304






/** Creates the log file and starts it with the current time. If it can't
*   create the file it reports it to stderr */
void init_log();
/** Finishes the log with the current time and closes the log file */
void end_log();

/** Given an event, it reports it ot the log file*/
void report_new_event(ear_event_t *event);

/** Given a job id and a frequency value, reports to the log file the change
*   of frequency because of the energy policy */
void log_report_new_freq(job_id id,job_id step_id,ulong newf);
/** Given a job id, reports to the log file that the DynAIS has been turned off */
void log_report_dynais_off(job_id id,job_id step_id);
/** Given a job id and a frequency value, reports to the log file the change
*   of frequency because of a policy projections failure */
void log_report_max_tries(job_id id,job_id step_id,ulong newf);
/** Given a job id and a frequency value, reports to the log file the change
*   of frequency because of Energy Budget*/
void log_report_global_policy_freq(job_id id,job_id step_id,ulong newf);


/** Sets the minimum interval between two runtime errors to avoid saturating the DB */
void log_report_eard_min_interval(uint secs);
/** Reports a RT error from EARD */
void log_report_eard_rt_error(job_id job,job_id sid,uint eventid,ulong value);
/* Reports and error when initializing EARD */
void log_report_eard_init_error(job_id job,job_id sid,uint eventid,ulong value);

#endif
