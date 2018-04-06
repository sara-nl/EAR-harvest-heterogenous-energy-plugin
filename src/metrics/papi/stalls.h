/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _STALL_METRICS_H_
#define _STALL_METRICS_H_

// Stall cycles
/** Initializes the event metrics for stall cycles. */
void init_stall_metrics();

/** Resets the metrics for stall cycles. */
void reset_stall_metrics();

/** Starts tracking stall cycle metrics. */
void start_stall_metrics();

/** Stops stall metrics and accumulates registered stall cycles to the
*   variable recieved by parameter. */
void stop_stall_metrics(long long *stall_cycles);

/** Puts the current stall cycles metrics in the variable recieved by parameter. */
void get_stall_metrics(long long *total_stall_cycles);

#else
#endif
