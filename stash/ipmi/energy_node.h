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
#include <freeipmi/freeipmi.h>

typedef struct energy_handler{
	ipmi_ctx_t ctx;
	int ear_energy_node_connected;
	int ear_energy_node_status;
}energy_handler_t;

/** Detects cpu model and platform and initialize data structures and devices if required
*   Returns 0 on success and -1 on error */
int node_energy_init(energy_handler_t *eh);

/** Returns the data size length in bytes returned by read_dc_energy and read_ac_energy 
*   Returns 0 on success and -1 on error */
int count_energy_data_length(energy_handler_t *eh);

/** Update energy with the accumulated node DC energy in mJ 
*   Returns 0 on success and -1 on error */
int read_dc_energy(energy_handler_t *eh,unsigned long *energy);
int read_dc_energy_try(energy_handler_t *eh,unsigned long *energy);

/** Update energy with the accumulated node DC energy in mJ and time in ms
*   Returns 0 on success and -1 on error */
int read_dc_energy_time(energy_handler_t *eh,ulong *energy,ulong *time_ms);
int read_dc_energy_time_try(energy_handler_t *eh,ulong *energy,ulong *time_ms);
int read_dc_energy_time_debug(energy_handler_t *eh,ulong *energy_j,ulong *energy_mj,ulong *time_sec,ulong *time_ms);
int read_dc_energy_time_debug_try(energy_handler_t *eh,ulong *energy_j,ulong *energy_mj,ulong *time_sec,ulong *time_ms);

/** Update energy with the accumulated node AC energy in mJ 
*   Returns 0 on success and -1 on error */
int read_ac_energy(energy_handler_t *eh,unsigned long *energy);

/** Releases resources allocated in node_energy_init 
*   Returns 0 on success and -1 on error */
int node_energy_dispose(energy_handler_t *eh);

/** Computes the energy node frequency of updates automatically 
*   returns the frequency (in usec) on success and 0 on error 
*   node_energy_init() must be called before */
unsigned long node_energy_frequency(energy_handler_t *eh);

#else
#endif
