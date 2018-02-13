/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _TURBO_METRICS_H_
#define _TURBO_METRICS_H_
void init_turbo_metrics();
void reset_turbo_metrics();
void start_turbo_metrics();
void stop_turbo_metrics();
void print_turbo_metrics(long long total_inst);
#else
#endif
