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
#include <hardware.h>
#include <intel_model_list.h>

//#define AVX_METRICS
#define EAR_TURBO_EVENTS_SETS 1
#define EAR_TURBO_EVENTS 1
#define AVX_ALL 0
int ear_TurboEventSets[EAR_TURBO_EVENTS_SETS];
long long ear_turbo_acum_values[EAR_TURBO_EVENTS_SETS][EAR_TURBO_EVENTS];
long long ear_turbo_values[EAR_TURBO_EVENTS_SETS][EAR_TURBO_EVENTS];

extern int ear_papi_init;
int ear_turbo_perf_event_cid;
PAPI_attach_option_t turbo_attach_opt[EAR_TURBO_EVENTS_SETS];
int metrics_avx_supported=0;

void init_turbo_metrics()
{
#ifdef AVX_METRICS
	int retval;
	int cpu_model;
	int sets;
	int events;
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

	for (sets=0;sets<EAR_TURBO_EVENTS_SETS;sets++){
		/* Event values set to 0 */
		for (events=0;events<EAR_TURBO_EVENTS;events++) {
			ear_turbo_acum_values[sets][events]=0;
			ear_turbo_values[sets][events]=0;
		}
		/* Init event sets */
    	ear_TurboEventSets[sets]=PAPI_NULL;
        if ((retval=PAPI_create_eventset(&ear_TurboEventSets[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s): Creating %d eventset.Exiting.Exiting:%s\n",__FILE__,sets,PAPI_strerror(retval));
			exit(1);
		}
		if ((retval=PAPI_assign_eventset_component(ear_TurboEventSets[sets],ear_turbo_perf_event_cid))!=PAPI_OK){		
			ear_verbose(0,"EAR(%s): PAPI_assign_eventset_component.Exiting:%s\n",__FILE__,sets,PAPI_strerror(retval));
			exit(1);
		}
		turbo_attach_opt[sets].eventset=ear_TurboEventSets[sets];
 		turbo_attach_opt[sets].tid=getpid();
		if ((retval=PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*)&turbo_attach_opt[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s): PAPI_set_opt.%s\n",__FILE__,PAPI_strerror(retval));
		}
                cpu_model = get_model();
                switch(cpu_model){
                        case CPU_HASWELL_X:
				metrics_avx_supported=1;
				// AVX: Counts AVX instructions, masks:Approximate counts of AVX and AVX2 256-bit instructions, including non-arithmetic instructions, loads, and stores. May count non-AVX instructions using 256-bit operations
				if ((retval=PAPI_add_named_event(ear_TurboEventSets[sets],"AVX"))!=PAPI_OK){
					ear_verbose(0,"EAR(%s): PAPI_add_named_event AVX.%s\n",__FILE__,PAPI_strerror(retval));
				}
				break;
                        case CPU_BROADWELL_X:
                        case CPU_SKYLAKE_X:
                                break;
                }

		
    }
	ear_debug(2,"EAR(%s):METRICS ON\n",__FILE__);
	
#endif
}
void reset_turbo_metrics()
{
#ifdef AVX_METRICS
	int sets,events,retval;
	if (!metrics_avx_supported) return;
	for (sets=0;sets<EAR_TURBO_EVENTS_SETS;sets++){
		if ((retval=PAPI_reset(ear_TurboEventSets[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s): ResetTurboMetrics.%s\n",__FILE__,PAPI_strerror(retval));
		}
		for (events=0;events<EAR_TURBO_EVENTS;events++) ear_turbo_values[sets][events]=0;
		
	}
#endif
}
void start_turbo_metrics()
{
#ifdef AVX_METRICS
	int sets,retval;
	if (!metrics_avx_supported) return;
	for (sets=0;sets<EAR_TURBO_EVENTS_SETS;sets++){
		if ((retval=PAPI_start(ear_TurboEventSets[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s):StartTurboMetrics.%s\n",__FILE__,PAPI_strerror(retval));
		}
	}
#endif
}
/* Stops includes accumulate metrics */
void stop_turbo_metrics()
{
#ifdef AVX_METRICS
	int sets,retval;
	if (!metrics_avx_supported) return;
	for (sets=0;sets<EAR_TURBO_EVENTS_SETS;sets++){
		if ((retval=PAPI_stop(ear_TurboEventSets[sets],(long long *)&ear_turbo_values[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s) StopTurboMetrics.%s\n",__FILE__,PAPI_strerror(retval));
		}else{	
			ear_turbo_acum_values[sets][AVX_ALL]+=ear_turbo_values[sets][AVX_ALL];
		}
	
	}
#endif
}
void print_turbo_metrics(long long total_inst)
{
#ifdef AVX_METRICS
	int sets,events;
	if (!metrics_avx_supported) return;
	ear_verbose(0,"AVX metrics total inst %llu\n",total_inst);
	for (sets=0;sets<EAR_TURBO_EVENTS_SETS;sets++){
			for (events=0;events<EAR_TURBO_EVENTS;events++){
				ear_verbose(0,"AVX metrics set %d event %d value %llu (%2.4lf%%  of total inst)\n",sets,events,ear_turbo_acum_values[sets][events],((double)ear_turbo_acum_values[sets][events]/(double)total_inst)*100);
			}

	}
#endif
}
