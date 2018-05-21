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

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <common/types/job.h>

void init_job(job_t *job, ulong def_f, char *policy, double th, ulong procs)
{
    memset(job, 0, sizeof(job_t));
    job->def_f = def_f;
    strcpy(job->policy, policy);
    job->th = th;
    job->procs = procs;
}
void start_job(job_t *job)
{
   
    time(&job->start_time);
}

void end_job(job_t *job)
{
    time(&job->end_time);
}

void start_mpi(job_t *job)
{
    time(&job->start_mpi_time);
}

void end_mpi(job_t *job)
{
    time(&job->end_mpi_time);
}

void copy_job(job_t *destiny, job_t *source)
{
    memcpy(destiny, source, sizeof(job_t));
}

void print_job_fd(int fd, job_t *job)
{
	char job_buff[1024];
	assert(job!=NULL);
	if ((job->user_id==NULL) || (job->app_id==NULL) || (job->policy==NULL)){
		fprintf(stderr,"print_job_fd some of the args are null\n");
		return;
	}
    sprintf(job_buff, "%s;%lu;%s;%s;%lf", job->user_id, job->id, job->app_id, job->policy, job->th);
	write(fd,job_buff,strlen(job_buff));
}
