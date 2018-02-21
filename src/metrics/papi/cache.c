#include <stdio.h>
#include <stdlib.h>
#include <papi.h>

#include <metrics/custom/hardware_info.h>
#include <metrics/papi/cache.h>
#include <common/states.h>

#define CACHE_SETS 	3
#define CACHE_EVS 	2
#define L1 0
#define L2 1
#define L3 2

static const char *__NAME__ = "METRICS_CACHE";
static long long values[CACHE_SETS][CACHE_EVS];
static long long acum_values[CACHE_EVS];
static int event_sets[CACHE_SETS];

int init_cache_metrics()
{
	PAPI_option_t attach_opt[CACHE_SETS];
	int sets, events;
	int cid, ret;

	PAPI_INIT_TEST(__NAME__);
	PAPI_INIT_MULTIPLEX_TEST(__NAME__);
	PAPI_GET_COMPONENT(cid, "perf_event", __NAME__);

	for (sets = 0; sets < CACHE_SETS; sets++)
	{
		acum_values[sets] = 0;

		/* Event values set to 0 */
		for (events=0;events<CACHE_EVS;events++)
		{
			values[sets][events]=0;
		}

		/* Init event sets */
	    	event_sets[sets]=PAPI_NULL;
	        if ((ret=PAPI_create_eventset(&event_sets[sets])) != PAPI_OK){
			VERBOSE_N(0, "Creating %d eventset.Exiting:%s", sets,PAPI_strerror(ret));
			exit(1);
		}

		if ((ret=PAPI_assign_eventset_component(event_sets[sets],cid)) != PAPI_OK){
			VERBOSE_N(0, "PAPI_assign_eventset_component.Exiting:%s",
					sets,PAPI_strerror(ret));
			exit(1);
		}

		attach_opt[sets].attach.eventset=event_sets[sets];
 		attach_opt[sets].attach.tid=getpid();

		if ((ret=PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*)&attach_opt[sets])) != PAPI_OK){
			VERBOSE_N(0, "PAPI_set_opt.%s",PAPI_strerror(ret));
		}

		ret = PAPI_set_multiplex(event_sets[sets]);
		if ((ret == PAPI_EINVAL) && (PAPI_get_multiplex(event_sets[sets]) > 0))
		{
			VERBOSE_N(0, "Event set to compute CACHE already has multiplexing enabled");
		}else if (ret != PAPI_OK){ 
			VERBOSE_N(0, "Error, event set to compute CACHE can not be multiplexed %s",
					  PAPI_strerror(ret));
		}

		switch (sets)
		{
		case L1:
			if ((ret=PAPI_add_named_event(event_sets[sets], "PAPI_L1_DCM")) != PAPI_OK) {
				VERBOSE_N(0, "PAPI_add_named_event %s.%s", "PAPI_L1_DCM", PAPI_strerror(ret));
			}
			if ((ret=PAPI_add_named_event(event_sets[sets], "PAPI_L2_ICM")) != PAPI_OK) {
				VERBOSE_N(0, "PAPI_add_named_event %s.%s", "PAPI_L2_ICM", PAPI_strerror(ret));
			}
			break;
		case L2:
			if ((ret=PAPI_add_named_event(event_sets[sets], "PAPI_L2_TCM")) != PAPI_OK) {
				VERBOSE_N(0, "PAPI_add_named_event %s.%s", "PAPI_L2_TCM", PAPI_strerror(ret));
			}
			break;
		case L3:
			if ((ret=PAPI_add_named_event(event_sets[sets], "PAPI_L3_TCM")) != PAPI_OK) {
				VERBOSE_N(0, "PAPI_add_named_event %s.%s", "PAPI_L3_TCM", PAPI_strerror(ret));
			}
			break;
		}
	}

	return EAR_SUCCESS;
}

void reset_cache_metrics()
{
	int sets,events,ret;
	for (sets=0;sets<CACHE_SETS;sets++)
	{
		if ((ret=PAPI_reset(event_sets[sets])) != PAPI_OK) {
			VERBOSE_N(0, "reset_cache_metrics.%s", PAPI_strerror(ret));
		}
		for (events = 0; events < CACHE_EVS; events++) {
			values[sets][events]=0;
		}
	}
}

void start_cache_metrics()
{
	int sets,ret;
	for (sets=0;sets<CACHE_SETS;sets++) {
		if ((ret = PAPI_start(event_sets[sets])) != PAPI_OK) {
			VERBOSE_N(0, "start_cache_metrics.%s", PAPI_strerror(ret));
		}
	}
}

/* Stops includes accumulate metrics */
void stop_cache_metrics(long long *L1_misses, long long *L2_misses, long long *L3_misses)
{
	int sets,ev,ret;

	*L1_misses=0;
	*L2_misses=0;
	*L3_misses=0;

	for (sets = 0; sets < CACHE_SETS; sets++)
	{
		if ((ret = PAPI_stop(event_sets[sets], (long long *) &values[sets])) != PAPI_OK)
		{
			VERBOSE_N(0, "stop_cache_metrics.%s", PAPI_strerror(ret));
		} else {
			if (sets == L1){
				*L1_misses=values[sets][0]+values[sets][1];
				acum_values[sets]+=values[sets][0]+values[sets][1];
			}
			if (sets == L2){
				*L2_misses=values[sets][0];
				acum_values[sets]+=values[sets][0];
			}
			if (sets == L3){
				*L3_misses=values[sets][0];
				acum_values[sets]+=values[sets][0];
			}
		}
	}
}

void get_cache_metrics(long long *total_L1_misses, long long * total_L2_misses,
					   long long * total_L3_misses)
{
	*total_L1_misses=acum_values[0];
	*total_L2_misses=acum_values[1];
	*total_L3_misses=acum_values[2];	
}