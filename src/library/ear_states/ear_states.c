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

#include <library/common/externs.h>
#include <library/ear_gui/ear_gui.h>
#include <library/ear_states/ear_states.h>
#include <library/ear_frequency/ear_cpufreq.h>
#include <library/ear_metrics/ear_metrics.h>
#include <library/ear_models/ear_models.h>
#include <common/ear_verbose.h>
#include <common/states.h>

// static defines
#define NO_PERIOD				0
#define FIRST_ITERATION			1
#define EVALUATING_SIGNATURE	2
#define SIGNATURE_STABLE		3
#define PROJECTION_ERROR		4
#define RECOMPUTING_N			5
#define SIGNATURE_HAS_CHANGED	6
#define DPD_NUM_STATES			7

static application_t last_signature;
static projection_t *PP;

static long long comp_N_begin, comp_N_end, comp_N_time;
static uint begin_iter, N_iter;
static ulong policy_freq;

static double perf_accuracy_min_time = 1000000;
static uint perf_count_period = 100;
static uint EAR_STATE = NO_PERIOD;
static int loop_with_signature = 0;
static int current_loop_id;

void states_end_job(int my_id, FILE *ear_fd, char *app_name)
{
	ear_verbose(1, "EAR(%s) Ends execution. \n", app_name);
}

void states_begin_job(int my_id, FILE *ear_fd, char *app_name)
{
	char *verbose, *loop_time, *who;

	init_application(&last_signature);

	if (my_id) return;

	perf_accuracy_min_time = get_ear_performance_accuracy();
	ear_debug(3, "EAR(%s) JOB %s STARTS EXECUTION. Performance accuracy set to (min) %.5lf usecs\n",
			  __FILE__, app_name, perf_accuracy_min_time);
	EAR_STATE = NO_PERIOD;
	policy_freq = EAR_default_frequency;
}

int states_my_state()
{
	return EAR_STATE;
}

void states_begin_period(int my_id, FILE *ear_fd, unsigned long event, unsigned int size)
{
	if (my_id == 0)
	{
		ear_verbose(4, "EAR(%s): ________BEGIN_PERIOD: Computing N for period %d size %u_____BEGIN_____\n",
					ear_app_name, event, size);

		EAR_STATE = FIRST_ITERATION;

		models_new_period();
		comp_N_begin = metrics_time();
		traces_new_period(ear_my_rank, my_id, event);
		loop_with_signature = 0;
	}
}

void states_end_period(int my_id, FILE *ear_fd, unsigned int size, int iterations, unsigned long event)
{
	if (loop_with_signature)
	{
		ear_verbose(1, "EAR: Loop id %lu finished with %d iterations. Estimated time %lf sec.\n",
					current_loop_id, iterations, loop_signature.time * (double) iterations);
	}

	loop_with_signature = 0;
	ear_verbose(4, "EAR(%s)::____________END_PERIOD: END loop detected (Loop ID: %u,size %u)______%d iters______ \n",
				ear_app_name, event, size, iterations);
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
	if (equal_with_th(A->CPI, B->CPI, performance_penalty_th) &&
		equal_with_th(A->GBS, B->GBS, performance_penalty_th))
	{
		return 0;
	}
	return 1;
}

void states_new_iteration(int my_id, FILE *ear_fd, uint period, int iterations, ulong event, uint level)
{
	double CPI, TPI, GBS, POWER, TIME, ENERGY, EDP;
	unsigned long prev_f;
	int result;

	prev_f = ear_my_frequency();

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

				ear_verbose(3,
							"\nEAR(%s) at %u, %d iterations are considered to compute app signature...start computing\n",
							ear_app_name, prev_f, perf_count_period);

				// Once min iterations is computed for performance accuracy we start computing application signature
				EAR_STATE = EVALUATING_SIGNATURE;
				metrics_compute_signature_begin();
				begin_iter = iterations;

				ear_debug(3, "EAR(%s) FIRST_ITERATION -> EVALUATING_SIGNATURE\n", ear_app_name);
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
				report = 1;
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
					// Saving this loop info to its summary file
					append_application_text_file(loop_summary_path, &loop_signature);
					
					loop_with_signature = 1;
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
					// Saving this loop info to its summary file
					append_application_text_file(loop_summary_path, &loop_signature);

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

					begin_iter = iterations;
					// We compare the projection with the signature and the old signature
					PP = performance_projection(policy_freq);

					if (policy_ok(PP, &loop_signature, &last_signature))
					{
						perf_count_period = perf_count_period * 2;
						ear_verbose(3,
									"\n\nEAR(%s): Policy ok Signature (Time %lf Power %lf Energy %lf) Projection(Time %lf Power %lf Energy %lf)\n",
									ear_app_name, TIME, POWER, ENERGY, PP->Time, PP->Power, PP->Time * PP->Power);
					}
					else
					{
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

							models_new_period();
						} else {
							EAR_STATE = EVALUATING_SIGNATURE;
						}
					}
				}
			}
			break;
		default: break;
	}
}



