#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <ear_metrics/ear_basic.h>

#define EAR_BASIC_EVENTS_SETS 1
#define EAR_BASIC_EVENTS 2


int basic_event_sets[EAR_BASIC_EVENTS_SETS];
long long basic_acum_values[EAR_BASIC_EVENTS];
long long basic_values[EAR_BASIC_EVENTS_SETS][EAR_BASIC_EVENTS];

int ear_basic_perf_event_cid;
PAPI_option_t basic_attach_opt[EAR_BASIC_EVENTS_SETS];




void init_basic_metrics()
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
	}
	// Here , papi is initialized

	ear_basic_perf_event_cid=PAPI_get_component_index("perf_event");
   	 if (ear_basic_perf_event_cid<0){
   	     fprintf(stderr,"basic_metrics: perf_event component not found.Exiting:%s\n",PAPI_strerror(ear_basic_perf_event_cid));
   	     exit(1);
   	 } 
	for (sets=0;sets<EAR_BASIC_EVENTS_SETS;sets++){
		basic_acum_values[sets]=0;
		/* Event values set to 0 */
		for (events=0;events<EAR_BASIC_EVENTS;events++) {
			basic_values[sets][events]=0;
		}
		/* Init event sets */
	    	basic_event_sets[sets]=PAPI_NULL;
	        if ((retval=PAPI_create_eventset(&basic_event_sets[sets]))!=PAPI_OK){
			fprintf(stderr,"basic_metrics: Creating %d eventset.Exiting:%s\n",sets,PAPI_strerror(retval));
			exit(1);
		}

		if ((retval=PAPI_assign_eventset_component(basic_event_sets[sets],ear_basic_perf_event_cid))!=PAPI_OK){		
			fprintf(stderr,"basic_metrics: PAPI_assign_eventset_component.Exiting:%s\n",sets,PAPI_strerror(retval));
			exit(1);
		}
		basic_attach_opt[sets].attach.eventset=basic_event_sets[sets];
 		basic_attach_opt[sets].attach.tid=getpid();
		if ((retval=PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*)&basic_attach_opt[sets]))!=PAPI_OK){
			fprintf(stderr,"basic_metrics: PAPI_set_opt.%s\n",PAPI_strerror(retval));
		}
		retval = PAPI_set_multiplex(basic_event_sets[sets]);
		if ((retval == PAPI_EINVAL) && (PAPI_get_multiplex(basic_event_sets[sets]) > 0)){
			fprintf(stderr,"basic_metrics: Event set to compute BASIC already has multiplexing enabled\n");
		}else if (retval != PAPI_OK){ 
			fprintf(stderr,"basic_metrics: Error , event set to compute BASIC can not be multiplexed %s\n",PAPI_strerror(retval));
		}
		if ((retval=PAPI_add_named_event(basic_event_sets[sets],"PAPI_TOT_CYC"))!=PAPI_OK){
				fprintf(stderr,"basic_metrics: PAPI_add_named_event %s.%s\n","ix86arch::UNHALTED_CORE_CYCLES",PAPI_strerror(retval));
		}
		if ((retval=PAPI_add_named_event(basic_event_sets[sets],"PAPI_TOT_INS"))!=PAPI_OK){
				fprintf(stderr,"basic_metrics: PAPI_add_named_event %s.%s\n","ix86arch::INSTRUCTION_RETIRED",PAPI_strerror(retval));
		}
	}
}
void reset_basic_metrics()
{
	int sets,events,retval;
	for (sets=0;sets<EAR_BASIC_EVENTS_SETS;sets++){
		if ((retval=PAPI_reset(basic_event_sets[sets]))!=PAPI_OK){
			fprintf(stderr,"basic_metrics: reset_basic_metrics set %d.%s\n",basic_event_sets[sets],PAPI_strerror(retval));
		}
		for (events=0;events<EAR_BASIC_EVENTS;events++) basic_values[sets][events]=0;
		
	}
}
void start_basic_metrics()
{
	int sets,retval;
	for (sets=0;sets<EAR_BASIC_EVENTS_SETS;sets++){
		if ((retval=PAPI_start(basic_event_sets[sets]))!=PAPI_OK){
			fprintf(stderr,"basic_metrics:start_basic_metrics set %d .%s\n",basic_event_sets[sets],PAPI_strerror(retval));
		}
	}
}
/* Stops includes accumulate metrics */
void stop_basic_metrics(long long *cycles,long long *instructions)
{
	int sets,ev,retval;
	*cycles=0;
	*instructions=0;
	// There is a single event set for basic instruction
	for (sets=0;sets<EAR_BASIC_EVENTS_SETS;sets++){
		if ((retval=PAPI_stop(basic_event_sets[sets],(long long *)&basic_values[sets]))!=PAPI_OK){
			fprintf(stderr,"basic_metrics: stop_basic_metrics set %d.%s\n",basic_event_sets[sets],PAPI_strerror(retval));
		}else{
			*cycles=basic_values[sets][0];
			*instructions=basic_values[sets][1];
			basic_acum_values[0]=basic_acum_values[0]+basic_values[sets][0];
			basic_acum_values[1]=basic_acum_values[1]+basic_values[sets][1];
		}
	}
}

void get_basic_metrics(long long *total_cycles,long long * total_instructions)
{
	*total_cycles=basic_acum_values[0];
	*total_instructions=basic_acum_values[1];
}

