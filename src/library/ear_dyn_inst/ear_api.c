/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
// EAR includes
#include <environment.h>
#include <ear_verbose.h>
#include <ear_dyn_inst/MPI_types.h>
#include <ear_dyn_inst/MPI_calls_coded.h>
#include <ear_dynais/ear_dynais.h>
#include <ear_models/ear_models.h>
#include <ear_metrics/ear_papi.h>
#include <ear_metrics/ear_node_energy.h>
#include <ear_frequency/ear_cpufreq.h>
#include <ear_db/ear_db.h>
#include <ear_gui/ear_gui.h>
#include <ear_states/ear_states.h>


#define BUFFSIZE 128
unsigned int ear_loop_size;
unsigned long int app_eru_init,app_eru_end,app_eru_diff;

int report=0;
// long long pmpi_app_begin_time,pmpi_app_end_time,pmpi_app_total_time;
struct timeval pmpi_app_begin_time, pmpi_app_end_time;
long long pmpi_app_total_time;


int period,new_period; 
int my_id,my_size;
int ear_my_rank;
int ear_iterations=0,min_period=10;
const char *ear_out;
const char *ear_iters;
unsigned long ear_frequency;
unsigned long EAR_default_frequency;
unsigned int EAR_default_pstate;
unsigned long ear_current_freq;
int ear_current_cpuid;
unsigned int ear_resources;
char *ear_tmp;
FILE *ear_fd;
int ear_whole_app=0;
int ear_use_turbo=USE_TURBO;
char ear_app_name[MAX_APP_NAME];
char ear_node_name[MAX_APP_NAME];
char ear_lock_file[BUFFSIZE];
int ear_lock_fd;
unsigned long ear_avg_turbo_freq;
int ear_app_id;
struct App_info *ear_my_app_info;
//#define MEASURE_DYNAIS_OV
#ifdef MEASURE_DYNAIS_OV
long long begin_ov,end_ov,ear_acum=0;
unsigned int calls=0;
#endif
#define DYNAIS_TRACE
#ifdef DYNAIS_TRACE
FILE *stdtrace,*stdtracebin;
#endif
int EAR_VERBOSE_LEVEL=0;

# define __USE_GNU
# include <dlfcn.h>
# undef  __USE_GNU
int (*my_omp_get_max_threads)(void) = NULL;


int check_threads()
{
        my_omp_get_max_threads = (int(*)(void)) dlsym (RTLD_DEFAULT, "mkl_get_max_threads");
	if (my_omp_get_max_threads==NULL){
		my_omp_get_max_threads = (int(*)(void)) dlsym (RTLD_DEFAULT, "omp_get_max_threads");
		if (my_omp_get_max_threads==NULL) return 0;
		else return 1;
	} 
	else return 1;
}

void ear_init(){
	char file_name[BUFFSIZE],node_name[BUFFSIZE],*ear_summary_filename,my_name[BUFFSIZE];
	char tmp[BUFFSIZE];
	char *freq,*ear_tmp;
	int size,node_name_length;
	int local_id;	
	unsigned long optimal_freq;
	struct stat lock_st;
	char *snum_nodes;
	unsigned int num_nodes,ppnode;
    	char *header_instances="USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;GFLOPS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;POLICY;POLICY_TH\n";
    	//char *header_instances="USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;EDP;POLICY;POLICY_TH\n";
	int new_fd=0,ret;


	PMPI_Comm_rank(MPI_COMM_WORLD,&ear_my_rank);
	PMPI_Comm_size(MPI_COMM_WORLD,&my_size);
	ear_lib_environment();
	set_ear_total_processes(my_size);
	EAR_VERBOSE_LEVEL=get_ear_verbose();
	if (get_ear_app_name()!=NULL){
		if (ear_my_rank==0) ear_verbose(1,"________ EAR: Application %s starts__________\n",get_ear_app_name());
	}else{
		if (ear_my_rank==0) ear_verbose(1,"EAR: Application starts.....\n");
	}
	ear_debug(2,"EAR Starting initialization\n");	
	ear_whole_app=get_ear_learning_phase();
	ear_verbose(3,"EAR using %d levels in dynais with %d of window size \n",get_ear_dynais_levels(),get_ear_dynais_window_size());
   	dynais_init(get_ear_dynais_window_size(),get_ear_dynais_levels());

	gethostname(node_name,sizeof(node_name));
	strcpy(ear_node_name,node_name);
	my_id=get_ear_local_id();
	if (my_id<0){
		num_nodes=get_ear_num_nodes();
        ppnode=my_size/num_nodes;
        my_id=(ear_my_rank%ppnode);
	}
	if (my_id) return;
	if (check_threads()){
		ear_verbose(2,"EAR: OpenMP is used\n");
		ear_verbose(1,"EAR: OpenMP max_threads %d\n",my_omp_get_max_threads());
	}else{
		ear_verbose(2,"EAR: OpenMP is not used\n");
	}

#ifdef DYNAIS_TRACE
	if (ear_my_rank==0){
        stdtrace=fopen("app.dynais.trace.csv","w");
        if (stdtrace==NULL){
                perror("Error opening app.dynais.trace.csv trace file");
                exit(1);
        }
        stdtracebin=fopen("app.dynais.trace.bin","w");
        if (stdtracebin==NULL){
                perror("Error opening app.dynais.trace.bin trace file");
                exit(1);
        }
	}
#endif
	// Connecting with ear_daemon
	if (ear_daemon_client_connect()<0){
		ear_verbose(0,"EAR: Connect with EAR daemon fails\n");
		exit(1);
	}
	ear_verbose(1,"EAR: MPI rank %d defined as node master for %s pid: %d\n",ear_my_rank,ear_node_name, getpid());
	metrics_init(my_id,getpid()); // PAPI_init starts counters
	ear_cpufreq_init(); //Initialize cpufreq info
	if (ear_whole_app==1 && ear_use_turbo==1){
		ear_verbose(1,"EAR: Turbo learning phase, turbo selected and start computing\n");
		ear_daemon_client_set_turbo();
	}else ear_verbose(1,"EAR: learning phase %d, turbo %d\n",ear_whole_app,ear_use_turbo);
	ear_daemon_client_begin_compute_turbo_freq();
	ear_summary_filename=get_ear_user_db_pathname();
	sprintf(file_name,"%s.%s",ear_summary_filename,ear_node_name);
	if (open(file_name,O_RDONLY)<0) new_fd=1;
	ear_fd=fopen(file_name,"a");	
	if (ear_fd==NULL) {
		ear_verbose(0,"EAR: Opening EAR user_db file, user metrics will not be written : %s (%s)\n",file_name, strerror(errno));
	}else{
		ear_verbose(1,"EAR: Opening user DB file %s\n",file_name);
	}
	if ((new_fd)&& (ear_fd!=NULL)) fprintf(ear_fd,"%s",header_instances);

	db_get_app_name(ear_app_name);
    	states_begin_job(my_id,ear_fd,ear_app_name);
	ear_current_cpuid=getCPU_ID();// this function needs papi JORDI_NEW
	ear_current_freq=ear_cpufreq_get(ear_current_cpuid);	
	init_power_policy();
	init_power_models(ear_get_num_p_states(),ear_get_pstate_list());
	// Application info
	db_init(ear_whole_app,ear_app_name);
    	if(init_dc_energy()<0){
    		ear_verbose(0,"EAR:: Node Energy can not be measured, AEM is not loaded, exiting\n");
    	    exit(1) ;
    	}else{
		ear_debug(1,"EAR: init_dc_energy ok!\n");
	}
    	app_eru_init=read_dc_energy();
	 
	gettimeofday(&pmpi_app_begin_time,NULL);
	fflush(stderr);
	gui_init(ear_my_rank,my_id,ear_app_name,ear_node_name,num_nodes,my_size,ppnode);
	gui_frequency(ear_my_rank,my_id,ear_current_freq);
	ear_debug(1,"EAR Initialized successfully\n");	
	ear_print_lib_environment();
	ear_verbose(1,"______________EAR loaded___________________\n");
}
void ear_mpi_call(mpi_call call_type, p2i buf, p2i dest){
	unsigned ear_status;
	int ret;
	char men[128];
	if (my_id) return;
// If ear_whole_app we are in the learning phase, DyNAIS is disabled then
if (!ear_whole_app){
	ear_debug(3,"EAR(%s) EAR executing before an MPI Call\n",__FILE__);
	// Create the event for DynAIS
	unsigned long ear_event = (unsigned long)((((buf>>5)^dest)<<5)|call_type);
    unsigned int ear_size;
	unsigned int ear_level;
	unsigned long trace_data[5];
// DYNAIS_TRACE generates a text trace file with values used as dynais imput, it is used for dynais evaluation and optimization
#ifdef DYNAIS_TRACE
	if (ear_my_rank==0){
	fprintf(stdtrace,"%u;%u;%u;%u;%llu\n",buf,dest,call_type,ear_event,PAPI_get_real_usec());
	trace_data[0]=(unsigned long)buf;
	trace_data[1]=(unsigned long)dest;
	trace_data[2]=(unsigned long)call_type;
	trace_data[3]=(unsigned long)ear_event;
	trace_data[4]=(unsigned long)PAPI_get_real_usec();
	fwrite(trace_data,sizeof(unsigned long),5,stdtracebin);
	}
#endif

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
	switch (ear_status){
	case NO_LOOP:
	case IN_LOOP:
		break;
	case NEW_LOOP:
		ear_debug(4,"NEW_LOOP event %u level %u size %u\n",ear_event,ear_level,ear_size);
		ear_iterations=0;
		states_begin_period(my_id,ear_fd,ear_event,ear_size);
		ear_loop_size=ear_size;
		break;
	case END_NEW_LOOP:
		ear_debug(4,"END_LOOP - NEW_LOOP event %u level %u\n",ear_event,ear_level);
		gui_end_period(ear_my_rank,my_id);
		states_end_period(my_id,ear_fd,ear_loop_size,ear_iterations,ear_event);
		ear_iterations=0;
		ear_loop_size=ear_size;
		states_begin_period(my_id,ear_fd,ear_event,ear_size);
		break;
	case NEW_ITERATION:
		ear_iterations++;
		if (report==1){
		ear_verbose(3,"NEW_ITERATION level %u event %u size %u iterations %u\n",ear_level,
		ear_event,ear_loop_size,ear_iterations);
		}
		gui_new_n_iter(ear_my_rank,my_id,ear_event,ear_loop_size,ear_iterations,states_my_state());	
		states_new_iteration(my_id,ear_fd,ear_loop_size,ear_iterations,ear_event,ear_level);
		break;
	case END_LOOP:
		ear_debug(4,"END_LOOP event %u\n",ear_event);
		gui_end_period(ear_my_rank,my_id);
		states_end_period(my_id,ear_fd,ear_loop_size,ear_iterations,ear_event);
		ear_iterations=0;
		break;
	default:
		break;
	}
} //ear_whole_app
}


void ear_finalize(){
	if (my_id) return;
	int iters=0;
#ifdef MEASURE_DYNAIS_OV
	ear_verbose(0,"EAR:: Dynais algorithm consumes %llu usecs in %u calls\n",ear_acum,calls);
#endif
	app_eru_end=read_dc_energy();
	app_eru_diff=energy_diff(app_eru_end , app_eru_init);
	gettimeofday(&pmpi_app_end_time,NULL);
	pmpi_app_total_time=(pmpi_app_end_time.tv_sec*1000000+pmpi_app_end_time.tv_usec)-(pmpi_app_begin_time.tv_sec*1000000+pmpi_app_begin_time.tv_usec);

	//ear_verbose(0,"EAR: Accumulated energy %llu J. Execution time %llu (usecs) Avg. Power %5.5lf (W)\n",app_eru_diff/1000000,
	//	pmpi_app_total_time,(double)app_eru_diff/(double)pmpi_app_total_time);

	gui_end(ear_my_rank,my_id,app_eru_diff);
	metrics_end(ear_whole_app,my_id,ear_fd,&app_eru_diff); // PAPI_end stops counters and process metrics
	db_end(ear_whole_app);
	dynais_dispose();

    states_end_job(my_id,ear_fd,ear_app_name);
	ear_cpufreq_end();
	end_dc_energy();
	ear_avg_turbo_freq=ear_daemon_client_end_compute_turbo_freq();
	ear_daemon_client_disconnect();
#ifdef DYNAIS_TRACE
	if (ear_my_rank==0){
		fclose(stdtrace);
		fclose(stdtracebin);
	}
#endif
}



