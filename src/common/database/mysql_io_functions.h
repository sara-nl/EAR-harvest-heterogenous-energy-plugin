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

#include <mysql/mysql.h>
#include <common/config.h>
#include <common/types/job.h>
#include <common/types/application.h>
#include <common/types/loop.h>
#include <common/types/signature.h>

int mysql_insert_application(MYSQL *connection, application_t *app);

int mysql_retrieve_applications(MYSQL *connection, char *query, application_t **apps);

int mysql_insert_loop(MYSQL *connection, loop_t *loop);

int mysql_retrieve_loops(MYSQL *connection, char *query, loop_t **loops);

/** Given a DB connection and a job, inserts said job to the DB. Returns
*	0 on success, -1 on error. */
int mysql_insert_job(MYSQL *connection, job_t *job);

/** Given a DB connection and a DB query, stores in jobs the jobs found
*	that correspond to said query, if any. Returns the number of jobs
*	found on success */
int mysql_retrieve_jobs(MYSQL *connection, char *query, job_t **jobs);

int mysql_insert_signature(MYSQL *connection, signature_t *sig);

int mysql_retrieve_signatures(MYSQL *connection, char *query, signature_t **sigs);
