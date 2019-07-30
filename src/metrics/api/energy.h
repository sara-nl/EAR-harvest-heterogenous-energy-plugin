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

#ifndef _API_ENERGY_H_
#define _API_ENERGY_H_

#include <common/states.h>
#include <common/types/generic.h>
#include <metrics/api/energy_data.h>

state_t energy_init(void **c);

state_t energy_dispose(void **c);

state_t energy_datasize(void *c, size_t *size);

state_t energy_frequency(void *c, ulong *freq_us);

state_t energy_dc_read(void *c, edata_t energy_mj);

state_t energy_dc_time_read(void *c, edata_t energy_mj, ulong *time_ms);

state_t energy_ac_read(void *c, edata_t energy_mj);

/* Energy units are 1=Joules, 1000=mJ, 1000000=uJ, 1000000000nJ */
state_t energy_units(uint *units);

state_t energy_accumulated(unsigned long *e,edata_t init,edata_t end);


#endif
