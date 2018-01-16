#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <hardware.h>
#include <environment.h>
#include <ear_verbose.h>

#define EAR_FLOPS_EVENTS_SETS 2
#define EAR_FLOPS_EVENTS 4
#define SP_OPS 0
#define DP_OPS 1

int ear_flops_event_sets[EAR_FLOPS_EVENTS_SETS];
long long ear_flops_acum_values[EAR_FLOPS_EVENTS_SETS][EAR_FLOPS_EVENTS];
long long ear_flops_values[EAR_FLOPS_EVENTS_SETS][EAR_FLOPS_EVENTS];

extern int ear_papi_init;
int ear_flops_perf_event_cid;
PAPI_option_t flops_attach_opt[EAR_FLOPS_EVENTS_SETS];
int flops_supported=0;

extern int (*my_omp_get_max_threads)(void);

int FP_OPS_WEIGTH[EAR_FLOPS_EVENTS_SETS][EAR_FLOPS_EVENTS]={{1,4,8,16},{1,2,4,8}};

#define FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N 		"FP_ARITH:SCALAR_DOUBLE"
#define FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N	"FP_ARITH:128B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N	"FP_ARITH:256B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N	"FP_ARITH:512B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_PACKED_SINGLE_N		"FP_ARITH:SCALAR_SINGLE"
#define FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N	"FP_ARITH:128B_PACKED_SINGLE"
#define FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N	"FP_ARITH:256B_PACKED_SINGLE"
#define FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N	"FP_ARITH:512B_PACKED_SINGLE"

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

	ear_flops_perf_event_cid=PAPI_get_component_index("perf_event");
   	 if (ear_flops_perf_event_cid<0){
   	     ear_verbose(0,"EAR(%s): perf_event component not found.Exiting:%s\n",__FILE__,PAPI_strerror(ear_flops_perf_event_cid));
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

		if ((retval=PAPI_assign_eventset_component(ear_flops_event_sets[sets],ear_flops_perf_event_cid))!=PAPI_OK){		
			ear_verbose(0,"EAR(%s): PAPI_assign_eventset_component.Exiting:%s\n",__FILE__,sets,PAPI_strerror(retval));
			exit(1);
		}
		flops_attach_opt[sets].attach.eventset=ear_flops_event_sets[sets];
 		flops_attach_opt[sets].attach.tid=getpid();
		if ((retval=PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*)&flops_attach_opt[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s): PAPI_set_opt.%s\n",__FILE__,PAPI_strerror(retval));
		}
		retval = PAPI_set_multiplex(sets);
		if ((retval == PAPI_EINVAL) && (PAPI_get_multiplex(sets) > 0)){
			ear_verbose(0,"EAR: Event set to compute FLOPS already has multiplexing enabled\n");
		}else if (retval != PAPI_OK){ 
			ear_verbose(0,"EAR: Error , event set to compute FLOPS can not be multiplexed %s\n",PAPI_strerror(retval));
		}
		ear_verbose(2,"EAR: Set %d to compute flops has been multiplexed\n",sets);
		cpu_model = get_model();
		switch(cpu_model){
			case CPU_HASWELL_X:
			case CPU_BROADWELL_X:
				break;
			case CPU_SKYLAKE_X:
				flops_supported=1;
				switch (sets){
				case SP_OPS:
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_PACKED_SINGLE_N))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				
				break;
				case DP_OPS:
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N))!=PAPI_OK){
					ear_verbose(0,"EAR: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N,PAPI_strerror(retval));
				}
				ear_verbose(2,"EAR: PAPI_DP_OPS added to set %d\n",sets);
				break;
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
			ear_verbose(0,"EAR(%s): ResetFlopsMetrics.%s\n",__FILE__,PAPI_strerror(retval));
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
			ear_verbose(0,"EAR(%s):StartFlopsMetrics.%s\n",__FILE__,PAPI_strerror(retval));
		}
	}
}
/* Stops includes accumulate metrics */
void stop_flops_metrics(long long *flops)
{
	int sets,ev,retval;
	if (!flops_supported) return;
	*flops=0;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		if ((retval=PAPI_stop(ear_flops_event_sets[sets],(long long *)&ear_flops_values[sets]))!=PAPI_OK){
			ear_verbose(0,"EAR(%s) StopFlopsMetrics.%s\n",__FILE__,PAPI_strerror(retval));
		}else{
			for (ev=0;ev<EAR_FLOPS_EVENTS;ev++){ 
				ear_flops_acum_values[sets][ev]+=ear_flops_values[sets][ev];
				*flops+=(ear_flops_values[sets][ev]*FP_OPS_WEIGTH[sets][ev]);
			}
		}
	}
}
void print_gflops(long long total_inst,unsigned long total_time)
{
	int sets,ev;
	int procs_per_node;
	long long total=0;
	if (!flops_supported) return;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		if (sets==SP_OPS){
			ear_verbose(0,"Single precision floating-point arithmetic instructions\n");
		}else{
			ear_verbose(0,"Double precision floating-point arithmetic instructions\n");
		}
		for (ev=0;ev<EAR_FLOPS_EVENTS;ev++){
			ear_verbose(0,"GFLOPS computation: set %d ev %d value %llu\n",sets,ev,ear_flops_acum_values[sets][ev]);
			total=total+(FP_OPS_WEIGTH[sets][ev]*ear_flops_acum_values[sets][ev]);
		}
	}
	procs_per_node=get_ear_total_processes()/get_ear_num_nodes();
	ear_verbose(0,"GFlops per process = %.3lf \n", (double)(total)/(double)(total_time*1000));
	if (my_omp_get_max_threads!=NULL){ 	
		ear_verbose(0,"GFlops per node    = %.3lf \n", (double)(total*procs_per_node*my_omp_get_max_threads())/(double)(total_time*1000));
	}else{ 
		ear_verbose(0,"GFlops per node    = %.3lf \n", (double)(total*procs_per_node)/(double)(total_time*1000));
	}
	
}
double gflops(unsigned long total_time)
{
        int sets,ev;
        int procs_per_node;
        long long total=0;
	double Gflops;
        if (!flops_supported) return;
        for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
                for (ev=0;ev<EAR_FLOPS_EVENTS;ev++){
                        total=total+(FP_OPS_WEIGTH[sets][ev]*ear_flops_acum_values[sets][ev]);
                }
        }
        procs_per_node=get_ear_total_processes()/get_ear_num_nodes();
        if (my_omp_get_max_threads!=NULL){
	 	Gflops=(double)(total*procs_per_node*my_omp_get_max_threads())/(double)(total_time*1000);
        }else{ 
		Gflops=(double)(total*procs_per_node)/(double)(total_time*1000);
	}
	return Gflops;
}

