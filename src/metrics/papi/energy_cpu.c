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



#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/ear_verbose.h>
#include <common/states.h>
#include <metrics/papi/generics.h>
#include <metrics/papi/energy_cpu.h>

#define RAPL_SETS		1
#define MAX_RAPL_EVENTS		64

static const char *__NAME__ = "metrics[rapl]";
long long acum_values[RAPL_SETS][RAPL_EVS];
long long values[RAPL_SETS][RAPL_EVS];
int event_sets[RAPL_SETS];

static int ear_papi_energy_connected=0;

int init_rapl_metrics()
{
	char units[MAX_RAPL_EVENTS][PAPI_MIN_STR_LEN];
	const PAPI_component_info_t *ear_rapl_cmpinfo = NULL;
	PAPI_granularity_option_t gran_opt;
	PAPI_domain_option_t domain_opt;
	PAPI_event_info_t rapl_evinfo;
	int ear_rapl_num_events = 0;
	int retval, cid, code;
	int sets, ret;
	int events;

	if (ear_papi_energy_connected) return EAR_SUCCESS;

	PAPI_INIT_TEST(__NAME__);
	PAPI_INIT_MULTIPLEX_TEST(__NAME__);
	PAPI_GET_COMPONENT(cid, "rapl", __NAME__);

	VERBOSE_N(1, "rapl component found atd cid %d", cid);

	// Checking RAPL is enabled
	ear_rapl_cmpinfo = PAPI_get_component_info(cid);

	if (ear_rapl_cmpinfo == NULL) {
		VERBOSE_N(0, "rapl component error when accessing component info");
		return EAR_ERROR;
	}
	else{
		if (ear_rapl_cmpinfo->disabled){
			VERBOSE_N(0, "rapl component disabled");
			return EAR_ERROR;
		}
		VERBOSE_N(2, "rapl component enabled");
	}

	// We get rapl event code numbers and units info, just for verbose/debug
	code=PAPI_NATIVE_MASK;
	retval=PAPI_enum_cmp_event( &code, PAPI_ENUM_FIRST, cid );

	if (retval!=PAPI_OK){	
		VERBOSE_N(0, "PAPI_enum_cmp_event PAPI_ENUM_FIRST");
		return EAR_ERROR;
	}

	ear_rapl_num_events=0;

	while (retval == PAPI_OK)
	{
    	retval = PAPI_get_event_info(code, &rapl_evinfo);
		if (retval != PAPI_OK) {
			VERBOSE_N(0, "PAPI_get_event_info");
			return EAR_ERROR;
		}

		DEBUG_F(4, "Processing event %d in pos %d name %s units %s", rapl_evinfo.event_code,
				  ear_rapl_num_events, rapl_evinfo.symbol, rapl_evinfo.units);

    	strncpy(units[ear_rapl_num_events],rapl_evinfo.units, sizeof(units[0])-1);

		// buffer must be null terminated to safely use strstr operation on it below
    	units[ear_rapl_num_events][sizeof(units[0])-1] = '\0'; 
    	ear_rapl_num_events++;
        retval = PAPI_enum_cmp_event( &code, PAPI_ENUM_EVENTS, cid );
	}

	VERBOSE_N(2, "%d RAPL events found",ear_rapl_num_events);
	
	// We only  have 1 event set at this time
	for (sets=0;sets<RAPL_SETS;sets++)
	{
		/* Event values set to 0 */
		for (events=0;events<RAPL_EVS;events++) {
			acum_values[sets][events]=0;
			values[sets][events]=0;
		}

		/* Init event sets */
		event_sets[sets]=PAPI_NULL;
		if (PAPI_create_eventset(&event_sets[sets]) != PAPI_OK) {
			VERBOSE_N(0, "Creating %d eventset",sets);
			return EAR_ERROR;
		}

		VERBOSE_N(2, "Event set %d created",sets);
		if (PAPI_assign_eventset_component(event_sets[sets],cid)!=PAPI_OK){		
			VERBOSE_N(0, "PAPI_assign_eventset_component");
			return EAR_ERROR;
		}

   		gran_opt.def_cidx=cid;
   		gran_opt.eventset=event_sets[sets];
   		gran_opt.granularity=PAPI_GRN_SYS;

   		retval = PAPI_set_opt(PAPI_GRANUL,(PAPI_option_t*) &gran_opt);
   		if (retval != PAPI_OK) {
				VERBOSE_N(0, "PAPI_set_opt PAPI_GRANUL");
				return EAR_ERROR;
   		}   

   		/* we need to set domain to be as inclusive as possible */
   		domain_opt.def_cidx = cid;
   		domain_opt.eventset = event_sets[sets];
   		domain_opt.domain = PAPI_DOM_ALL;

   		retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t *) &domain_opt);
   		if (retval != PAPI_OK) {
				VERBOSE_N(0, "PAPI_set_opt PAPI_DOMAIN");
				return EAR_ERROR;
   		}   

		// rapl:::DRAM_ENERGY:PACKAGE0
		ret = PAPI_add_named_event(event_sets[sets], "rapl:::DRAM_ENERGY:PACKAGE0");
		if (ret != PAPI_OK){
				VERBOSE_N(0, "PAPI_add_named_event rapl:::DRAM_ENERGY:PACKAGE0 (%s)",
				PAPI_strerror(ret));
				return EAR_ERROR;
		}
		DEBUG_F(3, "PAPI_add_named_event rapl:::DRAM_ENERGY:PACKAGE0 success");

		// rapl:::DRAM_ENERGY:PACKAGE1
		ret = PAPI_add_named_event(event_sets[sets],"rapl:::DRAM_ENERGY:PACKAGE1");
		if (ret != PAPI_OK) {
				VERBOSE_N(0, "PAPI_add_named_event rapl:::DRAM_ENERGY:PACKAGE1 (%s)",
				PAPI_strerror(ret));
				return EAR_ERROR;
		}
		DEBUG_F(3, "PAPI_add_named_event rapl:::DRAM_ENERGY:PACKAGE1  success");

		// rapl:::PACKAGE_ENERGY:PACKAGE0
		ret = PAPI_add_named_event(event_sets[sets],"rapl:::PACKAGE_ENERGY:PACKAGE0");
		if (ret != PAPI_OK){
				VERBOSE_N(0, "PAPI_add_named_event PACKAGE_ENERGY:PACKAGE0 (%s)",
				PAPI_strerror(ret));
				return EAR_ERROR;
		}
		DEBUG_F(3, "PAPI_add_named_event PACKAGE_ENERGY:PACKAGE0 success");

		// rapl:::PACKAGE_ENERGY:PACKAGE1
		ret = PAPI_add_named_event(event_sets[sets],"rapl:::PACKAGE_ENERGY:PACKAGE1");
		if (ret != PAPI_OK){
				VERBOSE_N(0, "PAPI_add_named_event rapl:::PACKAGE_ENERGY:PACKAGE1(%s)",
				PAPI_strerror(ret));
				return EAR_ERROR;
		}
		DEBUG_F(3, "PAPI_add_named_event rapl:::PACKAGE_ENERGY:PACKAGE1 success");
    }
	VERBOSE_N(2, "METRICS ON");
	ear_papi_energy_connected=1;
	return EAR_SUCCESS;
	
}

int reset_rapl_metrics()
{
	int  events, ret;
	int sets=0;
	if ((ret = PAPI_reset(event_sets[sets])) != PAPI_OK)
	{
			VERBOSE_N(0, "ResetRAPLMetrics (%s)", PAPI_strerror(ret));
			return EAR_ERROR;
	}

	for (events = 0; events < RAPL_EVS; events++) {
			values[sets][events]=0;
	}
	return 0;
}

int start_rapl_metrics()
{
	int  ret;
	int sets=0;

	if ((ret = PAPI_start(event_sets[sets])) != PAPI_OK) {
			VERBOSE_N(0, "StartRAPLMetrics (%s)", PAPI_strerror(ret));
			return EAR_ERROR;
	}
	return 0;
}
int read_rapl_metrics(unsigned long long *_values)
{
    unsigned long long acum_rapl = 0;
    int sets=0;
    int  counts, ret;
    ret = PAPI_read(event_sets[sets], (long long *) _values);
    if (ret != PAPI_OK)
    {
            VERBOSE_N(0, "ReadRAPLMetrics (%s)", PAPI_strerror(ret));
            return EAR_ERROR;
    }

    return 0;
}



/* Stops includes accumulate metrics */
int stop_rapl_metrics(unsigned long long *_values)
{
	unsigned long long acum_rapl = 0;
	int sets=0;
	int  counts, ret;
	ret = PAPI_stop(event_sets[sets], (long long *) &values[sets]);
	if (ret != PAPI_OK)
	{
			VERBOSE_N(0, "StopRAPLMetrics (%s)", PAPI_strerror(ret));
			return EAR_ERROR;
	}
	else
	{
			for (counts=0;counts<RAPL_EVS;counts++)
			{
				acum_values[sets][counts]+=values[sets][counts];

				_values[counts] = (unsigned long long)values[sets][counts];
				DEBUG_F(4, "Value for event %d %llu", counts, _values[counts]);

				acum_rapl += _values[counts];
			}
			DEBUG_F(3, "total energy %llu",acum_rapl);
	}
	
	return 0;
}

void print_rapl_metrics()
{
	int events;
	int sets=0;
	for (events = 0; events < RAPL_EVS; events++)
	{	
		switch (events)
		{
				case RAPL_DRAM0:
					VERBOSE_N(2, "Energy used by DRAM on package 0 (units nJ) %llu",
					acum_values[sets][RAPL_DRAM0]);
					break;
				case RAPL_DRAM1:
					VERBOSE_N(2, "Energy used by DRAM on package 1 (units nJ) %llu",
					acum_values[sets][RAPL_DRAM1]);
					break;
				case RAPL_PACKAGE0:
					VERBOSE_N(2, "Energy used by all cores in package 0 (units nJ) %llu",
					acum_values[sets][RAPL_PACKAGE0]);
					break;
				case RAPL_PACKAGE1:
					VERBOSE_N(2, "Energy used by all cores in package 1 (units nJ) %llu",
					acum_values[sets][RAPL_PACKAGE1]);
					break;
				default:
					VERBOSE_N(0, "Event not supported");
		}
	}
}
