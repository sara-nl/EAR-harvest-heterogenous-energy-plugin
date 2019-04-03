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
#include <common/config.h>
#include <common/states.h>
#include <common/environment.h>
#include <common/output/debug.h>
#include <common/output/verbose.h>
#include <common/types/application.h>
#include <library/common/externs_alloc.h>
#include <library/dynais/dynais.h>
#include <library/tracer/tracer.h>
#include <library/tracer/tracer_mpi.h>
#include <library/states/states.h>
#include <library/states/states_periodic.h>
#include <library/models/models.h>
#include <library/metrics/metrics.h>
#include <library/mpi_intercept/ear_api.h>
#include <library/mpi_intercept/MPI_types.h>
#include <library/mpi_intercept/MPI_calls_coded.h>
#include <library/mpi_intercept/freq_synchro.h>
#include <control/frequency.h>
#include <daemon/eard_api.h>
#include <daemon/shared_configuration.h>
#include <metrics/metrics.h>


// Statics
#define BUFFSIZE 			128
#define JOB_ID_OFFSET		100
#define USE_LOCK_FILES 		1

#if USE_LOCK_FILES
#include <common/file.h>
static char fd_lock_filename[BUFFSIZE];
static int fd_master_lock;
#endif

#if MEASURE_DYNAIS_OV
static long long begin_ov, end_ov, ear_acum;
static unsigned int calls=0;
time_t start_loop_time;
#endif
#if IN_MPI_TIME
extern long long ear_total_in_mpi;
#endif


// Process information
static int my_id = 1;
static int my_size;
static int num_nodes;
static int ppnode;

// Loop information
static uint mpi_calls_per_loop;
static uint ear_iterations=0;
static uint ear_loop_size;
static uint ear_loop_level;
static int in_loop;

#if EAR_OVERHEAD_CONTROL
/* in us */
// These variables are shared
uint ear_periodic_mode=PERIODIC_MODE_OFF;
uint mpi_calls_in_period=10000;
uint total_mpi_calls=0;
static uint dynais_timeout=MAX_TIME_DYNAIS_WITHOUT_SIGNATURE;
static uint lib_period=PERIOD;
static uint check_every=MPI_CALLS_TO_CHECK_PERIODIC;
#if EAR_PERFORMANCE_TESTS
long long init_start_time,init_end_time;
static int only_load=0;
static int use_dynais=1;
int first_time_disabled=0;
#endif
#endif
#if EAR_LIB_SYNC
#define MASTERS_SYNC_VERBOSE 1
MPI_Comm new_world_comm,masters_comm;
int num_masters;
int my_master_rank;
int my_master_size;
int masters_connected=0;
unsigned masters_comm_created=0;
#endif

//
static void print_local_data()
{
	#if EAR_LIB_SYNC 
	if (my_master_rank==0) {
	#endif
	verbose(1, "--------------------------------");
	verbose(1, "App/user id: '%s'/'%s'", application.job.app_id, application.job.user_id);
	verbose(1, "Node/job id/step_id: '%s'/'%u'/'%u'", application.node_id, application.job.id,application.job.step_id);
	verbose(2, "App/loop summary file: '%s'/'%s'", app_summary_path, loop_summary_path);
	verbose(1, "P_STATE/frequency (turbo): %u/%u (%d)", EAR_default_pstate, application.job.def_f, ear_use_turbo);
	verbose(1, "Tasks/nodes/ppn: %u/%d/%d", my_size, num_nodes, ppnode);
	verbose(1, "Policy (learning): %s (%d)", application.job.policy, ear_whole_app);
	verbose(1, "Policy threshold/Perf accuracy: %0.2lf/%0.2lf", application.job.th, get_ear_performance_accuracy());
	verbose(1, "DynAIS levels/window/AVX512: %d/%d/%d", get_ear_dynais_levels(), get_ear_dynais_window_size(), dynais_build_type());
	verbose(1, "VAR path: %s", get_ear_tmp());
	verbose(1, "--------------------------------");
	#if EAR_LIB_SYNC
	}
	#endif
}


/* notifies mpi process has succesfully connected with EARD */
void notify_eard_connection(int status)
{
	#if EAR_LIB_SYNC
	char *buffer_send;
	char *buffer_recv;
	int i;
	if (masters_comm_created){
	buffer_send = calloc(    1, sizeof(char));
	buffer_recv = calloc(my_master_size, sizeof(char));
	if ((buffer_send==NULL) || (buffer_recv==NULL)){
			verbose(0,"Error, memory not available for synchronization\n");
			my_id=1;
			masters_comm_created=0;
			return;
	}
	buffer_send[0] = (char)status; 

	/* Not clear the error values */
	if (my_master_rank==0) {
		verbose(MASTERS_SYNC_VERBOSE,"Number of nodes expected %d\n",my_master_size);
	}
	PMPI_Allgather(buffer_send, 1, MPI_BYTE, buffer_recv, 1, MPI_BYTE, masters_comm);


	for (i = 0; i < my_master_size; ++i)
	{       
		masters_connected+=(int)buffer_recv[i];
	}
	if (my_master_rank==0) {
		verbose(MASTERS_SYNC_VERBOSE,"Total number of masters connected %d",masters_connected);
	}
	if (masters_connected!=num_nodes){
	/* Some of the nodes is not ok , setting off EARL */
		if (my_master_rank==0) {
			verbose(0,"Number of nodes expected %d , number of nodes connected %d, setting EAR to off \n",num_nodes,masters_connected);
		}
		my_id=1;
		return;
	}
	}
	#endif
}
/* Connects the mpi process to a new communicator composed by masters */
void attach_to_master_set(int master)
{
	#if EAR_LIB_SYNC
	int color;
	my_master_rank=0;
	if (master) color=0;
	else color=MPI_UNDEFINED;
	PMPI_Comm_dup(MPI_COMM_WORLD,&new_world_comm);
	PMPI_Comm_split(new_world_comm, color, my_master_rank, &masters_comm);
	masters_comm_created=1;
	if ((masters_comm_created) && (!color)){
		PMPI_Comm_rank(masters_comm,&my_master_rank);
		PMPI_Comm_size(masters_comm,&my_master_size);
		verbose(MASTERS_SYNC_VERBOSE+1,"New master communicator created with %d masters. My master rank %d\n",my_master_size,my_master_rank);
	}
	#endif
}
/* returns the local id in the node */
static int get_local_id(char *node_name)
{
	int master = 1;

#if USE_LOCK_FILES
	sprintf(fd_lock_filename, "%s/.ear_app_lock.%d", get_ear_tmp(), create_ID(my_job_id,my_step_id));

	if ((fd_master_lock = file_lock_master(fd_lock_filename)) < 0) {
		master = 1;
	} else {
		master = 0;
	}

	if (master) {
		verbose(2, "Rank %d is not the master in node %s", ear_my_rank, node_name);
	}else{
		verbose(2, "Rank %d is the master in node %s", ear_my_rank, node_name);
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
	char *account_id=getenv("SLURM_JOB_ACCOUNT");

	// It is missing to use SLURM_JOB_ACCOUNT

	if (job_id != NULL) {
		my_job_id=atoi(job_id);
		if (step_id != NULL) {
			my_step_id=atoi(step_id);
		} else {
			step_id = getenv("SLURM_STEPID");
			if (step_id != NULL) {
				my_step_id=atoi(step_id);
			} else {
				verbose(0, "Neither SLURM_STEP_ID nor SLURM_STEPID are defined, using SLURM_JOB_ID");
			}
		}
	} else {
		my_job_id = getppid();
		my_step_id=0;
	}
	if (account_id==NULL) strcpy(my_account,"NO_SLURM_ACCOUNT");	
	else strcpy(my_account,account_id);
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


/*** We update EARL configuration based on shared memory information **/
void update_configuration()
{
	/* print_settings_conf(system_conf);*/
	set_ear_power_policy(system_conf->policy);
	set_ear_power_policy_th(system_conf->th);
	set_ear_p_state(system_conf->def_p_state);
	set_ear_coeff_db_pathname(system_conf->lib_info.coefficients_pathname);
	set_ear_dynais_levels(system_conf->lib_info.dynais_levels);
	/* Included for dynais tunning */
	set_ear_dynais_window_size(system_conf->lib_info.dynais_window);
	set_ear_learning(system_conf->learning);
	dynais_timeout=system_conf->lib_info.dynais_timeout;
	lib_period=system_conf->lib_info.lib_period;
	check_every=system_conf->lib_info.check_every;
	ear_whole_app=system_conf->learning;
#if EAR_PERFORMANCE_TESTS
	char *ear_dynais_timeout,*ear_lib_period,*ear_check_every,*ear_use_dynais,*ear_dynais_size,*ear_mode;
	ear_dynais_timeout=getenv("EAR_DYNAIS_TIMEOUT");
	ear_lib_period=getenv("EAR_LIB_PERIOD");
	ear_check_every=getenv("EAR_CHECK_EVERY");
	ear_use_dynais=getenv("EAR_DYNAIS");
	ear_dynais_size=getenv("EAR_DYNAIS_SIZE");
	ear_mode=getenv("EAR_MODE");
	if (ear_lib_period!=NULL) lib_period=atoi(ear_lib_period);
	if (ear_check_every!=NULL) check_every=atoi(ear_check_every);
	if (ear_use_dynais!=NULL) use_dynais=atoi(ear_use_dynais);
	if (ear_dynais_size!=NULL) set_ear_dynais_window_size(atoi(ear_dynais_size));
	if (ear_dynais_timeout!=NULL) dynais_timeout=atoi(ear_dynais_timeout);
	if (ear_mode!=NULL)	ear_periodic_mode=atoi(ear_mode);
	verbose(0,"EAR_PERFORMANCE_TESTS ON: dynais %d dynais_timeout %d ear_dynais_size %d \n",use_dynais,dynais_timeout,atoi(ear_dynais_size));
	verbose(0,"EAR_PERFORMANCE_TESTS ON: lib_period %d check_every %d\n",lib_period,check_every);
	verbose(0,"EAR_PERFORMANCE_TESTS ON: ear_mode %d (PERIODIC=%d DYNAIS=%d)\n",ear_periodic_mode,PERIODIC_MODE_ON,PERIODIC_MODE_OFF);
#endif
}

void ear_init()
{
	unsigned long ear_current_freq;
	char *summary_pathname;
	char *freq;
	int size;
#if EAR_PERFORMANCE_TESTS
	char *ear_only_load;
	ear_only_load=getenv("EAR_ONLY_LOAD");
	if (ear_only_load!=NULL) only_load=atoi(ear_only_load);
	if (only_load) return;
	init_start_time=PAPI_get_real_usec();
#endif

	// MPI
	PMPI_Comm_rank(MPI_COMM_WORLD, &ear_my_rank);
	PMPI_Comm_size(MPI_COMM_WORLD, &my_size);

	// Environment initialization
	ear_lib_environment();

	// Fundamental data
	gethostname(node_name, sizeof(node_name));
	strtok(node_name, ".");

	verb_level = get_ear_verbose();
	verb_channel=2;
	set_ear_total_processes(my_size);
	ear_whole_app = get_ear_learning_phase();
	num_nodes = get_ear_num_nodes();
	ppnode = my_size / num_nodes;

	get_job_identification();
	// Getting if the local process is the master or not
	my_id = get_local_id(node_name);

	attach_to_master_set(my_id==0);

	// if we are not the master, we return
	if (my_id != 0) {
		return;
	}
	get_settings_conf_path(get_ear_tmp(),system_conf_path);
	system_conf = attach_settings_conf_shared_area(system_conf_path);
	get_resched_path(get_ear_tmp(),resched_conf_path);
	resched_conf = attach_resched_shared_area(resched_conf_path);

	/* Updating configuration */
	if ((system_conf!=NULL) && (resched_conf!=NULL) && (system_conf->id==create_ID(my_job_id,my_step_id))){
		update_configuration();	
	}else{
		debug("Shared memory not present");
		notify_eard_connection(0);
		my_id=1;
	}	

	if (my_id != 0) {
		return;
	}


	// Application static data and metrics
	init_application(&application);
	init_application(&loop_signature);
	application.is_mpi=1;
	loop_signature.is_mpi=1;
	application.is_learning=ear_whole_app;
	application.job.def_f=getenv_ear_p_state();
	loop_signature.is_learning=ear_whole_app;

	// Getting environment data
	get_app_name(ear_app_name);
	if (application.is_learning){
		verbose(1,"Learning phase app %s p_state %lu\n",ear_app_name,application.job.def_f);
	}
	strcpy(application.job.user_id, getenv("LOGNAME"));
	strcpy(application.node_id, node_name);
	strcpy(application.job.user_acc,my_account);
	application.job.id = my_job_id;
	application.job.step_id = my_step_id;

	//sets the job start_time
	start_job(&application.job);

	verbose(2, "Connecting with EAR Daemon (EARD) %d", ear_my_rank);
	if (eards_connect(&application) == EAR_SUCCESS) {
		verbose(1, "Rank %d connected with EARD", ear_my_rank);
		notify_eard_connection(1);
	}else{   
		notify_eard_connection(0);
	}
	configure_global_synchronization();
	
	// Initializing sub systems
	dynais_init(get_ear_dynais_window_size(), get_ear_dynais_levels());
	
	metrics_init();
	frequency_init(metrics_get_node_size()); //Initialize cpufreq info

	if (ear_my_rank == 0)
	{
		if (ear_whole_app == 1 && ear_use_turbo == 1) {
			verbose(2, "turbo learning phase, turbo selected and start computing");
			eards_set_turbo();
		} else {
			verbose(2, "learning phase %d, turbo %d", ear_whole_app, ear_use_turbo);
		}
	}

	ear_current_freq = frequency_get_cpu_freq(0);

	// Policies
	init_power_policy();
	init_power_models(frequency_get_num_pstates(), frequency_get_freq_rank_list());


	// Policy name is set in ear_models
	strcpy(application.job.app_id, ear_app_name);

	// Passing the frequency in KHz to MHz
	application.signature.def_f=application.job.def_f = EAR_default_frequency;
	application.job.procs = my_size;
	application.job.th =get_global_th();

	// Copying static application info into the loop info
	memcpy(&loop_signature, &application, sizeof(application_t));

	// States
	states_begin_job(my_id, NULL, ear_app_name);
	states_periodic_begin_job(my_id, NULL, ear_app_name);

	if (ear_periodic_mode==PERIODIC_MODE_ON){
		states_periodic_begin_period(my_id, NULL, 1, 1);
	}
	// Summary files
	summary_pathname = get_ear_user_db_pathname();

	if (summary_pathname != NULL) {
		sprintf(app_summary_path, "%s%s.csv", summary_pathname, node_name);
		sprintf(loop_summary_path, "%s%s.loop_info.csv", summary_pathname, node_name);
	}

	// Print things
	print_local_data();
	// ear_print_lib_environment();
	fflush(stderr);

	// Tracing init
	#if EAR_LIB_SYNC	
	traces_init(application.job.app_id,my_master_rank, my_id, num_nodes, my_size, ppnode);
	#else
	traces_init(application.job.app_id,ear_my_rank, my_id, num_nodes, my_size, ppnode);
	#endif

	traces_start();
	traces_frequency(ear_my_rank, my_id, ear_current_freq);
	traces_stop();

	traces_mpi_init();
	
	// All is OK :D
#if EAR_PERFORMANCE_TESTS
	init_end_time=PAPI_get_real_usec();
	verbose(1, "EAR initialized successfully : Initialization cost %llu usecs",(init_end_time-init_start_time));
#else
	#if EAR_LIB_SYNC
	if (my_master_rank==0) {
		verbose(1, "EAR initialized successfully ");
	}
	#else
	if (ear_my_rank==0) {
		verbose(1, "EAR initialized successfully ");
	}
	#endif
#endif
}

void ear_finalize()
{
	char summary_fullpath[BUFFSIZE];
	char node_name[BUFFSIZE];

#if EAR_PERFORMANCE_TESTS
	if (only_load) return;
#endif

	// if we are not the master, we return
	if (my_id) {
		return;
	}	

#if USE_LOCK_FILES
	verbose(2, "Application master releasing the lock %d %s", ear_my_rank,fd_lock_filename);
	file_unlock_master(fd_master_lock,fd_lock_filename);
#endif

#if MEASURE_DYNAIS_OV
	#if EAR_LIB_SYNC 
    if (my_master_rank==0) {
    #endif
	verbose(0, "DynAIS algorithm consumes %llu usecs in %u calls", ear_acum, calls);
	#if EAR_LIB_SYNC
	}
	#endif	
#endif

	// Tracing
	traces_stop();
	traces_end(ear_my_rank, my_id, 0);

	traces_mpi_end();

	// Closing and obtaining global metrics
	metrics_dispose(&application.signature, get_total_resources());
	dynais_dispose();
	frequency_dispose();

	// Writing application data
	eards_write_app_signature(&application);
	append_application_text_file(app_summary_path, &application, 1);
	report_mpi_application_data(&application);

	// Closing any remaining loop
	if (loop_with_signature) {
		verbose(2, "loop ends with %d iterations detected", ear_iterations);
	}
	#if IN_MPI_TIME
	verbose(1,"Total mpi time %.3lf secs",(double)ear_total_in_mpi/1000000.0);
	#endif
#if EAR_OVERHEAD_CONTROL
	switch(ear_periodic_mode){
		case PERIODIC_MODE_OFF:
			if (in_loop) {
				states_end_period(ear_iterations);
			}
			states_end_job(my_id, NULL, ear_app_name);
			break;
		case PERIODIC_MODE_ON:
			states_periodic_end_period(ear_iterations);
			states_periodic_end_job(my_id, NULL, ear_app_name);
			break;
	}
#else
	if (in_loop) {
		states_end_period(ear_iterations);
	}
	states_end_job(my_id, NULL, ear_app_name);
#endif

	dettach_settings_conf_shared_area();
	dettach_resched_shared_area();

	// Cest fini
	eards_disconnect();
}

void ear_mpi_call_dynais_on(mpi_call call_type, p2i buf, p2i dest);
void ear_mpi_call_dynais_off(mpi_call call_type, p2i buf, p2i dest);

void ear_mpi_call(mpi_call call_type, p2i buf, p2i dest)
{
#if EAR_PERFORMANCE_TESTS
	if (only_load) return;
#endif

#if EAR_OVERHEAD_CONTROL
	time_t curr_time;
	double time_from_mpi_init;

	if (my_id) return;
	/* The learning phase avoids EAR internals ear_whole_app is set to 1 when learning-phase is set */
	if (!ear_whole_app)
	{
		unsigned long  ear_event_l = (unsigned long)((((buf>>5)^dest)<<5)|call_type);
		unsigned short ear_event_s = dynais_sample_convert(ear_event_l);

	    traces_mpi_call(ear_my_rank, my_id,
                        (ulong) PAPI_get_real_usec(),
                        (ulong) ear_event_l,
                        (ulong) buf,
                        (ulong) dest,
                        (ulong) call_type);

		total_mpi_calls++;
		/* EAR can be driven by Dynais or periodically in those cases where dynais can not detect any period. 
		 * ear_periodic_mode can be ON or OFF 
		 */
	switch(ear_periodic_mode){
		case PERIODIC_MODE_OFF:
			switch(dynais_enabled){
				case DYNAIS_ENABLED:
					{
					/* First time EAR computes a signature using dynais, check_periodic_mode is set to 0 */
					if (check_periodic_mode==0){  // check_periodic_mode=0 the first time a signature is computed
						ear_mpi_call_dynais_on(call_type,buf,dest);
					}else{ // Check every N=check_every mpi calls
						/* Check here if we must move to periodic mode, do it every N mpicalls to reduce the overhead */
						if ((total_mpi_calls%check_every)==0){
							time(&curr_time);
							time_from_mpi_init=difftime(curr_time,application.job.start_time);
						
							/* In that case, the maximum time without signature has been passed, go to set ear_periodic_mode ON */
							if (time_from_mpi_init >=dynais_timeout){
								// we must compute N here
								ear_periodic_mode=PERIODIC_MODE_ON;
								mpi_calls_in_period=(uint)(total_mpi_calls/dynais_timeout)*lib_period;
								traces_start();
								verbose(1,"Going to periodic mode after %lf secs: mpi calls in period %u\n",
									time_from_mpi_init,mpi_calls_in_period);
								states_periodic_begin_period(my_id, NULL, 1, 1);
								ear_iterations=0;
								states_periodic_new_iteration(my_id, 1, ear_iterations, 1, 1,mpi_calls_in_period);
							}else{

								/* We continue using dynais */
								ear_mpi_call_dynais_on(call_type,buf,dest);	
							}
						}else{	// We check the periodic mode every check_every mpi calls
							ear_mpi_call_dynais_on(call_type,buf,dest);
						}
					}
					}
					break;
				case DYNAIS_DISABLED:
					#if EAR_PERFORMANCE_TESTS
					#if MEASURE_DYNAIS_OV
					if (first_time_disabled==0){
						first_time_disabled=1;
						verbose(1,"Number of Dynais calls executed before setting OFF %d\n",calls);
					}
					#endif
					#endif
					/** That case means we have computed some signature and we have decided to set dynais disabled */
					ear_mpi_call_dynais_off(call_type,buf,dest);
					break;
		
			}
			break;
		case PERIODIC_MODE_ON:
				/* EAR energy policy is called periodically */
				if ((total_mpi_calls%mpi_calls_in_period)==0){
					ear_iterations++;
					#if EAR_PERFORMANCE_TESTS
					verbose(1,"New preriodic iteration %d\n",ear_iterations);
					#endif
					states_periodic_new_iteration(my_id, 1, ear_iterations, 1, 1,mpi_calls_in_period);
				}
				break;
	}
	}
#else
    if (dynais_enabled==DYNAIS_ENABLED) ear_mpi_call_dynais_on(call_type,buf,dest);
    else ear_mpi_call_dynais_off(call_type,buf,dest);
#endif
}



void ear_mpi_call_dynais_on(mpi_call call_type, p2i buf, p2i dest)
{
	char men[128];
	short ear_status;
	int ret;

	if (my_id) {
		return;
	}
	// If ear_whole_app we are in the learning phase, DyNAIS is disabled then
	if (!ear_whole_app)
	{
		// Create the event for DynAIS
		unsigned long  trace_data[5];
		unsigned long  ear_event_l;
		unsigned short ear_event_s;
		unsigned short ear_size;
		unsigned short ear_level;

		ear_event_l = (unsigned long)((((buf>>5)^dest)<<5)|call_type);
		ear_event_s = dynais_sample_convert(ear_event_l);

		debug("EAR(%s) EAR executing before an MPI Call: DYNAIS ON\n",__FILE__);

		/*traces_mpi_call(ear_my_rank, my_id,
						(ulong) PAPI_get_real_usec(),
						(ulong) ear_event_l,
						(ulong) buf,
						(ulong) dest,
						(ulong) call_type);*/

		mpi_calls_per_loop++;
		// MEASURE_DYNAIS_OV flag is used to compute the time consumed by DyNAIs algorithm

		#if MEASURE_DYNAIS_OV
		begin_ov=PAPI_get_real_usec();
		#endif

		#if EAR_PERFORMANCE_TESTS
		if (use_dynais) {
			ear_status = dynais(ear_event_s, &ear_size, &ear_level);
		}else{
			check_periodic_mode = 0;
			return;
		}
		#else
		// This is key to detect periods
		ear_status = dynais(ear_event_s, &ear_size, &ear_level);
		#endif

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
				debug("NEW_LOOP event %u level %hu size %hu\n",ear_event_l,ear_level,ear_size);
				ear_iterations=0;
				states_begin_period(my_id, NULL, ear_event_l, (uint)ear_size);
				ear_loop_size=(uint)ear_size;
				ear_loop_level=(uint)ear_level;
				in_loop=1;
				mpi_calls_per_loop=1;
				#if MEASURE_DYNAIS_OV
				time(&start_loop_time);
				#endif
				break;
			case END_NEW_LOOP:
				debug("END_LOOP - NEW_LOOP event %u level %hu\n",ear_event_l,ear_level);
				if (loop_with_signature) {
					verbose(1, "loop ends with %d iterations detected", ear_iterations);
				}

				loop_with_signature=0;
				traces_end_period(ear_my_rank, my_id);
				states_end_period(ear_iterations);
				ear_iterations=0;
				mpi_calls_per_loop=1;
				ear_loop_size=(uint)ear_size;
				ear_loop_level=(uint)ear_level;
				states_begin_period(my_id, NULL, ear_event_l, (uint)ear_size);
				break;
			case NEW_ITERATION:
				ear_iterations++;

				if (loop_with_signature)
				{
					verbose(4,"new iteration detected for level %u, event %u, size %u and iterations %u",
							  ear_loop_level, ear_event_l, ear_loop_size, ear_iterations);
				}

				traces_new_n_iter(ear_my_rank, my_id, ear_event_l, ear_loop_size, ear_iterations);
				states_new_iteration(my_id, ear_loop_size, ear_iterations, ear_loop_level, ear_event_l, mpi_calls_per_loop);
				mpi_calls_per_loop=1;
				break;
			case END_LOOP:
				debug("END_LOOP event %u\n",ear_event_l);
				if (loop_with_signature) {
					verbose(1, "loop ends with %d iterations detected", ear_iterations);
				}
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
	short ear_status;
	int ret;
	char men[128];

	if (my_id) {
		return;
	}

	// If ear_whole_app we are in the learning phase, DyNAIS is disabled then
	if (!ear_whole_app)
	{
		// Create the event for DynAIS: we will report anyway
		unsigned long  trace_data[5];
		unsigned long  ear_event_l;
		unsigned short ear_event_s;
		unsigned short ear_size;
		unsigned short ear_level;

		ear_event_l = (unsigned long)((((buf>>5)^dest)<<5)|call_type);
		ear_event_s = dynais_sample_convert(ear_event_l);

		debug("EAR(%s) EAR executing before an MPI Call: DYNAIS ON \n",__FILE__);

		traces_mpi_call(ear_my_rank, my_id,
						(unsigned long) PAPI_get_real_usec(),
						(unsigned long) buf,
						(unsigned long) dest,
						(unsigned long) call_type,
						(unsigned long) ear_event_l);

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
				debug("NEW_LOOP event %u level %hu size %hu\n", ear_event_l, ear_level, ear_size);
				ear_iterations=0;
				//TODO
				states_begin_period(my_id, NULL, ear_event_l, (uint) ear_size);
				ear_loop_size=(uint)ear_size;
				in_loop=1;
				mpi_calls_per_loop=1;
				break;
			case END_NEW_LOOP:
				debug("END_LOOP - NEW_LOOP event %u level %hu\n",ear_event_l,ear_level);
				if (loop_with_signature) {
					verbose(1, "loop ends with %d iterations detected", ear_iterations);
				}

				loop_with_signature=0;
				traces_end_period(ear_my_rank, my_id);
				states_end_period(ear_iterations);
				ear_iterations=0;
				mpi_calls_per_loop=1;
				ear_loop_size=(uint)ear_size;
				states_begin_period(my_id, NULL, ear_event_l, (uint)ear_size);
				break;
			case NEW_ITERATION:
				ear_iterations++;

				if (loop_with_signature)
				{
					verbose(4,"new iteration detected for level %hu, event %u, size %u and iterations %u",
							  ear_level, ear_event_l, ear_loop_size, ear_iterations);
				}

				traces_new_n_iter(ear_my_rank, my_id, ear_event_l, ear_loop_size, ear_iterations);
				#if EAR_PERFORMANCE_TESTS
				verbose(3,"New estimated iteration\n");
				#endif
				states_new_iteration(my_id, ear_loop_size, ear_iterations, (uint)ear_level, ear_event_l, mpi_calls_per_loop);
				mpi_calls_per_loop=1;
				break;
			case END_LOOP:
				debug("END_LOOP event %u\n",ear_event_l);
				if (loop_with_signature) {
					verbose(1, "loop ends with %d iterations detected", ear_iterations);
				}
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

