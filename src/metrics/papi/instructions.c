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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <papi.h>
#include <metrics/papi/generics.h>
#include <metrics/papi/instructions.h>
#include <common/output/verbose.h>

#define BASIC_SETS		1
#define BASIC_EVS		2

static const char *__NAME__ = "METRICS_INSTRS";
static long long values[BASIC_SETS][BASIC_EVS];
static long long acum_values[BASIC_EVS];
static int event_sets[BASIC_SETS];

void init_basic_metrics()
{
	PAPI_option_t attach_op[BASIC_SETS];
	int sets, events;
	int cid, ret;

	PAPI_INIT_TEST(__NAME__);
	PAPI_INIT_MULTIPLEX_TEST(__NAME__);
	PAPI_GET_COMPONENT(cid, "perf_event", __NAME__);

	for (sets=0;sets<BASIC_SETS;sets++)
	{
		acum_values[sets] = 0;

		/* Event values set to 0 */
		for (events = 0; events < BASIC_EVS; events++) {
			values[sets][events] = 0;
		}

		/* Init event sets */
		event_sets[sets]=PAPI_NULL;
		if ((ret = PAPI_create_eventset(&event_sets[sets])) != PAPI_OK)
		{
			verbose(0, "Creating %d eventset.Exiting:%s",
					sets,PAPI_strerror(ret));
			exit(1);
		}

		if ((ret=PAPI_assign_eventset_component(event_sets[sets],cid))!=PAPI_OK)
		{
			verbose(0, "PAPI_assign_eventset_component.Exiting:%s", PAPI_strerror(ret));
			exit(1);
		}

		attach_op[sets].attach.eventset=event_sets[sets];
 		attach_op[sets].attach.tid=getpid();
		
		if ((ret = PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*) &attach_op[sets])) != PAPI_OK){
			verbose(0, "PAPI_set_opt.%s", PAPI_strerror(ret));
		}
		
		ret = PAPI_set_multiplex(event_sets[sets]);
		
		if ((ret == PAPI_EINVAL) && (PAPI_get_multiplex(event_sets[sets]) > 0)){
			verbose(0, "Event set to compute BASIC already has multiplexing enabled");
		}else if (ret != PAPI_OK) { 
			verbose(0, "Error, event set to compute BASIC can not be multiplexed %s",
						PAPI_strerror(ret));
		}
		
		if ((ret = PAPI_add_named_event(event_sets[sets],"PAPI_TOT_CYC")) != PAPI_OK){
			verbose(0, "PAPI_add_named_event %s.%s",
						"ix86arch::UNHALTED_CORE_CYCLES",PAPI_strerror(ret));
		}
		if ((ret = PAPI_add_named_event(event_sets[sets],"PAPI_TOT_INS")) != PAPI_OK){
			verbose(0, "PAPI_add_named_event %s.%s",
						"ix86arch::INSTRUCTION_RETIRED", PAPI_strerror(ret));
		}
	}
}

void reset_basic_metrics()
{
	int sets,events,ret;

	for (sets=0; sets < BASIC_SETS; sets++)
	{
		if ((ret=PAPI_reset(event_sets[sets]))!=PAPI_OK)
		{
			verbose(0, "reset_basic_metrics set %d.%s",
					  event_sets[sets],PAPI_strerror(ret));
		}

		for (events = 0; events < BASIC_EVS; events++) {
			values[sets][events] = 0;
		}
	}
}

void start_basic_metrics()
{
	int sets,ret;
	for (sets=0;sets<BASIC_SETS;sets++)
	{
		if ((ret=PAPI_start(event_sets[sets])) != PAPI_OK)
		{
			verbose(0, "start_basic_metrics set %d .%s",
					  event_sets[sets],PAPI_strerror(ret));
		}
	}
}

/* Stops includes accumulate metrics */
void stop_basic_metrics(long long *cycles,long long *instructions)
{
	int sets, ret;

	*cycles=0;
	*instructions=0;

	// There is a single event set for basic instruction
	for (sets=0;sets<BASIC_SETS;sets++)
	{
		if ((ret = PAPI_stop(event_sets[sets], (long long *) &values[sets])) != PAPI_OK)
		{
			verbose(0, "stop_basic_metrics set %d.%s",
					event_sets[sets], PAPI_strerror(ret));
		} else
		{
			*cycles = *cycles + values[sets][0];
			*instructions = *instructions + values[sets][1];
			acum_values[0] = acum_values[0] + values[sets][0];
			acum_values[1] = acum_values[1] + values[sets][1];
		}
	}
}

void get_basic_metrics(long long *total_cycles, long long *total_instructions)
{
	*total_cycles = acum_values[0];
	*total_instructions = acum_values[1];
}
