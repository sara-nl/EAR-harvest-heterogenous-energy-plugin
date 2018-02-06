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
#include <errno.h>

#include <ear_daemon_client.h>
#include <ear_db/ear_db.h>
#include <ear_metrics/ear_papi.h>
#include <ear_metrics/ear_basic.h>
#include <ear_metrics/ear_turbo_metrics.h>
#include <ear_metrics/ear_flops_metrics.h>
#include <ear_metrics/ear_cache.h>
#include <ear_models/ear_models.h>
#include <ear_verbose.h>
#include <types/application.h>
#include <environment.h>
#include <externs.h>
#include <states.h>

#define MAX_SETS 			1
#define EVENT_SET_PRESET 	0 //perf component
#define	EAR_PAPI_TOT_CYC 	0 // Total Cycles
#define EAR_PAPI_TOT_INS 	1 // Total instructions
#define READS_OFFSET 		0
#define WRITES_OFFSET 		1
#define DRAM_OFFSET 		0
#define PACK_OFFSET 		1
#define	EAR_ACUM_TOT_CYC 	0 // Total Cycles
#define EAR_ACUM_TOT_INS 	1 // Total instructions
#define EAR_ACUM_LD_INS 	2 //Load instructions
#define EAR_ACUM_SR_INS 	3 //Store instructions
#define EAR_ACUM_DRAM_ENER 	4 //ENERGY  reported by rapl (DRAM)
#define EAR_ACUM_PCKG_ENER 	5 //ENERGY  reported by rapl (CORES+CACHE)
#define TOTAL_EVENTS 		6
#define NUM_EVENTS 			2

static long long start_time = 0, end_time = 0, iter_time = 0;
static long long app_start_time = 0, app_end_time = 0, app_exec_time = 0;
static int ear_cache_line_size;
static int ear_node_size;
static int num_ctrs;

static const PAPI_hw_info_t *ear_hwinfo = NULL;
static long long events_values[MAX_SETS][NUM_EVENTS];
static char units[NUM_EVENTS][PAPI_MIN_STR_LEN];
static char event_names[NUM_EVENTS][PAPI_MIN_STR_LEN];
static int data_type[NUM_EVENTS];
static int perf_cid = -1;

static long long *event_values_uncore; // UNCORE
static unsigned long uncore_size, uncore_elements;
static long long *event_values_rapl; // RAPL
static unsigned long rapl_size, rapl_elements;
static long long acum_event_values[TOTAL_EVENTS]; // This is for all the metrics
static long long last_iter_event_values[TOTAL_EVENTS];
static long long diff_event_values[TOTAL_EVENTS];
static long long acum_iter_time=0;
static long long acum_energy=0;
static int ear_cpu_id;

#define MULTIPLEX_PAPI
#ifdef MULTIPLEX_PAPI
int papi_multiplex=1;
#endif

#ifdef EAR_EXTRA_METRICS
static long long l1,l2,l3;
static long long *flops_metrics;
static int flops_events;
static int *flops_weigth;
static long long total_flops;
static FILE* fd_extra;
static int fops_supported;
#endif


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
	int retval;
	const PAPI_exe_info_t *prginfo = NULL;
        if (PAPI_is_initialized()==PAPI_NOT_INITED){
                retval=PAPI_library_init(PAPI_VER_CURRENT );
                if ( retval != PAPI_VER_CURRENT ) {
                        ear_verbose(0,"EAR: Error intializing the PAPI library.Exiting\n");
                        exit(1);
                }
        }
	if ( ( prginfo = PAPI_get_executable_info( ) ) == NULL ){
		ear_verbose(0,"EAR(%s): Executable info not available. Exiting\n",__FILE__);
		exit(2 );
	}
	strcpy(app_name,prginfo->fullname );
}


void ear_papi_error(int ret,char *m)
{
        if (ret!=PAPI_OK){
                ear_verbose(0,"EAR(%s):%s -  %s\n",__FILE__,m,PAPI_strerror(ret));
                exit(2);
        }
}
// This function is not used at this moment
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
void metrics_start()
{
	int sets;
	char buff[128];
	ear_daemon_client_start_uncore();
	ear_daemon_client_start_rapl();
	start_basic_metrics();

    #ifdef EAR_EXTRA_METRICS
	start_turbo_metrics();
	if (fops_supported) start_flops_metrics();
	start_cache_metrics();
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
	stop_basic_metrics(&events_values[EVENT_SET_PRESET][EAR_PAPI_TOT_CYC],&events_values[EVENT_SET_PRESET][EAR_PAPI_TOT_INS]);

	#ifdef EAR_EXTRA_METRICS
	stop_turbo_metrics();

	// FLOPS are merged and returned
	stop_flops_metrics(&total_flops, flops_metrics);

	stop_cache_metrics(&l1, &l2, &l3);
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
	reset_basic_metrics();
#ifdef EAR_EXTRA_METRICS
	reset_turbo_metrics();
	reset_flops_metrics();
	reset_cache_metrics();
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
 
///////////////////////
/////// Init function
///////////////////////
int metrics_init(int my_id,int pid)
{
	int retval,ret;
	int sets,i;
#ifdef EAR_EXTRA_METRICS
	char extra_filename[MAX_APP_NAME];
#endif
	

	if (my_id) return 1;
	// This must be change to new EAR fucntion
	ear_cache_line_size=get_cache_line_size();
	ear_debug(2,"EAR(%s):: Cache line size %d\n",__FILE__,ear_cache_line_size);
	// Init the PAPI library
	if (PAPI_is_initialized()==PAPI_NOT_INITED){
		retval=PAPI_library_init(PAPI_VER_CURRENT );
		if ( retval != PAPI_VER_CURRENT ) {
			ear_verbose(0,"EAR: Error intializing the PAPI library.Exiting\n");
			exit(1);
 		}
		if ((ret=PAPI_multiplex_init())!=PAPI_OK){
			ear_verbose(0,"EAR: WARNING PAPI_multiplex_init fails: %s\n",PAPI_strerror(ret));
		}	
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
	init_basic_metrics();
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
	fops_supported=init_flops_metrics();
	init_cache_metrics();
	if (fops_supported){ 
		flops_events=get_number_fops_events();
		flops_metrics=(long long *) malloc(sizeof(long long)*flops_events);
		if (flops_metrics==NULL){
			ear_verbose(0,"EAR: Error allocating memory for flops %s\n",strerror(errno));
			exit(1);
		}
		flops_weigth=(int *)malloc(sizeof(int)*flops_events);
		if (flops_weigth==NULL){
			ear_verbose(0,"EAR: Error allocating memory for flops weigth %s\n",strerror(errno));
			exit(1);
		}
		get_weigth_fops_instructions(flops_weigth);	
	}
	sprintf(extra_filename,"%s.loop_info.csv",get_ear_app_name());
	ear_verbose(1,"Opening/Creating %s file \n",extra_filename);
	fd_extra=fopen(extra_filename,"w+");
	if (fd_extra==NULL){
		ear_verbose(0,"EAR: File for extra metrics can not be created %s\n",strerror(errno));
	}else{
		fprintf(fd_extra,"USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;GFLOPS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;POLICY;POLICY_TH;LOOP_ID;SIZE;LEVEL;L1_MISSES;L2_MISSES;L3_MISSES;PERC_DPSINGLE;PERC_DP128;PERC_DP256;PERC_DP512;ITERATIONS\n");
	}
#endif
	reset_values();
	metrics_reset();
	metrics_start();
	app_start_time=PAPI_get_real_usec();


	return 0;
}

void metrics_end(unsigned int whole_app, int my_id, char* summary_file, unsigned long int *eru)
{
	metrics_stop();
	// Save in global counters
	acum_counters();

	app_end_time=PAPI_get_real_usec();
	app_exec_time=app_end_time-app_start_time;
	acum_energy=*eru;
	// This function computes metrics, signature etc

	metrics_print_summary(whole_app, my_id, summary_file);
	
	#ifdef EAR_EXTRA_METRICS
	print_turbo_metrics(acum_event_values[EAR_ACUM_TOT_INS]);
	//print_gflops(acum_event_values[EAR_ACUM_TOT_INS],app_exec_time);
	#endif
}

// Called after metrics_stop
application_t* set_metrics(int period, int iteration, long long *counters, long long iter_time,
						  ulong *eru, uint N_iters)
{
	double CPI, GBS, seconds, TPI, POWER;
	application_t *app_info;

	ear_debug(4,"EAR(%s): Setting DB metrics for period %d iteration %d\n",
			  __FILE__,period,N_iters);

	seconds = (double) iter_time / (double) 1000000;

	GBS  = ((double)(counters[EAR_ACUM_LD_INS]*ear_cache_line_size) / (seconds * (double) (1024*1024*1024)));
	GBS += ((double)(counters[EAR_ACUM_SR_INS]*ear_cache_line_size) / (seconds * (double) (1024*1024*1024)));

	CPI  = (double) counters[EAR_ACUM_TOT_CYC] / (double) counters[EAR_ACUM_TOT_INS];
	TPI  = (double) (counters[EAR_ACUM_LD_INS] + counters[EAR_ACUM_SR_INS]);
	TPI /= (double) (counters[EAR_ACUM_TOT_INS] / ear_cache_line_size);

	POWER = (double) *eru / (double) (seconds * 1000000);

	ear_debug(4,"EAR(%s):: Set_metrics: seconds %.5lf GBS %.5lf POWER %12.6f TPI %12.6f CPI %5lf\n",
			  __FILE__, seconds / (double) N_iters, GBS, POWER, TPI, CPI);

	app_info = db_current_app();
	db_set_GBS(app_info,GBS);
	db_set_POWER(app_info,POWER);
	db_set_TPI(app_info,TPI);
	db_set_seconds(app_info,seconds/(double)N_iters);
	db_set_CPI(app_info,CPI);
	db_set_frequency(app_info,ear_daemon_client_end_compute_turbo_freq());

	return app_info;
}

void metrics_print_summary(unsigned int whole_app,int my_id, char* summary_file)
{
	double CPI,GBS,GIBS,seconds,GIBS_ranks,TPI,POWER,POWER_DC,DRAM_POWER,PCK_POWER,CORE_POWER,GFLOPS,EDP;
	double PP,TP,EP,perf_deg,power_sav,energy_sav,ener,new_EDP;
	long long total_fops_instructions;
	application_t *app_info;
	application_t SIGNATURE;
	unsigned long f, optimal;
	unsigned int app_id;
	char *app_name;
	int i;

	app_info = db_current_app();

	// Compute signature
	seconds= (double) app_exec_time / (double)1000000;

	GBS  = ((double) (acum_event_values[EAR_ACUM_LD_INS] * ear_cache_line_size) / (seconds*(double)(1024*1024*1024)));
	GBS += ((double) (acum_event_values[EAR_ACUM_SR_INS] * ear_cache_line_size) / (seconds*(double)(1024*1024*1024)));

	CPI  = (double) acum_event_values[EAR_ACUM_TOT_CYC] / (double) acum_event_values[EAR_ACUM_TOT_INS];
	TPI  = (double) (acum_event_values[EAR_ACUM_LD_INS] + acum_event_values[EAR_ACUM_SR_INS]);
	TPI /= (double) (acum_event_values[EAR_ACUM_TOT_INS] / ear_cache_line_size);

	GFLOPS = gflops(app_exec_time,get_total_resources());
	POWER_DC = (double)acum_energy/(double)(seconds*1000000);
	EDP = seconds*seconds*POWER_DC;

	DRAM_POWER =(double)(acum_event_values[EAR_ACUM_DRAM_ENER]/1000000000)/seconds;
	PCK_POWER = (double)(acum_event_values[EAR_ACUM_PCKG_ENER]/1000000000)/seconds;
	f = ear_daemon_client_end_app_compute_turbo_freq();
	app_name=get_ear_app_name();

	if (app_name!=NULL) strcpy(SIGNATURE.app_id,app_name);
	else strcpy(SIGNATURE.app_id,"NA");

	#ifdef EAR_EXTRA_METRICS
	get_cache_metrics(&l1, &l2, &l3);
	fprintf(stderr,"_____________________EAR Extra Summary for %s ___________________\n",SIGNATURE.app_id);

	if (fops_supported)
	{
		// Application total FLOPS
		get_total_fops(flops_metrics);
		total_fops_instructions = 0;

		for (i=0;i < flops_events;i++)
		{
			total_fops_instructions += flops_metrics[i];
		}

		ear_verbose(1,"EAR: Total FP instructions %llu \n",total_fops_instructions);

		//
		ear_verbose(1,"EAR: AVX_512 instructions %llu (percentage from total %lf)\n",flops_metrics[flops_events-1],
					(double)flops_metrics[flops_events-1]/(double)total_fops_instructions);
		ear_verbose(1,"EAR: Total instructions %llu\n",acum_event_values[EAR_ACUM_TOT_INS]);
		ear_verbose(1,"EAR: Percentage of AVX512 instructions %lf \n",(double)flops_metrics[flops_events-1]/(double)acum_event_values[EAR_ACUM_TOT_INS]);
	}

	ear_verbose(1,"EAR: Cache misses L1 %llu ,L2 %llu, L3 %llu\n",l1,l2,l3);
	fprintf(stderr,"_______________________________\n");
	#endif

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

		// Reporting application signature metrics at stderr
		fprintf(stderr,"_____________________EAR Summary for %s ___________________\n",app_info->node_id);
		fprintf(stderr,"EAR job_id %s user_id %s app_id %s exec_time %.3lf\n",app_info->job_id,app_info->user_id,SIGNATURE.app_id,(double)app_exec_time/(double)1000000);
		fprintf(stderr,"EAR CPI=%.3lf GBS=%.3lf GFlops=%.3lf\n",CPI,GBS,GFLOPS);
		fprintf(stderr,"EAR avg. node power=%.3lfW, avg. RAPL dram power=%.3lfW, avg. RAPL pck. power=%.3lfW EDP=%.3lf GFlops/Watts=%.3lf\n",POWER_DC,DRAM_POWER,PCK_POWER,EDP,GFLOPS/POWER_DC);
		fprintf(stderr,"EAR def. frequency %.3lf GHz avg. frequency %.3lf GHz\n",(double)app_info->def_f/(double)1000000,(double)f/(double)1000000);

		// app_info links to the DB information . Update SIGNATURE metrics to report it at user_db and system_db
		strcpy(SIGNATURE.job_id,app_info->job_id);
		strcpy(SIGNATURE.user_id,app_info->user_id);
		strcpy(SIGNATURE.node_id,app_info->node_id);
		db_set_GBS(&SIGNATURE,GBS);
		db_set_POWER(&SIGNATURE,POWER_DC);
		db_set_TPI(&SIGNATURE,TPI);
		db_set_seconds(&SIGNATURE,seconds);
		db_set_CPI(&SIGNATURE,CPI);
		db_set_CYCLES(&SIGNATURE,acum_event_values[EAR_ACUM_TOT_CYC]);
		db_set_INSTRUCTIONS(&SIGNATURE,acum_event_values[EAR_ACUM_TOT_INS]);
		db_set_DRAM_POWER(&SIGNATURE,DRAM_POWER);
		db_set_PCK_POWER(&SIGNATURE,PCK_POWER);
		db_set_frequency(&SIGNATURE,f);
		db_set_Gflops(&SIGNATURE,GFLOPS);
		db_set_EDP(&SIGNATURE,EDP);
		db_set_default(&SIGNATURE,app_info->def_f);
		db_set_policy(&SIGNATURE,ear_policy_name);
		db_set_th(&SIGNATURE,get_ear_power_policy_th());
		if ((power_model_policy==MONITORING_ONLY) && (ear_my_rank==0) && (app_info->def_f==ear_get_freq(1))) {
			optimal=optimal_freq_min_energy(0.1,&SIGNATURE,&PP,&TP);
			perf_deg=((TP-seconds)/seconds)*100.0;
			power_sav=((POWER_DC-PP)/POWER_DC)*100.0;
			ener=POWER_DC*seconds;
			EP=TP*PP;
			new_EDP=TP*TP*PP;
			energy_sav=((ener-EP)/ener)*100.0;
			fprintf(stderr,"EAR hint. at %.3lfGHz application %s would degrade by %.1lf\%  (%.3lf sec.), avg.power would reduce by %.1lf\% (%.3lfW), saving %.1lf%s energy (estimated new EDP %.3lf)\n",(double)optimal/(double)1000000,SIGNATURE.app_id,perf_deg,TP,power_sav,PP,energy_sav,"%",new_EDP);
		}
		fprintf(stderr,"____________________________________________________\n");
		// We save it in the historical DB
		db_update_historical(whole_app,&SIGNATURE);

	append_application_text_file(summary_file, &SIGNATURE);
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
	#define SOCKETS ear_hwinfo->sockets

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
	ear_daemon_client_begin_compute_turbo_freq();
}

void metrics_set_signature_start_time()
{
	start_time=PAPI_get_real_usec();
}
struct App_info* metrics_end_compute_signature(int period,unsigned long int *eru,unsigned int N_iters,long long min_t)
{
	struct App_info *app;
	unsigned long avg_f;
	ear_verbose(3,"EAR______________metrics_end_compute_signature __________\n");
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
	// acum_time_time is total, 
	acum_iter_time=acum_iter_time+iter_time;
	// If application signature is correct, start time is set to current time
	start_time=end_time;
	acum_energy=acum_energy+*eru;

	app = set_metrics(period, 0, diff_event_values, iter_time, eru, N_iters);

	// Once processes, we reset actual counters
	reset_values();
	metrics_start();
	ear_verbose(3,"Signature: eneregy %ld Niters %u totalTime %llu \n",*eru,N_iters,iter_time);
	ear_verbose(3,"EAR______________Application signature ready __________\n");
	return app;
}



#ifdef EAR_EXTRA_METRICS
void metrics_get_extra_metrics(struct App_info_extended *my_extra)
{
	int i;
	my_extra->L1_misses=l1;
	my_extra->L2_misses=l2;
	my_extra->L3_misses=l3;
	for (i=0;i<flops_events;i++) my_extra->FLOPS[i]=flops_metrics[i];
}
void metrics_print_extra_metrics(struct App_info *my_sig,struct App_info_extended *my_extra,int iterations,unsigned long loop_id,int period,unsigned int level)
{
	long long sp=0,dp=0;
	long long fops_single,fops_128,fops_256,fops_512,total_fops,dp_fops;
	double psingle,p128,p256,p512,my_gflops;
	int i;

	for (i=0;i<flops_events/2;i++) sp+=my_extra->FLOPS[i];
	for (i=flops_events/2;i<flops_events;i++){ 
		dp+=my_extra->FLOPS[i];
	}
	fops_single=my_extra->FLOPS[flops_events/2]*flops_weigth[flops_events/2];
	fops_128=my_extra->FLOPS[flops_events/2+1]*flops_weigth[flops_events/2+1];
	fops_256=my_extra->FLOPS[flops_events/2+2]*flops_weigth[flops_events/2+2];
	fops_512=my_extra->FLOPS[flops_events/2+3]*flops_weigth[flops_events/2+3];
	total_fops=fops_single+fops_128+fops_256+fops_512;
	dp_fops=total_fops;
	// SP are usually 0 but we have to include in total
	total_fops+=my_extra->FLOPS[0]*flops_weigth[0]+my_extra->FLOPS[1]*flops_weigth[1]+my_extra->FLOPS[2]*flops_weigth[2]+my_extra->FLOPS[3]*flops_weigth[3];
	psingle=(double)fops_single/(double)total_fops;
	p128=(double)fops_128/(double)total_fops;
	p256=(double)fops_256/(double)total_fops;
	p512=(double)fops_512/(double)total_fops;
	my_gflops=((double)(total_fops*get_total_resources())/(my_sig->iter_time*iterations))/(double)1000000000;
	ear_verbose(1,"\t\tEAR_extra: L1 misses %llu L2 misses %llu L3 misses %llu\n",my_extra->L1_misses,my_extra->L2_misses,my_extra->L3_misses);
	ear_verbose(1,"\t\tEAR_extra: GFlops %lf --> SP inst %llu DP inst %llu SP ops %llu DP ops %llu DP_fops_perc_per_type (%.2lf \%,%.2lf \%,%.2lf \%,%.2lf \%)\n",
		my_gflops,sp,dp,(total_fops-dp_fops),dp_fops,psingle*100,p128*100,p256*100,p512*100);

	// fprintf(fd_extra,"USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;GFLOPS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;POLICY;POLICY_TH;LOOP_ID;SIZE;LEVEL;L1_MISSES;L2_MISSES;L3_MISSES;PERC_DPSINGLE;PERC_DP128;PERC_DP256;PERC_DP512;ITERATIONS\n");
	fprintf(fd_extra,"%s;%s;%s;%s;",my_sig->user_id,my_sig->job_id,my_sig->node_id,my_sig->app_id);
	fprintf(fd_extra,"%lu;%.5lf;%.5lf;%.5lf;%.5lf;%.5lf;",my_sig->avg_f,my_sig->iter_time,my_sig->CPI,my_sig->TPI,my_sig->GBS,my_gflops);
	fprintf(fd_extra,"%.2lf;%.2lf;%.2lf;",my_sig->DC_power,my_sig->DRAM_power,my_sig->PCK_power);
	fprintf(fd_extra,"%u;%s;%.2lf;",my_sig->def_f,ear_policy_name,my_sig->policy_th);
	fprintf(fd_extra,"%lu;%d;%u;",loop_id,period,level);
	fprintf(fd_extra,"%llu;%llu;%llu;",my_extra->L1_misses,my_extra->L2_misses,my_extra->L3_misses);
	fprintf(fd_extra,"%.2lf;%.2lf;%.2lf;%.2lf",psingle*100,p128*100,p256*100,p512*100);	
	
}

void metrics_end_loop_extra_metrics(int iters)
{
	fprintf(fd_extra,";%d\n",iters);
}
#endif

