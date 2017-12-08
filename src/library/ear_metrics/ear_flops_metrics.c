/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ear_verbose.h>
#include <intel_model_list.h>
#include <ear_arch_type.h>

#define EAR_FLOPS_EVENTS_SETS 1
#define EAR_FLOPS_EVENTS 2
#define SP_OPS 0
#define DP_OPS 1
int ear_flops_event_sets[EAR_FLOPS_EVENTS_SETS];
long long ear_flops_acum_values[EAR_FLOPS_EVENTS_SETS][EAR_FLOPS_EVENTS];
long long ear_flops_values[EAR_FLOPS_EVENTS_SETS][EAR_FLOPS_EVENTS];

extern int ear_papi_init;
int ear_turbo_perf_event_cid;
PAPI_attach_option_t turbo_attach_opt[EAR_FLOPS_EVENTS_SETS];
int flops_supported=0;

void init_flops_metrics()
{
	int retval;
	int sets;
	int events;
	int cpu_model;
	// PAPI should be already initialized, but just in case
	if (!ear_papi_init){
		if (PAPI_is_initialized()==PAPI_NOT_INITED){
    		retval=PAPI_library_init(PAPI_VER_CURRENT );
    		if ( retval != PAPI_VER_CURRENT ) {
        		ear_verbose(0,"EAR(%s): Error intializing the PAPI library.Exiting:%s\n",__FILE__,PAPI_strerror(retval));
        		exit(1);
    		}    
		}
		ear_papi_init=1;
	}
	// Here , papi is initialized

	ear_turbo_perf_event_cid=PAPI_get_component_index("perf_event");
   	 if (ear_turbo_perf_event_cid<0){
   	     ear_verbose(0,"EAR(%s): perf_event component not found.Exiting:%s\n",__FILE__,PAPI_strerror(ear_turbo_perf_event_cid));
   	     exit(1);
   	 }

	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		/* Event values set to 0 */
		for (events=0;events<EAR_FLOPS_EVENTS;events++) {
			ear_flops_acum_values[sets][events]=0;
			ear_flops_values[sets][events]=0;
		}
		/* Init event sets */
    	ear_flops_event_sets[sets]=PAPI_NULL;
        if ((retval=PAPI_create_eventset(&ear_flops_event_sets[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s): Creating %d eventset.Exiting.Exiting:%s\n",__FILE__,sets,PAPI_strerror(retval));
			exit(1);
		}
		retval = PAPI_set_multiplex(sets);
		if ((retval == PAPI_EINVAL) && (PAPI_get_multiplex(sets) > 0)){
			ear_verbose(0,"EAR: Event set to compute FLOPS already has multiplexing enabled\n");
		}else if (retval != PAPI_OK){ 
			ear_verbose(0,"EAR: Error , event set to compute FLOPS can not be multiplexed %s\n",PAPI_strerror(retval));
		}

		if ((retval=PAPI_assign_eventset_component(ear_flops_event_sets[sets],ear_turbo_perf_event_cid))!=PAPI_OK){		
			ear_verbose(0,"EAR(%s): PAPI_assign_eventset_component.Exiting:%s\n",__FILE__,sets,PAPI_strerror(retval));
			exit(1);
		}
		turbo_attach_opt[sets].eventset=ear_flops_event_sets[sets];
 		turbo_attach_opt[sets].tid=getpid();
		if ((retval=PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*)&turbo_attach_opt[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s): PAPI_set_opt.%s\n",__FILE__,PAPI_strerror(retval));
		}
		cpu_model = get_model();
		switch(cpu_model){
			case CPU_HASWELL_X:
			case CPU_BROADWELL_X:
				break;
			case CPU_SKYLAKE_X:
				flops_supported=1;
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],"PAPI_SP_OPS"))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event FP_SP_OPS.%s\n",PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],"PAPI_DP_OPS"))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event FP_DP_OPS.%s\n",PAPI_strerror(retval));
				}
		}
		
    }
	if (flops_supported){ ear_verbose(2,"EAR:Computation of Flops initialized\n");
	}else ear_verbose(2,"EAR: Computation of Flops not supported\n");
	
}
void reset_flops_metrics()
{
	int sets,events,retval;
	if (!flops_supported) return;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		if ((retval=PAPI_reset(ear_flops_event_sets[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s): ResetTurboMetrics.%s\n",__FILE__,PAPI_strerror(retval));
		}
		for (events=0;events<EAR_FLOPS_EVENTS;events++) ear_flops_values[sets][events]=0;
		
	}
}
void start_flops_metrics()
{
	int sets,retval;
	if (!flops_supported) return;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		if ((retval=PAPI_start(ear_flops_event_sets[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s):StartTurboMetrics.%s\n",__FILE__,PAPI_strerror(retval));
		}
	}
}
/* Stops includes accumulate metrics */
void stop_flops_metrics(long long *flops)
{
	int sets,retval;
	if (!flops_supported) return;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		if ((retval=PAPI_stop(ear_flops_event_sets[sets],(long long *)&ear_flops_values[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s) StopTurboMetrics.%s\n",__FILE__,PAPI_strerror(retval));
		}else{	
			ear_flops_acum_values[sets][SP_OPS]+=ear_flops_values[sets][SP_OPS];
			ear_flops_acum_values[sets][DP_OPS]+=ear_flops_values[sets][DP_OPS];
		}
		*flops=ear_flops_values[sets][SP_OPS]+ear_flops_values[sets][DP_OPS];
	}
}
void print_gflops(long long total_inst,unsigned long total_time)
{
	int sets,events;
	if (!flops_supported) return;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		ear_verbose(0,"EAR: Computation of GFLOPS=%.3lf: SP_FLOPS %llu DP_FLOPS %llu (%llu total inst)\n",
		(double)(ear_flops_acum_values[sets][SP_OPS]+ear_flops_acum_values[sets][DP_OPS])/(double)total_time,
		ear_flops_acum_values[sets][SP_OPS],
		ear_flops_acum_values[sets][DP_OPS],
		total_inst);
	}
}
double gflops(unsigned long total_time)
{
	return (double)(ear_flops_acum_values[0][SP_OPS]+ear_flops_acum_values[0][DP_OPS])/(double)total_time;
}

