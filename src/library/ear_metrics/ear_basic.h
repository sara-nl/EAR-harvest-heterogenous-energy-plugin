/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _BASIC_METRICS_H_
#define _BASIC_METRICS_H_
// BASIC metrics are CYCLES and INSTRUCTIONS
void init_basic_metrics();
void reset_basic_metrics();
void start_basic_metrics();
void stop_basic_metrics(long long *cycles,long long *instructions);
void get_basic_metrics(long long *total_cycles,long long *instructions);
#else
#endif
