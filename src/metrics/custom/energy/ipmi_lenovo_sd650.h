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

#ifndef _IPMI_LENOVO_SD650_H_
#define _IPMI_LENOVO_SD650_H__

#include <common/sizes.h>
#include <common/states.h>

state_t plug_energy_init(void **c);

state_t plug_energy_dispose(void **c);

state_t plug_energy_getdata_length(void *c, size_t *size);

state_t plug_energy_dc_read(void *c, ulong *emj);

state_t plug_energy_dc_time_read(void *c, ulong *emj, ulong *tms);

state_t plug_energy_dc_time_debug(void *c, ulong *ej, ulong *emj, ulong *ts, ulong *tms);

state_t plug_energy_ac_read(void *c, ulong *emj);

#endif
