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

#include <mpi.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <papi.h>
#include <control/frequency.h>
#include <library/common/externs_alloc.h>
#include <library/dynais/dynais.h>
#include <library/tracer/tracer.h>
#include <library/states/states.h>
#include <library/models/models.h>
#include <library/metrics/metrics.h>
#include <library/mpi_intercept/ear_api.h>
#include <library/mpi_intercept/MPI_types.h>
#include <library/mpi_intercept/MPI_calls_coded.h>
#include <common/shared_configuration.h>
#include <common/types/application.h>
#include <common/ear_verbose.h>
#include <common/environment.h>
#include <common/states.h>
#include <common/config.h>

// Statics
static const char *__NAME__ = "API";

#define BUFFSIZE 			128
#define JOB_ID_OFFSET		100
#define USE_LOCK_FILES 		1
#define MEASURE_DYNAIS_OV	0

#if USE_LOCK_FILES
#include <common/file_lock.h>
static char fd_lock_filename[BUFFSIZE];
static int fd_master_lock;
#endif

#if MEASURE_DYNAIS_OV
static long long begin_ov, end_ov, ear_acum;
static unsigned int calls;
#endif

// Process information
static int my_id = 1;
static int my_size;
static int num_nodes;
static int ppnode;

// Loop information
static uint mpi_calls_per_loop;
static uint ear_iterations;
static uint ear_loop_size;
static int in_loop;

#define EAR_OVERHEAD_CONTROL 0
#if EAR_OVERHEAD_CONTROL
/* in us */
#define MAX_TIME_DYNAIS_WITHOUT_SIGNATURE	30000000
#define MPI_CALLS_TO_CHECK_PERIODIC			1000
#define PERIOD								20000000
#define DYNAIS_ON 		0
#define DYNAIS_OFF		1
#define PERIODIC_MODE_ON	0
#define PERIODIC_MODE_OFF	1
// These variables are shared
uint ear_periodic_mode=PERIODIC_MODE_OFF;
uint dynais_enabled=DYNAIS_ON;
uint check_periodic_mode=1;
uint mpi_calls_in_period=0;
#endif

//
static void print_local_data()
{
	VERBOSE_N(1, "--------------------------------");
	VERBOSE_N(1, "App/user id: '%s'/'%s'", application.job.app_id, application.job.user_id);
	VERBOSE_N(1, "Node/job id: '%s'/'%u'", application.node_id, application.job.id);
	VERBOSE_N(1, "App/loop summary file: '%s'/'%s'", app_summary_path, loop_summary_path);
	VERBOSE_N(1, "P_STATE/frequency (turbo): %u/%u (%d)", EAR_default_pstate, application.job.def_f, ear_use_turbo);
	VERBOSE_N(1, "Procs/nodes/ppn: %u/%d/%d", application.job.procs, num_nodes, ppnode);
	VERBOSE_N(1, "Policy (learning): %s (%d)", application.job.policy, ear_whole_app);
	VERBOSE_N(1, "Policy threshold/Perf accuracy: %0.2lf/%0.2lf", application.job.th, get_ear_performance_accuracy());
	VERBOSE_N(1, "DynAIS levels/window/AVX512: %d/%d/%d", get_ear_dynais_levels(), get_ear_dynais_window_size(), dynais_build_type());
	VERBOSE_N(1, "VAR path: %s", get_ear_tmp());
	VERBOSE_N(1, "--------------------------------");
}

//
static int get_local_id(char *node_name)
{
	int master = 1;

	#if USE_LOCK_FILES
	sprintf(fd_lock_filename, "%s/.ear_app_lock.%d", get_ear_tmp(), my_job_id);

	if ((fd_master_lock = lock_master(fd_lock_filename)) < 0) {
		master = 1;
	} else {
		master = 0;
	}

	if (master) {
		VERBOSE_N(2, "Rank %d is not the master in node %s", ear_my_rank, node_name);
	}else{
		VERBOSE_N(1, "Rank %d is the master in node %s", ear_my_rank, node_name);
	}
	#else
	master = get_ear_local_id();

	if (master < 0) {
		master = (ear_my_rank % ppnode);
	}
	#endif

	return master;
}

// Getting the job identification (job_id * 100 + job_step_id)
static void get_job_identification()
{
	char *job_id  = getenv("SLURM_JOB_ID");
	char *step_id = getenv("SLURM_STEP_ID");

	if (job_id != NULL) {
		my_job_id=atoi(job_id);
		if (step_id != NULL) {
			my_job_id = my_job_id * JOB_ID_OFFSET + atoi(step_id);
		} else {
			step_id = getenv("SLURM_STEPID");
			if (step_id != NULL) {
				my_job_id = my_job_id * JOB_ID_OFFSET + atoi(step_id);
			} else {
				VERBOSE_N(0, "Neither SLURM_STEP_ID nor SLURM_STEPID are defined, using SLURM_JOB_ID");
			}
		}
	} else {
		my_job_id = getppid();
	}
}

static void get_app_name(char *my_name)
{
	int defined = 0;
	char *app_name;

	app_name = get_ear_app_name();

	if (app_name == NULL)
	{
		if (PAPI_is_initialized() == PAPI_NOT_INITED) {
			strcpy(my_name, "unknown");
		} else {
			metrics_get_app_name(my_name);
		}
		set_ear_app_name(my_name);
	} else {
		defined = 1;
		strcpy(my_name, app_name);
	}
}

void ear_init()
{
	unsigned long ear_current_freq;
	char node_name[BUFFSIZE];
	char *summary_pathname;
	char *freq;
	int size;

	// MPI
	PMPI_Comm_rank(MPI_COMM_WORLD, &ear_my_rank);
	PMPI_Comm_size(MPI_COMM_WORLD, &my_size);

	// Environment initialization
	ear_lib_environment();

	// Fundamental data
	gethostname(node_name, sizeof(node_name));
	EAR_VERBOSE_LEVEL = get_ear_verbose();
	set_ear_total_processes(my_size);
	ear_whole_app = get_ear_learning_phase();
	num_nodes = get_ear_num_nodes();
	ppnode = my_size / num_nodes;

	// Getting if the local process is the master or not
	my_id = get_local_id(node_name);

	// if we are not the master, we return
	if (my_id != 0) {
		return;
	}

    VERBOSE_N(1, "Connecting with EAR Daemon (EARD) %d", ear_my_rank);
    if (eards_connect() == EAR_SUCCESS) {
        VERBOSE_N(1, "Rank %d connected with EARD", ear_my_rank);
    }

	#if SHARED_MEMORY
	system_conf = attach_ear_conf_shared_area(get_ear_tmp());
	#endif

	// Application static data and metrics
	init_application(&application);
	init_application(&loop_signature);
	application.is_mpi=1;
	loop_signature.is_mpi=1;
	application.is_learning=ear_whole_app;
	loop_signature.is_learning=ear_whole_app;

	// Initializing sub systems
	dynais_init(get_ear_dynais_window_size(), get_ear_dynais_levels());
	metrics_init();
	frequency_init(metrics_get_node_size()); //Initialize cpufreq info

	if (ear_my_rank == 0)
	{
		if (ear_whole_app == 1 && ear_use_turbo == 1) {
			VERBOSE_N(2, "turbo learning phase, turbo selected and start computing");
			eards_set_turbo();
		} else {
			VERBOSE_N(2, "learning phase %d, turbo %d", ear_whole_app, ear_use_turbo);
		}
	}

	// Getting environment data
	get_job_identification();
	get_app_name(ear_app_name);
	ear_current_freq = frequency_get_num_pstates(0);

	// Policies
	init_power_policy();
	init_power_models(frequency_get_num_pstates(), frequency_get_freq_rank_list());

	// Policy name is set in ear_models
	strcpy(application.job.user_id, getenv("LOGNAME"));
	strcpy(application.job.app_id, ear_app_name);
	strcpy(application.node_id, node_name);

	// Passing the frequency in KHz to MHz
	application.job.id = my_job_id;
	application.job.def_f = EAR_default_frequency;
	application.job.procs = get_total_resources();
	application.job.th = get_ear_power_policy_th();

	// Copying static application info into the loop info
	memcpy(&loop_signature, &application, sizeof(application_t));
	//sets the job start_time
	start_job(&application.job);

	// States
	states_begin_job(my_id, NULL, ear_app_name);

	// Summary files
	summary_pathname = get_ear_user_db_pathname();

	if (summary_pathname != NULL) {
		sprintf(app_summary_path, "%s%s.csv", summary_pathname, node_name);
		sprintf(loop_summary_path, "%s%s.loop_info.csv", summary_pathname, node_name);
	}

	// Print things
	print_local_data();
	ear_print_lib_environment();
	fflush(stderr);

	// Tracing init
	traces_init(ear_my_rank, my_id, num_nodes, my_size, ppnode);
	traces_frequency(ear_my_rank, my_id, ear_current_freq);

	// All is OK :D
	DEBUG_F(1, "EAR initialized successfully");
}

void ear_finalize()
{
	char summary_fullpath[BUFFSIZE];
	char node_name[BUFFSIZE];

	// if we are not the master, we return
	if (my_id) {
		return;
	}	

	#if USE_LOCK_FILES
	VERBOSE_N(1, "Application master releasing the lock %d", ear_my_rank);
	unlock_master(fd_master_lock,fd_lock_filename);
	#endif

	#if MEASURE_DYNAIS_OV
	VERBOSE_N(0, "DynAIS algorithm consumes %llu usecs in %u calls", ear_acum, calls);
	#endif
	
	// Tracing
	traces_end(ear_my_rank, my_id, 0);

	// Closing and obtaining global metrics
	metrics_dispose(&application.signature, application.job.procs);
	dynais_dispose();
	frequency_dispose();

	// Writing application data
	eards_write_app_signature(&application);
	append_application_text_file(app_summary_path, &application);
	report_application_data(&application);

	// Closing any remaining loop
	if (loop_with_signature) {
		VERBOSE_N(1, "loop ends with %d iterations detected", ear_iterations);
	}
	if (in_loop) {
		states_end_period(ear_iterations);
	}
	states_end_job(my_id, NULL, ear_app_name);

	#if SHARED_MEMORY
	dettach_ear_conf_shared_area();
	#endif

	// Cest fini
	eards_disconnect();
}

void ear_mpi_call_dynais_on(mpi_call call_type, p2i buf, p2i dest);
void ear_mpi_call_dynais_off(mpi_call call_type, p2i buf, p2i dest);

void ear_mpi_call(mpi_call call_type, p2i buf, p2i dest)
{

#if EAR_OVERHEAD_CONTROL
	switch(ear_periodic_mode){
		case PERIODIC_MODE_OFF:
			switch(dynais_enabled){
				case DYNAIS_ON:
					/** We must control here we are not consuming too much time tryng to detect a signature */
					/*
					if some signature has been detected --> nothing to check
					else if num_mpis % MAX_TO_CHECK{ compute time, if time > MAX-TIME --> set periodic mode on
					*/
					if (!check_periodic_mode){  // check_periodic_mode=0 the first time a signature is computed
						ear_mpi_call_dynais_on(call_type,buf,dest);
					}else{
						// Check here if we must move to p
					}
					break;
				case DYNAIS_OFF:
					/** That case means we have computed some signature and we have decided to set dynais disabled */
					ear_mpi_call_dynais_off(call_type,buf,dest);
			}
			break;
		case PERIODIC_MODE_ON:
				/* first time compute number of mpicalls to be considered based on total mpi calls and time
				call first iteration
				after N mpi calls call new_periodic_iteration, never return to non-periodic mode
				*/
	}
#else
    if (dynais_enabled) ear_mpi_call_dynais_on(call_type,buf,dest);
    else ear_mpi_call_dynais_off(call_type,buf,dest);
#endif
}



void ear_mpi_call_dynais_on(mpi_call call_type, p2i buf, p2i dest)
{
	unsigned int ear_status;
	int ret;
	char men[128];

	if (my_id) {
		return;
	}

	// If ear_whole_app we are in the learning phase, DyNAIS is disabled then
	if (!ear_whole_app)
	{
		// Create the event for DynAIS
		unsigned long ear_event = (unsigned long)((((buf>>5)^dest)<<5)|call_type);
		unsigned int ear_size;
		unsigned int ear_level;
		unsigned long trace_data[5];

		ear_debug(3,"EAR(%s) EAR executing before an MPI Call: DYNAIS ON\n",__FILE__);

		traces_mpi_call(ear_my_rank, my_id,
						(unsigned long) PAPI_get_real_usec(),
						(unsigned long) buf,
						(unsigned long) dest,
						(unsigned long) call_type,
						(unsigned long) ear_event);

		mpi_calls_per_loop++;
		// MEASURE_DYNAIS_OV flag is used to compute the time consumed by DyNAIs algorithm

		#if MEASURE_DYNAIS_OV
		begin_ov=PAPI_get_real_usec();
		#endif

		// This is key to detect periods
		ear_status=dynais(ear_event,&ear_size,&ear_level);

		#if MEASURE_DYNAIS_OV
		end_ov=PAPI_get_real_usec();
		calls++;
		ear_acum=ear_acum+(end_ov-begin_ov);
		#endif

		switch (ear_status)
		{
			case NO_LOOP:
			case IN_LOOP:
				break;
			case NEW_LOOP:
				ear_debug(4,"NEW_LOOP event %u level %u size %u\n",ear_event,ear_level,ear_size);
				ear_iterations=0;
				states_begin_period(my_id, NULL, ear_event, ear_size);
				ear_loop_size=ear_size;
				in_loop=1;
				mpi_calls_per_loop=1;
				break;
			case END_NEW_LOOP:
				ear_debug(4,"END_LOOP - NEW_LOOP event %u level %u\n",ear_event,ear_level);
				if (loop_with_signature) VERBOSE_N(1, "loop ends with %d iterations detected", ear_iterations);

				loop_with_signature=0;
				traces_end_period(ear_my_rank, my_id);
				states_end_period(ear_iterations);
				ear_iterations=0;
				mpi_calls_per_loop=1;
				ear_loop_size=ear_size;
				states_begin_period(my_id, NULL, ear_event, ear_size);
				break;
			case NEW_ITERATION:
				ear_iterations++;

				if (loop_with_signature)
				{
					VERBOSE_N(4,"new iteration detected for level %u, event %u, size %u and iterations %u",
							  ear_level, ear_event, ear_loop_size, ear_iterations);
				}

				traces_new_n_iter(ear_my_rank, my_id, ear_event, ear_loop_size, ear_iterations);
				states_new_iteration(my_id, ear_loop_size, ear_iterations, ear_level, ear_event, mpi_calls_per_loop);
				mpi_calls_per_loop=1;
				break;
			case END_LOOP:
				ear_debug(4,"END_LOOP event %u\n",ear_event);
				if (loop_with_signature) VERBOSE_N(1, "loop ends with %d iterations detected", ear_iterations);
				loop_with_signature=0;
				states_end_period(ear_iterations);
				traces_end_period(ear_my_rank, my_id);
				ear_iterations=0;
				in_loop=0;
				mpi_calls_per_loop=0;
				break;
			default:
				break;
		}
	} //ear_whole_app
}

void ear_mpi_call_dynais_off(mpi_call call_type, p2i buf, p2i dest)
{
	unsigned int ear_status;
	int ret;
	char men[128];

	if (my_id) {
		return;
	}

	// If ear_whole_app we are in the learning phase, DyNAIS is disabled then
	if (!ear_whole_app)
	{
		// Create the event for DynAIS: we will report anyway
		unsigned long ear_event = (unsigned long)((((buf>>5)^dest)<<5)|call_type);
		unsigned int ear_size;
		unsigned int ear_level;
		unsigned long trace_data[5];

		ear_debug(3,"EAR(%s) EAR executing before an MPI Call: DYNAIS ON \n",__FILE__);

		traces_mpi_call(ear_my_rank, my_id,
						(unsigned long) PAPI_get_real_usec(),
						(unsigned long) buf,
						(unsigned long) dest,
						(unsigned long) call_type,
						(unsigned long) ear_event);

		mpi_calls_per_loop++;

		if (last_calls_in_loop==mpi_calls_per_loop){
				ear_status=NEW_ITERATION;
		}else{
				ear_status=IN_LOOP;
		}


		switch (ear_status)
		{
			case NO_LOOP:
			case IN_LOOP:
				break;
			case NEW_LOOP:
				ear_debug(4,"NEW_LOOP event %u level %u size %u\n",ear_event,ear_level,ear_size);
				ear_iterations=0;
				states_begin_period(my_id, NULL, ear_event, ear_size);
				ear_loop_size=ear_size;
				in_loop=1;
				mpi_calls_per_loop=1;
				break;
			case END_NEW_LOOP:
				ear_debug(4,"END_LOOP - NEW_LOOP event %u level %u\n",ear_event,ear_level);
				if (loop_with_signature) VERBOSE_N(1, "loop ends with %d iterations detected", ear_iterations);

				loop_with_signature=0;
				traces_end_period(ear_my_rank, my_id);
				states_end_period(ear_iterations);
				ear_iterations=0;
				mpi_calls_per_loop=1;
				ear_loop_size=ear_size;
				states_begin_period(my_id, NULL, ear_event, ear_size);
				break;
			case NEW_ITERATION:
				ear_iterations++;

				if (loop_with_signature)
				{
					VERBOSE_N(4,"new iteration detected for level %u, event %u, size %u and iterations %u",
							  ear_level, ear_event, ear_loop_size, ear_iterations);
				}

				traces_new_n_iter(ear_my_rank, my_id, ear_event, ear_loop_size, ear_iterations);
				states_new_iteration(my_id, ear_loop_size, ear_iterations, ear_level, ear_event, mpi_calls_per_loop);
				mpi_calls_per_loop=1;
				break;
			case END_LOOP:
				ear_debug(4,"END_LOOP event %u\n",ear_event);
				if (loop_with_signature) VERBOSE_N(1, "loop ends with %d iterations detected", ear_iterations);
				loop_with_signature=0;
				states_end_period(ear_iterations);
				traces_end_period(ear_my_rank, my_id);
				ear_iterations=0;
				in_loop=0;
				mpi_calls_per_loop=0;
				break;
            default:
                break;
        }
    } //ear_whole_app
}

