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



#ifndef _IBM_H_
#define _IBM_H_
#include <freeipmi/freeipmi.h>
/** Specific functions for CPU XX PLATFORM YY 
*   Grants access to ipmi device */
int ibm_node_energy_init(ipmi_ctx_t *ipmi_ctx);
int ibm_count_energy_data_length(ipmi_ctx_t ipmi_ctx);
int ibm_read_dc_energy(ipmi_ctx_t ipmi_ctx,ulong *energy);
int ibm_read_dc_energy_time(ipmi_ctx_t ipmi_ctx,ulong *energy,ulong *ms);
int ibm_read_dc_energy_and_time(ipmi_ctx_t ipmi_ctx,ulong *energy,ulong *energy_mj,ulong *seconds,ulong *ms);

/** AC energy is not yet supported */
int ibm_read_ac_energy(ipmi_ctx_t ipmi_ctx,unsigned long *energy);

/** Release access to ipmi device */
int ibm_node_energy_dispose(ipmi_ctx_t *ipmi_ctx);


#else
#endif
