/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/



#ifndef _NODO_ENERGY_METRICS_H_
#define _NODO_ENERGY_METRICS_H_

/** Detects cpu model and platform and initialize data structures and devices if required
*   Returns 0 on success and -1 on error */
int node_energy_init();

/** Returns the data size length in bytes returned by read_dc_energy and read_ac_energy 
*   Returns 0 on success and -1 on error */
int count_energy_data_length();

/** Update energy with the accumulated node DC energy in mJ 
*   Returns 0 on success and -1 on error */
int read_dc_energy(unsigned long *energy);

/** Update energy with the accumulated node AC energy in mJ 
*   Returns 0 on success and -1 on error */
int read_ac_energy(unsigned long *energy);

/** Releases resources allocated in node_energy_init 
*   Returns 0 on success and -1 on error */
int node_energy_dispose();

/** Computes the energy node frequency of updates automatically 
*   returns the frequency (in usec) on success and 0 on error 
*   node_energy_init() must be called before */
unsigned long node_energy_frequency();

#else
#endif
