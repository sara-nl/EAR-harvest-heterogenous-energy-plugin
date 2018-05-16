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
#if SHARED_MEMORY
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <common/math_operations.h>
#include <common/types/periodic_metric.h>


void copy_periodic_metric(periodic_metric_t *destiny, periodic_metric_t *source)
{
    memcpy(destiny, source, sizeof(periodic_metric_t));
}

void init_periodic_metric(periodic_metric_t *pm)
{
    memset(pm, 0, sizeof(periodic_metric_t));
	gethostname(pm->node_id,NODE_SIZE);
	pm->job_id=0;
	pm->step_id=0;	
}
void init_sample_period(periodic_metric_t *pm)
{
    time(&pm->start_time);
}

void end_sample_period(periodic_metric_t *pm,ulong energy)
{
	time(&pm->end_time);
	pm->DC_energy=energy;	
}

void new_job_for_period(periodic_metric_t *pm,ulong job_id, ulong s_id)
{
	pm->job_id=job_id;
	pm->step_id=s_id;	
}
void end_job_for_period(periodic_metric_t *pm)
{
	pm->job_id=0;
	pm->step_id=0;	
}

void print_periodic_metric_t(int fd, periodic_metric_t *pm)
{
    /* print order: JOB_ID;STEP_ID;NODE_ID;START_TIME;END_TIME;ENERGY*/
    int i;
	dprintf(fd, "%d;%d", pm ->job_id,pm->step_id);    
    dprintf(fd, "%s;", pm->node_id);
	dprintf(fd, "%s;%s;", ctime(&pm->start_time), ctime(&pm->start_time));
    dprinft(fd, "%llu;", pm->DC_energy);
}


#endif
