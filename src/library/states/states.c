/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#if MPI
#include <mpi.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <common/config.h>
#include <common/states.h>
//#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/math_operations.h>
#include <common/types/log.h>
#include <common/types/application.h>
#include <library/common/externs.h>
#include <library/common/global_comm.h>
#include <library/tracer/tracer.h>
#include <library/states/states.h>
#include <library/metrics/metrics.h>
#include <library/policies/policy.h>
#include <library/policies/policy_state.h>

#include <management/cpufreq/frequency.h>
#include <daemon/local_api/eard_api.h>
#include <common/environment.h>

extern masters_info_t masters_info;
extern float ratio_PPN;




// static defines
#define NO_PERIOD				0
#define FIRST_ITERATION			1
#define EVALUATING_LOCAL_SIGNATURE	2
#define SIGNATURE_STABLE		3
#define PROJECTION_ERROR		4
#define RECOMPUTING_N			5
#define SIGNATURE_HAS_CHANGED	6
#define TEST_LOOP		7
#define EVALUATING_GLOBAL_SIGNATURE	8

application_t *signatures;
uint *sig_ready;
// static application_t last_signature;
static projection_t *PP;

static long long comp_N_begin, comp_N_end, comp_N_time;
static uint begin_iter, N_iter;
static ulong policy_freq;
static uint tries_current_loop=0;
static uint tries_current_loop_same_freq=0;

static ulong perf_accuracy_min_time = 1000000;
static uint perf_count_period = 100,loop_perf_count_period,perf_count_period_10p;
static uint EAR_STATE = NO_PERIOD;
static uint EAR_POLICY_STATE = EAR_POLICY_NO_STATE;
static int current_loop_id;
static int MAX_POLICY_TRIES;
static state_t pst;
static uint total_th;

#define DYNAIS_CUTOFF	1

#if EAR_OVERHEAD_CONTROL
extern uint check_periodic_mode;
#endif


#define SET_VARIABLES() \
      CPI = loop_signature.signature.CPI; \
      GBS = loop_signature.signature.GBS; \
      POWER = loop_signature.signature.DC_power; \
      TPI = loop_signature.signature.TPI; \
      TIME = loop_signature.signature.time; \
			AVGF = loop_signature.signature.avg_f; \
      loop_signature.signature.def_f=prev_f; \
      VI=metrics_vec_inst(&loop_signature.signature); \
      VPI=(double)VI/(double)loop_signature.signature.instructions; \
      ENERGY = TIME * POWER; \
      EDP = ENERGY * TIME;

#if USE_GPU_LIB
#define SET_VARIABLES_GPU() \
		{ \
		int gpui; \
		GPU_POWER=0;GPU_FREQ=0;GPU_UTIL=0; \
		if (loop_signature.signature.gpu_sig.num_gpus>0){ \
		for(gpui=0;gpui<loop_signature.signature.gpu_sig.num_gpus;gpui++){ \
			GPU_POWER += loop_signature.signature.gpu_sig.gpu_data[gpui].GPU_power; \
			GPU_FREQ += loop_signature.signature.gpu_sig.gpu_data[gpui].GPU_freq; \
			GPU_UTIL += loop_signature.signature.gpu_sig.gpu_data[gpui].GPU_util; \
		} \
		GPU_FREQ = GPU_FREQ/loop_signature.signature.gpu_sig.num_gpus; \
		GPU_UTIL = GPU_UTIL/loop_signature.signature.gpu_sig.num_gpus; \
		} \
		}
#else
#define SET_VARIABLES_GPU()
#endif




#define  REPORT_TRACES() \
			if (masters_info.my_master_rank>=0){\
      	traces_new_signature(ear_my_rank, my_id, &loop.signature); \
      	traces_frequency(ear_my_rank, my_id, policy_freq); \
			}
      // traces_PP(ear_my_rank, my_id, PP->Time, PP->Power);

#define NEW_FREQ_REPORT() \
    if (masters_info.my_master_rank>=0 || show_signatures){\
      float policy_freqf;\
      policy_freqf=(float)policy_freq/1000000.0; \
      verbose(1,"\tNew freq %.1fGHz",policy_freqf);\
    }

#if USE_GPU_LIB
#define VERBOSE_SIG() \
			if (masters_info.my_master_rank>=0 || show_signatures){\
        float AVGFF,prev_ff,GPU_f; \
        AVGFF=(float)AVGF/1000000.0; \
        prev_ff=(float)prev_f/1000000.0; \
				GPU_f=(float)GPU_FREQ/1000000.0; \
        verbose(2,"EAR+D(%s) at %.2f in %s: LoopID=%lu, LoopSize=%u-%u,iterations=%d",ear_app_name, prev_ff,application.node_id,event, period, level,iterations); \
        verbosen(1,"EAR+D(%s) at %.2f in %s MR[%d]: ",ear_app_name, prev_ff,application.node_id,masters_info.my_master_rank); \
				if (GPU_UTIL) {\
        verbose(1,"(CPI=%.3lf GBS=%.2lf Power=%.2lfW Time=%.3lfsec.  AVGF=%.2fGHz)\n\t (GPU_power %.2lfW GPU_freq %.1fGHz GPU_util %lu)", CPI, GBS, POWER, TIME,  AVGFF,GPU_POWER,GPU_f,GPU_UTIL);\
				}else { \
        verbose(1,"(CPI=%.3lf GBS=%.2lf Power=%.2lfW Time=%.3lfsec.  AVGF=%.2fGHz)", CPI, GBS, POWER, TIME,  AVGFF);\
				}\
			}

#else
#define VERBOSE_SIG() \
			if (masters_info.my_master_rank>=0 || show_signatures){\
        float AVGFF,prev_ff; \
        AVGFF=(float)AVGF/1000000.0; \
        prev_ff=(float)prev_f/1000000.0; \
        verbose(2,"EAR+D(%s) at %.2f in %s: LoopID=%lu, LoopSize=%u-%u,iterations=%d",ear_app_name, prev_ff,application.node_id,event, period, level,iterations); \
        verbosen(1,"EAR+D(%s) at %.2f in %s MR[%d]: ",ear_app_name, prev_ff,application.node_id,masters_info.my_master_rank); \
        verbose(1,"(CPI=%.3lf GBS=%.2lf Power=%.2lfW Time=%.3lfsec. Energy=%.1lfJ AVGF=%.2fGHz", CPI, GBS, POWER, TIME, ENERGY, AVGFF);\
			}
#endif

void report_policy_state(int st)
{
	return;
	if (masters_info.my_master_rank >= 0){
	switch(st){
		case EAR_POLICY_READY:verbose(2,"Policy new state EAR_POLICY_READY");break;
		case EAR_POLICY_CONTINUE:verbose(2,"Policy new state EAR_POLICY_CONTINUE");break;
		case EAR_POLICY_GLOBAL_EV:verbose(2,"Policy new state EAR_POLICY_GLOBAL_EV");break;
		case EAR_POLICY_GLOBAL_READY: verbose(2,"Policy new state EAR_POLICY_GLOBAL_READY"); break;
	}
	}
}


/** This funcion must be policy dependent */
ulong select_near_freq(ulong avg)
{
	ulong near;
	ulong norm;
	norm=avg/100000;
	near=norm*100000;
	return near;
}

void states_end_job(int my_id,  char *app_name)
{
	debug("EAR(%s) Ends execution. ", app_name);
	end_log();
}

void states_begin_job(int my_id,  char *app_name)
{
	ulong	architecture_min_perf_accuracy_time;
	int i;


	signatures=(application_t *) calloc(frequency_get_num_pstates(),sizeof(application_t));
	sig_ready=(uint *)calloc(frequency_get_num_pstates(),sizeof(uint));
	for (i=0;i<frequency_get_num_pstates();i++){ 
		init_application(&signatures[i]);
		sig_ready[i]=0;
	}
	//init_application(&last_signature);
#if ONLY_MASTER
	if (my_id) return;
#endif

	/* LOOP WAS CREATED HERE BEFORE */

	perf_accuracy_min_time = get_ear_performance_accuracy();
	if (masters_info.my_master_rank>=0){
		architecture_min_perf_accuracy_time=eards_node_energy_frequency();
	}else{
		architecture_min_perf_accuracy_time=1000000;
	}
	if (architecture_min_perf_accuracy_time>perf_accuracy_min_time) perf_accuracy_min_time=architecture_min_perf_accuracy_time;
	
	EAR_STATE = NO_PERIOD;
	policy_freq = EAR_default_frequency;
	init_log();
	pst=policy_max_tries(&MAX_POLICY_TRIES);
  total_th =  get_total_resources();
  debug("Using %u total_threads",total_th);

	
}

int states_my_state()
{
	return EAR_STATE;
}

void states_begin_period(int my_id, ulong event, ulong size,ulong level)
{
	int i;
	EAR_STATE = TEST_LOOP;
	tries_current_loop=0;
	tries_current_loop_same_freq=0;


	if (loop_init(&loop,&loop_signature.job,event,size,level)!=EAR_SUCCESS){
		error("Error creating loop");
	}

	policy_loop_init(&loop.id);
	comp_N_begin = metrics_time();
	if (masters_info.my_master_rank>=0) traces_new_period(ear_my_rank, my_id, event);
	loop_with_signature = 0;
    for (i=0;i<frequency_get_num_pstates();i++){
        init_application(&signatures[i]);
        sig_ready[i]=0;
    }
}

void states_end_period(uint iterations)
{
	if (loop_with_signature)
	{
		loop.total_iterations = iterations;
		append_loop_text_file(loop_summary_path, &loop,&loop_signature.job);
		if (system_conf->report_loops){
		#if USE_DB
		if (masters_info.my_master_rank>=0){ 
			clean_db_signature(&loop.signature);
			eards_write_loop_signature(&loop);
		}
		#endif
		}
	}

	loop_with_signature = 0;
	policy_loop_end(&loop.id);
}

static void check_dynais_on(signature_t *A, signature_t *B)
{
	if (!equal_with_th(A->CPI, B->CPI, EAR_ACCEPTED_TH*2) || !equal_with_th(A->GBS, B->GBS, EAR_ACCEPTED_TH*2)){
		dynais_enabled=DYNAIS_ENABLED;
		debug("Dynais ON ");
	}
}

static void check_dynais_off(ulong mpi_calls_iter,uint period, uint level, ulong event)
{
#if EAR_OVERHEAD_CONTROL
    ulong dynais_overhead_usec=0;
    float dynais_overhead_perc;

    dynais_overhead_usec=mpi_calls_iter;
    dynais_overhead_perc=((float)dynais_overhead_usec/(float)1000000)*(float)100/loop_signature.signature.time;
    if (dynais_overhead_perc>MAX_DYNAIS_OVERHEAD){
    // Disable dynais : API is still pending
    	#if DYNAIS_CUTOFF
    	dynais_enabled=DYNAIS_DISABLED;
    	#endif
    	if (masters_info.my_master_rank>=0) log_report_dynais_off(application.job.id,application.job.step_id);
    }
	if (dynais_enabled==DYNAIS_DISABLED){
    	debug("DYNAIS_DISABLED: Total time %lf (s) dynais overhead %lu usec in %lu mpi calls(%lf percent), event=%lu min_time=%lu",
    	loop_signature.signature.time,dynais_overhead_usec,mpi_calls_iter,dynais_overhead_perc,event,perf_accuracy_min_time);
	}
    last_first_event=event;
    last_calls_in_loop=mpi_calls_iter;
    last_loop_size=period;
    last_loop_level=level;
#endif
    
}
static int policy_had_effect(signature_t *A, signature_t *B)
{
	if (equal_with_th(A->CPI, B->CPI, EAR_ACCEPTED_TH) &&
		equal_with_th(A->GBS, B->GBS, EAR_ACCEPTED_TH))
	{
		return 1;
	}
	return 0;
}

static void print_loop_signature(char *title, signature_t *loop)
{
	float avg_f = (float) loop->avg_f / 1000000.0;

	debug("(%s) Avg. freq: %.2lf (GHz), CPI/TPI: %0.3lf/%0.3lf, GBs: %0.3lf, DC power: %0.3lf, time: %0.3lf, GFLOPS: %0.3lf",
                title, avg_f, loop->CPI, loop->TPI, loop->GBS, loop->DC_power, loop->time, loop->Gflops);
}

void states_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event, ulong mpi_calls_iter);
static void report_loop_signature(uint iterations,loop_t *my_loop,job_t *job)
{
   	my_loop->total_iterations = iterations;
	#if DB_FILES
   	append_loop_text_file(loop_summary_path, my_loop,job);
	#endif
	#if USE_DB
    if (masters_info.my_master_rank>=0){ 
			clean_db_signature(&my_loop->signature);
			eards_write_loop_signature(my_loop);
		}
    #endif
}

void states_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event,ulong mpi_calls_iter)
{
	double CPI, TPI, GBS, POWER, TIME, ENERGY, EDP, VPI, IN_MPI_PERC,IN_MPI_SEC,GPU_POWER;
	ulong AVGF,GPU_FREQ,GPU_UTIL;
	unsigned long prev_f;
	int ready;
	ull VI;
	int result;
	ulong global_f=0;
	int pok;
	int curr_pstate,def_pstate;
	signature_t *l_sig;
	ulong policy_def_freq;
	signature_t app_signature;	
	uint new_signature;
	state_t st;

	/***************************************************************************************************/
	/**** This function can potentially include data sharing between masters, depends on the policy ****/
	/***************************************************************************************************/
	pst=policy_new_iteration(&loop.id);

	prev_f = ear_frequency;
	curr_pstate=frequency_closest_pstate(ear_frequency);
	pst=policy_get_default_freq(&policy_def_freq);
	def_pstate=frequency_closest_pstate(policy_def_freq);

	if (system_conf!=NULL){
	if (masters_info.my_master_rank>=0){
	if (resched_conf->force_rescheduling){
		traces_reconfiguration(ear_my_rank, my_id);
		resched_conf->force_rescheduling=0;
		verbose(1,"EAR: rescheduling forced by eard: current_state %u max freq %lu def_freq %lu def_th %lf",EAR_STATE,system_conf->max_freq,system_conf->def_freq,system_conf->settings[0]);
		EAR_STATE = SIGNATURE_HAS_CHANGED;
		comp_N_begin = metrics_time();
		traces_policy_state(ear_my_rank, my_id,SIGNATURE_HAS_CHANGED);
		tries_current_loop_same_freq=0;
    tries_current_loop=0;
		verbose(1,"New state by reschedduling %u",EAR_STATE);
		return;
	}
	}
	}

	switch (EAR_STATE)
	{
		/* This is a new state to minimize overhead, we just check the loop has , at least, some granularity */
		case TEST_LOOP:
			comp_N_end = metrics_time();
			comp_N_time = metrics_usecs_diff(comp_N_end, comp_N_begin);
			if (comp_N_time>(perf_accuracy_min_time*0.1)){
				debug("Going to FIRST_ITERATION after %d iterations",iterations);
				comp_N_begin=comp_N_end;
				EAR_STATE=FIRST_ITERATION;
				if (masters_info.my_master_rank>=0) traces_start();
			}
			break;
		case FIRST_ITERATION: /********* FIRST_ITERATION *********/
			comp_N_end = metrics_time();
			comp_N_time = metrics_usecs_diff(comp_N_end, comp_N_begin);

			if (comp_N_time == 0) comp_N_time = 1;
			// We include a dynamic configurarion of EAR
			if (comp_N_time < (long long) perf_accuracy_min_time) {
				perf_count_period = (perf_accuracy_min_time / comp_N_time) + 1;
			} else {
				perf_count_period = 1;
			}
			loop_perf_count_period=perf_count_period;
			/* Included to accelerate the signature computation */
			perf_count_period_10p=perf_count_period/10;
			if (perf_count_period_10p==0) perf_count_period_10p=1;
			/**/

			// Once min iterations is computed for performance accuracy we start computing application signature
			EAR_STATE = EVALUATING_LOCAL_SIGNATURE;
			if (masters_info.my_master_rank>=0){ 
				traces_policy_state(ear_my_rank, my_id,EVALUATING_LOCAL_SIGNATURE);
				verbose(2,"MR[%d]: Going to EVALUATING_LOCAL_SIGNATURE",masters_info.my_master_rank);
			}
			metrics_compute_signature_begin();
			begin_iter = iterations;
			
			// Loop printing algorithm
			loop.id.event = event;
			loop.id.level = level;
			loop.id.size = period;
			break;
		case SIGNATURE_HAS_CHANGED: /********* SIGNATURE_HAS_CHANGED *************/
			comp_N_end = metrics_time();
			comp_N_time = metrics_usecs_diff(comp_N_end, comp_N_begin);

			if (comp_N_time < (long long) perf_accuracy_min_time) {
				perf_count_period = (perf_accuracy_min_time / comp_N_time) + 1;
			} else {
				perf_count_period = 1;
			}                                        
			/* Included to accelerate the signature computation */
			perf_count_period_10p=perf_count_period/10;
			if (perf_count_period_10p==0) perf_count_period_10p=1;
			/**/
			loop_perf_count_period=perf_count_period;
			#if 0
			if (masters_info.my_master_rank>=0){
				verbose(1,"Going to EVALUATING_SIGNATURE perf_count_period %u- %u",perf_count_period,perf_count_period_10p);
			}
			#endif
			EAR_STATE = EVALUATING_LOCAL_SIGNATURE;
			if (masters_info.my_master_rank>=0){ 
				verbose(2,"MR[%d]: Going to EVALUATING_LOCAL_SIGNATURE",masters_info.my_master_rank);
				traces_policy_state(ear_my_rank, my_id,EVALUATING_LOCAL_SIGNATURE);
			}
			break;
		case RECOMPUTING_N:/************ RECOMPUTING_N *************/

			comp_N_time = metrics_usecs_diff(comp_N_end, comp_N_begin);

			if (comp_N_time == 0) {
				error( "EAR(%s):PANIC comp_N_time must be >0", ear_app_name);
			}

			// We include a dynamic configurarion of DPD behaviour
			if (comp_N_time < (long long) perf_accuracy_min_time) {
				perf_count_period = (perf_accuracy_min_time / comp_N_time) + 1;
			} else {
				perf_count_period = 1;
			}
			/* Included to accelerate the signature computation */
			perf_count_period_10p=perf_count_period/10;
			if (perf_count_period_10p==0) perf_count_period_10p=1;
			/**/
			loop_perf_count_period=perf_count_period;
			EAR_STATE = SIGNATURE_STABLE;
			if (masters_info.my_master_rank>=0){ 
				verbose(2,"MR[%d]: Going to SIGNATURE_STABLE period=%d iterations=%d",masters_info.my_master_rank,perf_count_period,iterations);
				traces_policy_state(ear_my_rank, my_id,SIGNATURE_STABLE);
			}
			break;
		case EVALUATING_LOCAL_SIGNATURE: /********* EVALUATING_LOCAL_SIGNATURE **********/
			/* We check from time to time if if the signature is ready */
			/* Included to accelerate the signature computation */
			if ((iterations%perf_count_period_10p)==0){
				if (time_ready_signature(perf_accuracy_min_time)){	
					perf_count_period=iterations - 1;
					if (perf_count_period==0) perf_count_period=1;
				}
			}
			if (((iterations - 1) % perf_count_period) || (iterations == 1)) return;
			N_iter = iterations - begin_iter;
			result = metrics_compute_signature_finish(&loop_signature.signature, N_iter, perf_accuracy_min_time, total_th);	
			if (result == EAR_NOT_READY)
			{
				perf_count_period++;
				return;
			}
			//verbose(1,"Signature ready for process %d time %lld",my_node_id,metrics_time());
			//print_loop_signature("signature computed", &loop_signature.signature);
      /* Included for dynais test */
      if (loop_with_signature==0){
                time_t curr_time;
                double time_from_mpi_init;
                time(&curr_time);    
                time_from_mpi_init=difftime(curr_time,application.job.start_time);
                debug("Number of seconds since the application start_time at which signature is computed %lf",time_from_mpi_init);
      }
      /* END */

			loop_with_signature = 1;
			#if EAR_OVERHEAD_CONTROL
			check_periodic_mode=0;
			#endif

			// Computing dynais overhead
			// Change dynais_enabled to ear_tracing_status=DYNAIS_ON/DYNAIS_OFF
			if (dynais_enabled==DYNAIS_ENABLED){
				check_dynais_off(mpi_calls_iter,period,level,event);
			}
			current_loop_id = event;

			SET_VARIABLES();
			SET_VARIABLES_GPU();
			begin_iter = iterations;

			// memcpy(&last_signature, &loop_signature, sizeof(application_t));
			memcpy(&signatures[curr_pstate], &loop_signature, sizeof(application_t));
			sig_ready[curr_pstate]=1;

			/* This function executes the energy policy */
			adapt_signature_to_node(&app_signature,&loop_signature.signature,ratio_PPN);
			//if (masters_info.my_master_rank>=0) verbose(1,"EVALUATING_LOCAL_SIGNATURE");
			pst=policy_node_apply(&app_signature,&policy_freq,&ready);
			report_policy_state(ready);
			/****** We mark our local signature as ready ************/
			signature_ready(&sig_shared_region[my_node_id],EVALUATING_LOCAL_SIGNATURE);
			/* For no masters, ready will be 0, pending */
			EAR_POLICY_STATE = ready;

			/* When the policy is ready to be evaluated, we go to the next state */
			if (EAR_POLICY_STATE == EAR_POLICY_READY){
			NEW_FREQ_REPORT();
			/*if (policy_freq != policy_def_freq)*/
			if (policy_freq != prev_f)
			{
					tries_current_loop++;
					comp_N_begin = metrics_time();
					EAR_STATE = RECOMPUTING_N;
					if (masters_info.my_master_rank>=0){ 
						log_report_new_freq(application.job.id,application.job.step_id,policy_freq);
						verbose(2,"MR[%d]: Going to RECOMPUTING_N",masters_info.my_master_rank);
					}
			}
			else
			{
					EAR_STATE = SIGNATURE_STABLE;
					if (masters_info.my_master_rank>=0){ 
						verbose(2,"MR[%d]: Going to SIGNATURE_STABLE",masters_info.my_master_rank);
						traces_policy_state(ear_my_rank, my_id,SIGNATURE_STABLE);
					}
			}
			}
			if (EAR_POLICY_STATE == EAR_POLICY_CONTINUE){
				/* Should we check if N needs to be recomputed ?*/
				if (masters_info.my_master_rank>=0){ 
					traces_policy_state(ear_my_rank, my_id,EVALUATING_LOCAL_SIGNATURE);
				}
			}
			if (EAR_POLICY_STATE == EAR_POLICY_GLOBAL_EV){
				EAR_STATE = EVALUATING_GLOBAL_SIGNATURE;
				if (masters_info.my_master_rank>=0){ 
					verbose(2,"MR[%d]: Going to EVALUATING_GLOBAL_SIGNATURE",masters_info.my_master_rank);
					traces_policy_state(ear_my_rank, my_id,EVALUATING_GLOBAL_SIGNATURE);
				}
			}
			signature_copy(&loop.signature, &loop_signature.signature);
		  report_loop_signature(iterations,&loop,&loop_signature.job);
			/* VERBOSE */
			VERBOSE_SIG();
			REPORT_TRACES();
			/* END VERBOSE */
			break;
		case EVALUATING_GLOBAL_SIGNATURE:
			//if (masters_info.my_master_rank>=0) verbose(1,"EVALUATING_GLOBAL_SIGNATURE");
			st = policy_app_apply(&policy_freq,&ready);
			EAR_POLICY_STATE = ready;
			if (EAR_POLICY_STATE == EAR_POLICY_READY){
			NEW_FREQ_REPORT();
			/* New scenario */
      /*if (policy_freq != policy_def_freq)*/
      if (policy_freq != prev_f)
      {   
          tries_current_loop++;
          comp_N_begin = metrics_time();
          EAR_STATE = RECOMPUTING_N;
          if (masters_info.my_master_rank>=0){ 
						verbose(2,"MR[%d]: Going to RECOMPUTING_N",masters_info.my_master_rank);
						log_report_new_freq(application.job.id,application.job.step_id,policy_freq);
					}
      }
      else
      {   
          EAR_STATE = SIGNATURE_STABLE;
          if (masters_info.my_master_rank>=0){ 
						verbose(2,"MR[%d]: Going to SIGNATURE_STABLE",masters_info.my_master_rank);
						traces_policy_state(ear_my_rank, my_id,SIGNATURE_STABLE);
					}
      }
			}
			break;
		case SIGNATURE_STABLE:

			// I have executed N iterations more with a new frequency, we must check the signature
			if (((iterations - 1) % perf_count_period) ) return;
			/* We can compute the signature */
			N_iter = iterations - begin_iter;
			result = metrics_compute_signature_finish(&loop_signature.signature, N_iter, perf_accuracy_min_time, total_th);
			if (result == EAR_NOT_READY)
			{
				perf_count_period++;
				return;
			}
      signature_ready(&sig_shared_region[my_node_id],SIGNATURE_STABLE);

			/*print_loop_signature("signature refreshed", &loop_signature.signature);*/

			SET_VARIABLES();
			SET_VARIABLES_GPU();


			signature_copy(&loop.signature, &loop_signature.signature);
			report_loop_signature(iterations,&loop,&loop_signature.job);
			/* VERBOSE */
			VERBOSE_SIG();
			REPORT_TRACES();

			/* END VERBOSE */

			
			begin_iter = iterations;

			if (sig_ready[curr_pstate]==0){
            	memcpy(&signatures[curr_pstate], &loop_signature, sizeof(application_t));
            	sig_ready[curr_pstate]=1;
			}

			/* We must evaluate policy decissions */
			//pok=policy_ok(PP, &loop_signature.signature, &last_signature.signature);
			l_sig=&signatures[def_pstate].signature;
			if (sig_ready[def_pstate]==0){
				debug("Signature at default freq not available, assuming policy ok");
				/* If default is not available, that means a dynamic configuration has been decided, we assume we are ok */
				if (sig_ready[curr_pstate]==0){
            		memcpy(&signatures[curr_pstate], &loop_signature, sizeof(application_t));
            		sig_ready[curr_pstate]=1;
				}
				return;
			}
			// We compare the projection with the signature and the old signature
			pst=policy_ok(&loop_signature.signature, l_sig,&pok);
			if (pok)
			{
				/* When collecting traces, we maintain the period */
				if (masters_info.my_master_rank>=0){
				if (traces_are_on()==0)	perf_count_period = perf_count_period * 2;
				}
				tries_current_loop=0;
				debug("Policy ok");
			}else{
				debug("Policy NOT ok");
			}
			debug("EAR(%s): CurrentSig (Time %2.3lf Power %3.2lf Energy %.2lf) LastSig (Time %2.3lf Power %3.2lf Energy %.2lf) ",
			ear_app_name, TIME, POWER, ENERGY, l_sig->time,l_sig->DC_power, (l_sig->time*l_sig->DC_power));

			if (pok) return;

			// We can give the library a second change in case we are at def freq
			if ((tries_current_loop<MAX_POLICY_TRIES) && (curr_pstate==def_pstate))
			{
				EAR_STATE = EVALUATING_LOCAL_SIGNATURE;
				if (masters_info.my_master_rank>=0){ 
					traces_policy_state(ear_my_rank, my_id,EVALUATING_LOCAL_SIGNATURE);
					verbose(2,"MR[%d]: Going to EVALUATING_LOCAL_SIGNATURE",masters_info.my_master_rank);		
				}
				return;
			}


			/* We must report a problem and go to the default configuration*/
			if (tries_current_loop>=MAX_POLICY_TRIES){
				if (masters_info.my_master_rank>=0) log_report_max_tries(application.job.id,application.job.step_id, application.job.def_f);
				EAR_STATE = PROJECTION_ERROR;
				pst=policy_get_default_freq(&policy_freq);
				if (masters_info.my_master_rank>=0){ 
					verbose(2,"MR[%d]: Going to PROJECTION_ERROR",masters_info.my_master_rank);
					traces_policy_state(ear_my_rank, my_id,PROJECTION_ERROR);
					traces_frequency(ear_my_rank, my_id, policy_freq);
				}
				return;
			}
			/** We are not going better **/
			/* If signature is different, we consider as a new loop case */
			//if (!policy_had_effect(&loop_signature.signature, &last_signature.signature))
			if (!policy_had_effect(&loop_signature.signature,l_sig))
			{
				EAR_STATE = SIGNATURE_HAS_CHANGED;
				if (masters_info.my_master_rank>=0){ 
					verbose(2,"MR[%d]: Going to SIGNATURE_HAS_CHANGED",masters_info.my_master_rank);
				}
				comp_N_begin = metrics_time();
				policy_loop_init(&loop.id);
        #if DYNAIS_CUTOFF
				check_dynais_on(&loop_signature.signature, l_sig);
        #endif
			} else {
					EAR_STATE = EVALUATING_LOCAL_SIGNATURE;
					if (masters_info.my_master_rank>=0){ 
						verbose(2,"MR[%d]: Going to EVALUATING_LOCAL_SIGNATURE",masters_info.my_master_rank);
						traces_policy_state(ear_my_rank, my_id,EVALUATING_LOCAL_SIGNATURE);
					}
			}
			break;
		case PROJECTION_ERROR:
			if (masters_info.my_master_rank>=0){
			if (traces_are_on())
			{
			/* We compute the signature just in case EAR_GUI is on */
            if (((iterations - 1) % perf_count_period) ) return;
            /* We can compute the signature */
            N_iter = iterations - begin_iter;
            result = metrics_compute_signature_finish(&loop_signature.signature, N_iter, perf_accuracy_min_time, total_th);
            if (result == EAR_NOT_READY)
            {
                perf_count_period++;
                return;
            }

            CPI = loop_signature.signature.CPI;
            GBS = loop_signature.signature.GBS;
            POWER = loop_signature.signature.DC_power;
            TPI = loop_signature.signature.TPI;
            TIME = loop_signature.signature.time;
            VI=metrics_vec_inst(&loop_signature.signature);
            VPI=(double)VI/(double)loop_signature.signature.instructions;

            ENERGY = TIME * POWER;
            EDP = ENERGY * TIME;

            /* VERBOSE */
						if (masters_info.my_master_rank>=0){
            traces_new_signature(ear_my_rank, my_id, &loop_signature.signature);
            traces_frequency(ear_my_rank, my_id, policy_freq);
						}
			}
			}
			/* We run here at default freq */
			break;
		default: break;
	}
	debug("End states_end_iteration");
}


