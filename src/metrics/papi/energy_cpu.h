/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _RAPL_METRICS_H_
#define _RAPL_METRICS_H_

#define RAPL_EVS 			4
#define RAPL_DRAM0 			0
#define RAPL_DRAM1 			1
#define RAPL_PACKAGE0 		2
#define RAPL_PACKAGE1 		3

/** Initializes the PAPI library if needed and the "DRAM_ENERGY:PACKAGE0",
*   "DRAM_ENERGY:PACKAGE1", "PACKAGE_ENERGY:PACKAGE0", "PACKAGE_ENERGY:PACKAGE1"
*   events. Returns 0 on success and -1 on error. */
int init_rapl_metrics();

/** Resets (zeroes) rapl events. Returns 0 on success and -1 on error. */
int reset_rapl_metrics();

/** Starts rapl events. Returns 0 on success and -1 on error. */
int start_rapl_metrics();

/** Stops rapl events and copies events values in values array. Returns 0 on
*   success and -1 on error. */
int stop_rapl_metrics(unsigned long long *values);

/** Reads rapl events and copies events values in values array. Returns 0 on
*   success and -1 on error. */
int read_rapl_metrics(unsigned long long *_values);

/** Prints rapl metrics in the stderr. Returns 0 on success and -1 on error. */
void print_rapl_metrics();

#else
#endif
