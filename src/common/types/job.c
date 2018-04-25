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

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <common/types/job.h>

void init_job(job_t *job, ulong def_f, char *policy, double th, ulong procs)
{
    memset(job, 0, sizeof(job_t));
    job->def_f = def_f;
    strcpy(job->policy, policy);
    job->th = th;
    job->procs = procs;
}

ulong start_mpi(job_t *job)
{
    job->start_mpi_time = time(&job->start_mpi_time);
}

ulong end_mpi(job_t *job)
{
    job->end_mpi_time = time(&job->start_mpi_time);
}

void copy_job(job_t *destiny, job_t *source)
{
    memcpy(destiny, source, sizeof(job_t));
}

void print_job_fd(int fd, job_t *job)
{
    dprintf(fd, "%s;%lu;%s;%s;%lf", job->user_id, job->id, job->app_id, job->policy, job->th);
}

#if DB_MYSQL

#define JOB_QUERY           "INSERT IGNORE INTO Jobs (id, step_id, user_id, app_id, start_time, end_time, start_mpi_time, \
                            end_mpi_time, policy, threshold, procs, job_type, def_f) VALUES \
                            (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

#include <mysql.h>

int mysql_insert_job(MYSQL *connection, job_t *job)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return 1;

    if (mysql_stmt_prepare(statement, JOB_QUERY, strlen(JOB_QUERY))) return mysql_statement_error(statement);

    MYSQL_BIND bind[13];
    memset(bind, 0, sizeof(bind));

    //integer types
    bind[0].buffer_type = bind[4].buffer_type = bind[5].buffer_type = bind[12].buffer_type
    = bind[6].buffer_type = bind[7].buffer_type = bind[10].buffer_type = bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[0].is_unsigned = bind[10].is_unsigned = 1;

    //string types
    bind[2].buffer_type = bind[3].buffer_type = bind[8].buffer_type = MYSQL_TYPE_VARCHAR;
    bind[2].buffer_length = strlen(job->user_id);
    bind[3].buffer_length = strlen(job->app_id);
    bind[8].buffer_length = strlen(job->policy);

    //double types
    bind[9].buffer_type = MYSQL_TYPE_DOUBLE;

    //storage variable assignation
    bind[0].buffer = (char *)&job->id;
    bind[1].buffer = (char *)&job->step_id;
    bind[2].buffer = (char *)&job->user_id;
    bind[3].buffer = (char *)&job->app_id;
    bind[4].buffer = (char *)&job->start_time;
    bind[5].buffer = (char *)&job->end_time;
    bind[6].buffer = (char *)&job->start_mpi_time;
    bind[7].buffer = (char *)&job->end_mpi_time;
    bind[8].buffer = (char *)&job->policy;
    bind[9].buffer = (char *)&job->th;
    bind[10].buffer = (char *)&job->procs;
    bind[11].buffer = (char *)&job->type;
    bind[12].buffer = (char *)&job->def_f;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    if (mysql_stmt_close(statement)) return 1;

    return 0;
}

int mysql_retrieve_jobs(MYSQL *connection, char *query, job_t **jobs)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return -1;
    job_t *job_aux = calloc(1, sizeof(job_t));
    job_t *jobs_aux;
    int status = 0;
    int num_jobs;

    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    MYSQL_BIND bind[13];
    memset(bind, 0, sizeof(bind));
    //integer types
    bind[0].buffer_type = bind[4].buffer_type = bind[5].buffer_type = bind[12].buffer_type
    = bind[6].buffer_type = bind[7].buffer_type = bind[10].buffer_type = bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[0].is_unsigned = bind[10].is_unsigned = 1;

    //string types
    bind[2].buffer_type = bind[3].buffer_type = bind[8].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[2].buffer_length = bind[3].buffer_length = bind[8].buffer_length = 256;

    //double types
    bind[9].buffer_type = MYSQL_TYPE_DOUBLE;


    //reciever variables assignation
    bind[0].buffer = &job_aux->id;
    bind[1].buffer = &job_aux->step_id;
    bind[2].buffer = &job_aux->user_id;
    bind[3].buffer = &job_aux->app_id;
    bind[4].buffer = &job_aux->start_time;
    bind[5].buffer = &job_aux->end_time;
    bind[6].buffer = &job_aux->start_mpi_time;
    bind[7].buffer = &job_aux->end_mpi_time;
    bind[8].buffer = &job_aux->policy;
    bind[9].buffer = &job_aux->th;
    bind[10].buffer = &job_aux->procs;
    bind[11].buffer = &job_aux->type;
    bind[12].buffer = &job_aux->def_f;

    if (mysql_stmt_bind_result(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    if (mysql_stmt_store_result(statement)) return mysql_statement_error(statement);

    num_jobs = mysql_stmt_num_rows(statement);

    if (num_jobs < 1)
    {
        mysql_stmt_close(statement);
        return -1;
    }

    jobs_aux = (job_t*) calloc(num_jobs, sizeof(job_t));
    int i = 0;
    
    //fetching and storing of jobs    
    status = mysql_stmt_fetch(statement);
    while (status == 0 || status == MYSQL_DATA_TRUNCATED)
    {
        copy_job(&jobs_aux[i], job_aux);
        status = mysql_stmt_fetch(statement);
        i++;
    }

    *jobs = jobs_aux;

    if (mysql_stmt_close(statement)) return 0;

    free(job_aux);

    return num_jobs;

}

#endif