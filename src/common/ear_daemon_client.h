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

int ear_daemon_client_connect();
void ear_daemon_client_disconnect();

// Frequency services
unsigned long ear_daemon_client_change_freq(unsigned long newfreq);
void ear_daemon_client_set_turbo();

unsigned long ear_daemon_client_get_data_size_frequency();

void ear_daemon_client_begin_compute_turbo_freq();
unsigned long ear_daemon_client_end_compute_turbo_freq();

void ear_daemon_client_begin_app_compute_turbo_freq();
unsigned long ear_daemon_client_end_app_compute_turbo_freq();

// Uncore services
int ear_daemon_client_read_uncore(unsigned long long *values);
int ear_daemon_client_start_uncore();
int ear_daemon_client_reset_uncore();
unsigned long ear_daemon_client_get_data_size_uncore();

// RAPL services
int ear_daemon_client_read_rapl(unsigned long long *values);
int ear_daemon_client_start_rapl();
int ear_daemon_client_reset_rapl();
unsigned long ear_daemon_client_get_data_size_rapl();

// System services
unsigned long ear_daemon_client_write_app_signature(application_t *app_signature);

// Node energy services
unsigned long ear_daemon_client_node_energy_data_size();
int ear_daemon_client_node_dc_energy(unsigned long *energy);


#else
#endif
