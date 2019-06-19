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

#ifndef EAR_ENERGY_H
#define EAR_ENERGY_H

typedef struct energy_handler {
	char name_manufacturer[SZ_NAME_MEDIUM];
	char name_product[SZ_NAME_MEDIUM];
	char path_object[SZ_PATH];
	int interface;
	int connected;
	int status;
	void *context;
} ehandler_t;

state_t energy_init(ehandler_t *eh);

state_t energy_dispose(ehandler_t *eh);

state_t energy_getdata_length(ehandler_t *eh);

state_t energy_getdate_frequency(ehandler_t *eh);

state_t energy_dc_read(ehandler_t *eh, ulong *energy);

state_t energy_dc_readtry(ehandler_t *eh, ulong *energy);

state_t energy_dc_time_read(ehandler_t *eh, ulong *energy, ulong *time_ms);

state_t energy_dc_time_readtry(ehandler_t *eh, ulong *energy, ulong *time_ms);

state_t energy_dc_time_debug(ehandler_t *eh, ulong *ej, ulong *emj, ulong *ts, ulong *tms);

state_t energy_dc_time_debugtry(ehandler_t *eh, ulong *ej, ulong *emj, ulong *ts, ulong *tms);

state_t energy_ac_read(ehandler_t *eh, ulong *energy);

#endif //EAR_ENERGY_H
