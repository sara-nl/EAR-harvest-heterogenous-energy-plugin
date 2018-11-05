/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <common/config.h>
#include <control/frequency.h>
#include <library/common/externs.h>
#include <library/tracer/tracer.h>
#include <library/states/states.h>
#include <library/metrics/metrics.h>
#include <library/models/models.h>
#include <library/mpi_intercept/freq_synchro.h>
#include <common/ear_verbose.h>
#include <common/types/log.h>
#include <common/types/application.h>
#include <common/states.h>
#include <common/math_operations.h>
#include <daemon/eard_api.h>

static const char *__NAME__ = "ear/states";
extern char *__HOST__ ;

// static defines
#define NO_PERIOD				0
#define FIRST_ITERATION			1
#define EVALUATING_SIGNATURE	2
#define SIGNATURE_STABLE		3
#define PROJECTION_ERROR		4
#define RECOMPUTING_N			5
#define SIGNATURE_HAS_CHANGED	6

static application_t last_signature;
static projection_t *PP;

static long long comp_N_begin, comp_N_end, comp_N_time;
static uint begin_iter, N_iter;
static ulong policy_freq;
static uint tries_current_loop=0;
static uint tries_current_loop_same_freq=0;

static ulong perf_accuracy_min_time = 1000000;
static uint perf_count_period = 100,loop_perf_count_period;
static uint EAR_STATE = NO_PERIOD;
static int current_loop_id;

#define DYNAIS_CUTOFF	1


#if EAR_OVERHEAD_CONTROL
extern uint check_periodic_mode;
#endif

/** This funcion must be policy dependent */
ulong select_near_freq(ulong avg)
{
	ulong near;
	ulong norm;
	norm=avg/100000;
	near=norm*100000;
	return near;
}

void states_end_job(int my_id, FILE *ear_fd, char *app_name)
{
	ear_verbose(1, "EAR(%s) Ends execution. \n", app_name);
	end_log();
}

void states_begin_job(int my_id, FILE *ear_fd, char *app_name)
{
	char *verbose, *loop_time, *who;
	ulong	architecture_min_perf_accuracy_time;


	init_application(&last_signature);
	if (my_id) return;

	loop_init(&loop,&loop_signature.job);	


	perf_accuracy_min_time = get_ear_performance_accuracy();
	architecture_min_perf_accuracy_time=eards_node_energy_frequency();
	if (architecture_min_perf_accuracy_time>perf_accuracy_min_time) perf_accuracy_min_time=architecture_min_perf_accuracy_time;
	//printf("min_hw %lu min_env %lu \n",architecture_min_perf_accuracy_time,perf_accuracy_min_time);
	
	EAR_STATE = NO_PERIOD;
	policy_freq = EAR_default_frequency;
	init_log();
}

int states_my_state()
{
	return EAR_STATE;
}

void states_begin_period(int my_id, FILE *ear_fd, unsigned long event, unsigned int size)
{
	ear_verbose(4, "EAR(%s): ________BEGIN_PERIOD: Computing N for period %lu size %u_____BEGIN_____\n",
					ear_app_name, event, size);

	EAR_STATE = FIRST_ITERATION;
	tries_current_loop=0;
	tries_current_loop_same_freq=0;

	policy_new_loop();
	comp_N_begin = metrics_time();
	traces_new_period(ear_my_rank, my_id, event);
	loop_with_signature = 0;
}

void states_end_period(uint iterations)
{
	if (loop_with_signature)
	{
		loop.total_iterations = iterations;
		append_loop_text_file(loop_summary_path, &loop,&loop_signature.job);
		#if !LARGE_CLUSTERS
		#if DB_MYSQL
		eards_write_loop_signature(&loop);
		#endif
		#endif
	}

	loop_with_signature = 0;
	policy_end_loop();
}

static void check_dynais_on(signature_t *A, signature_t *B)
{
	if (!equal_with_th(A->CPI, B->CPI, EAR_ACCEPTED_TH*2) || !equal_with_th(A->GBS, B->GBS, EAR_ACCEPTED_TH*2)){
		dynais_enabled=DYNAIS_ENABLED;
		earl_verbose(1,"Dynais ON \n");
	}
}

static void check_dynais_off(ulong mpi_calls_iter,uint period, uint level, ulong event)
{
    ulong dynais_overhead_usec=0;
    float dynais_overhead_perc;

    dynais_overhead_usec=mpi_calls_iter;
    dynais_overhead_perc=((float)dynais_overhead_usec/(float)1000000)*(float)100/loop_signature.signature.time;
    if (dynais_overhead_perc>MAX_DYNAIS_OVERHEAD){
    // Disable dynais : API is still pending
    	#if DYNAIS_CUTOFF
    	dynais_enabled=DYNAIS_DISABLED;
    	#endif
	#if EAR_PERFORMANCE_TESTS
    	earl_verbose(1,"Warning: Dynais is consuming too much time, DYNAIS=%d,overhead=%lf",dynais_enabled, dynais_overhead_perc);
	#endif
    	log_report_dynais_off(application.job.id,application.job.step_id);
    }
    earl_verbose(2,"Total time %lf (s) dynais overhead %lu usec in %lu mpi calls(%lf percent), event=%u min_time=%u",
    loop_signature.signature.time,dynais_overhead_usec,mpi_calls_iter,dynais_overhead_perc,event,perf_accuracy_min_time);
    last_first_event=event;
    last_calls_in_loop=mpi_calls_iter;
    last_loop_size=period;
    last_loop_level=level;
    
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

	earl_verbose(2, "(%s) Avg. freq: %.2lf (GHz), CPI/TPI: %0.3lf/%0.3lf, GBs: %0.3lf, DC power: %0.3lf, time: %0.3lf, GFLOPS: %0.3lf",
                title, avg_f, loop->CPI, loop->TPI, loop->GBS, loop->DC_power, loop->time, loop->Gflops);
}

void states_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event, ulong mpi_calls_iter);
static void report_loop_signature(uint iterations,loop_t *my_loop,job_t *job)
{
   	my_loop->total_iterations = iterations;
	#if DB_FILES
   	append_loop_text_file(loop_summary_path, my_loop,job);
	#endif
	#if DB_MYSQL
    eards_write_loop_signature(my_loop);
    #endif
}

void states_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event,ulong mpi_calls_iter)
{
	double CPI, TPI, GBS, POWER, TIME, ENERGY, EDP;
	unsigned long prev_f;
	int result;
	ulong global_f=0;
	int pok;

	prev_f = ear_frequency;

	if (system_conf!=NULL){
	if (resched_conf->force_rescheduling){
		ear_verbose(0,"EAR: rescheduling forced by eard: max freq %lu new min_time_th %lf\n",system_conf->max_freq,system_conf->th);

		// We set the default number of iterations to the default for this loop
		perf_count_period=loop_perf_count_period;
		resched_conf->force_rescheduling=0;
		// If the loop was already evaluated, we force the rescheduling
		if (EAR_STATE==SIGNATURE_STABLE){ 
			ear_verbose(0,"EAR state forced to be EVALUATING_SIGNATURE because of power capping policies\n");
			EAR_STATE = EVALUATING_SIGNATURE;
		}
		// Should we reset these controls?
		tries_current_loop_same_freq=0;
		tries_current_loop=0;
	}
	}

	switch (EAR_STATE)
	{
		case FIRST_ITERATION:
			if (iterations == 1)
			{
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

				// Once min iterations is computed for performance accuracy we start computing application signature
				EAR_STATE = EVALUATING_SIGNATURE;
				metrics_compute_signature_begin();
				begin_iter = iterations;

			
				// Loop printing algorithm
				loop.id.event = event;
				loop.id.level = level;
				loop.id.size = period;
			}
			break;
		case SIGNATURE_HAS_CHANGED:
			comp_N_end = metrics_time();
			comp_N_time = metrics_usecs_diff(comp_N_end, comp_N_begin);

			if (comp_N_time < (long long) perf_accuracy_min_time) {
				perf_count_period = (perf_accuracy_min_time / comp_N_time) + 1;
			} else {
				perf_count_period = 1;
			}                                        
			loop_perf_count_period=perf_count_period;
			EAR_STATE = EVALUATING_SIGNATURE;
			break;
		case RECOMPUTING_N:

			comp_N_end = metrics_time();
			comp_N_time = metrics_usecs_diff(comp_N_end, comp_N_begin);

			if (comp_N_time == 0) ear_verbose(0, "EAR(%s):PANIC comp_N_time must be >0\n", ear_app_name);

			// We include a dynamic configurarion of DPD behaviour
			if (comp_N_time < (long long) perf_accuracy_min_time) {
				perf_count_period = (perf_accuracy_min_time / comp_N_time) + 1;
			} else {
				perf_count_period = 1;
			}
			loop_perf_count_period=perf_count_period;
			EAR_STATE = SIGNATURE_STABLE;
			break;
		case EVALUATING_SIGNATURE:

			if (((iterations - 1) % perf_count_period) || (iterations == 1)) return;
			N_iter = iterations - begin_iter;
			result = metrics_compute_signature_finish(&loop_signature.signature, N_iter, perf_accuracy_min_time, loop_signature.job.procs);	
			if (result == EAR_NOT_READY)
			{
				perf_count_period++;
				return;
			}
			//print_loop_signature("signature computed", &loop_signature.signature);

			loop_with_signature = 1;
			#if EAR_OVERHEAD_CONTROL
			check_periodic_mode=0;
			#if EAR_PERFORMANCE_TESTS
			earl_verbose(1,"Switching check periodic mode to %d\n",check_periodic_mode);
			#endif
			#endif

			// Computing dynais overhead
			// Change dynais_enabled to ear_tracing_status=DYNAIS_ON/DYNAIS_OFF
			if (dynais_enabled==DYNAIS_ENABLED){
				check_dynais_off(mpi_calls_iter,period,level,event);
			}
			current_loop_id = event;
			CPI = loop_signature.signature.CPI;
			GBS = loop_signature.signature.GBS;
			POWER = loop_signature.signature.DC_power;
			TPI = loop_signature.signature.TPI;
			TIME = loop_signature.signature.time;
			ENERGY = TIME * POWER;
			EDP = ENERGY * TIME;
			begin_iter = iterations;
			policy_freq = policy_power(0, &loop_signature.signature);
			PP = projection_get(frequency_freq_to_pstate(policy_freq));
			loop_signature.signature.def_f=prev_f;

			/* This function only sends selected frequency */
			if (global_synchro){
				global_frequency_selection_send(policy_freq);
			}

			if (policy_freq != prev_f)
			{
				comp_N_begin = metrics_time();
				EAR_STATE = RECOMPUTING_N;
				memcpy(&last_signature, &loop_signature, sizeof(application_t));
				log_report_new_freq(application.job.id,application.job.step_id,policy_freq);
				tries_current_loop++;
			}
			else
			{
				if (tries_current_loop_same_freq>=MAX_POLICY_TRIES){
					EAR_STATE = SIGNATURE_STABLE;
				}else tries_current_loop_same_freq++;
			}
			signature_copy(&loop.signature, &loop_signature.signature);
			/* VERBOSE */
			earl_verbose(1,"EAR(%s)at%u: LoopID=%u, LoopSize=%u-%u,iterations=%d",ear_app_name, prev_f, event, period, level,iterations);
			earl_verbose(1,"\tAppSig-POL (CPI=%.5lf GBS=%.3lf Power=%.3lf Time=%.5lf Energy=%.3lfJ EDP=%.5lf)(New Freq %u in %s)\n",
			CPI, GBS, POWER, TIME, ENERGY, EDP, policy_freq,application.node_id);

			traces_new_signature(ear_my_rank, my_id, TIME, CPI, TPI, GBS, POWER);
			traces_frequency(ear_my_rank, my_id, policy_freq);
			traces_PP(ear_my_rank, my_id, PP->Time, PP->CPI, PP->Power);
			report_loop_signature(iterations,&loop,&loop_signature.job);
			/* END VERBOSE */
			break;
		case SIGNATURE_STABLE:

			// I have executed N iterations more with a new frequency, we must check the signature
			if (((iterations - 1) % perf_count_period) ) return;
			/* We can compute the signature */
			N_iter = iterations - begin_iter;
			result = metrics_compute_signature_finish(&loop_signature.signature, N_iter, perf_accuracy_min_time, loop_signature.job.procs);
			if (result == EAR_NOT_READY)
			{
				perf_count_period++;
				return;
			}
			//print_loop_signature("signature refreshed", &loop_signature.signature);

			CPI = loop_signature.signature.CPI;
			GBS = loop_signature.signature.GBS;
			POWER = loop_signature.signature.DC_power;
			TPI = loop_signature.signature.TPI;
			TIME = loop_signature.signature.time;
			loop_signature.signature.def_f=prev_f;

			ENERGY = TIME * POWER;
			EDP = ENERGY * TIME;

			signature_copy(&loop.signature, &loop_signature.signature);
			report_loop_signature(iterations,&loop,&loop_signature.job);
			/* VERBOSE */
			traces_new_signature(ear_my_rank, my_id, TIME, CPI, TPI, GBS, POWER);
			traces_frequency(ear_my_rank, my_id, policy_freq);
			traces_PP(ear_my_rank, my_id, PP->Time, PP->CPI, PP->Power);
			earl_verbose(1,"EAR(%s)at%u: LoopID=%u, LoopSize=%u-%u,iterations=%d",ear_app_name, prev_f, event, period,level, iterations);
			earl_verbose(1,"\tAppSig-VAL (CPI=%.5lf GBS=%.3lf Power=%.3lf Time=%.5lf Energy=%.3lfJ EDP=%.5lf)(New Freq %u in %s)\n",
			CPI, GBS, POWER, TIME, ENERGY, EDP, policy_freq,application.node_id);
			/* END VERBOSE */

			begin_iter = iterations;
			// We compare the projection with the signature and the old signature
			PP = projection_get(frequency_freq_to_pstate(policy_freq));
			/* If global synchronizations are on, we get frequencies for the rest of the app */
			if (global_synchro){
				global_f=global_frequency_selection_synchro();
			}
			/* We must evaluate policy decissions */
			pok=policy_ok(PP, &loop_signature.signature, &last_signature.signature);
			if (pok)
			{
				perf_count_period = perf_count_period * 2;
				tries_current_loop=0;
				earl_verbose(1,"Policy ok");
			}else{
				earl_verbose(1,"Policy NOT ok");
			}
			earl_verbose(1,"EAR(%s): CurrentSig (Time %lf Power %lf Energy %lf) LastSig (Time %lf Power %lf Energy %lf) \n",
			ear_app_name, TIME, POWER, ENERGY, last_signature.signature.time,last_signature.signature.DC_power,
			(last_signature.signature.time*last_signature.signature.DC_power));
			if (pok) return;
			/* If policy is not ok and we are not running with the same freq, we can check again */
			if (global_synchro){		
				earl_verbose(1,"Global synchro on: local freq %lu and global %lu",policy_freq,global_f);
				if ((global_f) && (global_f!=policy_freq)){
					policy_freq=select_near_freq(global_f);
					force_global_frequency(policy_freq);
					earl_verbose(0,"trying with same frequency %lu",policy_freq);
					tries_current_loop++;
					return;
				}
			}
			/* We must report a problem and go to the default configuration*/
			if (tries_current_loop>=MAX_POLICY_TRIES){
				log_report_max_tries(application.job.id,application.job.step_id, application.job.def_f);
				EAR_STATE = PROJECTION_ERROR;
				policy_default_configuration();
				return;
			}
			/** We are not going better **/
			/* If signature is different, we consider as a new loop case */
			if (!policy_had_effect(&loop_signature.signature, &last_signature.signature))
			{
				EAR_STATE = SIGNATURE_HAS_CHANGED;
				comp_N_begin = metrics_time();
				policy_new_loop();
                        	#if DYNAIS_CUTOFF
				check_dynais_on(&loop_signature.signature, &last_signature.signature);
                        	#endif
				} else {
					EAR_STATE = EVALUATING_SIGNATURE;
				}
			break;
		case PROJECTION_ERROR:
			/* We run here at default freq */
			break;
		default: break;
	}
}


