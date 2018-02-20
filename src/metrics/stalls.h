/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _STALL_METRICS_H_
#define _STALL_METRICS_H_

// Stall cycles
void init_stall_metrics();

void reset_stall_metrics();

void start_stall_metrics();

void stop_stall_metrics(long long *stall_cycles);

void get_stall_metrics(long long *total_stall_cycles);

#else
#endif
