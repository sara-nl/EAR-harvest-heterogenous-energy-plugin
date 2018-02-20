/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
   	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/ear_verbose.h>
#include <metrics/papi/energy_cpu.h>

#define EAR_RAPL_EVENTS_SETS		1
#define CORE_AND_DRAM_SET		0
#define MAX_RAPL_EVENTS			64

PAPI_attach_option_t rapl_attach_opt[EAR_RAPL_EVENTS_SETS];
long long ear_rapl_acum_values[EAR_RAPL_EVENTS_SETS][EAR_RAPL_EVENTS];
long long ear_rapl_values[EAR_RAPL_EVENTS_SETS][EAR_RAPL_EVENTS];
char ear_rapl_units[MAX_RAPL_EVENTS][PAPI_MIN_STR_LEN];
int ear_RAPLEventSets[EAR_RAPL_EVENTS_SETS];

const PAPI_component_info_t *ear_rapl_cmpinfo = NULL;
int ear_rapl_perf_event_cid;
int ear_rapl_num_events =0;

int init_rapl_metrics()
{
	int retval;
	int sets,ret;
	int events;
	PAPI_event_info_t rapl_evinfo;
   	PAPI_granularity_option_t gran_opt;
   	PAPI_domain_option_t domain_opt;
	// PAPI initialization
        if (PAPI_is_initialized()==PAPI_NOT_INITED){
                retval=PAPI_library_init(PAPI_VER_CURRENT );
                if ( retval != PAPI_VER_CURRENT ) {
			ear_verbose(0,"eard (rapl):Error initializing RAPL \n");
                }
                PAPI_multiplex_init();
        }
        // Here , papi is initialized


	ear_rapl_perf_event_cid=PAPI_get_component_index("rapl");
   	if (ear_rapl_perf_event_cid<0){
   	    ear_verbose(0,"ear_daemon_rapl:: rapl component not found\n");
		return -1;
   	 }
	 ear_verbose(1,"ear_daemon_rapl:: rapl component found atd cid %d\n",ear_rapl_perf_event_cid);
	// Checking RAPL is enabled
	ear_rapl_cmpinfo=PAPI_get_component_info(ear_rapl_perf_event_cid);
	if (ear_rapl_cmpinfo == NULL) {
		ear_verbose(0,"ear_daemon_rapl:: rapl component error when accessing component info\n");
		return -1;
	}
	else{
		if (ear_rapl_cmpinfo->disabled){
			ear_verbose(0,"ear_daemon_rapl:: rapl component disabled\n");
			return -1;
		}
		ear_verbose(2,"ear_daemon_rapl:: rapl component enabled\n");
	}
	// We get rapl event code numbers and units info, just for verbose/debug
	int code;
	code=PAPI_NATIVE_MASK;
	retval=PAPI_enum_cmp_event( &code, PAPI_ENUM_FIRST, ear_rapl_perf_event_cid );	
	if (retval!=PAPI_OK){	
			ear_verbose(0,"ear_daemon_rapl:: PAPI_enum_cmp_event PAPI_ENUM_FIRST\n");
			return -1;
     }
	ear_rapl_num_events=0;
	while ( retval == PAPI_OK ) {
    	retval = PAPI_get_event_info(code,&rapl_evinfo);
		if (retval!=PAPI_OK){	
			ear_verbose(0,"ear_daemon_rapl:: PAPI_get_event_info\n");
			return -1;
		}
		ear_debug(4,"ear_daemon_rapl::: Processing event %d in pos %d name %s units %s \n",rapl_evinfo.event_code,ear_rapl_num_events,rapl_evinfo.symbol,rapl_evinfo.units);
    	strncpy(ear_rapl_units[ear_rapl_num_events],rapl_evinfo.units,sizeof(ear_rapl_units[0])-1);
    	// buffer must be null terminated to safely use strstr operation on it below
    	ear_rapl_units[ear_rapl_num_events][sizeof(ear_rapl_units[0])-1] = '\0'; 
    	ear_rapl_num_events++;
        retval = PAPI_enum_cmp_event( &code, PAPI_ENUM_EVENTS, ear_rapl_perf_event_cid );
     }
	ear_verbose(2,"ear_daemon_rapl:: %d RAPL events found\n",ear_rapl_num_events);
	
	// We only  have 1 event set at this time
	for (sets=0;sets<EAR_RAPL_EVENTS_SETS;sets++){
		/* Event values set to 0 */
		for (events=0;events<EAR_RAPL_EVENTS;events++) {
			ear_rapl_acum_values[sets][events]=0;
			ear_rapl_values[sets][events]=0;
		}
		/* Init event sets */
    		ear_RAPLEventSets[sets]=PAPI_NULL;
        	if (PAPI_create_eventset(&ear_RAPLEventSets[sets])!=PAPI_OK){
			ear_verbose(0,"ear_daemon_rapl:: Creating %d eventset\n",sets);
			return -1;
		}
		ear_verbose(2,"ear_daemon_rapl:: Event set %d created\n",sets);
		if (PAPI_assign_eventset_component(ear_RAPLEventSets[sets],ear_rapl_perf_event_cid)!=PAPI_OK){		
			ear_verbose(0,"ear_daemon_rapl:: PAPI_assign_eventset_component\n");
			return -1;
		}
		switch(sets){
		case CORE_AND_DRAM_SET:

   			gran_opt.def_cidx=ear_rapl_perf_event_cid;
   			gran_opt.eventset=ear_RAPLEventSets[sets];
   			gran_opt.granularity=PAPI_GRN_SYS;

   			retval = PAPI_set_opt(PAPI_GRANUL,(PAPI_option_t*)&gran_opt);
   			if (retval != PAPI_OK) {
				ear_verbose(0,"ear_daemon_rapl:: PAPI_set_opt PAPI_GRANUL\n");
				return -1;
   			}   

   			/* we need to set domain to be as inclusive as possible */


   			domain_opt.def_cidx=ear_rapl_perf_event_cid;
   			domain_opt.eventset=ear_RAPLEventSets[sets];
   			domain_opt.domain=PAPI_DOM_ALL;

   			retval = PAPI_set_opt(PAPI_DOMAIN,(PAPI_option_t*)&domain_opt);
   			if (retval != PAPI_OK) {
				ear_verbose(0,"ear_daemon_rapl:: PAPI_set_opt PAPI_DOMAIN\n");
				return -1;
   			}   

			// rapl:::DRAM_ENERGY:PACKAGE0
			if ((ret=PAPI_add_named_event(ear_RAPLEventSets[sets],"rapl:::DRAM_ENERGY:PACKAGE0"))!=PAPI_OK){
				ear_verbose(0,"ear_daemon_rapl:: PAPI_add_named_event rapl:::DRAM_ENERGY:PACKAGE0 (%s)\n",
				PAPI_strerror(ret));
				return -1;
			}
			ear_debug(3,"ear_daemon_rapl:: PAPI_add_named_event  rapl:::DRAM_ENERGY:PACKAGE0 success\n");
			// rapl:::DRAM_ENERGY:PACKAGE1
			if ((ret=PAPI_add_named_event(ear_RAPLEventSets[sets],"rapl:::DRAM_ENERGY:PACKAGE1"))!=PAPI_OK){
				ear_verbose(0,"ear_daemon_rapl:: PAPI_add_named_event rapl:::DRAM_ENERGY:PACKAGE1 (%s)\n",
				PAPI_strerror(ret));
				return -1;
			}
			ear_debug(3,"ear_daemon_rapl:: PAPI_add_named_event rapl:::DRAM_ENERGY:PACKAGE1  success\n");
			// rapl:::PACKAGE_ENERGY:PACKAGE0
			if ((ret=PAPI_add_named_event(ear_RAPLEventSets[sets],"rapl:::PACKAGE_ENERGY:PACKAGE0"))!=PAPI_OK){
				ear_verbose(0,"ear_daemon_rapl:: PAPI_add_named_event PACKAGE_ENERGY:PACKAGE0 (%s)\n",
					PAPI_strerror(ret));
				return -1;
			}
			ear_debug(3,"ear_daemon_rapl:: PAPI_add_named_event PACKAGE_ENERGY:PACKAGE0 success\n");
			// rapl:::PACKAGE_ENERGY:PACKAGE1
			if ((ret=PAPI_add_named_event(ear_RAPLEventSets[sets],"rapl:::PACKAGE_ENERGY:PACKAGE1"))!=PAPI_OK){
				ear_verbose(0,"ear_daemon_rapl:: PAPI_add_named_event rapl:::PACKAGE_ENERGY:PACKAGE1(%s)\n",
					PAPI_strerror(ret));
				return -1;
			}
			ear_debug(3,"ear_daemon_rapl:: PAPI_add_named_event rapl:::PACKAGE_ENERGY:PACKAGE1 success\n");
		break;
		default: ear_verbose(0,"ear_daemon_rapl:: Error only one set is implemented at this moment, you must not see this message\n");
		}
    }
	ear_verbose(2,"ear_daemon_rapl::METRICS ON\n");
	return 0;
	
}
int reset_rapl_metrics()
{
	int sets,events,ret;
	for (sets=0;sets<EAR_RAPL_EVENTS_SETS;sets++){
		if ((ret=PAPI_reset(ear_RAPLEventSets[sets]))!=PAPI_OK){
			ear_verbose(0,"ear_daemon_rapl:: ResetRAPLMetrics (%s)\n",PAPI_strerror(ret));
			return -1;
		}
		for (events=0;events<EAR_RAPL_EVENTS;events++) ear_rapl_values[sets][events]=0;
		
	}
	return 0;
}
int start_rapl_metrics()
{
	int sets;
	int ret;
	for (sets=0;sets<EAR_RAPL_EVENTS_SETS;sets++){
		if ((ret=PAPI_start(ear_RAPLEventSets[sets]))!=PAPI_OK){
			ear_verbose(0,"ear_daemon_rapl::StartRAPLMetrics (%s) \n",PAPI_strerror(ret));
			return -1;
		}
	}
	return 0;
}
/* Stops includes accumulate metrics */
int stop_rapl_metrics(unsigned long long *values)
{
	int sets,counts;
	unsigned long long acum_rapl=0;
	int ret;
	for (sets=0;sets<EAR_RAPL_EVENTS_SETS;sets++)
	{
		if ((ret=PAPI_stop(ear_RAPLEventSets[sets],(long long *)&ear_rapl_values[sets]))!=PAPI_OK){
			ear_verbose(0,"ear_daemon_rapl: StopRAPLMetrics (%s)\n",PAPI_strerror(ret));
			return -1;
		}else{	
			for (counts=0;counts<EAR_RAPL_EVENTS;counts++){ 
				ear_rapl_acum_values[sets][counts]+=ear_rapl_values[sets][counts];
				values[counts]=ear_rapl_values[sets][counts];
				ear_debug(4,"ear_daemon_rapl:: Value for event %d %llu\n",counts,values[counts]);
				acum_rapl+=values[counts];
			}
			ear_debug(3,"ear_daemon_rapl:: total energy %llu\n",acum_rapl);
		}
	
	}
	return 0;
}
void print_rapl_metrics()
{
	int sets,events;
	for (sets=0;sets<EAR_RAPL_EVENTS_SETS;sets++){
		switch (sets){
			case CORE_AND_DRAM_SET:
			for (events=0;events<EAR_RAPL_EVENTS;events++){
				switch (events){
					case RAPL_DRAM0:
						ear_verbose(2,"ear_daemon_rapl:: Energy used by DRAM on package 0 (units nJ) %llu\n",ear_rapl_acum_values[sets][RAPL_DRAM0]);
						break;
					case RAPL_DRAM1:
						ear_verbose(2,"ear_daemon_rapl:: Energy used by DRAM on package 1 (units nJ) %llu\n",ear_rapl_acum_values[sets][RAPL_DRAM1]);
						break;
					case RAPL_PACKAGE0:
						ear_verbose(2,"ear_daemon_rapl:: Energy used by all cores in package 0 (units nJ) %llu\n",ear_rapl_acum_values[sets][RAPL_PACKAGE0]);
						break;
					case RAPL_PACKAGE1:
						ear_verbose(2,"ear_daemon_rapl:: Energy used by all cores in package 1 (units nJ) %llu\n",ear_rapl_acum_values[sets][RAPL_PACKAGE1]);
						break;
					default:
						ear_verbose(0,"ear_daemon_rapl:: Event not supported\n");
				}
			}
			break;
			default:
			ear_verbose(0,"ear_daemon_rapl:: rapl set not supported\n");
		}

	}
}
