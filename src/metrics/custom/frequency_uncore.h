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

#ifndef EAR_FREQUENCY_UNCORE_H
#define EAR_FREQUENCY_UNCORE_H

#define U_MSR_PMON_FIXED_CTR_OFF	0x000704
#define U_MSR_PMON_FIXED_CTL_OFF	0x000703
#define U_MSR_PMON_FIXED_CTL_STA	0x400000
#define U_MSR_PMON_FIXED_CTL_STO	0x000000

/* */
state_t frequency_uncore_init(uint cpus_num, uint cpus_model);

/* */
state_t frequency_uncore_dispose();

/* */
state_t frequency_uncore_counters_start();

/* */
state_t frequency_uncore_counters_stop(uint64_t *buffer);

#endif //EAR_FREQUENCY_UNCORE_H
