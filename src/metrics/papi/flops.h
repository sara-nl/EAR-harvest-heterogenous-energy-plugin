/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _FLOPS_METRICS_H_
#define _FLOPS_METRICS_H_

/** Initializes the event metrics for flops. */
int init_flops_metrics();

/** Resets the metrics for flops */
void reset_flops_metrics();

/** Starts tracking flops metrics. */
void start_flops_metrics();

/** Stops flops metrics and accumulates registered flops and number of operations
*   to the variables recieved by parameter. */
void stop_flops_metrics(long long *total_flops, long long *f_operations);

/** Returns the total number of fops events. */
int get_number_fops_events();

/** Puts the current flops metrics in the variable recieved by parameter. */
void get_total_fops(long long *metrics);

/** Returns the amount of floating point operations done in the time total_timei,
*   in GFlops. */
double gflops(ulong total_timei, uint total_cores);

/** Puts the weights of the fops instructions into *weight_vector. */
void get_weigth_fops_instructions(int *weigth_vector);

/** Prints the total GFlops per process and node if verbosity level is at 1 or
*   higher. */
void print_gflops(long long total_inst, ulong total_time, uint total_cores);

#else
#endif
