/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <control/frequency.h>
#include <library/common/externs.h>
#include <library/tracer/tracer.h>
#include <library/states/states.h>
#include <library/metrics/metrics.h>
#include <library/models/models.h>
#include <common/ear_verbose.h>
#include <common/types/log.h>
#include <common/types/application.h>
#include <common/states.h>
#include <common/config.h>

static const char *__NAME__ = "STATES";

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

static ulong perf_accuracy_min_time = 1000000;
static uint perf_count_period = 100,loop_perf_count_period;
static uint EAR_STATE = NO_PERIOD;
static int current_loop_id;

#define DYNAIS_CUTOFF	1


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

	perf_accuracy_min_time = get_ear_performance_accuracy();
	architecture_min_perf_accuracy_time=eards_node_energy_frequency();
	if (architecture_min_perf_accuracy_time>perf_accuracy_min_time) perf_accuracy_min_time=architecture_min_perf_accuracy_time;
	ear_verbose(1, "EARLib JOB %s STARTS EXECUTION. Performance accuracy set to (min) %lu usecs\n",
		app_name, perf_accuracy_min_time);
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

	policy_new_loop();
	comp_N_begin = metrics_time();
	traces_new_period(ear_my_rank, my_id, event);
	loop_with_signature = 0;
}

void states_end_period(uint iterations)
{
	if (loop_with_signature)
	{
		loop.iterations = iterations;
		append_loop_text_file(loop_summary_path, &loop);
	}

	loop_with_signature = 0;
	policy_end_loop();
}

static unsigned int equal_with_th(double a, double b, double th)
{
	int eq;
	if (a > b) {
		if (a < (b * (1 + th))) eq = 1;
		else eq = 0;
	} else {
		if ((a * (1 + th)) > b) eq = 1;
		else eq = 0;
	}
	return eq;
}

static int signature_has_changed(application_t *A, application_t *B)
{
	if (equal_with_th(A->CPI, B->CPI, EAR_ACCEPTED_TH) &&
		equal_with_th(A->GBS, B->GBS, EAR_ACCEPTED_TH))
	{
		return 0;
	}
	return 1;
}

static void print_loop_signature(char *title, application_t *loop)
{
	float avg_f = (float) loop->avg_f / 1000000.0;

	VERBOSE_N(2, "(%s) Avg. freq: %.2lf (GHz), CPI/TPI: %0.3lf/%0.3lf, GBs: %0.3lf, DC power: %0.3lf, time: %0.3lf, GFLOPS: %0.3lf",
                title, avg_f, loop->CPI, loop->TPI, loop->GBS, loop->DC_power, loop->time, loop->Gflops);
}

void states_new_iteration(int my_id, uint period, uint iterations, uint level, ulong event,ulong mpi_calls_iter)
{
	double CPI, TPI, GBS, POWER, TIME, ENERGY, EDP;
	unsigned long prev_f;
	int result;
	ulong dynais_overhead_usec=0;
	double dynais_overhead_perc;

	prev_f = ear_frequency;

	#if SHARED_MEMORY
	if (system_conf->force_rescheduling){
		ear_verbose(0,"EAR: rescheduling forced by eard: max freq %lu new min_time_th %lf\n",system_conf->max_freq,system_conf->th);

		// We set the default number of iterations to the default for this loop
		perf_count_period=loop_perf_count_period;
		system_conf->force_rescheduling=0;
		// If the loop was already evaluated, we force the rescheduling
		if (EAR_STATE==SIGNATURE_STABLE){ 
			ear_verbose(0,"EAR state forced to be EVALUATING_SIGNATURE because of power capping policies\n");
			EAR_STATE = EVALUATING_SIGNATURE;
		}
	}
	#endif

	switch (EAR_STATE)
	{
		case FIRST_ITERATION:
			if (iterations == 1)
			{
				ear_verbose(3, "EAR(%s): FIRST_ITERATION state LoopID %u LoopSize %u\n",
							ear_app_name, event, period);
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
				ear_verbose(3,
							"\nEAR(%s) at %u, %d iterations are considered to compute app signature...start computing\n",
							ear_app_name, prev_f, perf_count_period);

				// Once min iterations is computed for performance accuracy we start computing application signature
				EAR_STATE = EVALUATING_SIGNATURE;
				metrics_compute_signature_begin();
				begin_iter = iterations;

				ear_debug(3, "EAR(%s) FIRST_ITERATION -> EVALUATING_SIGNATURE\n", ear_app_name);
			
				// Loop printing algorithm
				loop.first_event = event;
				loop.level = level;
				loop.size = period;
			}
			break;
		case SIGNATURE_HAS_CHANGED:
			ear_debug(3, "EAR(%s): SIGNATURE_HAS_CHANGED state LoopID %u LoopSize %u iterations %d\n",
					  ear_app_name, event, period, iterations);
			comp_N_end = metrics_time();
			comp_N_time = metrics_usecs_diff(comp_N_end, comp_N_begin);

			// We include a dynamic configurarion of DPD behaviour
			if (comp_N_time < (long long) perf_accuracy_min_time) {
				perf_count_period = (perf_accuracy_min_time / comp_N_time) + 1;
			} else {
				perf_count_period = 1;
			}                                        
			loop_perf_count_period=perf_count_period;

			ear_verbose(3,
						"EAR(%s) at %u ::(SIGNATURE_HAS_CHANGED)Recomputing N because signature has changed, N set to %d\n",
						ear_app_name, prev_f, perf_count_period);
			EAR_STATE = EVALUATING_SIGNATURE;
			ear_debug(3, "EAR(%s) SIGNATURE_HAS_CHANGED -> EVALUATING_SIGNATURE\n", ear_app_name);
			break;
		case RECOMPUTING_N:
			ear_debug(3, "EAR(%s): RECOMPUTING_N state LoopID %u LoopSize %u iterations %d\n",
					  ear_app_name, event, period, iterations);

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

			ear_verbose(3, "EAR(%s) at %u ::(RECOMPUTING_N)Recomputing N because F has changed, N set to %d\n",
						ear_app_name, prev_f, perf_count_period);
			EAR_STATE = SIGNATURE_STABLE;
			ear_debug(3, "EAR(%s) RECOMPUTING_N -> SIGNATURE_STABLE\n", ear_app_name);
			break;
		case EVALUATING_SIGNATURE:
			ear_debug(3, "EAR(%s): EVALUATING_SIGNATURE state LoopID %u LoopSize %u iterations %d\n",
					  ear_app_name, event, period, iterations);

			if ((((iterations - 1) % perf_count_period) == 0) && (iterations > 1))
			{
				N_iter = iterations - begin_iter;

				ear_debug(4,"EAR(%s): getting metrics for period %d and iteration %d\n",
						  __FILE__, period, N_iter);

				result = metrics_compute_signature_finish(&loop_signature, N_iter, perf_accuracy_min_time);	

				if (result == EAR_NOT_READY)
				{
					comp_N_begin = metrics_time();
					EAR_STATE = SIGNATURE_HAS_CHANGED;
					ear_debug(3, "EAR(%s) EVALUATING_SIGNATURE -> SIGNATURE_HAS_CHANGED\n",
							  ear_app_name);
				}
				else
				{
					print_loop_signature("signature computed", &loop_signature);

					loop_with_signature = 1;

					// Computing dynais overhead
					if (dynais_enabled){
						dynais_overhead_usec=mpi_calls_iter;
						dynais_overhead_perc=((double)dynais_overhead_usec/(double)1000000)*(double)100/loop_signature.time;
						if (dynais_overhead_perc>MAX_DYNAIS_OVERHEAD){
							// Disable dynais : API is still pending
							#if DYNAIS_CUTOFF
							dynais_enabled=0;
							#endif
							VERBOSE_N(0,"Warning: Dynais is consuming too much time, DYNAIS=OFF");
							log_report_dynais_off(my_job_id);
						}
						VERBOSE_N(0,"Total time %lf (s) dynais overhead %lu usec in %lu mpi calls(%lf percent), event=%u min_time=%u",
						loop_signature.time,dynais_overhead_usec,mpi_calls_iter,dynais_overhead_perc,event,perf_accuracy_min_time);	
						last_first_event=event;
						last_calls_in_loop=mpi_calls_iter;
						last_loop_size=period;
						last_loop_level=level;
					//end dynais overhead
					}
					current_loop_id = event;

					CPI = loop_signature.CPI;
					GBS = loop_signature.GBS;
					POWER = loop_signature.DC_power;
					TPI = loop_signature.TPI;
					TIME = loop_signature.time;

					ENERGY = TIME * POWER;
					EDP = ENERGY * TIME;
					begin_iter = iterations;
					policy_freq = policy_power(0, &loop_signature);
					PP = performance_projection(policy_freq);

					if (policy_freq != prev_f)
					{
						comp_N_begin = metrics_time();
						EAR_STATE = RECOMPUTING_N;

						ear_debug(3, "EAR(%s) EVALUATING_SIGNATURE --> RECOMPUTING_N \n", ear_app_name);

						memcpy(&last_signature, &loop_signature, sizeof(application_t));
						log_report_new_freq(my_job_id,policy_freq);
					}
					else
					{
						EAR_STATE = SIGNATURE_STABLE;
						ear_debug(3, "EAR(%s) EVALUATING_SIGNATURE --> SIGNATURE_STABLE \n",
								  ear_app_name);
					}

					traces_new_signature(ear_my_rank, my_id, TIME, CPI, TPI, GBS, POWER);
					traces_frequency(ear_my_rank, my_id, policy_freq);
					traces_PP(ear_my_rank, my_id, PP->Time, PP->CPI, PP->Power);

					if (policy_freq != prev_f)
					{
						ear_verbose(0,
									"\n\nEAR(%s) at %u: LoopID=%u, LoopSize=%u,iterations=%d\n\t\tAppplication Signature (CPI=%.5lf GBS=%.3lf Power=%.3lf Time=%.5lf Energy=%.3lfJ EDP=%.5lf)--> New frequency selected %u\n",
									ear_app_name, prev_f, event, period, iterations, CPI, GBS, POWER, TIME, ENERGY, EDP,
									policy_freq);
					} else {
						ear_verbose(0,
									"\n\nEAR(%s) at %u: LoopID=%u, LoopSize=%u-%u,iterations=%d\n\t\t Application Signature (CPI=%.5lf GBS=%.3lf Power=%.3lf Time=%.5lf Energy=%.3lfJ EDP=%.5lf)\n",
									ear_app_name, prev_f, event, period, level, iterations, CPI, GBS, POWER, TIME,
									ENERGY, EDP);
					}


					// Loop printing algorithm
					copy_application(&loop.signature, &loop_signature);
				}
			}
			break;
		case SIGNATURE_STABLE:
			ear_debug(3, "EAR(%s): SIGNATURE_STABLE state LoopID=%u, LoopSize=%u,iterations=%d\n",
					  ear_app_name, event, period, iterations);

			// I have executed N iterations more with a new frequency, we must check the signature
			if (((iterations - 1) % perf_count_period) == 0)
			{
				N_iter = iterations - begin_iter;
				
				// GET power consumption for this N iterations
				ear_debug(4,"EAR(%s): getting metrics for period %d and iteration %d\n",
						  __FILE__, period, N_iter);

				result = metrics_compute_signature_finish(&loop_signature, N_iter, perf_accuracy_min_time);

				if (result == EAR_NOT_READY)
				{
					comp_N_begin = metrics_time();
					EAR_STATE = SIGNATURE_HAS_CHANGED;
					ear_verbose(3, "EAR(%s) SIGNATURE_STABLE(NULL) --> SIGNATURE_HAS_CHANGED \n", __FILE__);
				}
				else
				{
					print_loop_signature("signature refreshed", &loop_signature);

					CPI = loop_signature.CPI;
					GBS = loop_signature.GBS;
					POWER = loop_signature.DC_power;
					TPI = loop_signature.TPI;
					TIME = loop_signature.time;

					ENERGY = TIME * POWER;
					EDP = ENERGY * TIME;

					traces_new_signature(ear_my_rank, my_id, TIME, CPI, TPI, GBS, POWER);
					traces_frequency(ear_my_rank, my_id, policy_freq);
					traces_PP(ear_my_rank, my_id, PP->Time, PP->CPI, PP->Power);
					ear_verbose(1,
					"\n\nEAR(%s) at %u: LoopID=%u, LoopSize=%u-%u,iterations=%d\n\t\t Application Signature (CPI=%.5lf GBS=%.3lf Power=%.3lf Time=%.5lf Energy=%.3lfJ EDP=%.5lf)\n",
					ear_app_name, prev_f, event, period, level, iterations, CPI, GBS, POWER, TIME,
					ENERGY, EDP);

					begin_iter = iterations;
					// We compare the projection with the signature and the old signature
					PP = performance_projection(policy_freq);

					if (policy_ok(PP, &loop_signature, &last_signature))
					{
						perf_count_period = perf_count_period * 2;
						ear_verbose(3,
									"\n\nEAR(%s): Policy ok Signature (Time %lf Power %lf Energy %lf) Projection(Time %lf Power %lf Energy %lf)\n",
									ear_app_name, TIME, POWER, ENERGY, PP->Time, PP->Power, PP->Time * PP->Power);
						tries_current_loop=0;
					}
					else
					{
						tries_current_loop++;
						if (tries_current_loop==MAX_POLICY_TRIES){
							// We must report a problem and go to the default configuration
							log_report_max_tries(my_job_id,application.def_f);
							EAR_STATE = PROJECTION_ERROR;
						}else{
						/** If we are not going better **/
						ear_verbose(3,
									"\n\nEAR(%s): Policy not ok Signature (Time %lf Power %lf Energy %lf) Projection(Time %lf Power %lf Energy %lf)\n",
									ear_app_name, TIME, POWER, ENERGY, PP->Time, PP->Power, PP->Time * PP->Power);

						if (signature_has_changed(&loop_signature, &last_signature))
						{
							EAR_STATE = SIGNATURE_HAS_CHANGED;
							ear_verbose(3, "EAR(%s) SIGNATURE_STABLE --> SIGNATURE_HAS_CHANGED \n",
										ear_app_name);
							comp_N_begin = metrics_time();

							policy_new_loop();
						} else {
							EAR_STATE = EVALUATING_SIGNATURE;
						}
						}// tries_current_loop<MAX_POLICY_TRIES
				    } 
				}
			}
			break;
		case PROJECTION_ERROR:
				// Go to the default frequency: PENDING
				break;
		default: break;
	}
}



