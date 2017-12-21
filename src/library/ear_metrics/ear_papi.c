/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <string.h>
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>


#include <ear_verbose.h>
#include <ear_daemon_client.h>
#include <ear_db/ear_db.h>
#include <ear_metrics/ear_papi.h>
#include <ear_metrics/ear_turbo_metrics.h>
#include <ear_metrics/ear_flops_metrics.h>
#include <ear_models/ear_models.h>

#define EAR_EXTRA_METRICS

extern int ear_whole_app;
extern int ear_use_turbo;
extern int ear_frequency;
extern int ear_resources;
extern int ear_my_rank;
extern int power_model_policy;
extern char ear_policy_name[MAX_APP_NAME];
extern double ear_policy_th;
extern char ear_app_name[MAX_APP_NAME];


int ear_node_size;
int ear_papi_init=0;
int ear_cache_line_size;
#define MIN_TIME_BETWEEN_CHANGES 1000000
int num_ctrs;
static long long start_time=0, end_time=0, iter_time=0;
long long app_start_time=0,app_end_time=0,app_exec_time=0;
int num_ctrs;

#define MULTIPLEX_PAPI
#ifdef MULTIPLEX_PAPI
int papi_multiplex=1;
#endif

typedef union { long long ll; double dbl; } ll_dbl_union_t;

#define MAX_SETS 1

#define EVENT_SET_PRESET 0

int EventSet[MAX_SETS];
//perf component
#define	EAR_PAPI_TOT_CYC 0 // Total Cycles
#define EAR_PAPI_TOT_INS 1 // Total instructions
#define READS_OFFSET 0
#define WRITES_OFFSET 1
#define DRAM_OFFSET 0
#define PACK_OFFSET 1


#define	EAR_ACUM_TOT_CYC 0 // Total Cycles
#define EAR_ACUM_TOT_INS 1 // Total instructions
#define EAR_ACUM_LD_INS 2 //Load instructions
#define EAR_ACUM_SR_INS 3 //Store instructions
#define EAR_ACUM_DRAM_ENER 4 //ENERGY  reported by rapl (DRAM)
#define EAR_ACUM_PCKG_ENER 5 //ENERGY  reported by rapl (CORES+CACHE)

#define TOTAL_EVENTS 6
#define NUM_EVENTS 2

// PAPI specific
long long events_values[MAX_SETS][NUM_EVENTS];
int event_codes[NUM_EVENTS];
int data_type[NUM_EVENTS];
char units[NUM_EVENTS][PAPI_MIN_STR_LEN];
char event_names[NUM_EVENTS][PAPI_MIN_STR_LEN];
PAPI_cpu_option_t cpu_opt[MAX_SETS];
PAPI_granularity_option_t gran_opt[MAX_SETS];
PAPI_domain_option_t domain_opt[MAX_SETS];
PAPI_attach_option_t attach_opt[MAX_SETS];
PAPI_event_info_t evinfo[NUM_EVENTS];
const PAPI_hw_info_t *ear_hwinfo=NULL;
int perf_cid=-1;

// UNCORE
long long *event_values_uncore;
unsigned long uncore_size,uncore_elements;
//RAPL
long long *event_values_rapl;
unsigned long rapl_size,rapl_elements;

//FLOPS

long long total_flops;

// This is for all the metrics
long long acum_event_values[TOTAL_EVENTS];
long long last_iter_event_values[TOTAL_EVENTS];
long long diff_event_values[TOTAL_EVENTS];

long long acum_iter_time=0;
long long acum_energy=0;
#define TOTAL_CPUS (ear_hwinfo->threads*ear_hwinfo->cores*ear_hwinfo->sockets)
#define SOCKETS ear_hwinfo->sockets
int ear_cpu_id;


#define MY_MAX 281474976710655

long long metrics_usecs_diff(long long end,long long init)
{
	long long to_max;
	//LLONG_MAX
	if (end < init){
		to_max=LLONG_MAX-init;
		ear_debug(0,"EAR(%s) UsecsDiff TIME END < INIT %ll - %ll \n",__FILE__,end,init);
		return (to_max+end);
	}else return (end-init);
	
}

void metrics_get_app_name(char *app_name)
{
	const PAPI_exe_info_t *prginfo = NULL;
	if (!ear_papi_init){
		ear_debug(1,"EAR(%s): Warning, PAPI not initialized yet,using DEFAULT as application name\n",__FILE__);
		sprintf(app_name,"default");
		return;
	}
	if ( ( prginfo = PAPI_get_executable_info( ) ) == NULL ){
		ear_verbose(0,"EAR(%s): Executable info not available. Exiting\n",__FILE__);
		exit(2 );
	}
	strcpy(app_name,prginfo->fullname );
}
int getCPU_ID()
{
        int cpuid=-1,c=0;
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        sched_getaffinity(getpid(),sizeof(cpu_set_t), &cpu_set);
        while((cpuid==-1)&& (c<TOTAL_CPUS)){
                if (CPU_ISSET(c,&cpu_set)) cpuid=c;
                else c++;
        }
	return cpuid;
}


void ear_papi_error(int ret,char *m)
{
        if (ret!=PAPI_OK){
                ear_verbose(0,"EAR(%s):%s -  %s\n",__FILE__,m,PAPI_strerror(ret));
                exit(2);
        }
}
void print_units(FILE* fd)
{
	int i;
	for (i=0;i<NUM_EVENTS;i++){
		ear_debug(4,"EAR(%s) EAR_Print_units : Units for %s is %s\n",__FILE__,event_names[i],units[i]);
		if (data_type[i] == PAPI_DATATYPE_FP64){
			ear_debug(4,"EAR(%s): Data type PAPI_DATATYPE_FP64,__FILE__\n");
		}else{
			ear_debug(4,"EAR(%s): Data type PAPI_DATATYPE_UINT64\n",__FILE__);
		}

	}
}


void init_basic_event_info(int ear_event,PAPI_event_info_t *info){
		strncpy(units[ear_event], info->units, sizeof( units[0] )-1 );
        // buffer must be null terminated to safely use strstr operation on it below
        units[ear_event][sizeof( units[0] )-1] = '\0';
        data_type[ear_event] = info->data_type;
}
void papi_initialice_events(int sets,int pid)
{
	int retval;
	ear_verbose(3,"EAR(%s) Initializing event set %d\n",ear_app_name,sets);
	switch(sets){
	case EVENT_SET_PRESET:
		// Attach event set to process id
		ear_papi_error(PAPI_assign_eventset_component(EventSet[sets],0),"EAR(PAPI):Assigning perf component to event set");
		attach_opt[sets].eventset=EventSet[sets];
		attach_opt[sets].tid=pid;
		ear_papi_error(PAPI_set_opt(PAPI_ATTACH,(PAPI_option_t*)&attach_opt[sets]),"EAR(PAPI):Attaching the PID ");
		// Selecting events	
		strncpy(event_names[EAR_PAPI_TOT_CYC],"ix86arch::UNHALTED_CORE_CYCLES",PAPI_MIN_STR_LEN);
		strncpy(event_names[EAR_PAPI_TOT_INS],"ix86arch::INSTRUCTION_RETIRED",PAPI_MIN_STR_LEN);
		ear_papi_error(PAPI_add_named_event(EventSet[sets],"ix86arch::UNHALTED_CORE_CYCLES"),"Adding events to event set: ix86arch::UNHALTED_CORE_CYCLES");	
		ear_papi_error(PAPI_add_named_event(EventSet[sets],"ix86arch::INSTRUCTION_RETIRED"),"Adding events to event set: ix86arch::INSTRUCTION_RETIRED");	
		ear_papi_error(PAPI_event_name_to_code("ix86arch::UNHALTED_CORE_CYCLES",&event_codes[EAR_PAPI_TOT_CYC]),"Getting ix86arch::UNHALTED_CORE_CYCLES event code");
		ear_papi_error(PAPI_event_name_to_code("ix86arch::INSTRUCTION_RETIRED",&event_codes[EAR_PAPI_TOT_INS]),"Getting ix86arch::INSTRUCTION_RETIRED event code");
		ear_papi_error(PAPI_get_event_info(event_codes[EAR_PAPI_TOT_CYC],&evinfo[EAR_PAPI_TOT_CYC]),"Getting Event info for Total cycles");
		init_basic_event_info(EAR_PAPI_TOT_CYC,&evinfo[EAR_PAPI_TOT_CYC]);
		ear_papi_error(PAPI_get_event_info(event_codes[EAR_PAPI_TOT_INS],&evinfo[EAR_PAPI_TOT_INS]),"Getting Event info for Total instructions");
		init_basic_event_info(EAR_PAPI_TOT_INS,&evinfo[EAR_PAPI_TOT_INS]);
		break;
	default:ear_verbose(0,"EAR(%s): Error event set not supported in EAR_PAPI_Initialice_events\n",ear_app_name);
	}
	
}
void metrics_start()
{
	int sets;
	char buff[128];
	ear_daemon_client_start_uncore();
	ear_daemon_client_start_rapl();
	for (sets=0;sets<MAX_SETS;sets++){
		ear_debug(4,"EAR(%s): Starting counter sets %d\n",__FILE__,sets);
        ear_papi_error(PAPI_start(EventSet[sets]),buff);
	}
#ifdef EAR_EXTRA_METRICS
	start_turbo_metrics();
	start_flops_metrics();
#endif
}

void metrics_stop()
{
	int sets;
	// We should ask for size when initializing and reserving memory with malloc
	if (ear_daemon_client_read_uncore(event_values_uncore)<0){
		// Should we exit??
		ear_verbose(0,"EAR(%s): Reading uncore counters to compute MB is failing\n",ear_app_name);
	}
	if (ear_daemon_client_read_rapl(event_values_rapl)<0){
		// Should we exit??
		ear_verbose(0,"EAR(%s): Reading rapl is is failing\n",ear_app_name);
	}
	for (sets=0;sets<MAX_SETS;sets++){
		ear_papi_error(PAPI_stop(EventSet[sets],(long long *)&events_values[sets]),"EAR:Stopping and reading PAPI counters");
	}
#ifdef EAR_EXTRA_METRICS
	stop_turbo_metrics();
	stop_flops_metrics(&total_flops);
#endif
}

void metrics_reset()
{
	int sets;
	if (ear_daemon_client_reset_uncore()<0){
		// Should we exit??
		ear_verbose(0,"EAR(%s): Reset uncore counters to compute MB is failing\n",ear_app_name);
	}
	if (ear_daemon_client_reset_rapl()<0){
		// Should we exit??
		ear_verbose(0,"EAR(%s): Reset uncore counters to compute MB is failing\n",ear_app_name);
	}
	for (sets=0;sets<MAX_SETS;sets++){
		if (PAPI_reset(EventSet[sets])!=PAPI_OK) ear_verbose(0,"EAR(%s):PAPI reset counters at each new period or iteration",ear_app_name);
	}
#ifdef EAR_EXTRA_METRICS
	reset_turbo_metrics();
	reset_flops_metrics();
#endif
}

void reset_values()
{
	int sets,i;
	ear_debug(4,"EAR(%s): EAR_Reset_values\n",__FILE__);
	for (sets=0;sets<MAX_SETS;sets++){
		for (i=0;i<NUM_EVENTS;i++) events_values[sets][i]=0;
	}
	for (i=0;i<uncore_elements;i++) event_values_uncore[i]=0;
	for (i=0;i<rapl_elements;i++) event_values_rapl[i]=0;
}

void metrics_get_hw_info(int *sockets,int *cores_socket,unsigned long *max_f,unsigned long *min_f,unsigned long *nom_f,char *CPU_model,char *CPU_name)
{
	*sockets=ear_hwinfo->sockets;
	*cores_socket=ear_hwinfo->cores;
	*max_f=ear_hwinfo->cpu_max_mhz;
	*min_f=ear_hwinfo->cpu_min_mhz;
	*nom_f=ear_get_nominal_frequency();
	strcpy(CPU_model,ear_hwinfo->model_string);
	strcpy(CPU_name,ear_hwinfo->vendor_string);
}
 
char *iters;
/////// Init function
int metrics_init(int my_id,int pid)
{
	int retval,ret;
	int sets,i;
	

	if (my_id) return 1;
	// This must be change to new EAR fucntion
	ear_cache_line_size=ear_discover_cache_line_size();
	ear_debug(2,"EAR(%s):: Cache line size %d\n",__FILE__,ear_cache_line_size);
	// Init the PAPI library
	if (!ear_papi_init){
	if (PAPI_is_initialized()==PAPI_NOT_INITED){
		retval=PAPI_library_init(PAPI_VER_CURRENT );
		if ( retval != PAPI_VER_CURRENT ) {
			ear_verbose(0,"EAR: Error intializing the PAPI library.Exiting\n");
			exit(1);
 		}
	}
	else{
		ear_verbose(3,"EAR PAPI_library_init initialized\n");
	}
	ear_papi_init=1;
	}
	// Checking the perf_event component  component 
	perf_cid=PAPI_get_component_index("perf_event");
	if (perf_cid<0){
		ear_verbose(0,"EAR: perf_event component not found(%s) .Exiting\n",PAPI_strerror(perf_cid));
		exit(1);
	}
	// Checking HW info
    ear_hwinfo= PAPI_get_hardware_info();
	if (ear_hwinfo==NULL){
		ear_verbose(0,"EAR: WARNING PAPI_get_hardware_info returns NULL\n");
	}else{
    	ear_verbose(3,"EAR: %d CPUsPerNUMA-Node, \n \
                %d Number of hdw threads per core\n \
                %d Number of cores per socket \n \
                %d Number of sockets \n \
                %d Total Number of NUMA Nodes \n \
                (max-mhz %d min-mhz %d)\n\
				model %d model_name %s\n",
				ear_hwinfo->ncpu,
                ear_hwinfo->threads,
                ear_hwinfo->cores,
                ear_hwinfo->sockets,
                ear_hwinfo->nnodes,
                ear_hwinfo->cpu_max_mhz,ear_hwinfo->cpu_min_mhz,
			ear_hwinfo->model,ear_hwinfo->model_string);
	}
	ear_node_size=ear_hwinfo->threads*ear_hwinfo->cores;
	// This must be change to new EAR fucntion
    ear_cpu_id=getCPU_ID();
#ifdef MULTIPLEX_PAPI
	if ((ret=PAPI_multiplex_init())!=PAPI_OK){
		ear_verbose(0,"EAR: WARNING PAPI_multiplex_init fails: %s\n",PAPI_strerror(ret));
		papi_multiplex=0;
	}	
#endif
	// We create N event sets: preset
	for (sets=0;sets<MAX_SETS;sets++){
		EventSet[sets]=PAPI_NULL;
	    ear_papi_error(PAPI_create_eventset(&EventSet[sets]),"Creating event set");
#ifdef MULTIPLEX_PAPI
		if (papi_multiplex){
			if ((ret=PAPI_set_multiplex(EventSet[sets]))!=PAPI_OK){ 
				if (ret!=PAPI_OK){ 
					ret=PAPI_get_multiplex(EventSet[sets]);
					if (ret>0){        ear_verbose(1,"EAR: Event set %d is ready for multiplexing\n",sets);
					}else if (ret==0){ ear_verbose(1,"EAR: Event set %d is not enabled for multiplexing\n",sets);
					}else if (ret<0) { ear_verbose(0,"EAR: Event set %d cannot be multiplexed: %s\n",sets,PAPI_strerror(ret));
					}
				}
			}
		}
#endif
		papi_initialice_events(sets,pid);
	}
	// We ask for uncore and rapl metrics sizes
	uncore_size=ear_daemon_client_get_data_size_uncore();
	rapl_size=ear_daemon_client_get_data_size_rapl();
	// We reserve memory for uncore and rapl metrics
	event_values_uncore=malloc(uncore_size);
	if (event_values_uncore==NULL){
		ear_verbose(0,"EAR: Error, malloc returns NULL when reserving memory for uncore metrics.Exiting\n");
		exit(1);
	}
	uncore_elements=uncore_size/sizeof(long long);
	event_values_rapl=malloc(rapl_size);
	if (event_values_rapl==NULL){
		ear_verbose(0,"EAR: Error, malloc returns NULL when reserving memory for rapl metrics.Exiting\n");
		exit(1);
	}
	rapl_elements=rapl_size/sizeof(long long);
	for (i=0;i<TOTAL_EVENTS;i++){
		acum_event_values[i]=0;
		last_iter_event_values[i]=0;
		diff_event_values[i]=0;
	}
#ifdef EAR_EXTRA_METRICS
	init_turbo_metrics();
	init_flops_metrics();
#endif
	reset_values();
	metrics_reset();
	metrics_start();
	app_start_time=PAPI_get_real_usec();


	return 0;
}
/////// End function
void metrics_end(unsigned int whole_app,int my_id,FILE* fd,unsigned long int *eru)
{
	metrics_stop();
	// Save in global counters
	acum_counters();

	app_end_time=PAPI_get_real_usec();
	app_exec_time=app_end_time-app_start_time;
	acum_energy=*eru;
	// This function computes metrics, signature etc
	metrics_print_summary(whole_app,my_id,fd);
	
#ifdef EAR_EXTRA_METRICS
	print_turbo_metrics(acum_event_values[EAR_ACUM_TOT_INS]);
	//print_gflops(acum_event_values[EAR_ACUM_TOT_INS],app_exec_time);
#endif
}

struct App_info* set_metrics(int period,int iteration,long long *counters,long long iter_time,unsigned long int *eru,unsigned int N_iters)
{
		double CPI,GBS,Seconds,TPI,POWER;
		unsigned int app_id;
		struct App_info *app_info;
		unsigned long f;
		ear_debug(4,"EAR(%s): Setting DB metrics for period %d iteration %d\n",__FILE__,period,N_iters);
		Seconds=(double)iter_time/(double)1000000;
		CPI=(double)counters[EAR_ACUM_TOT_CYC]/(double)counters[EAR_ACUM_TOT_INS];
		GBS=((double)(counters[EAR_ACUM_LD_INS]*ear_cache_line_size)/(Seconds*(double)(1024*1024*1024)))+ ((double)(counters[EAR_ACUM_SR_INS]*ear_cache_line_size)/(Seconds*(double)(1024*1024*1024)));
		TPI=(double)(counters[EAR_ACUM_LD_INS]+counters[EAR_ACUM_SR_INS])/(double)(counters[EAR_ACUM_TOT_INS]/ear_cache_line_size);
		POWER=(double)*eru/(double)(Seconds*1000000);
		ear_debug(4,"EAR(%s):: Set_metrics: seconds %.5lf GBS %.5lf POWER %12.6f TPI %12.6f CPI %5lf\n",
		__FILE__,Seconds/(double)N_iters,GBS,POWER,TPI,CPI);
        app_info=db_current_app();
        db_set_GBS(app_info,GBS);
        db_set_POWER(app_info,POWER);
        db_set_TPI(app_info,TPI);
        db_set_seconds(app_info,Seconds/(double)N_iters);
        db_set_CPI(app_info,CPI);
		db_set_frequency(app_info,ear_cpufreq_get(0));
		return app_info;
}
void metrics_print_summary(unsigned int whole_app,int my_id,FILE* fd)
{
		double CPI,GBS,GIBS,Seconds,GIBS_ranks,TPI,POWER,POWER_DC,DRAM_POWER,PCK_POWER,CORE_POWER,GFLOPS,EDP;

		unsigned int app_id;
		struct App_info *app_info;
		struct App_info SIGNATURE;
		unsigned long f,optimal;
		double PP,TP,EP,perf_deg,power_sav,energy_sav,ener,new_EDP;
	    char *app_name;
		
		int i,new;
		app_info=db_current_app();

		// Compute signature=METRICS
		Seconds=(double)app_exec_time/(double)1000000;
		CPI=(double)acum_event_values[EAR_ACUM_TOT_CYC]/(double)acum_event_values[EAR_ACUM_TOT_INS];
		GBS=((double)(acum_event_values[EAR_ACUM_LD_INS]*ear_cache_line_size)/(Seconds*(double)(1024*1024*1024)))+ ((double)(acum_event_values[EAR_ACUM_SR_INS]*ear_cache_line_size)/(Seconds*(double)(1024*1024*1024)));
		TPI=(double)(acum_event_values[EAR_ACUM_LD_INS]+acum_event_values[EAR_ACUM_SR_INS])/(double)(acum_event_values[EAR_ACUM_TOT_INS]/ear_cache_line_size);
		GIBS=(double)ear_node_size*(double)ear_frequency/(CPI*1000000);
		GFLOPS=gflops(app_exec_time);
		GIBS_ranks=(double)ear_resources*(double)ear_frequency/(CPI*1000000);
		POWER_DC=(double)acum_energy/(double)(Seconds*1000000);
		EDP=Seconds*Seconds*POWER_DC;
		DRAM_POWER=(double)(acum_event_values[EAR_ACUM_DRAM_ENER]/1000000000)/Seconds;
		PCK_POWER=(double)(acum_event_values[EAR_ACUM_PCKG_ENER]/1000000000)/Seconds;
		f=ear_daemon_client_end_compute_turbo_freq();
		// We can write summary metrics in any DB or any other place
#if 0
		fprintf(stderr,"_____________________APP SUMMARY ___________________\n");
		fprintf(stderr,"MPI_RANK= %d\n",my_id);
		fprintf(stderr,"ACUM CYCLES %llu\n",acum_event_values[EAR_ACUM_TOT_CYC]);
		fprintf(stderr,"ACUM INS %llu\n",acum_event_values[EAR_ACUM_TOT_INS]);
		fprintf(stderr,"ACUM COUNT_CAS_RD %llu\n",acum_event_values[EAR_ACUM_LD_INS]);
		fprintf(stderr,"ACUM COUNT_CAS_WR %llu\n",acum_event_values[EAR_ACUM_SR_INS]);
		fprintf(stderr,"ACUM DC ENERGY %llu mJ\n",acum_energy);
		fprintf(stderr,"ACUM RAPL DRAM ENERGY  %llu\n",acum_event_values[EAR_ACUM_DRAM_ENER]); 
		fprintf(stderr,"ACUM RAPL PCKG ENERGY  %llu\n",acum_event_values[EAR_ACUM_PCKG_ENER]); 
#endif
		app_name=get_ear_app_name();
		if (app_name!=NULL) strcpy(SIGNATURE.app_id,app_name);
		else strcpy(SIGNATURE.app_id,"NA");
		// Reporting application signature metrics at stderr
		fprintf(stderr,"_____________________EAR Summary for %s ___________________\n",app_info->node_id);
		fprintf(stderr,"EAR job_id %s user_id %s app_id %s exec_time %.3lf\n",app_info->job_id,app_info->user_id,SIGNATURE.app_id,(double)app_exec_time/(double)1000000);
		fprintf(stderr,"EAR CPI=%.3lf GBS=%.3lf GFlops=%.3lf\n",CPI,GBS,GFLOPS);
		fprintf(stderr,"EAR avg. node power=%.3lfW, avg. RAPL dram power=%.3lfW, avg. RAPL pck. power=%.3lfW EDP=%.3lf GFlops/Watts=%.3lf\n",POWER_DC,DRAM_POWER,PCK_POWER,EDP,GFLOPS/POWER_DC);
		fprintf(stderr,"EAR def. frequency %.3lf GHz avg. frequency %.3lf GHz\n",(double)app_info->nominal/(double)1000000,(double)f/(double)1000000);

		// app_info links to the DB information . Update SIGNATURE metrics to report it at user_db and system_db
		strcpy(SIGNATURE.job_id,app_info->job_id);
		strcpy(SIGNATURE.user_id,app_info->user_id);
		strcpy(SIGNATURE.node_id,app_info->node_id);
		db_set_GBS(&SIGNATURE,GBS);
		db_set_GIPS(&SIGNATURE,GIBS);
		db_set_POWER(&SIGNATURE,POWER_DC);
		db_set_TPI(&SIGNATURE,TPI);
		db_set_seconds(&SIGNATURE,Seconds);
		db_set_CPI(&SIGNATURE,CPI);
		db_set_CYCLES(&SIGNATURE,acum_event_values[EAR_ACUM_TOT_CYC]);
		db_set_INSTRUCTIONS(&SIGNATURE,acum_event_values[EAR_ACUM_TOT_INS]);
		db_set_POWER_DC(&SIGNATURE,POWER_DC);
		db_set_DRAM_POWER(&SIGNATURE,DRAM_POWER);
		db_set_PCK_POWER(&SIGNATURE,PCK_POWER);
		db_set_frequency(&SIGNATURE,f);
		db_set_Gflops(&SIGNATURE,GFLOPS);
		db_set_EDP(&SIGNATURE,EDP);
		db_set_default(&SIGNATURE,app_info->nominal);
		db_set_policy(&SIGNATURE,ear_policy_name);
		db_set_th(&SIGNATURE,ear_policy_th);
		if ((power_model_policy==MONITORING_ONLY) && (ear_my_rank==0) && (app_info->nominal==ear_get_nominal_frequency())) {
			optimal=optimal_freq_min_energy(0.1,&SIGNATURE,&PP,&TP);
			perf_deg=((TP-Seconds)/Seconds)*100.0;
			power_sav=((POWER_DC-PP)/POWER_DC)*100.0;
			ener=POWER_DC*Seconds;
			EP=TP*PP;
			new_EDP=TP*TP*PP;
			energy_sav=((ener-EP)/ener)*100.0;
			fprintf(stderr,"EAR hint. at %.3lfGHz application %s would degrade by %.1lf\%  (%.3lf sec.), avg.power would reduce by %.1lf\% (%.3lfW), saving %.1lf%s energy (estimated new EDP %.3lf)\n",(double)optimal/(double)1000000,SIGNATURE.app_id,perf_deg,TP,power_sav,PP,energy_sav,"%",new_EDP);
		}
		fprintf(stderr,"____________________________________________________\n");
		// We save it in the historical DB
		db_update_historical(whole_app,&SIGNATURE);
		// Pending to compute GFlops
		//"USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;GFLOPS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;POLICY;POLICY_TH\n";
        if (fd!=NULL) fprintf(fd,"%s;%s;%s;%s;%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;%s;%.3lf\n",SIGNATURE.user_id,SIGNATURE.job_id,SIGNATURE.node_id,
		SIGNATURE.app_id,SIGNATURE.f,SIGNATURE.seconds,SIGNATURE.CPI,SIGNATURE.TPI_f0,SIGNATURE.GBS_f0,SIGNATURE.Gflops,SIGNATURE.POWER_f0,SIGNATURE.DRAM_POWER,SIGNATURE.PCK_POWER,SIGNATURE.nominal,SIGNATURE.policy,SIGNATURE.th);
		// fd is the user_db file. Gflops are not reported
        //if (fd!=NULL) fprintf(fd,"%s;%s;%s;%s;%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;%.3lf;%s;%.3lf\n",SIGNATURE.user_id,SIGNATURE.job_id,SIGNATURE.node_id,
		//SIGNATURE.app_id,SIGNATURE.f,SIGNATURE.seconds,SIGNATURE.CPI,SIGNATURE.TPI_f0,SIGNATURE.GBS_f0,SIGNATURE.POWER_f0,SIGNATURE.DRAM_POWER,SIGNATURE.PCK_POWER,SIGNATURE.nominal,SIGNATURE.EDP,SIGNATURE.policy,SIGNATURE.th);
}

void copy_last_iter_counters()
{
	int i;
	for (i=0;i<TOTAL_EVENTS;i++){
		last_iter_event_values[i]=acum_event_values[i];
	}
}
void diff_counters()
{
	int i;
	for (i=0;i<TOTAL_EVENTS;i++){
		diff_event_values[i]=acum_event_values[i]-last_iter_event_values[i];
	}
}
void acum_counters()
{
	int i,s;
	acum_event_values[EAR_ACUM_TOT_CYC]+=events_values[EVENT_SET_PRESET][EAR_PAPI_TOT_CYC];
	acum_event_values[EAR_ACUM_TOT_INS]+=events_values[EVENT_SET_PRESET][EAR_PAPI_TOT_INS];
	for (i=0;i< uncore_elements;i=i+2){	
		// Ead channel returns two metrics, corresponding to memory reads and writes
		acum_event_values[EAR_ACUM_LD_INS]+=event_values_uncore[i+READS_OFFSET];
		acum_event_values[EAR_ACUM_SR_INS]+=event_values_uncore[i+WRITES_OFFSET];
	}
	// Rapl metrics are returned first 1 value per socket per dram energy and later 1 value per socket per energy package
	for (s=0;s< SOCKETS;s++){	
		acum_event_values[EAR_ACUM_DRAM_ENER]+=event_values_rapl[DRAM_OFFSET*SOCKETS+s];
		acum_event_values[EAR_ACUM_PCKG_ENER]+=event_values_rapl[PACK_OFFSET*SOCKETS+s];
	}
}

long long  metrics_time()
{
	return PAPI_get_real_usec();
}

void metrics_start_computing_signature()
{
	ear_debug(3,"EAR ______________metrics_start_computing_signature __________\n");
	start_time=PAPI_get_real_usec();
	// We read and save actual counters
	metrics_stop();
	// Save in global counters
	acum_counters();
	// We save here values for last iteration
	copy_last_iter_counters();
	metrics_reset();
	reset_values();
	metrics_start();
}

void metrics_set_signature_start_time()
{
	start_time=PAPI_get_real_usec();
}
struct App_info* metrics_end_compute_signature(int period,unsigned long int *eru,unsigned int N_iters,long long min_t)
{
	struct App_info *app;
	ear_debug(3,"EAR______________metrics_end_compute_signature __________\n");
	// POWER_DC is provided
	// WE Get iteration TIme
    end_time=PAPI_get_real_usec();
    iter_time=metrics_usecs_diff(end_time,start_time);
	if (iter_time< min_t) return NULL;
	// WE Get counters (Stop&Read)
	metrics_stop();
	// Reset actual counters
	metrics_reset();
	// We accumulate in the global counters acum_event_values
	acum_counters();
	diff_counters();
	copy_last_iter_counters();
	// This can generate an overflow: PENDING TO DETECT
	// acum_time_time is total, 
	acum_iter_time=acum_iter_time+iter_time;
    start_time=end_time;
	acum_energy=acum_energy+*eru;
	app=set_metrics(period,0,diff_event_values,iter_time,eru,N_iters);
	// Once processes, we reset actual counters
	reset_values();
	metrics_start();
	ear_verbose(3,"EAR______________Application signature ready __________\n");
	return app;
}

// BAsed on files provided by Lenovo
//
//
int cache_line_size;
int ear_discover_cache_line_size()
{
        FILE *f_cpuinfo;
        char buffer[1024];

        f_cpuinfo = fopen("/proc/cpuinfo", "r");
        if (!f_cpuinfo)
        {
                fprintf(stderr,"Could not open /proc/cpuinfo for reading.\n");
                return -1;
        }
        while ( fgets(buffer, 1024, f_cpuinfo)!=NULL )
        {
                if (strncmp(buffer, "cache_alignment", sizeof("cache_alignment") - 1) == 0)
                {
                        sscanf(buffer, "cache_alignment\t: %d", &cache_line_size);
                        fclose(f_cpuinfo);
                        return cache_line_size;
                }
        }
        fclose(f_cpuinfo);
        return cache_line_size;


}


