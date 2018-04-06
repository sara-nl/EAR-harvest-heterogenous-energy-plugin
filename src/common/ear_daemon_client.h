/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _EAR_DAEMON_CLIENT_H
#define _EAR_DAEMON_CLIENT_H

#include <common/ear_daemon_common.h>
#include <common/types/generic.h>

/** Tries to connect with the daemon. Returns 0 on success and -1 otherwise. */
int eards_connect();
/** Closes the connection with the daemon. */
void eards_disconnect();

// Frequency services
/** Given a frequency value, sends a request to change the frequency to that
*   value. Returns -1 if there's an error, 0 if the change_freq service has not
*   been provided, and the final frequency value otherwise. */
unsigned long eards_change_freq(unsigned long newfreq);
/** Tries to set the frequency to the turbo value */
void eards_set_turbo();

/** Requests the frequency data size. Returns -1 if there's an error, and the
*   actual value otherwise. */
unsigned long eards_get_data_size_frequency();

/** Sends a request to read the CPU frequency. */
void eards_begin_compute_turbo_freq();
/** Requests to stop reading the CPU frequency. Returns the average frequency
*   between the begin call and the end call on success, -1 otherwise. */
unsigned long eards_end_compute_turbo_freq();

/** Sends a request to read the global CPU frequency. */
void eards_begin_app_compute_turbo_freq();
/** Requests to stop reading the CPU global frequency. Returns the average 
*   frequency between the begin call and the end call on success, -1 otherwise. */
unsigned long eards_end_app_compute_turbo_freq();

// Uncore services
/** Sends a request to read the uncores. Returns -1 if there's an error, on
*   success stores the uncores values' into *values and returns 0. */
int eards_read_uncore(unsigned long long *values);
/** Sends the request to start the uncores. Returns 0 on success, -1 on error */
int eards_start_uncore();
/** Sends a request to reset the uncores. Returns 0 on success, -1 on error */
int eards_reset_uncore();
/** Requests the uncore data size. Returns -1 if there's an error, and the
*   actual value otherwise. */
unsigned long eards_get_data_size_uncore();

// RAPL services
/** Sends a request to read the RAPL counters. Returns -1 if there's an error,
*   and on success returns 0 and fills the array given by parameter with the
*   counters' values. */
int eards_read_rapl(unsigned long long *values);
/** Sends the request to start the RAPL counters. Returns 0 on success or if
*   the application is not connected, -1 if there's an error. */
int eards_start_rapl();
/** Sends the request to reset the RAPL counters. Returns 0 on success or if
*    the application is not connected, -1 if there's an error. */
int eards_reset_rapl();
/** Requests the rapl data size. Returns -1 if there's an error, and the
*   actual value otherwise. */
unsigned long eards_get_data_size_rapl();

// System services
/** Sends a request to the deamon to write the whole application signature.
*   Returns 0 on success, -1 on error. */
unsigned long eards_write_app_signature(application_t *app_signature);

// Node energy services
/** Requests the IPMI data size. Returns -1 if there's an error, and the
*   actual value otherwise. */
unsigned long eards_node_energy_data_size();
/** Requests the DC energy to the node. Returns 0 on success, -1 if there's
*   an error. */
int eards_node_dc_energy(unsigned long *energy);
/** Requests the dc energy node frequency. Returns -1 if there's an error,
*   and 10000000 otherwise. */
ulong eards_node_energy_frequency();


#else
#endif
