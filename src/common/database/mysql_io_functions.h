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
#include <common/types/loop.h>
#include <common/types/signature.h>
#include <common/types/application.h>
#if SHARED_MEMORY
#include <common/types/power_signature.h>
#endif

int mysql_insert_application(MYSQL *connection, application_t *app);

/** Given a MYSQL connection and a valid MYSQL query, stores in apps the 
*   applications found in the database corresponding to the query. Returns the 
*   number of applications found on success, and either EAR_MYSQL_ERROR or
*   EAR_MYSQL_STMT_ERROR on error. */
int mysql_retrieve_applications(MYSQL *connection, char *query, application_t **apps);


/** Given a MYSQL connection and a loop, inserts said loop into
*   the database. Returns EAR_SUCCESS on success, and either EAR_MYSQL_ERROR or
*   EAR_MYSQL_STMT_ERROR on error.*/
int mysql_insert_loop(MYSQL *connection, loop_t *loop);

/** Given a MYSQL connection and a valid MYSQL query, stores in loops the 
*   loops found in the database corresponding to the query. Returns the 
*   number of loops found on success, and either EAR_MYSQL_ERROR or
*   EAR_MYSQL_STMT_ERROR on error. */
int mysql_retrieve_loops(MYSQL *connection, char *query, loop_t **loops);


/** Given a MYSQL connection and a job, inserts said job into
*   the database. Returns EAR_SUCCESS on success, and either EAR_MYSQL_ERROR or
*   EAR_MYSQL_STMT_ERROR on error.*/
int mysql_insert_job(MYSQL *connection, job_t *job);

/** Given a MYSQL connection and a valid MYSQL query, stores in jobs the 
*   jobs found in the database corresponding to the query. Returns the 
*   number of jobs found on success, and either EAR_MYSQL_ERROR or
*   EAR_MYSQL_STMT_ERROR on error. */
int mysql_retrieve_jobs(MYSQL *connection, char *query, job_t **jobs);

/** Given a MYSQL connection and a signature, inserts said signature into
*   the database. Returns the signature's database id on success, and either 
*   EAR_MYSQL_ERROR or EAR_MYSQL_STMT_ERROR on error.*/
int mysql_insert_signature(MYSQL *connection, signature_t *sig);

/** Given a MYSQL connection and a valid MYSQL query, stores in sigs the 
*   signatures found in the database corresponding to the query. Returns the 
*   number of signatures found on success, and either EAR_MYSQL_ERROR or
*   EAR_MYSQL_STMT_ERROR on error. */
int mysql_retrieve_signatures(MYSQL *connection, char *query, signature_t **sigs);

#if SHARED_MEMORY
/** Given a MYSQL connection and a power_signature, inserts said power_signature into
*   the database. Returns the power_signature's database id on success, and either 
*   EAR_MYSQL_ERROR or EAR_MYSQL_STMT_ERROR on error.*/
int mysql_insert_power_signature(MYSQL *connection, power_signature_t *pow_sig);
#endif

