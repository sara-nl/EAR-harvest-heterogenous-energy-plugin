/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <papi.h>

#include <metrics/papi/generics.h>
#include <metrics/custom/hardware_info.h>
#include <metrics/papi/flops.h>
#include <common/environment.h>
#include <common/ear_verbose.h>

#define FLOPS_SETS		2
#define FLOPS_EVS		4
#define SP_OPS			0
#define DP_OPS			1

#define FP_ARITH_INST_RETIRED_PACKED_SINGLE_N		"FP_ARITH:SCALAR_SINGLE"
#define FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N	"FP_ARITH:128B_PACKED_SINGLE"
#define FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N	"FP_ARITH:256B_PACKED_SINGLE"
#define FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N	"FP_ARITH:512B_PACKED_SINGLE"
#define FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N 		"FP_ARITH:SCALAR_DOUBLE"
#define FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N	"FP_ARITH:128B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N	"FP_ARITH:256B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N	"FP_ARITH:512B_PACKED_DOUBLE"

static const char *__NAME__ = "METRICS_FLOPS";
static int weights[FLOPS_SETS][FLOPS_EVS] = {{1,4,8,16}, {1,2,4,8}};
static long long acum_values[FLOPS_SETS][FLOPS_EVS];
static long long values[FLOPS_SETS][FLOPS_EVS];
static int event_sets[FLOPS_SETS];
static int flops_supported = 0;

int init_flops_metrics()
{
	PAPI_option_t attach_ops[FLOPS_SETS];
	int retval, cpu_model, cid;
	int sets, events;

	PAPI_INIT_TEST(__NAME__);
	PAPI_INIT_MULTIPLEX_TEST(__NAME__);
	PAPI_GET_COMPONENT(cid, "perf_event", __NAME__);

	for (sets = 0; sets < FLOPS_SETS; sets++)
	{
		/* Event values set to 0 */
		for (events=0;events<FLOPS_EVS;events++) {
			acum_values[sets][events]=0;
			values[sets][events]=0;
		}
		/* Init event sets */
		event_sets[sets]=PAPI_NULL;
		if ((retval = PAPI_create_eventset(&event_sets[sets])) != PAPI_OK) {
			VERBOSE_N(0,"FP_METRICS: Creating %d eventset.%s", sets, PAPI_strerror(retval));
			return flops_supported;
		}

		if ((retval=PAPI_assign_eventset_component(event_sets[sets],cid)) != PAPI_OK){
			VERBOSE_N(0,"FP_METRICS: PAPI_assign_eventset_component.%s", PAPI_strerror(retval));
			return flops_supported;
		}

		attach_ops[sets].attach.eventset=event_sets[sets];
 		attach_ops[sets].attach.tid=getpid();
		if ((retval=PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*) &attach_ops[sets]))!=PAPI_OK){
			VERBOSE_N(0, "FP_METRICS: PAPI_set_opt.%s", PAPI_strerror(retval));
		}
		retval = PAPI_set_multiplex(event_sets[sets]);
		if ((retval == PAPI_EINVAL) && (PAPI_get_multiplex(event_sets[sets]) > 0)){
			VERBOSE_N(0,"FP_METRICS: Event set to compute FLOPS already has multiplexing enabled");
		}else if (retval != PAPI_OK){ 
			VERBOSE_N(0,"FP_METRICS: Error , event set to compute FLOPS can not be multiplexed %s",PAPI_strerror(retval));
		}
		VERBOSE_N(2,"FP_METRICS: Set %d to compute flops has been multiplexed",event_sets[sets]);
		cpu_model = get_model();
		switch(cpu_model){
			case CPU_HASWELL_X:
			case CPU_BROADWELL_X:
				break;
			case CPU_SKYLAKE_X:
				flops_supported=1;

				switch (sets)
				{
				case SP_OPS:
				if ((retval=PAPI_add_named_event(event_sets[sets],FP_ARITH_INST_RETIRED_PACKED_SINGLE_N))!=PAPI_OK){
					VERBOSE_N(0,"FP_METRICS: PAPI_add_named_event %s.%s",FP_ARITH_INST_RETIRED_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(event_sets[sets],FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N))!=PAPI_OK){
					VERBOSE_N(0,"FP_METRICS: PAPI_add_named_event %s.%s",FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(event_sets[sets],FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N))!=PAPI_OK){
					VERBOSE_N(0,"FP_METRICS: PAPI_add_named_event %s.%s",FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(event_sets[sets],FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N))!=PAPI_OK){
					VERBOSE_N(0,"FP_METRICS: PAPI_add_named_event %s.%s",FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				
				break;
				case DP_OPS:
				if ((retval=PAPI_add_named_event(event_sets[sets],FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N))!=PAPI_OK){
					VERBOSE_N(0,"FP_METRICS: PAPI_add_named_event %s.%s",FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(event_sets[sets],FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N))!=PAPI_OK){
					VERBOSE_N(0,"FP_METRICS: PAPI_add_named_event %s.%s",FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(event_sets[sets],FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N))!=PAPI_OK){
					VERBOSE_N(0,"FP_METRICS: PAPI_add_named_event %s.%s",FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(event_sets[sets],FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N))!=PAPI_OK){
					VERBOSE_N(0,"FP_METRICS: PAPI_add_named_event %s.%s",FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N,PAPI_strerror(retval));
				}
				VERBOSE_N(2,"FP_METRICS: PAPI_DP_OPS added to set %d",sets);
				break;
				}
		}
		
    }
	if (flops_supported){ VERBOSE_N(2, "FP_METRICS:Computation of Flops initialized");
	} else VERBOSE_N(2, "FP_METRICS: Computation of Flops not supported");
	return flops_supported;

	
}
void reset_flops_metrics()
{
	int sets,events,retval;
	if (!flops_supported) return;
	for (sets=0;sets<FLOPS_SETS;sets++){
		if ((retval=PAPI_reset(event_sets[sets]))!=PAPI_OK){
			VERBOSE_N(0,"FP_METRICS: ResetFlopsMetrics.%s",PAPI_strerror(retval));
		}
		for (events=0;events<FLOPS_EVS;events++) values[sets][events]=0;
		
	}
}
void start_flops_metrics()
{
	int sets,retval;
	if (!flops_supported) return;
	for (sets=0;sets<FLOPS_SETS;sets++){
		if ((retval=PAPI_start(event_sets[sets]))!=PAPI_OK){
			VERBOSE_N(0,"FP_METRICS:StartFlopsMetrics.%s",PAPI_strerror(retval));
		}
	}
}
/* Stops includes accumulate metrics */
void stop_flops_metrics(long long *flops, long long *f_operations)
{
	int sets,ev,retval;

	*flops = 0;
	if (!flops_supported) return;

	for (sets=0;sets<FLOPS_SETS;sets++)
	{
		if ((retval=PAPI_stop(event_sets[sets],(long long *)&values[sets]))!=PAPI_OK)
		{
			VERBOSE_N(0,"FP_METRICS: StopFlopsMetrics.%s",PAPI_strerror(retval));
		} else
		{
			if (sets==SP_OPS) VERBOSE_N(2,"fops_fp -->");
			if (sets==DP_OPS) VERBOSE_N(2,"fops_dp -->");

			for (ev=0; ev < FLOPS_EVS;ev++)
			{
				f_operations[sets*FLOPS_EVS+ev] = values[sets][ev];

				acum_values[sets][ev] += values[sets][ev];

				*flops += (values[sets][ev] * weights[sets][ev]);

				VERBOSE_N(2,"[%d]=%llu x %d, ", ev, values[sets][ev], weights[sets][ev]);
			}
		}
	}
}

void print_gflops(long long total_inst,unsigned long total_time,uint total_cores)
{
	int sets,ev;
	long long total=0;
	if (!flops_supported) return;
	for (sets=0;sets<FLOPS_SETS;sets++)
        {
		if (sets==SP_OPS){
			VERBOSE_N(1,"SP FOPS:");
		}else{
			VERBOSE_N(1,"DP FOPS");
		}
		for (ev=0;ev<FLOPS_EVS;ev++){
			total=total+(weights[sets][ev]*acum_values[sets][ev]);
		}
		if (total>0){

		for (ev=0;ev<FLOPS_EVS;ev++){
			VERBOSE_N(1,"[%d]=%llu x %d (%lf %%), ", ev, acum_values[sets][ev], weights[sets][ev],
			(double)(weights[sets][ev]*acum_values[sets][ev]*100)/(double)total);
		}
		}
	}
	VERBOSE_N(1,"GFlops per process = %.3lf ", (double)(total)/(double)(total_time*1000));
	VERBOSE_N(1,"GFlops per node    = %.3lf ", (double)(total*total_cores)/(double)(total_time*1000));
	
}
double gflops(unsigned long total_time,uint total_cores)
{
        int sets,ev;
        long long total=0;
	double Gflops;
        if (!flops_supported) return (double)0;
        for (sets=0;sets<FLOPS_SETS;sets++){
                for (ev=0;ev<FLOPS_EVS;ev++){
                        total=total+(weights[sets][ev]*acum_values[sets][ev]);
                }
        }
	Gflops=(double)(total*total_cores)/(double)(total_time*1000);
	return Gflops;
}

int get_number_fops_events()
{
	return (FLOPS_SETS * FLOPS_EVS);
}

void get_weigth_fops_instructions(int *weigth_vector)
{
	int sets,ev;
	for (sets=0;sets<FLOPS_SETS;sets++){
		for (ev=0;ev<FLOPS_EVS;ev++){
			weigth_vector[sets*FLOPS_EVS+ev]=weights[sets][ev];
		}
	}
}

void get_total_fops(long long *metrics)
{
	int sets, ev, i;

	if (!flops_supported) return;

	for (sets=0;sets < FLOPS_SETS; sets++)
	{
		for (ev=0;ev < FLOPS_EVS; ev++)
		{
			i = sets * FLOPS_EVS + ev;
			metrics[i] = acum_values[sets][ev];
		}
	}
}
