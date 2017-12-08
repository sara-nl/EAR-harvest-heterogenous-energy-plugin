/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _FLOPS_METRICS_H_
#define _FLOPS_METRICS_H_
int init_flops_metrics();
int reset_flops_metrics();
int start_flops_metrics();
int stop_flops_metrics(long long *total_flops);
void print_gflops(long long total_inst,unsigned long total_time);
double gflops(unsigned long total_time);
#else
#endif
