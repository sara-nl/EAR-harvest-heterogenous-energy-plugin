#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ear_verbose.h>
#include <environment.h>
#include <hardware.h>
#include <externs.h>

#define EAR_FLOPS_EVENTS_SETS 2
#define EAR_FLOPS_EVENTS 4
#define SP_OPS 0
#define DP_OPS 1

static int ear_flops_event_sets[EAR_FLOPS_EVENTS_SETS];
static long long ear_flops_acum_values[EAR_FLOPS_EVENTS_SETS][EAR_FLOPS_EVENTS];
static long long ear_flops_values[EAR_FLOPS_EVENTS_SETS][EAR_FLOPS_EVENTS];
static int ear_flops_perf_event_cid;
static PAPI_option_t flops_attach_opt[EAR_FLOPS_EVENTS_SETS];
static int flops_supported=0;

static int FP_OPS_WEIGTH[EAR_FLOPS_EVENTS_SETS][EAR_FLOPS_EVENTS]={{1,4,8,16},{1,2,4,8}};

#define FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N 		"FP_ARITH:SCALAR_DOUBLE"
#define FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N	"FP_ARITH:128B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N	"FP_ARITH:256B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N	"FP_ARITH:512B_PACKED_DOUBLE"
#define FP_ARITH_INST_RETIRED_PACKED_SINGLE_N		"FP_ARITH:SCALAR_SINGLE"
#define FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N	"FP_ARITH:128B_PACKED_SINGLE"
#define FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N	"FP_ARITH:256B_PACKED_SINGLE"
#define FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N	"FP_ARITH:512B_PACKED_SINGLE"

int init_flops_metrics()
{
	int retval;
	int sets;
	int events;
	int cpu_model;
	if (PAPI_is_initialized()==PAPI_NOT_INITED){
    		retval=PAPI_library_init(PAPI_VER_CURRENT );
    		if ( retval != PAPI_VER_CURRENT ) {
        		ear_verbose(0,"FP_METRICS: Error intializing the PAPI library.%s\n",PAPI_strerror(retval));
			return flops_supported;			
    		}    
		PAPI_multiplex_init();
	}
	// Here , papi is initialized

	ear_flops_perf_event_cid=PAPI_get_component_index("perf_event");
   	 if (ear_flops_perf_event_cid<0){
   	     ear_verbose(0,"FP_METRICS: perf_event component not found.%s\n",PAPI_strerror(ear_flops_perf_event_cid));
	     return flops_supported;
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
			ear_verbose(0,"FP_METRICS: Creating %d eventset.%s\n",sets,PAPI_strerror(retval));
			return flops_supported;
		}

		if ((retval=PAPI_assign_eventset_component(ear_flops_event_sets[sets],ear_flops_perf_event_cid))!=PAPI_OK){		
			ear_verbose(0,"FP_METRICS: PAPI_assign_eventset_component.%s\n",sets,PAPI_strerror(retval));
			return flops_supported;
		}
		flops_attach_opt[sets].attach.eventset=ear_flops_event_sets[sets];
 		flops_attach_opt[sets].attach.tid=getpid();
		if ((retval=PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*)&flops_attach_opt[sets]))!=PAPI_OK){
			ear_verbose(0,"FP_METRICS: PAPI_set_opt.%s\n",__FILE__,PAPI_strerror(retval));
		}
		retval = PAPI_set_multiplex(ear_flops_event_sets[sets]);
		if ((retval == PAPI_EINVAL) && (PAPI_get_multiplex(ear_flops_event_sets[sets]) > 0)){
			ear_verbose(0,"FP_METRICS: Event set to compute FLOPS already has multiplexing enabled\n");
		}else if (retval != PAPI_OK){ 
			ear_verbose(0,"FP_METRICS: Error , event set to compute FLOPS can not be multiplexed %s\n",PAPI_strerror(retval));
		}
		ear_verbose(2,"FP_METRICS: Set %d to compute flops has been multiplexed\n",ear_flops_event_sets[sets]);
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
					ear_verbose(0,"FP_METRICS: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N))!=PAPI_OK){
					ear_verbose(0,"FP_METRICS: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N))!=PAPI_OK){
					ear_verbose(0,"FP_METRICS: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N))!=PAPI_OK){
					ear_verbose(0,"FP_METRICS: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE_N,PAPI_strerror(retval));
				}
				
				break;
				case DP_OPS:
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N))!=PAPI_OK){
					ear_verbose(0,"FP_METRICS: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_SCALAR_DOUBLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N))!=PAPI_OK){
					ear_verbose(0,"FP_METRICS: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N))!=PAPI_OK){
					ear_verbose(0,"FP_METRICS: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE_N,PAPI_strerror(retval));
				}
				if ((retval=PAPI_add_named_event(ear_flops_event_sets[sets],FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N))!=PAPI_OK){
					ear_verbose(0,"FP_METRICS: PAPI_add_named_event %s.%s\n",FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE_N,PAPI_strerror(retval));
				}
				ear_verbose(2,"FP_METRICS: PAPI_DP_OPS added to set %d\n",sets);
				break;
				}
		}
		
    }
	if (flops_supported){ ear_verbose(2,"FP_METRICS:Computation of Flops initialized\n");
	}else ear_verbose(2,"FP_METRICS: Computation of Flops not supported\n");
	return flops_supported;

	
}
void reset_flops_metrics()
{
	int sets,events,retval;
	if (!flops_supported) return;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		if ((retval=PAPI_reset(ear_flops_event_sets[sets]))!=PAPI_OK){
			ear_verbose(0,"FP_METRICS: ResetFlopsMetrics.%s\n",PAPI_strerror(retval));
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
			ear_verbose(0,"FP_METRICS:StartFlopsMetrics.%s\n",PAPI_strerror(retval));
		}
	}
}
/* Stops includes accumulate metrics */
void stop_flops_metrics(long long *flops,long long *f_operations)
{
	int sets,ev,retval;
	if (!flops_supported) return;
	*flops=0;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		if ((retval=PAPI_stop(ear_flops_event_sets[sets],(long long *)&ear_flops_values[sets]))!=PAPI_OK){
			ear_verbose(0,"FP_METRICS: StopFlopsMetrics.%s\n",PAPI_strerror(retval));
		}else{
			if (sets==SP_OPS) ear_verbose(2,"fops_fp -->");
			if (sets==DP_OPS) ear_verbose(2,"fops_dp -->");
			for (ev=0;ev<EAR_FLOPS_EVENTS;ev++){ 
				f_operations[sets*EAR_FLOPS_EVENTS+ev]=ear_flops_values[sets][ev];
				ear_flops_acum_values[sets][ev]+=ear_flops_values[sets][ev];
				*flops+=(ear_flops_values[sets][ev]*FP_OPS_WEIGTH[sets][ev]);
				ear_verbose(2,"[%d]=%llu x %d, ",ev,ear_flops_values[sets][ev],FP_OPS_WEIGTH[sets][ev]);
			}
			ear_verbose(2,"\n");
		}
	}
	ear_verbose(2,"\n");
}
void print_gflops(long long total_inst,unsigned long total_time,uint total_cores)
{
	int sets,ev;
	int procs_per_node;
	long long total=0;
	if (!flops_supported) return;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		if (sets==SP_OPS){
			ear_verbose(1,"SP FOPS:");
		}else{
			ear_verbose(1,"DP FOPS");
		}
		for (ev=0;ev<EAR_FLOPS_EVENTS;ev++){
			total=total+(FP_OPS_WEIGTH[sets][ev]*ear_flops_acum_values[sets][ev]);
		}
		if (total>0){
		for (ev=0;ev<EAR_FLOPS_EVENTS;ev++){
			ear_verbose(1,"[%d]=%llu x %d (%lf \%), ",ev,ear_flops_acum_values[sets][ev],FP_OPS_WEIGTH[sets][ev],
			(double)(FP_OPS_WEIGTH[sets][ev]*ear_flops_acum_values[sets][ev]*100)/(double)total);
		}
		}
		ear_verbose(1,"\n");
	}
	ear_verbose(1,"GFlops per process = %.3lf \n", (double)(total)/(double)(total_time*1000));
	ear_verbose(1,"GFlops per node    = %.3lf \n", (double)(total*total_cores)/(double)(total_time*1000));
	
}
double gflops(unsigned long total_time,uint total_cores)
{
        int sets,ev;
        int resources;
        long long total=0;
	double Gflops;
        if (!flops_supported) return (double)0;
        for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
                for (ev=0;ev<EAR_FLOPS_EVENTS;ev++){
                        total=total+(FP_OPS_WEIGTH[sets][ev]*ear_flops_acum_values[sets][ev]);
                }
        }
	Gflops=(double)(total*total_cores)/(double)(total_time*1000);
	return Gflops;
}

int get_number_fops_events()
{
	return (EAR_FLOPS_EVENTS_SETS*EAR_FLOPS_EVENTS);
}

void get_weigth_fops_instructions(int *weigth_vector)
{
	int sets,ev;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		for (ev=0;ev<EAR_FLOPS_EVENTS;ev++){
			weigth_vector[sets*EAR_FLOPS_EVENTS+ev]=FP_OPS_WEIGTH[sets][ev];
		}
	}
}
void get_total_fops(long long *metrics)
{
	int sets,ev;
	if (!flops_supported) return;
	for (sets=0;sets<EAR_FLOPS_EVENTS_SETS;sets++){
		for (ev=0;ev<EAR_FLOPS_EVENTS;ev++){
			metrics[sets*EAR_FLOPS_EVENTS+ev]=ear_flops_acum_values[sets][ev];
		}
	}
	
}