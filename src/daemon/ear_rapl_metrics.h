/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _RAPL_METRICS_H_
#define _RAPL_METRICS_H_
#define EAR_RAPL_EVENTS 4

// Initializes the PAPI library if needed and the rapl::DRAM_ENERGY:PACKAGE0, rapl:::DRAM_ENERGY:PACKAGE1, rapl:::PACKAGE_ENERGY:PACKAGE0, rapl:::PACKAGE_ENERGY:PACKAGE1 events
// returns 0 on success and -1 on error

int init_rapl_metrics();
// Resets (zeroes) rapl events
// returns 0 on success and -1 on error
int reset_rapl_metrics();
// Starts rapl events
// returns 0 on success and -1 on error
int start_rapl_metrics();
// Stops rapl events and copies events values in values array.
// returns 0 on success and -1 on error
int stop_rapl_metrics(unsigned long long *values);
// Prints rapl metrics in the stderr
// returns 0 on success and -1 on error
void print_rapl_metrics();
#else
#endif
