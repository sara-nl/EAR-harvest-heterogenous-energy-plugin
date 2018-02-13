/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_PAPI_H_
#define _EAR_PAPI_H_

#include <types/application.h>

// Returns real time counter value in microseconds
long long metrics_time();

/*
 * BIG SHIT
 */

// Internals to ear_papi.c
void copy_metrics_last_iter_counters();
void diff_counters();
void acum_counters();

// Initialice PAPI counters and allocates memory for rapl and uncore counters. 
// Returns 0 on success and 1 if id!=0 (not a "master"process)
int metrics_init(int id, int pid);

// updates name with the application name
void metrics_get_app_name(char *name);


// Gets HW infor
void metrics_get_hw_info(int *sockets, int *cores_socket, ulong *max_f, ulong *min_f, ulong *nom_f,
	char *CPU_model, char *CPU_name);

//TODO: change the name
// Returns elapsed time (end-init)
long long metrics_usecs_diff(long long end,long long init);

/*
 * BIG BOOST
 */


//
void metrics_start();
application_t* metrics_end(ulong energy_mj);

// Starts performance/power counters and save values to start computing the
// application signature
void metrics_start_computing_signature();

// Stops & reads performance/power counters and computes application signature.
// This function also accumulates values to report application summary and
// starts performance/power counters again. Returns the application signature.
application_t* metrics_end_compute_signature(ulong energy_mj, uint N_iters, ulong min_time_us);

#else
#endif
