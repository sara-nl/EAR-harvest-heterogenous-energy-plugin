/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_PAPI_H_
#define _EAR_PAPI_H_

#include <ear_db_type.h>

// Internals to ear_papi.c
void copy_metrics_last_iter_counters();
void diff_counters();
void acum_counters();


// Initialice PAPI counters and allocates memory for rapl and uncore counters. 
// Returns 0 on success and 1 if id!=0 (not a "master"process)
int metrics_init(int id,int pid);

// Computes the application global metrics (whole application) and reports the application summary. It calls metrics_print_summary
void metrics_end(unsigned int whole_app,int id,FILE *fd,unsigned long int *eru);
// updates name with the application name
void metrics_get_app_name(char *name);

// Returns real time counter value in microseconds
long long  metrics_time();

// Starts performance/power counters and save values to start computing the application signature
void metrics_start_computing_signature();
// Stops&reads performance/power counters and computes application signature. This functiona also accumulates values to report application summary and starts performance/power counters again.
// Returns the application signatuer
struct App_info* metrics_end_compute_signature(int period,unsigned long int *eru_diff,unsigned int N_iters,long long min_t);

void metrics_set_signature_start_time();
// Gets HW infor
void metrics_get_hw_info(int *sockets,int *cores_socket,unsigned long *max_f,unsigned long *min_f,unsigned long *nom_f,char *CPU_model,char *CPU_name);
// Returns elapsed time (end-init)
long long metrics_usecs_diff(long long end,long long init);
// Computes total application signature at application end and prints it at the stderr and in fd
void metrics_print_summary(unsigned int whole_app,int my_id,FILE* fd);

#ifdef EAR_EXTRA_METRICS
void metrics_get_extra_metrics(struct App_info_extended *my_extra);
void metrics_print_extra_metrics(struct App_info_extended *my_extra,double seconds,int iterations);
#endif




#else
#endif
