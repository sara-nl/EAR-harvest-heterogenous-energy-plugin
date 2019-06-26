/**************************************************************
 * *	Energy Aware Runtime (EAR)
 * *	This program is part of the Energy Aware Runtime (EAR).
 * *
 * *	EAR provides a dynamic, transparent and ligth-weigth solution for
 * *	Energy management.
 * *
 * *    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017
 * *	BSC Contact 	mailto:ear-support@bsc.es
 * *	Lenovo contact 	mailto:hpchelp@lenovo.com
 * *
 * *	EAR is free software; you can redistribute it and/or
 * *	modify it under the terms of the GNU Lesser General Public
 * *	License as published by the Free Software Foundation; either
 * *	version 2.1 of the License, or (at your option) any later version.
 * *
 * *	EAR is distributed in the hope that it will be useful,
 * *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * *	Lesser General Public License for more details.
 * *
 * *	You should have received a copy of the GNU Lesser General Public
 * *	License along with EAR; if not, write to the Free Software
 * *	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * *	The GNU LEsser General Public License is contained in the file COPYING
 * */

#ifndef EAR_FREQUENCY_H
#define EAR_FREQUENCY_H

#include <common/states.h>
#include <common/types/generic.h>

#define MSR_IA32_APERF 0x000000E8
#define MSR_IA32_MPERF 0x000000E7

typedef struct snap_core
{
        uint64_t aperf;
        uint64_t mperf;
} snap_core_t;

/* */
state_t frequency_core_init(uint cores_num, uint64_t cores_nom_freq);

/* */
state_t frequency_core_dispose();

/* snaps must be a vector with cores_num entries */
state_t frequency_core_snap(snap_core_t *snaps);

/* snaps and freqs must vectors with cores_num entries */
state_t frequency_core_read(snap_core_t *snaps, uint64_t *freqs);

#endif //EAR_FREQUENCY_H
