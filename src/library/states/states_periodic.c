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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
//#define SHOW_DEBUGS 1
#include <common/config.h>
#include <common/states.h>
#include <common/colors.h>
#include <common/types/log.h>
#include <common/types/loop.h>
#include <common/types/application.h>
#include <common/output/verbose.h>
#include <common/math_operations.h>
#include <library/models/models.h>
#include <library/policies/policy.h>
#include <library/tracer/tracer.h>
#include <library/states/states.h>
#include <library/common/externs.h>
#include <library/common/global_comm.h>
#include <library/metrics/metrics.h>
#include <management/cpufreq/frequency.h>
#include <daemon/local_api/eard_api.h>
#include <library/policies/policy_state.h>
#include <library/states/states_comm.h>


extern uint mpi_calls_in_period;
extern masters_info_t masters_info;
extern float ratio_PPN;

static projection_t *PP;
static ulong policy_freq;
static int current_loop_id;
static ulong perf_accuracy_min_time = 1000000;
static uint EAR_STATE ;
static uint EAR_POLICY_STATE = EAR_POLICY_NO_STATE;
static ulong global_f;
static loop_id_t periodic_loop;
static uint total_th;


void states_periodic_end_job(int my_id, FILE *ear_fd, char *app_name)
{
	if (my_id) return;
	debug("EAR(%s) Ends execution. \n", app_name);
	end_log();
}

void states_periodic_begin_job(int my_id, FILE *ear_fd, char *app_name)
{
	ulong   architecture_min_perf_accuracy_time;
	if (my_id) return;
	policy_freq = EAR_default_frequency;
    perf_accuracy_min_time = get_ear_performance_accuracy();
    if (masters_info.my_master_rank>=0) {
			architecture_min_perf_accuracy_time=eards_node_energy_frequency();
		}else{	
			architecture_min_perf_accuracy_time=1000000;
		}
    if (architecture_min_perf_accuracy_time>perf_accuracy_min_time) perf_accuracy_min_time=architecture_min_perf_accuracy_time;
	periodic_loop.event=1;
	periodic_loop.size=1;
	periodic_loop.level=1;
	total_th =  get_total_resources();
}

void states_periodic_begin_period(int my_id, FILE *ear_fd, unsigned long event, unsigned int size)
{
  if (loop_init(&loop,&loop_signature.job,event,size,1)!=EAR_SUCCESS){
    error("Error creating loop");
  }

	policy_loop_init(&periodic_loop);
	if (masters_info.my_master_rank>=0) traces_new_period(ear_my_rank, my_id, event);
	loop_with_signature = 0;
	EAR_STATE=FIRST_ITERATION;
	
}

void states_periodic_end_period(uint iterations)
{
	if (loop_with_signature)
	{
		loop.total_iterations = iterations;
		#if DB_FILES
		append_loop_text_file(loop_summary_path, &loop,&loop_signature.job);
		#endif
		#if USE_DB
		if (masters_info.my_master_rank>=0){ 
			clean_db_loop(&loop);
			eards_write_loop_signature(&loop);
		}
		#endif
	}

	loop_with_signature = 0;
	policy_loop_end(&periodic_loop);
}



void states_periodic_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event, ulong mpi_calls_iter);
static void report_loop_signature(uint iterations,loop_t *loop)
{
   loop->total_iterations = iterations;
   #if DB_FILES
   append_loop_text_file(loop_summary_path, loop,&loop_signature.job);
	#endif
	#if USE_DB
    if (masters_info.my_master_rank>=0){ 
			clean_db_loop(loop);
			eards_write_loop_signature(loop);
		}
    #endif
	
	
}

/*
*	MAIN FUNCTION: When EAR is executed in periodic mode, only first itereation and evaluating signature states are applied
*
*/
void states_periodic_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event,ulong mpi_calls_iter)
{
	unsigned long long VI;
	unsigned long prev_f;
	int result;
	uint N_iter;
	int ready;
	state_t st;
	float AVGFF,prev_ff,policy_freqf;
	char gpu_buff[512];

	prev_f = ear_frequency;
	st=policy_new_iteration(&periodic_loop);
	if (resched_conf->force_rescheduling)
	{
		if (masters_info.my_master_rank>=0) traces_reconfiguration(ear_my_rank, my_id);
		debug("EAR: rescheduling forced by eard: max freq %lu new min_time_th %lf\n",
					system_conf->max_freq, system_conf->th);

		// We set the default number of iterations to the default for this loop
		resched_conf->force_rescheduling = 0;
	}

	switch (EAR_STATE)
	{
		case FIRST_ITERATION:
				EAR_STATE = EVALUATING_LOCAL_SIGNATURE;
				state_report_traces_state(masters_info.my_master_rank,ear_my_rank, my_id,EVALUATING_LOCAL_SIGNATURE);
				metrics_compute_signature_begin();
				// Loop printing algorithm
				loop.id.event = event;
				loop.id.level = level;
				loop.id.size = period;
				break;
		case EVALUATING_LOCAL_SIGNATURE:
			//if (masters_info.my_master_rank>=0) verbose(1,"EVALUATING_LOCAL_SIGNATURE");
				N_iter=1;
				result = metrics_compute_signature_finish(&loop_signature.signature, N_iter, perf_accuracy_min_time, total_th);	
				if (result != EAR_NOT_READY)
				{
					//print_loop_signature("signature computed", &loop_signature.signature);

					loop_with_signature = 1;
					current_loop_id = event;
		      signature_t app_signature;
      		adapt_signature_to_node(&app_signature,&loop_signature.signature,ratio_PPN);

					/* This function executes the CPU and GPU freq selection */
					st=policy_node_apply(&app_signature,&policy_freq,&ready);
					EAR_POLICY_STATE = ready;
					signature_ready(&sig_shared_region[my_node_id],EVALUATING_LOCAL_SIGNATURE);

					/* When the policy is ready to be evaluated, we go to the next state */
      		if ((EAR_POLICY_STATE == EAR_POLICY_READY) || (EAR_POLICY_STATE == EAR_POLICY_CONTINUE)){
						loop_signature.signature.def_f=prev_f;
						if (policy_freq != prev_f){
							if (masters_info.my_master_rank>=0) log_report_new_freq(application.job.id,application.job.step_id,policy_freq);
						}
					}
					if (EAR_POLICY_STATE == EAR_POLICY_GLOBAL_EV){
						EAR_STATE = EVALUATING_GLOBAL_SIGNATURE;
      		}
					signature_copy(&loop.signature, &loop_signature.signature);
					/* Verbose  */
          state_report_traces(masters_info.my_master_rank,ear_my_rank, my_id,&loop,policy_freq,EAR_STATE);
          state_verbose_signature(&loop,masters_info.my_master_rank,show_signatures,ear_app_name,application.node_id,iterations,prev_f,policy_freq,"P");
					report_loop_signature(iterations,&loop);
					/* End verbose region */
				} else{ /* Time was not enough */
					if (loop_signature.signature.time==0){
						mpi_calls_in_period=mpi_calls_in_period*2;
					}
				}
			break;
		case EVALUATING_GLOBAL_SIGNATURE:
			//if (masters_info.my_master_rank>=0) verbose(1,"EVALUATING_GLOBAL_SIGNATURE");
      	st = policy_app_apply(&policy_freq,&ready);
      	EAR_POLICY_STATE = ready;
      	if (EAR_POLICY_STATE == EAR_POLICY_READY){
					EAR_STATE = EVALUATING_LOCAL_SIGNATURE;
      		if (policy_freq != prev_f) {
          	if (masters_info.my_master_rank>=0) log_report_new_freq(application.job.id,application.job.step_id,policy_freq);
      		}
      }
      break;


		default: break;
	}
	state_report_traces_state(masters_info.my_master_rank,ear_my_rank, my_id,EAR_STATE);
}


