/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _BASIC_METRICS_H_
#define _BASIC_METRICS_H_

// BASIC metrics are CYCLES and INSTRUCTIONS
/** Initializes the event metrics for cycles and instructions. */
void init_basic_metrics();

/** Resets the event metrics for both cycles and instructions. */
void reset_basic_metrics();

/** Starts tracking cycles and instructions metrics. */
void start_basic_metrics();

/** Stops tracking cycles and instruction metrics and accumulates said metrics
*   into the respective variables recieved by parameter. */
void stop_basic_metrics(long long *cycles, long long *instructions);

/** Puts the current cycles and instruction metrics in the variable recieved by parameter. */
void get_basic_metrics(long long *total_cycles, long long *instructions);

#else
#endif
