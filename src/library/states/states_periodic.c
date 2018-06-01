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
#include <common/ear_verbose.h>
#include <common/types/log.h>
#include <common/types/application.h>
#include <common/types/loop.h>
#include <common/states.h>
#include <common/math_operations.h>

static const char *__NAME__ = "STATES_PERIOD";

// static defines
#define FIRST_ITERATION			1
#define EVALUATING_SIGNATURE	2

static projection_t *PP;
static ulong policy_freq;
static int current_loop_id;
static ulong perf_accuracy_min_time = 1000000;
static uint EAR_STATE ;

void states_periodic_end_job(int my_id, FILE *ear_fd, char *app_name)
{
	if (my_id) return;
	ear_verbose(1, "EAR(%s) Ends execution. \n", app_name);
	end_log();
}

void states_periodic_begin_job(int my_id, FILE *ear_fd, char *app_name)
{
	ulong   architecture_min_perf_accuracy_time;
	if (my_id) return;
	policy_freq = EAR_default_frequency;
    perf_accuracy_min_time = get_ear_performance_accuracy();
    architecture_min_perf_accuracy_time=eards_node_energy_frequency();
    if (architecture_min_perf_accuracy_time>perf_accuracy_min_time) perf_accuracy_min_time=architecture_min_perf_accuracy_time;
}

void states_periodic_begin_period(int my_id, FILE *ear_fd, unsigned long event, unsigned int size)
{
	ear_verbose(4, "EAR(%s): ________BEGIN_PERIOD: Computing N for period %lu size %u_____BEGIN_____\n",
					ear_app_name, event, size);

	policy_new_loop();
	traces_new_period(ear_my_rank, my_id, event);
	loop_with_signature = 0;
	EAR_STATE=FIRST_ITERATION;
}

void states_periodic_end_period(uint iterations)
{
	if (loop_with_signature)
	{
		loop.total_iterations = iterations;
		#if DB_FILES
		append_loop_text_file(loop_summary_path, &loop);
		#endif
		#if DB_MYSQL
		eards_write_loop_signature(&loop);
		#endif
	}

	loop_with_signature = 0;
	policy_end_loop();
}


static void print_loop_signature(char *title, signature_t *loop)
{
	float avg_f = (float) loop->avg_f / 1000000.0;

	VERBOSE_N(2, "(%s) Avg. freq: %.2lf (GHz), CPI/TPI: %0.3lf/%0.3lf, GBs: %0.3lf, DC power: %0.3lf, time: %0.3lf, GFLOPS: %0.3lf",
                title, avg_f, loop->CPI, loop->TPI, loop->GBS, loop->DC_power, loop->time, loop->Gflops);
}

void states_periodic_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event, ulong mpi_calls_iter);
static void report_loop_signature(uint iterations,loop_t *loop)
{
   loop->total_iterations = iterations;
   #if DB_FILES
   append_loop_text_file(loop_summary_path, loop);
	#endif
	#if DB_MYSQL
    eards_write_loop_signature(loop);
    #endif
	
	
}

/*
*	MAIN FUNCTION: When EAR is executed in periodic mode, only first itereation and evaluating signature states are applied
*
*/
void states_periodic_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event,ulong mpi_calls_iter)
{
	double CPI, TPI, GBS, POWER, TIME, ENERGY, EDP;
	unsigned long prev_f;
	int result;
	uint N_iter;

	prev_f = ear_frequency;

	#if SHARED_MEMORY
	if (system_conf->force_rescheduling){
		ear_verbose(0,"EAR: rescheduling forced by eard: max freq %lu new min_time_th %lf\n",system_conf->max_freq,system_conf->th);

		// We set the default number of iterations to the default for this loop
		system_conf->force_rescheduling=0;
	}
	#endif

	switch (EAR_STATE)
	{
		case FIRST_ITERATION:
				EAR_STATE = EVALUATING_SIGNATURE;
				metrics_compute_signature_begin();
				// Loop printing algorithm
				loop.id.event = event;
				loop.id.level = level;
				loop.id.size = period;
				break;
		case EVALUATING_SIGNATURE:
				N_iter=1;
				
				result = metrics_compute_signature_finish(&loop_signature.signature, N_iter, perf_accuracy_min_time, loop_signature.job.procs);	

				if (result == EAR_NOT_READY)
				{
					VERBOSE_N(0,"We must recompute number of mpi calls in period\n");
				}
				else
				{
					//print_loop_signature("signature computed", &loop_signature.signature);

					loop_with_signature = 1;
					current_loop_id = event;

					CPI = loop_signature.signature.CPI;
					GBS = loop_signature.signature.GBS;
					POWER = loop_signature.signature.DC_power;
					TPI = loop_signature.signature.TPI;
					TIME = loop_signature.signature.time;

					ENERGY = TIME * POWER;
					EDP = ENERGY * TIME;
					policy_freq = policy_power(0, &loop_signature.signature);
					PP = performance_projection(policy_freq);
					loop_signature.signature.def_f=prev_f;
					if (policy_freq != prev_f){
						log_report_new_freq(application.job.id,application.job.step_id,policy_freq);
					}

					traces_new_signature(ear_my_rank, my_id, TIME, CPI, TPI, GBS, POWER);
					traces_frequency(ear_my_rank, my_id, policy_freq);
					traces_PP(ear_my_rank, my_id, PP->Time, PP->CPI, PP->Power);

					if (policy_freq != prev_f)
					{
						ear_verbose(1,
									"\n\nEAR(%s) at %u: LoopID=%u, LoopSize=%u,iterations=%d\n\t\tAppplication Signature (CPI=%.5lf GBS=%.3lf Power=%.3lf Time=%.5lf Energy=%.3lfJ EDP=%.5lf)--> New frequency selected %u\n",
									ear_app_name, prev_f, event, period, iterations, CPI, GBS, POWER, TIME, ENERGY, EDP,
									policy_freq);
					} else {
						ear_verbose(1,
									"\n\nEAR(%s) at %u: LoopID=%u, LoopSize=%u-%u,iterations=%d\n\t\t Application Signature (CPI=%.5lf GBS=%.3lf Power=%.3lf Time=%.5lf Energy=%.3lfJ EDP=%.5lf)\n",
									ear_app_name, prev_f, event, period, level, iterations, CPI, GBS, POWER, TIME,
									ENERGY, EDP);
					}

					// Loop printing algorithm
					copy_signature(&loop.signature, &loop_signature.signature);
					report_loop_signature(iterations,&loop);
				}
			break;
		default: break;
	}
}


