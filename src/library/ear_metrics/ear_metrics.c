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
#include <ear_metrics/ear_metrics.h>
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
#define EVENT_SET_PRESET 	0 // perf component
#define	EAR_PAPI_TOT_CYC 	0 // Total Cycles
#define EAR_PAPI_TOT_INS 	1 // Total instructions
#define READS_OFFSET 		0
#define WRITES_OFFSET 		1
#define DRAM_OFFSET 		0
#define PACK_OFFSET 		1
#define	EAR_ACUM_TOT_CYC 	0 // Total Cycles
#define EAR_ACUM_TOT_INS 	1 // Total instructions
#define EAR_ACUM_LD_INS 	2 // Load instructions
#define EAR_ACUM_SR_INS 	3 // Store instructions
#define EAR_ACUM_DRAM_ENER 	4 // ENERGY reported by rapl (DRAM)
#define EAR_ACUM_PCKG_ENER 	5 // ENERGY reported by rapl (CORES+CACHE)
#define TOTAL_EVENTS 		6
#define NUM_EVENTS 			2

static long long start_time = 0, end_time = 0;
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
static long long last_iter_event_values[TOTAL_EVENTS];
static long long acum_event_values[TOTAL_EVENTS]; // This is for all the metrics
static long long diff_event_values[TOTAL_EVENTS];
static long long acum_iter_time=0;
static long long acum_energy=0;
static int ear_cpu_id;

static long long l1,l2,l3;
static long long *flops_metrics;
static int flops_events;
static int *flops_weigth;
static long long total_flops;
static FILE* fd_extra;
static int fops_supported;

//TODO: ADD APP INIT
static application_t app_temporal;
static application_t app_global;

#define MULTIPLEX_PAPI
#ifdef MULTIPLEX_PAPI
int papi_multiplex=1;
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

void init_basic_event_info(int ear_event,PAPI_event_info_t *info){
		strncpy(units[ear_event], info->units, sizeof( units[0] )-1 );
        // buffer must be null terminated to safely use strstr operation on it below
        units[ear_event][sizeof( units[0] )-1] = '\0';
        data_type[ear_event] = info->data_type;
}

void metrics_reset()
{
	int sets;

	//TODO: DAEMON COUPLED
	if (ear_daemon_client_reset_uncore() < 0) {
		// Should we exit??
		ear_verbose(0,"EAR(%s): Reset uncore counters to compute MB is failing\n",ear_app_name);
	}
	if (ear_daemon_client_reset_rapl()<0) {
		// Should we exit??
		ear_verbose(0,"EAR(%s): Reset uncore counters to compute MB is failing\n",ear_app_name);
	}

	reset_basic_metrics();
	reset_turbo_metrics();
	reset_flops_metrics();
	reset_cache_metrics();
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

void metrics_get_hw_info(int *sockets, int *cores_socket, ulong *max_f, ulong *min_f, ulong *nom_f,
						 char *CPU_model, char *CPU_name)
{
	*sockets = ear_hwinfo->sockets;
	*cores_socket = ear_hwinfo->cores;
	*max_f = ear_hwinfo->cpu_max_mhz;
	*min_f = ear_hwinfo->cpu_min_mhz;
	*nom_f = ear_get_nominal_frequency();
	strcpy(CPU_model,ear_hwinfo->model_string);
	strcpy(CPU_name,ear_hwinfo->vendor_string);
}
 
int metrics_init(int my_id, int pid)
{
	char extra_filename[MAX_APP_NAME];
	int retval, ret;
	int sets, i;

	if (my_id) return 1;

	init_application(&app_temporal);
	init_application(&app_global);

	// TODO: REVISION

	// This must be change to new EAR fucntion
	ear_cache_line_size=get_cache_line_size();
	ear_debug(2,"EAR(%s):: Cache line size %d\n",__FILE__,ear_cache_line_size);

	// Init the PAPI library
	if (PAPI_is_initialized()==PAPI_NOT_INITED)
	{
		retval = PAPI_library_init(PAPI_VER_CURRENT);

		if (retval != PAPI_VER_CURRENT) {
			ear_verbose(0,"EAR: Error intializing the PAPI library.Exiting\n");
			exit(1);
 		}
		if ((ret=PAPI_multiplex_init()) != PAPI_OK){
			ear_verbose(0,"EAR: WARNING PAPI_multiplex_init fails: %s\n", PAPI_strerror(ret));
		}
	}

	// Checking the perf_event component  component 
	perf_cid = PAPI_get_component_index("perf_event");
	if (perf_cid < 0) {
		ear_verbose(0,"EAR: perf_event component not found(%s) .Exiting\n",PAPI_strerror(perf_cid));
		exit(1);
	}

	// Checking HW info
	ear_hwinfo = PAPI_get_hardware_info();

	if (ear_hwinfo == NULL)
	{
		ear_verbose(0,"EAR: WARNING PAPI_get_hardware_info returns NULL\n");
	} else {
    	ear_verbose(3,"EAR: %d CPUsPerNUMA-Node, \n \
                %d Number of hdw threads per core\n \
                %d Number of cores per socket \n \
                %d Number of sockets \n \
                %d Total Number of NUMA Nodes \n \
                (max-mhz %d min-mhz %d) \n \
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
	event_values_uncore = malloc(uncore_size);

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
	for (i=0;i<TOTAL_EVENTS;i++)
	{
		acum_event_values[i]=0;
		last_iter_event_values[i]=0;
		diff_event_values[i]=0;
	}

	init_turbo_metrics();
	fops_supported=init_flops_metrics();
	init_cache_metrics();

	if (fops_supported)
	{
		flops_events = get_number_fops_events();
		flops_metrics = (long long *) malloc(sizeof(long long) * flops_events);

		if (flops_metrics == NULL){
			ear_verbose(0,"EAR: Error allocating memory for flops %s\n",strerror(errno));
			exit(1);
		}

		flops_weigth = (int *) malloc(sizeof(int)*flops_events);

		if (flops_weigth == NULL) {
			ear_verbose(0,"EAR: Error allocating memory for flops weigth %s\n",strerror(errno));
			exit(1);
		}

		get_weigth_fops_instructions(flops_weigth);
	}

	// TODO: esto deberiamos verlo, me está creando ficheros sin nada.
	sprintf(extra_filename,"%s.loop_info.csv",get_ear_user_db_pathname());
	ear_verbose(1,"Opening/Creating %s file \n",extra_filename);
	fd_extra=fopen(extra_filename,"w+");

	if (fd_extra==NULL){
		ear_verbose(0,"EAR: File for extra metrics can not be created %s\n",strerror(errno));
	}else{
		fprintf(fd_extra,"USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;GFLOPS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;POLICY;POLICY_TH;LOOP_ID;SIZE;LEVEL;CYCLES;INSTRUCTIONS;L1_MISSES;L2_MISSES;L3_MISSES;DPSINGLE_OPS;DP128_OPS;DP256_OPS;DP512_OPS;ITERATIONS\n");
	}

	reset_values();
	metrics_reset();
	metrics_start();

	app_start_time = PAPI_get_real_usec();

	return 0;
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

	for (i=0;i< uncore_elements;i=i+2)
	{
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

/*
 *
 *
 *
 *
 * BIG BOOST
 *
 *
 *
 *
 */
// TODO: PRINTED AT THE END OF THE LOOP (and after metrics_stop)
static void fill_application_metrics(application_t *app, const long long *counters,
	 ulong energy_mj, uint N_iters, ulong time_us)
{
	double time_s, aux;
	int i;

	time_s = (double) time_us / (double) 1000000;
	aux = time_s * (double) (1024 * 1024 * 1024);

	// Porque se utiliza el tiempo en global y no una división
	// por iteraciones:
	// 	- En las métricas básicas no importa, pues los bytes, cyclos e
	//    instrucciones aumentan línealmente junto al tiempo.
	//  - En las métricas de energía tampoco resulta relevante. Pues el coste
	//    sólamente guardamos medidas de potencia, que no dejan de ser una
	//    media de la energía por segundo.
	// Si guardasemos datos de energía

	// Basic metrics
	app->GBS  = ((double) (counters[EAR_ACUM_LD_INS] * ear_cache_line_size)) / aux;
	app->GBS += ((double) (counters[EAR_ACUM_SR_INS] * ear_cache_line_size)) / aux;
	app->CPI  = (double) (counters[EAR_ACUM_TOT_CYC] / (double) counters[EAR_ACUM_TOT_INS]);
	app->TPI  = (double) (counters[EAR_ACUM_LD_INS] + counters[EAR_ACUM_SR_INS]);
	app->TPI /= (double) (counters[EAR_ACUM_TOT_INS] / ear_cache_line_size);

	// Energy metrics
	app->DC_power = (double) energy_mj / (double) (time_s * 1000000);
	app->PCK_power  = (double) (counters[EAR_ACUM_PCKG_ENER] / 1000000000) / time_s;
	app->DRAM_power = (double) (counters[EAR_ACUM_DRAM_ENER] / 1000000000) / time_s;

	// Time dependent
	app->EDP = time_s * time_s * POWER_DC;
	app->time = time_s / (double) N_iters;

	// TODO: IF TEMPORAL (loop)
	if (N_iters > 0) {
		// Frequency
		app->avg_f = ear_daemon_client_end_compute_turbo_freq();
	}
	// TODO: IF GLOBAL (whole app)
	else {
		// Frequency
		app->avg_f = ear_daemon_client_end_app_compute_turbo_freq();

		// Instructions
		app->cycles = counters[EAR_ACUM_TOT_CYC];
		app->instructions = counters[EAR_ACUM_TOT_INS];

		// Cache
		get_cache_metrics(&l1, &l2, &l3);
		app->L1_misses = l1;
		app->L2_misses = l2;
		app->L3_misses = l3;

		// Flops
		for (i=0; i < flops_events; i++) {
			app->FLOPS[i] = flops_metrics[i] * flops_weigth[i];
		}

		report_application_data(app);
	}
}
void metrics_stop()
{
	// We should ask for size when initializing and reserving memory with malloc
	if (ear_daemon_client_read_uncore(event_values_uncore)<0){
		ear_verbose(0,"EAR(%s): Reading uncore counters to compute MB is failing\n", ear_app_name);
	}
	if (ear_daemon_client_read_rapl(event_values_rapl)<0){
		ear_verbose(0,"EAR(%s): Reading rapl is is failing\n", ear_app_name);
	}

	stop_turbo_metrics();
	stop_cache_metrics(&l1, &l2, &l3);
	stop_flops_metrics(&total_flops, flops_metrics);
	stop_basic_metrics(&events_values[EVENT_SET_PRESET][EAR_PAPI_TOT_CYC],
					   &events_values[EVENT_SET_PRESET][EAR_PAPI_TOT_INS]);
}

void metrics_start()
{
	ear_daemon_client_start_uncore();
	ear_daemon_client_start_rapl();

	start_basic_metrics();
	start_turbo_metrics();
	start_cache_metrics();
	start_flops_metrics();
}

application_t *metrics_end(ulong energy_mj)
{
	//
	metrics_stop();

	// Save in global counters
	acum_counters();

	//
	app_end_time = PAPI_get_real_usec();
	app_exec_time = app_end_time-app_start_time;
	acum_energy = energy_mj;

	// This function computes metrics, signature etc
	// TODO: This couldn't be done because it uses static app info
	// metrics_print_summary(whole_app, my_id, summary_file);
	// print_turbo_metrics(acum_event_values[EAR_ACUM_TOT_INS]);
	fill_application_metrics(&app_global, acum_event_values, energy_mj, 1, app_exec_time);

	return &app_global;
}

//TODO: CALLED WHEN FIRST_ITERATION
void metrics_start_computing_signature()
{
	ear_debug(3,"EAR ______________metrics_start_computing_signature __________\n");

	// TIME
	start_time = PAPI_get_real_usec();

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

//TODO: CALLED AT THE END OF THE LOOP
application_t* metrics_end_compute_signature(ulong energy_mj, uint N_iters, ulong min_time_us)
{
	application_t *app;
	long long metrics_time = 0;

	// TIME SINCE LAST METRICS START
	end_time = PAPI_get_real_usec();
	metrics_time = metrics_usecs_diff(end_time, start_time);

	if (metrics_time < min_time_us) {
		return NULL;
	}

	// STOP ALL METRICS (AND SAVE)
	metrics_stop();
	metrics_reset();

	// GLOBAL ACCUM COUNTERS FOR THE WHOLE APPLICATION
	acum_counters();
	diff_counters();
	copy_last_iter_counters();

	// acum_time_time is total, 
	acum_iter_time = acum_iter_time + metrics_time;

	// If application signature is correct, start time is set to current time
	start_time = end_time;

	// TODO: ENERGY
	acum_energy = acum_energy + energy_mj;

	// TODO: FUTURE
	fill_application_metrics(&app_temporal, diff_event_values, energy_mj, N_iters, iter_time);

	// Once processes, we reset actual counters
	reset_values();
	metrics_start();

	ear_verbose(3, "COMPUTED SIGNATURE: energy %ld, iters %u, time %llu\n",
				energy_mj, N_iters, metrics_time);

	return &app_temporal;
}
