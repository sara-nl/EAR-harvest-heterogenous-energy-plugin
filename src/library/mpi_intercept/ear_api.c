/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

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

// EAR includes
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


static const char *__NAME__ = "API";

#define BUFFSIZE 128

// Una cosa que
static struct timeval pmpi_app_begin_time;
static struct timeval pmpi_app_end_time;
static long long pmpi_app_total_time;

// Process information
static int my_id, my_size;
static unsigned long ear_current_freq;
static int ear_current_cpuid;

// Loop information
static uint ear_loop_size;
static uint ear_iterations;
static int in_loop = 0;
static uint mpi_calls_per_loop=0;

//#define MEASURE_DYNAIS_OV
#ifdef MEASURE_DYNAIS_OV
static long long begin_ov, end_ov, ear_acum = 0;
static unsigned int calls = 0;
#endif

// TODO: REFACTOR this function
int get_app_name_please(char *my_name)
{
	char *app_name;
	int defined = 0;
	app_name = get_ear_app_name();
	if (app_name == NULL)
	{
		//TODO: METRICS COUPLED
		if (PAPI_is_initialized() == PAPI_NOT_INITED)
			strcpy(my_name, "UnknownApplication");
		else
			metrics_get_app_name(my_name);

		set_ear_app_name(my_name);
	} else {
		defined = 1;
		strcpy(my_name, app_name);
	}

	return defined;
}

void ear_init()
{
	char node_name[BUFFSIZE];
	unsigned int num_nodes, ppnode;
	char *summary_pathname;
	char *job_id, *user_id;
	char *freq;
	int size;

	// MPI
	PMPI_Comm_rank(MPI_COMM_WORLD, &ear_my_rank);
	PMPI_Comm_size(MPI_COMM_WORLD, &my_size);

	//TODO: SWTICH TO APPLICATION
	gethostname(node_name, sizeof(node_name));
	
	// Environment initialization
	ear_lib_environment();

	//
	EAR_VERBOSE_LEVEL = get_ear_verbose();
	set_ear_total_processes(my_size);
	ear_whole_app = get_ear_learning_phase();
   	dynais_init(get_ear_dynais_window_size(), get_ear_dynais_levels());

	my_id = get_ear_local_id();

	// When SLURM is not found. TODO: maybe has to be fixed
	if (my_id < 0)
	{
		num_nodes = get_ear_num_nodes();
		ppnode = my_size / num_nodes;
		my_id = (ear_my_rank % ppnode);
	}

	if (my_id) return;

	if (ear_my_rank == 0)
	{
		ear_verbose(1,"EAR: Total resources %d\n", get_total_resources());
		ear_verbose(0,"EAR using %d levels in dynais with %d of window size \n",
				get_ear_dynais_levels(), get_ear_dynais_window_size());
	}

	#if SHARED_MEMORY
	system_conf=attach_ear_conf_shared_area(get_ear_tmp());
	#endif

	// Only one process can connect with the daemon
	// Connecting with ear_daemon
	ear_verbose(0,"EAR: Connecting with EARD\n");
	if (eards_connect() < 0) {
		ear_verbose(0,"EAR: Connect with EAR daemon fails\n");
	}

	// Application static data gathering
	init_application(&application);
	init_application(&loop_signature);

	// my_id is 0 in case is not local. Metrics gets the value
	// 'privileged_metrics'. This value has to be different to 0 when
	// my_id is different to 0.
	metrics_init(); // PAPI_init starts counters
	frequency_init(); //Initialize cpufreq info

	if (ear_my_rank == 0)
	{
		if (ear_whole_app == 1 && ear_use_turbo == 1) {
			VERBOSE_N(2, "turbo learning phase, turbo selected and start computing\n");
			eards_set_turbo();
		} else {
			VERBOSE_N(2, "learning phase %d, turbo %d\n", ear_whole_app, ear_use_turbo);
		}
	}

	// Getting environment data
	get_app_name_please(ear_app_name);
	ear_current_freq = frequency_get_num_pstates(0);
	job_id = getenv("SLURM_JOB_ID");
	user_id = getenv("LOGNAME");
	if (job_id != NULL){ 
		strcpy(application.job_id, job_id);
		my_job_id=atoi(job_id);
	}else{ 
		my_job_id=getppid();
		sprintf(application.job_id, "%d", my_job_id);
	}

	// Policies
	init_power_policy();
	init_power_models(frequency_get_num_pstates(), frequency_get_freq_rank_list());

	// Policy name is set in ear_models
	strcpy(application.app_id, ear_app_name);
	strcpy(application.user_id, user_id);
	strcpy(application.node_id, node_name);


	// Passing the frequency in KHz to MHz
	application.def_f = EAR_default_frequency;
	application.procs = get_total_resources();
	application.policy_th = get_ear_power_policy_th();;	

	// Copying static application info into the loop info
	memcpy(&loop_signature, &application, sizeof(application_t));
	// States
	states_begin_job(my_id, NULL, ear_app_name);

	// Summary files
	summary_pathname = get_ear_user_db_pathname();

	if (summary_pathname != NULL) {
		sprintf(app_summary_path, "%s%s.csv", summary_pathname, node_name);
		sprintf(loop_summary_path, "%s%s.loop_info.csv", summary_pathname, node_name);
	}

	//if (ear_my_rank == 0)
	//{
		VERBOSE_N(1, "--------------------------------");
		VERBOSE_N(1, "App/user id: '%s'/'%s'", application.app_id, application.user_id);
		VERBOSE_N(1, "Node/job id: '%s'/'%s'", application.node_id, application.job_id);
		VERBOSE_N(1, "App/loop summary file: '%s'/'%s'", app_summary_path, loop_summary_path);
		VERBOSE_N(1, "Default frequency (turbo): %u (%d)", application.def_f, ear_use_turbo);
		VERBOSE_N(1, "Procs/nodes/ppn: %u/%d/%d", application.procs, num_nodes, ppnode);
		VERBOSE_N(1, "Policy/threshold/learning: %s/%lf/%d", application.policy, application.policy_th, ear_whole_app);
		VERBOSE_N(1, "DynAIS levels/window: %d/%d", get_ear_dynais_levels(), get_ear_dynais_window_size());
		VERBOSE_N(1, "--------------------------------");
//	}

	//
	gettimeofday(&pmpi_app_begin_time, NULL);
	fflush(stderr);

	traces_init(ear_my_rank, my_id, num_nodes, my_size, ppnode);
	traces_frequency(ear_my_rank, my_id, ear_current_freq);

	ear_print_lib_environment();
	DEBUG_F(1, "EAR initialized successfully");
}

void ear_mpi_call(mpi_call call_type, p2i buf, p2i dest)
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

		ear_debug(3,"EAR(%s) EAR executing before an MPI Call\n",__FILE__);

		traces_mpi_call(ear_my_rank, my_id,
						(unsigned long) PAPI_get_real_usec(),
						(unsigned long) buf,
						(unsigned long) dest,
						(unsigned long) call_type,
						(unsigned long) ear_event);

		mpi_calls_per_loop++;
		// MEASURE_DYNAIS_OV flag is used to compute the time consumed by DyNAIs algorithm
		#ifdef MEASURE_DYNAIS_OV
		begin_ov=PAPI_get_real_usec();
		#endif
		// This is key to detect periods
		if (dynais_enabled){ 
			ear_status=dynais(ear_event,&ear_size,&ear_level);
		}else{
			if (last_calls_in_loop==mpi_calls_per_loop){
				ear_status=NEW_ITERATION;
			}else{		
				ear_status=IN_LOOP;
			}
		}
		#ifdef MEASURE_DYNAIS_OV
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
				ear_verbose(2,"NEW_ITERATION level %u event %u size %u iterations %u\n",
					ear_level, ear_event, ear_loop_size, ear_iterations);
			}

			traces_new_n_iter(ear_my_rank, my_id, ear_event, ear_loop_size, ear_iterations);
			states_new_iteration(my_id, ear_loop_size, ear_iterations, ear_level, ear_event, mpi_calls_per_loop);
			mpi_calls_per_loop=1;
			break;
		case END_LOOP:
			ear_debug(4,"END_LOOP event %u\n",ear_event);
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

void ear_finalize()
{
	char summary_fullpath[BUFFSIZE];
	char node_name[BUFFSIZE];
	char *summary_pathname;
	int iters=0;

	if (my_id) {
		return;
	}

	#ifdef MEASURE_DYNAIS_OV
	ear_verbose(0,"EAR:: Dynais algorithm consumes %llu usecs in %u calls\n",ear_acum,calls);
	#endif
	
	gettimeofday(&pmpi_app_end_time,NULL);
	
	pmpi_app_total_time = (pmpi_app_end_time.tv_sec * 1000000 + pmpi_app_end_time.tv_usec) -
                              (pmpi_app_begin_time.tv_sec * 1000000 + pmpi_app_begin_time.tv_usec);

	traces_end(ear_my_rank, my_id, 0);

	// Closing and obtaining global metrics
	metrics_dispose(&application);

	eards_write_app_signature(&application);
	append_application_text_file(app_summary_path, &application);
	report_application_data(&application);

	// Releasing DynAIS algorithm memory
	dynais_dispose();
	
	// Closing any remaining loop
	if (in_loop) states_end_period(ear_iterations);
	states_end_job(my_id, NULL, ear_app_name);

	frequency_dispose();

	#if SHARED_MEMORY
	dettach_ear_conf_shared_area();
	#endif

	eards_disconnect();
}
