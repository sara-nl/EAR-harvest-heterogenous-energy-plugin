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
#include <library/ear_dynais/ear_dynais.h>
#include <library/ear_states/ear_states.h>
#include <library/ear_dyn_inst/MPI_types.h>
#include <library/ear_dyn_inst/MPI_calls_coded.h>
#include <library/ear_frequency/ear_cpufreq.h>
#include <library/ear_models/ear_models.h>
#include <library/ear_metrics/ear_metrics.h>
#include <library/ear_gui/ear_gui.h>
#include <library/common/externs_alloc.h>
#include <common/types/application.h>
#include <common/ear_verbose.h>
#include <common/environment.h>
#include <common/states.h>
#include <common/shared_configuration.h>


static const char *__NAME__ = "API";

#define BUFFSIZE 128

static unsigned int ear_loop_size;
static struct timeval pmpi_app_begin_time, pmpi_app_end_time;
static long long pmpi_app_total_time;
static int my_id,my_size;
static int ear_iterations = 0;
static unsigned long ear_current_freq;
static int ear_current_cpuid;
static int in_loop=0;
static int period;

// #define MEASURE_DYNAIS_OV

#ifdef MEASURE_DYNAIS_OV
static long long begin_ov, end_ov, ear_acum = 0;
static unsigned int calls = 0;
#endif

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

	ear_verbose(1, "EAR: Application name is  %s\n", my_name);
	return defined;
}

void ear_init()
{
	char node_name[BUFFSIZE];
	unsigned int num_nodes,ppnode;
	char *job_id, *user_id;
	char *freq;
	int size;

	// MPI
	PMPI_Comm_rank(MPI_COMM_WORLD, &ear_my_rank);
	PMPI_Comm_size(MPI_COMM_WORLD, &my_size);
	ear_my_rank_size=my_size;

	//
	ear_lib_environment();
	set_ear_total_processes(my_size);
	EAR_VERBOSE_LEVEL=get_ear_verbose();

	if (get_ear_app_name() != NULL){
		if (ear_my_rank==0)
			ear_verbose(1, "________ EAR: Application %s starts__________\n", get_ear_app_name());
	}else{
		if (ear_my_rank==0)
			ear_verbose(1,"EAR: Application starts.....\n");
	}

	ear_debug(2,"EAR Starting initialization\n");	
	ear_whole_app=get_ear_learning_phase();
   	dynais_init(get_ear_dynais_window_size(),get_ear_dynais_levels());

	//TODO: SWTICH TO APPLICATION
	gethostname(node_name, sizeof(node_name));

	my_id = get_ear_local_id();

	if (my_id < 0)
	{
		num_nodes = get_ear_num_nodes();
		ppnode = my_size/num_nodes;
		my_id = (ear_my_rank % ppnode);
	}

	ear_my_local_id = my_id;

	#ifdef MASTER_ONLY
	if (ear_my_local_id) return;
	#endif

	ear_verbose(1,"EAR: Total resources %d\n", get_total_resources());
	ear_verbose(2,"EAR using %d levels in dynais with %d of window size \n",
				get_ear_dynais_levels(), get_ear_dynais_window_size());

#ifdef SHARED_MEMORY
	system_conf=attach_ear_conf_shared_area(get_ear_tmp());
#endif
	if (!ear_my_local_id)
	{
		// Only one process can connect with the daemon
		// Connecting with ear_daemon
		if (eards_connect() < 0) {
			ear_verbose(0,"EAR: Connect with EAR daemon fails\n");
			exit(1);
		}

		ear_verbose(1,"EAR: MPI rank %d defined as node master for %s pid: %d\n",
				ear_my_rank, ear_node_name, getpid());
	}

	// ear_my_local_id is 0 in case is not local. Metrics gets the value
	// 'privileged_metrics'. This value has to be different to 0 when
	// ear_my_local_id is different to 0.
	metrics_init(!ear_my_local_id); // PAPI_init starts counters
	ear_cpufreq_init(); //Initialize cpufreq info

	if (!ear_my_local_id)
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
	ear_current_freq = ear_cpufreq_get(0);
	job_id = getenv("SLURM_JOB_ID");
	user_id = getenv("LOGNAME");

	// States
    states_begin_job(my_id, NULL, ear_app_name);

	// Policies
	init_power_policy();
	init_power_models(ear_get_num_p_states(), ear_get_pstate_list());

	// Application static data gathering
	init_application(&application);
	init_application(&loop_signature);

	strcpy(application.app_id, ear_app_name);
	strcpy(application.user_id, user_id);
	strcpy(application.node_id, node_name);

	if (job_id != NULL) strcpy(application.job_id, job_id);
	else sprintf(application.job_id, "%d", getppid());

	// Passing the frequency in KHz to MHz
	application.def_f = EAR_default_frequency / 1000;
	application.procs = get_total_resources();
	
	// Copying static application info into the loop info
	memcpy(&loop_signature, &application, sizeof(application_t));

	// Summary files
	char *summary_pathname;
	summary_pathname = get_ear_user_db_pathname();
	sprintf(app_summary_path, "%s%s", summary_pathname, node_name);
	sprintf(loop_summary_path, "%s%s.loop_info", summary_pathname, node_name);

	VERBOSE_N(2, "App id: '%s'", application.app_id);
	VERBOSE_N(2, "User id: '%s'", application.user_id);
	VERBOSE_N(2, "Node id: '%s'", application.node_id);
	VERBOSE_N(2, "Job id: '%s'", application.job_id);
	VERBOSE_N(2, "Default frequency: %u", application.def_f);
	VERBOSE_N(2, "Procs: %u", application.procs);

	//
	gettimeofday(&pmpi_app_begin_time, NULL);
	fflush(stderr);

	traces_init(ear_my_rank,my_id,ear_app_name,ear_node_name,num_nodes,my_size,ppnode);
	traces_frequency(ear_my_rank,my_id,ear_current_freq);

	ear_debug(1,"EAR Initialized successfully\n");
	ear_print_lib_environment();
	ear_verbose(1,"______________EAR loaded___________________\n");
}

void ear_mpi_call(mpi_call call_type, p2i buf, p2i dest)
{
	unsigned int ear_status;
	int ret;
	char men[128];
#ifdef MASTER_ONLY
	if (ear_my_local_id) return;
#endif

	// If ear_whole_app we are in the learning phase, DyNAIS is disabled then
	if (!ear_whole_app){
	ear_debug(3,"EAR(%s) EAR executing before an MPI Call\n",__FILE__);
	// Create the event for DynAIS
	unsigned long ear_event = (unsigned long)((((buf>>5)^dest)<<5)|call_type);
	unsigned int ear_size;
	unsigned int ear_level;
	unsigned long trace_data[5];
    traces_mpi_call(ear_my_rank,my_id,(unsigned long)PAPI_get_real_usec(),(unsigned long)buf,(unsigned long)dest,(unsigned long)call_type,(unsigned long)ear_event);

// MEASURE_DYNAIS_OV flag is used to compute the time consumed by DyNAIs algorithm
#ifdef MEASURE_DYNAIS_OV
        begin_ov=PAPI_get_real_usec();
#endif
	// This is key to detect periods
        ear_status=dynais(ear_event,&ear_size,&ear_level);
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
		ear_verbose(4,"NEW_LOOP event %u level %u size %u\n",ear_event,ear_level,ear_size);
		ear_iterations=0;
		states_begin_period(my_id, NULL, ear_event, ear_size);
		ear_loop_size=ear_size;
		in_loop=1;
		break;
	case END_NEW_LOOP:
		ear_debug(4,"END_LOOP - NEW_LOOP event %u level %u\n",ear_event,ear_level);
		traces_end_period(ear_my_rank,my_id);
		states_end_period(my_id, NULL, ear_loop_size, ear_iterations, ear_event);
		ear_iterations=0;
		ear_loop_size=ear_size;
		states_begin_period(my_id, NULL, ear_event, ear_size);
		break;
	case NEW_ITERATION:
		ear_iterations++;

		if (report==1)
		{
			ear_verbose(3,"NEW_ITERATION level %u event %u size %u iterations %u\n",
				ear_level, ear_event, ear_loop_size, ear_iterations);
		}

		traces_new_n_iter(ear_my_rank,my_id,ear_event,ear_loop_size,ear_iterations,states_my_state());	
		states_new_iteration(my_id, NULL, ear_loop_size, ear_iterations, ear_event, ear_level);
		break;
	case END_LOOP:
		ear_debug(4,"END_LOOP event %u\n",ear_event);
		states_end_period(my_id, NULL, ear_loop_size, ear_iterations, ear_event);
		traces_end_period(ear_my_rank,my_id);
		states_end_period(my_id, NULL, ear_loop_size, ear_iterations, ear_event);
		ear_iterations=0;
		in_loop=0;
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
#ifdef MASTER_ONLY
	if (ear_my_local_id) return;
#endif
	int iters=0;

	#ifdef MEASURE_DYNAIS_OV
	ear_verbose(0,"EAR:: Dynais algorithm consumes %llu usecs in %u calls\n",ear_acum,calls);
	#endif
	
	gettimeofday(&pmpi_app_end_time,NULL);
	
	pmpi_app_total_time = (pmpi_app_end_time.tv_sec * 1000000 + pmpi_app_end_time.tv_usec) -
                              (pmpi_app_begin_time.tv_sec * 1000000 + pmpi_app_begin_time.tv_usec);

	traces_end(ear_my_rank, my_id, 0);

	// Closing and obtaining global metrics
	metrics_dispose(&application);

	if (!ear_my_local_id){
		// TODO: DAR ORDEN AL DAEMON DE ESCRIBIR LOS DBS
		eards_write_app_signature(&application);
	}

	append_application_text_file(app_summary_path, &application);
	report_application_data(&application);

	// DynAIS
	dynais_dispose();

	if (in_loop) states_end_period(my_id, NULL, 0, ear_iterations, 0);
	states_end_job(my_id, NULL, ear_app_name);
	ear_cpufreq_end();
#ifdef SHARED_MEMORY
	dettach_ear_conf_shared_area();	
#endif
	if (!ear_my_local_id)	eards_disconnect();
}
