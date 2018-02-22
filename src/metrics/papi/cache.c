#include <stdio.h>
#include <stdlib.h>
#include <papi.h>

#include <metrics/papi/cache.h>
#include <metrics/papi/generics.h>
#include <metrics/custom/hardware_info.h>
#include <common/ear_verbose.h>
#include <common/states.h>

#define CACHE_SETS      3
#define CACHE_EVS       2


#define EAR_CACHE_EVENTS_SETS 3
#define EAR_CACHE_EVENTS 2
#define L1 0
#define L2 1
#define L3 2

static const char *__NAME__ = "METRICS_CACHE";
static PAPI_option_t cache_attach_opt[EAR_CACHE_EVENTS_SETS];
static long long cache_acum_values[EAR_CACHE_EVENTS_SETS];
static long long cache_values[EAR_CACHE_EVENTS_SETS][EAR_CACHE_EVENTS];
static int cache_event_sets[EAR_CACHE_EVENTS_SETS];
static int ear_cache_perf_event_cid;

int init_cache_metrics()
{
	int retval;
	int sets;
	int events;
	int cpu_model;

	PAPI_INIT_TEST(__NAME__);
	PAPI_INIT_MULTIPLEX_TEST(__NAME__);
	PAPI_GET_COMPONENT(ear_cache_perf_event_cid, "perf_event", __NAME__);

	for (sets=0;sets<EAR_CACHE_EVENTS_SETS;sets++){
		cache_acum_values[sets]=0;
		/* Event values set to 0 */
		for (events=0;events<EAR_CACHE_EVENTS;events++) {
			cache_values[sets][events]=0;
		}
		/* Init event sets */
	    	cache_event_sets[sets]=PAPI_NULL;
	        if ((retval=PAPI_create_eventset(&cache_event_sets[sets]))!=PAPI_OK){
			fprintf(stderr,"cache_metrics: Creating %d eventset.Exiting:%s\n",sets,PAPI_strerror(retval));
			exit(1);
		}

		if ((retval=PAPI_assign_eventset_component(cache_event_sets[sets],ear_cache_perf_event_cid))!=PAPI_OK){		
			fprintf(stderr,"cache_metrics: PAPI_assign_eventset_component.Exiting:%s\n",sets,PAPI_strerror(retval));
			exit(1);
		}
		cache_attach_opt[sets].attach.eventset=cache_event_sets[sets];
 		cache_attach_opt[sets].attach.tid=getpid();
		if ((retval=PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*)&cache_attach_opt[sets]))!=PAPI_OK){
			fprintf(stderr,"cache_metrics: PAPI_set_opt.%s\n",PAPI_strerror(retval));
		}
		retval = PAPI_set_multiplex(cache_event_sets[sets]);
		if ((retval == PAPI_EINVAL) && (PAPI_get_multiplex(cache_event_sets[sets]) > 0)){
			fprintf(stderr,"cache_metrics: Event set to compute CACHE already has multiplexing enabled\n");
		}else if (retval != PAPI_OK){ 
			fprintf(stderr,"cache_metrics: Error , event set to compute CACHE can not be multiplexed %s\n",PAPI_strerror(retval));
		}
		switch (sets){
		case L1:
			if ((retval=PAPI_add_named_event(cache_event_sets[sets],"PAPI_L1_DCM"))!=PAPI_OK){
				fprintf(stderr,"cache_metrics: PAPI_add_named_event %s.%s\n","PAPI_L1_DCM",PAPI_strerror(retval));
			}
			if ((retval=PAPI_add_named_event(cache_event_sets[sets],"PAPI_L2_ICM"))!=PAPI_OK){
				fprintf(stderr,"cache_metrics: PAPI_add_named_event %s.%s\n","PAPI_L2_ICM",PAPI_strerror(retval));
			}
			break;
		case L2:
			if ((retval=PAPI_add_named_event(cache_event_sets[sets],"PAPI_L2_TCM"))!=PAPI_OK){
				fprintf(stderr,"cache_metrics: PAPI_add_named_event %s.%s\n","PAPI_L2_TCM",PAPI_strerror(retval));
			}
			break;
		case L3:
			if ((retval=PAPI_add_named_event(cache_event_sets[sets],"PAPI_L3_TCM"))!=PAPI_OK){
				fprintf(stderr,"cache_metrics: PAPI_add_named_event %s.%s\n","PAPI_L3_TCM",PAPI_strerror(retval));
			}
			break;
		}
		}
		return EAR_SUCCESS;
	
}
void reset_cache_metrics()
{
	int sets,events,retval;
	for (sets=0;sets<EAR_CACHE_EVENTS_SETS;sets++){
		if ((retval=PAPI_reset(cache_event_sets[sets]))!=PAPI_OK){
			fprintf(stderr,"cache_metrics: reset_cache_metrics.%s\n",PAPI_strerror(retval));
		}
		for (events=0;events<EAR_CACHE_EVENTS;events++) cache_values[sets][events]=0;
		
	}
}
void start_cache_metrics()
{
	int sets,retval;
	for (sets=0;sets<EAR_CACHE_EVENTS_SETS;sets++){
		if ((retval=PAPI_start(cache_event_sets[sets]))!=PAPI_OK){
			fprintf(stderr,"cache_metrics:start_cache_metrics.%s\n",PAPI_strerror(retval));
		}
	}
}
/* Stops includes accumulate metrics */
void stop_cache_metrics(long long *L1_misses,long long * L2_misses,long long * L3_misses)
{
	int sets,ev,retval;
	*L1_misses=0;
	*L2_misses=0;
	*L3_misses=0;
	for (sets=0;sets<EAR_CACHE_EVENTS_SETS;sets++){
		if ((retval=PAPI_stop(cache_event_sets[sets],(long long *)&cache_values[sets]))!=PAPI_OK){
			fprintf(stderr,"cache_metrics: stop_cache_metrics.%s\n",PAPI_strerror(retval));
		}else{
			if (sets==L1){ 
				*L1_misses=cache_values[sets][0]+cache_values[sets][1];
				cache_acum_values[sets]+=cache_values[sets][0]+cache_values[sets][1];
			}
			if (sets==L2){ 
				*L2_misses=cache_values[sets][0];
				cache_acum_values[sets]+=cache_values[sets][0];
			}
			if (sets==L3){ 
				*L3_misses=cache_values[sets][0];
				cache_acum_values[sets]+=cache_values[sets][0];
			}
			
		}
	}
}

void get_cache_metrics(long long *total_L1_misses,long long * total_L2_misses,long long * total_L3_misses)
{
	*total_L1_misses=cache_acum_values[0];
	*total_L2_misses=cache_acum_values[1];
	*total_L3_misses=cache_acum_values[2];	
}

