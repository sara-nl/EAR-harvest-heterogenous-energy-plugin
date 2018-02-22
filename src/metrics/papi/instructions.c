#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <metrics/papi/generics.h>
#include <metrics/papi/instructions.h>
#include <common/ear_verbose.h>

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
			VERBOSE_N(0, "Creating %d eventset.Exiting:%s",
					sets,PAPI_strerror(ret));
			exit(1);
		}

		if ((ret=PAPI_assign_eventset_component(event_sets[sets],cid))!=PAPI_OK)
		{
			VERBOSE_N(0, "PAPI_assign_eventset_component.Exiting:%s", PAPI_strerror(ret));
			exit(1);
		}

		attach_op[sets].attach.eventset=event_sets[sets];
 		attach_op[sets].attach.tid=getpid();
		
		if ((ret = PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*) &attach_op[sets])) != PAPI_OK){
			VERBOSE_N(0, "PAPI_set_opt.%s", PAPI_strerror(ret));
		}
		
		ret = PAPI_set_multiplex(event_sets[sets]);
		
		if ((ret == PAPI_EINVAL) && (PAPI_get_multiplex(event_sets[sets]) > 0)){
			VERBOSE_N(0, "Event set to compute BASIC already has multiplexing enabled");
		}else if (ret != PAPI_OK) { 
			VERBOSE_N(0, "Error, event set to compute BASIC can not be multiplexed %s",
						PAPI_strerror(ret));
		}
		
		if ((ret = PAPI_add_named_event(event_sets[sets],"PAPI_TOT_CYC")) != PAPI_OK){
			VERBOSE_N(0, "PAPI_add_named_event %s.%s",
						"ix86arch::UNHALTED_CORE_CYCLES",PAPI_strerror(ret));
		}
		if ((ret = PAPI_add_named_event(event_sets[sets],"PAPI_TOT_INS")) != PAPI_OK){
			VERBOSE_N(0, "PAPI_add_named_event %s.%s",
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
			VERBOSE_N(0, "reset_basic_metrics set %d.%s",
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
			VERBOSE_N(0, "start_basic_metrics set %d .%s",
					  event_sets[sets],PAPI_strerror(ret));
		}
	}
}
/* Stops includes accumulate metrics */
void stop_basic_metrics(long long *cycles,long long *instructions)
{
	int sets,ev,ret;

	*cycles=0;
	*instructions=0;

	// There is a single event set for basic instruction
	for (sets=0;sets<BASIC_SETS;sets++)
	{
		if ((ret = PAPI_stop(event_sets[sets], (long long *) &values[sets])) != PAPI_OK)
		{
			VERBOSE_N(0, "stop_basic_metrics set %d.%s",
					event_sets[sets], PAPI_strerror(ret));
		} else
		{
			*cycles=values[sets][0];
			*instructions=values[sets][1];
			acum_values[0]=acum_values[0]+values[sets][0];
			acum_values[1]=acum_values[1]+values[sets][1];
		}
	}
}

void get_basic_metrics(long long *total_cycles,long long * total_instructions)
{
	*total_cycles=acum_values[0];
	*total_instructions=acum_values[1];
}
