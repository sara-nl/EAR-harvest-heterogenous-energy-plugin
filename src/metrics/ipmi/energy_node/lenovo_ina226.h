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



#ifndef _LENOVO_WATER_COOLING_H_
#define _LENOVO_WATER_COOLING_H_
/** Specific functions for CPU XX PLATFORM YY
*   Grants access to ipmi device */
int lenovo_wct_node_energy_init();
int lenovo_wct_count_energy_data_length();
int lenovo_wct_read_dc_energy(unsigned long *energy);

/** AC energy is not yet supported */
int lenovo_wct_read_ac_energy(unsigned long *energy);

/** Release access to ipmi device */
int lenovo_wct_node_energy_dispose();

#if DEBUG_INA226
/** Energy and time is returned in a single ipmi command */
int lenovo_wct_read_dc_energy_and_time(ulong *energy,ulong *energy_mj,uint8_t *raw_out,ulong *seconds,ulong *ms);
#else
/** Energy and time is returned in a single ipmi command */
int lenovo_wct_read_dc_energy_and_time(ulong *energy,ulong *energy_mj,ulong *seconds,ulong *ms);
#endif


#else
#endif
