/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _CACHE_METRICS_H_
#define _CACHE_METRICS_H_

/** Initializes the event metrics for each cache level. */
int init_cache_metrics();

/** Resets the metrics for all cache levels. */
void reset_cache_metrics();

/** Starts tracking cache metrics. */
void start_cache_metrics();

/** Stops cache metrics and accumulates registered events in the variables corresponding
* 	to each cache level recieved by parameter. */
void stop_cache_metrics(long long *L1_misses, long long *L2_misses, long long *L3_misses);

/** Puts the current cache metrics in the variables recieved by parameter. */
void get_cache_metrics(long long *total_L1_misses, long long *total_L2_misses,
					   long long *total_L3_misses);

#else
#endif
