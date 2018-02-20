/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <papi.h>

#include <metrics/stalls.h>

#define EAR_STALL_EVENTS_SETS 1
#define EAR_STALL_EVENTS 2

static int stall_event_sets[EAR_STALL_EVENTS_SETS];
static long long stall_acum_values[EAR_STALL_EVENTS];
static long long stall_values[EAR_STALL_EVENTS_SETS][EAR_STALL_EVENTS];
static PAPI_option_t stall_attach_opt[EAR_STALL_EVENTS_SETS];
static int ear_stall_perf_event_cid;

void init_stall_metrics()
{
	int retval;
	int sets;
	int events;
	int cpu_model;
	if (PAPI_is_initialized()==PAPI_NOT_INITED){
    		retval=PAPI_library_init(PAPI_VER_CURRENT );
    		if ( retval != PAPI_VER_CURRENT ) {
			fprintf(stderr,"Basic metrics: Error when initializing PAPI\n");
        		exit(1);
    		}    
		PAPI_multiplex_init();
	}
	// Here , papi is initialized

	ear_stall_perf_event_cid=PAPI_get_component_index("perf_event");
   	 if (ear_stall_perf_event_cid<0){
   	     fprintf(stderr,"stall_metrics: perf_event component not found.Exiting:%s\n",PAPI_strerror(ear_stall_perf_event_cid));
   	     exit(1);
   	 } 
	for (sets=0;sets<EAR_STALL_EVENTS_SETS;sets++){
		stall_acum_values[sets]=0;
		/* Event values set to 0 */
		for (events=0;events<EAR_STALL_EVENTS;events++) {
			stall_values[sets][events]=0;
		}
		/* Init event sets */
	    	stall_event_sets[sets]=PAPI_NULL;
	        if ((retval=PAPI_create_eventset(&stall_event_sets[sets]))!=PAPI_OK){
			fprintf(stderr,"stall_metrics: Creating %d eventset.Exiting:%s\n",sets,PAPI_strerror(retval));
			exit(1);
		}

		if ((retval=PAPI_assign_eventset_component(stall_event_sets[sets],ear_stall_perf_event_cid))!=PAPI_OK){		
			fprintf(stderr,"stall_metrics: PAPI_assign_eventset_component.Exiting:%s\n",sets,PAPI_strerror(retval));
			exit(1);
		}
		stall_attach_opt[sets].attach.eventset=stall_event_sets[sets];
 		stall_attach_opt[sets].attach.tid=getpid();
		if ((retval=PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*)&stall_attach_opt[sets]))!=PAPI_OK){
			fprintf(stderr,"stall_metrics: PAPI_set_opt.%s\n",PAPI_strerror(retval));
		}
		retval = PAPI_set_multiplex(stall_event_sets[sets]);
		if ((retval == PAPI_EINVAL) && (PAPI_get_multiplex(stall_event_sets[sets]) > 0)){
			fprintf(stderr,"stall_metrics: Event set to compute STALL already has multiplexing enabled\n");
		}else if (retval != PAPI_OK){ 
			fprintf(stderr,"stall_metrics: Error , event set to compute STALL can not be multiplexed %s\n",PAPI_strerror(retval));
		}
		if ((retval=PAPI_add_named_event(stall_event_sets[sets],"CYCLE_ACTIVITY:STALLS_TOTAL"))!=PAPI_OK){
				fprintf(stderr,"stall_metrics: PAPI_add_named_event %s.%s\n","CYCLE_ACTIVITY:STALLS_TOTAL",PAPI_strerror(retval));
		}
	}
}
void reset_stall_metrics()
{
	int sets,events,retval;
	for (sets=0;sets<EAR_STALL_EVENTS_SETS;sets++){
		if ((retval=PAPI_reset(stall_event_sets[sets]))!=PAPI_OK){
			fprintf(stderr,"stall_metrics: reset_stall_metrics set %d.%s\n",stall_event_sets[sets],PAPI_strerror(retval));
		}
		for (events=0;events<EAR_STALL_EVENTS;events++) stall_values[sets][events]=0;
		
	}
}
void start_stall_metrics()
{
	int sets,retval;
	for (sets=0;sets<EAR_STALL_EVENTS_SETS;sets++){
		if ((retval=PAPI_start(stall_event_sets[sets]))!=PAPI_OK){
			fprintf(stderr,"stall_metrics:start_stall_metrics set %d .%s\n",stall_event_sets[sets],PAPI_strerror(retval));
		}
	}
}
/* Stops includes accumulate metrics */
void stop_stall_metrics(long long *stall_cycles)
{
	int sets,ev,retval;
	*stall_cycles=0;
	// There is a single event set for stall instruction
	for (sets=0;sets<EAR_STALL_EVENTS_SETS;sets++){
		if ((retval=PAPI_stop(stall_event_sets[sets],(long long *)&stall_values[sets]))!=PAPI_OK){
			fprintf(stderr,"stall_metrics: stop_stall_metrics set %d.%s\n",stall_event_sets[sets],PAPI_strerror(retval));
		}else{
			*stall_cycles=stall_values[sets][0];
			stall_acum_values[0]=stall_acum_values[0]+stall_values[sets][0];
		}
	}
}

void get_stall_metrics(long long *total_stall_cycles)
{
	*total_stall_cycles=stall_acum_values[0];
}

