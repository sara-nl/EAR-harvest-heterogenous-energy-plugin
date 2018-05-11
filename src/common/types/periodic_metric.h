/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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

#include <common/config.h>
#include <time.h>

#if SHARED_MEMORY

#ifndef _EAR_TYPES_POWER_SIGNATURE
#define _EAR_TYPES_POWER_SIGNATURE

#include <common/types/generic.h>


typedef struct periodic_metric 
{
    unsigned long long DC_energy;
    time_t start_period;
    time_t end_period;
} periodic_metric_t;


// Function declarations

/** Replicates the periodic_metric in *source to *destiny */
void copy_periodic_metric(periodic_metric_t *destiny, periodic_metric_t *source);

/** Initializes all values of the periodic_metric to 0 and initializes the period 
 *  setting the start_period to the current time. */
void init_periodic_metric(periodic_metric_t *pm);

/** Outputs the power_signature contents to the file pointed by the fd. */
void print_periodic_metric_fd(int fd, periodic_metric_t *pm);

#endif

#endif
