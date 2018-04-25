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

#ifndef _EAR_TYPES_JOB
#define _EAR_TYPES_JOB
#include <stdint.h>
#include <time.h>
#include <common/types/generic.h>

typedef uint8_t	job_type;
typedef ulong	job_id;

#define GENERIC_NAME 256
#define POLICY_NAME 32

typedef struct job
{
	job_id 	id;
	job_id 	step_id;
	char 	user_id[GENERIC_NAME];
	char 	app_id[GENERIC_NAME];
	time_t 	start_time;
	time_t	end_time;
	time_t 	start_mpi_time;
	time_t	end_mpi_time;
	char 	policy[POLICY_NAME];
	double  th;
	ulong 	procs; 
	job_type	type;	
	ulong 		def_f;
} job_t;



// Function declarations

// MANAGEMENT


// Must be called just once. memory is allocated if needed. values automatically initialized are (job_id, user_id, start_time,end_time,type). 
void init_job(job_t *job, ulong def_f, char *policy, double th, ulong procs);

ulong start_mpi(job_t *job);
ulong end_mpi(job_t *job);

//
void copy_job(job_t *destiny, job_t *source);
// ?? Really needed
void end_job(job_t *job);

void print_job_fd(int fd, job_t *job);

#if DB_MYSQL

/** Given a DB connection and a job, inserts said job to the DB. Returns
*	0 on success, -1 on error. */
int mysql_insert_job(MYSQL *connection, job_t *job)
/** Given a DB connection and a DB query, stores in jobs the jobs found
*	that correspond to said query, if any. Returns the number of jobs
*	found on success */
int mysql_retrieve_jobs(MYSQL *connection, char *query, job_t **jobs)
#endif


#endif
