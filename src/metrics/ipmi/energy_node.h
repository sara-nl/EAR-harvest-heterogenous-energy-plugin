/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _NODO_ENERGY_METRICS_H_
#define _NODO_ENERGY_METRICS_H_

/* Detects cpu model and platform and initialize data structures and devices if required*/
/* Returns 0 on success and -1 on error */
int node_energy_init();
/* Returns the data size length in bytes returned by read_dc_energy and read_ac_energy */
/* Returns 0 on success and -1 on error */
int count_energy_data_length();
/* Update energy with the accumulated node DC energy in mJ */
/* Returns 0 on success and -1 on error */
int read_dc_energy(unsigned long *energy);
/* Update energy with the accumulated node AC energy in mJ */
/* Returns 0 on success and -1 on error */
int read_ac_energy(unsigned long *energy);
/* Releases resources allocated in node_energy_init */
/* Returns 0 on success and -1 on error */
int node_energy_dispose();
/* Computes the energy node frequency of updates automatically */
/* returns the frequency (in usec) on success and 0 on error */
/* node_energy_init() must be called before */
unsigned long node_energy_frequency();

#else
#endif
