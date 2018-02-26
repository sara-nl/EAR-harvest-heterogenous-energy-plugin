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

int eards_connect();
void eards_disconnect();

// Frequency services
unsigned long eards_change_freq(unsigned long newfreq);
void eards_set_turbo();

unsigned long eards_get_data_size_frequency();

void eards_begin_compute_turbo_freq();
unsigned long eards_end_compute_turbo_freq();

void eards_begin_app_compute_turbo_freq();
unsigned long eards_end_app_compute_turbo_freq();

// Uncore services
int eards_read_uncore(unsigned long long *values);
int eards_start_uncore();
int eards_reset_uncore();
unsigned long eards_get_data_size_uncore();

// RAPL services
int eards_read_rapl(unsigned long long *values);
int eards_start_rapl();
int eards_reset_rapl();
unsigned long eards_get_data_size_rapl();

// System services
unsigned long eards_write_app_signature(application_t *app_signature);

// Node energy services
unsigned long eards_node_energy_data_size();
int eards_node_dc_energy(unsigned long *energy);


#else
#endif
