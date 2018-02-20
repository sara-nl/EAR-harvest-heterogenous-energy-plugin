/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _FLOPS_METRICS_H_
#define _FLOPS_METRICS_H_

int init_flops_metrics();

void reset_flops_metrics();

void start_flops_metrics();

void stop_flops_metrics(long long *total_flops, long long *f_operations);

int get_number_fops_events();

void get_total_fops(long long *metrics);

double gflops(ulong total_timei, uint total_cores);

void get_weigth_fops_instructions(int *weigth_vector);

void print_gflops(long long total_inst, ulong total_time, uint total_cores);

#else
#endif
