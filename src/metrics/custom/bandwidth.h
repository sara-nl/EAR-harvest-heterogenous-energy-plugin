/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

// All these functions returns the specific funcion errror.
// pmons.init for init_uncores etc.

/** Init the uncore counters for an specific cpu model number. */
int init_uncores(int cpu_model);

/** Get the number of performance monitor counters.
*   init_uncore_reading() have to be called before to scan the buses. */
int count_uncores();

/** Checks the state of the system uncore functions.
*   Returns EAR_SUCCESS, EAR_ERROR or EAR_WARNING. */
int check_uncores();

/** Freezes and resets all performance monitor (PMON) uncore counters. */
int reset_uncores();

/** Unfreezes all PMON uncore counters. */
int start_uncores();

/** Freezes all PMON uncore counters and gets it's values. The array
*   has to be greater or equal than the number of PMON uncore counters
*   returned by count_uncores() function. The returned values are the
*   read and write bandwith values in index [i] and [i+1] respectively. */
int stop_uncores(unsigned long long *values);

/** Gets the PMON uncore counters values. */
int read_uncores(unsigned long long *values);

/** Closes file descriptors and frees memory. */
int dispose_uncores();
