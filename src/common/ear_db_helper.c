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

#include <common/config.h>
#include <common/types/job.h>
#include <common/types/loop.h>
#include <common/types/signature.h>
#include <common/types/application.h>

#if DB_MYSQL

#include <mysql.h>


#define APPLICATION_QUERY   "INSERT INTO Applications (job_id, step_id, node_id, signature_id) VALUES" \
                            "(?, ?, ?, ?)"


#define LOOP_QUERY          "INSERT INTO Loops (event, size, level, job_id, step_id,  node_id, total_iterations, \
                            signature_id) VALUES (?, ?, ?, ?, ?, ?, ? ,?)"


#define JOB_QUERY           "INSERT IGNORE INTO Jobs (id, step_id, user_id, app_id, start_time, end_time, start_mpi_time, \
                            end_mpi_time, policy, threshold, procs, job_type, def_f) VALUES \
                            (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"


#define SIGNATURE_QUERY     "INSERT INTO Signatures (DC_power, DRAM_power, PCK_power, EDP,\
                            GBS, TPI, CPI, Gflops, time, FLOPS1, FLOPS2, FLOPS3, FLOPS4, \
                            FLOPS5, FLOPS6, FLOPS7, FLOPS8, L1_misses, L2_misses, L3_misses, \
                            instructions, cycles, avg_f, def_f) VALUES (?, ?, ?, ?, ?, ?, \
                            ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

int mysql_statement_error(MYSQL_STMT *statement)
{
    fprintf(stderr, "MYSQL statemnt error (%d): %s\n", mysql_stmt_errno(statement), mysql_stmt_error(staetment));
    mysql_stmt_close(statement);
    return -1;
}

int mysql_insert_application(MYSQL *connection, application_t *app)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return 1;

    if (mysql_stmt_prepare(statement, APPLICATION_QUERY, strlen(APPLICATION_QUERY))) return mysql_statement_error(statement);

    MYSQL_BIND bind[4];
    memset(bind, 0, sizeof(bind));

    mysql_insert_job(connection, &app->job);
    int sig_id = mysql_insert_signature(connection, &app->signature);

    //integer types
    bind[0].buffer_type = bind[1].buffer_type = bind[3].buffer_type = MYSQL_TYPE_LONG;
    bind[0].is_unsigned = bind[1].is_unsigned = bind[3].buffer_type = 1;

    //string types
    bind[2].buffer_type = MYSQL_TYPE_VARCHAR;
    bind[2].buffer_length = strlen(app->node_id);

    //storage variable assignation
    bind[0].buffer = (char *)&app->job.id;
    bind[1].buffer = (char *)&app->job.step_id;
    bind[2].buffer = (char *)&app->node_id;
    bind[3].buffer = (char *)&sig_id;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);


    if (mysql_stmt_close(statement)) return 1;

    return 0;
}

int mysql_retrieve_applications(MYSQL *connection, char *query, application_t **apps)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return 1;
    application_t *app_aux = calloc(1, sizeof(application_t));
    application_t *apps_aux;
    int status = 0;
    int num_jobs;

    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    MYSQL_BIND bind[4];
    unsigned long job_id, step_id, sig_id;
    int num_apps;
    memset(bind, 0, sizeof(bind));

    //integer types
    bind[0].buffer_type = bind[1].buffer_type = bind[3].buffer_type = MYSQL_TYPE_LONG;
    bind[0].is_unsigned = bind[1].is_unsigned = bind[3].buffer_type = 1;

    //string types
    bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[2].buffer_length = 256;

    //reciever variables assignation
    bind[0].buffer = &job_id;
    bind[1].buffer = &step_id;
    bind[2].buffer = &app_aux->node_id;
    bind[3].buffer = &sig_id;

    if (mysql_stmt_bind_result(statement, bind))
    {
        free(app_aux);
        return mysql_statement_error(statement);
    } 

    if (mysql_stmt_execute(statement))
    {
        free(app_aux);
        return mysql_statement_error(statement);
    } 

    if (mysql_stmt_store_result(statement))
    {
        free(app_aux);
        return mysql_statement_error(statement);
    } 

    num_apps = mysql_stmt_num_rows(statement);

    if (num_apps < 1)
    {
        mysql_stmt_close(statement);
        return -1;
    }

    apps_aux = (application_t*) calloc(num_apps, sizeof(application_t));
    int i = 0;
    char job_query[128];
    char sig_query[128];
    job_t *job_aux;
    signature_t *sig_aux;
    //fetching and storing of jobs    
    status = mysql_stmt_fetch(statement);
    while (status == 0 || status == MYSQL_DATA_TRUNCATED)
    {
        sprintf(job_query, "SELECT * FROM Jobs WHERE id=%d AND step_id=%d", job_id, step_id);
        num_jobs = mysql_retrieve_jobs(connection, job_query, &job_aux);
        copy_job(&app_aux->job, job_aux);
        free(job_aux);

        sprintf(sig_query, "SELECT * FROM Signatures WHERE id=%d", sig_id);
        int num_sigs = mysql_retrieve_signatures(connection, sig_query, &sig_aux);
        copy_signature(&app_aux->signature, sig_aux);
        free(sig_aux);

        copy_application(&apps_aux[i], app_aux);
        status = mysql_stmt_fetch(statement);
        i++;
    }

    *apps = apps_aux;

    free(app_aux);

    if (mysql_stmt_close(statement)) return 0;

    return num_apps;

}


int mysql_insert_loop(MYSQL *connection, loop_t *loop)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return 1;
    int i;

    if (mysql_stmt_prepare(statement, LOOP_QUERY, strlen(LOOP_QUERY))) return mysql_statement_error(statement);

    MYSQL_BIND bind[8];
    memset(bind, 0, sizeof(bind));

    mysql_insert_job(connection, loop->job);
    
    int sig_id = mysql_insert_signature(connection, &loop->signature);

    //integer types
    for (i = 0; i < 8; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONG;
        bind[i].is_unsigned = 1;
    }

    //string types
    bind[5].buffer_type = MYSQL_TYPE_VARCHAR;
    bind[5].buffer_length = strlen(loop->node_id);

    //storage variable assignation
    bind[0].buffer = (char *)&loop->id.event;
    bind[1].buffer = (char *)&loop->id.size;
    bind[2].buffer = (char *)&loop->id.level;
    bind[3].buffer = (char *)&loop->job->id;
    bind[4].buffer = (char *)&loop->job->step_id;
    bind[5].buffer = (char *)&loop->node_id;
    bind[6].buffer = (char *)&loop->total_iterations;
    bind[7].buffer = (char *)&sig_id;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    if (mysql_stmt_close(statement)) return 0;

    return 1;
}

int mysql_retrieve_loops(MYSQL *connection, char *query, loop_t **loops)
{
    
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return 1;
    loop_t *loop_aux = calloc(1, sizeof(loop_t));
    loop_t *loops_aux;
    int status = 0;
    int num_loops, num_jobs, i;

    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    MYSQL_BIND bind[8];
    unsigned long job_id, step_id, sig_id;
    int num_apps;
    memset(bind, 0, sizeof(bind));


    //integer types
    for (i = 0; i < 8; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONG;
        bind[i].is_unsigned = 1;
    }

    //string types
    bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[5].buffer_length = 256;

    //reciever variables assignation
    bind[0].buffer = &loop_aux->id.event;
    bind[1].buffer = &loop_aux->id.size;
    bind[2].buffer = &loop_aux->id.level;
    bind[3].buffer = &job_id;
    bind[4].buffer = &step_id;
    bind[5].buffer = &loop_aux->node_id;
    bind[6].buffer = &loop_aux->total_iterations;
    bind[7].buffer = &sig_id;


    if (mysql_stmt_bind_result(statement, bind)) 
    {
        free(loop_aux);
        return mysql_statement_error(statement);
    }

    if (mysql_stmt_execute(statement))
    {
        free(loop_aux);
        return mysql_statement_error(statement);
    } 

    if (mysql_stmt_store_result(statement))
    {
        free(loop_aux);
        return mysql_statement_error(statement);
    } 

    num_loops = mysql_stmt_num_rows(statement);

    if (num_loops < 1)
    {
        mysql_stmt_close(statement);
        free(loop_aux);
        return 1;
    }

    loops_aux = (loop_t*) calloc(num_loops, sizeof(loop_t));
    char job_query[128];
    char sig_query[128];
    job_t *job_aux;
    signature_t *sig_aux;
    //fetching and storing of jobs
    i = 0;
    status = mysql_stmt_fetch(statement);
    while (status == 0 || status == MYSQL_DATA_TRUNCATED)
    {
        sprintf(job_query, "SELECT * FROM Jobs WHERE id=%d AND step_id=%d", job_id, step_id);
        num_jobs = mysql_retrieve_jobs(connection, job_query, &job_aux);
        loop_aux->job = job_aux;

        sprintf(sig_query, "SELECT * FROM Signatures WHERE id=%d", sig_id);
        int num_sigs = mysql_retrieve_signatures(connection, sig_query, &sig_aux);
        copy_signature(&loop_aux->signature, sig_aux);
        free(sig_aux);

        copy_loop(&loops_aux[i], loop_aux);

        status = mysql_stmt_fetch(statement);
        i++;
    }

    *loops = loops_aux;

    free(loop_aux);
    if (mysql_stmt_close(statement)) return 0;

    return num_loops;
}


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

//returns id of the inserted signature
int mysql_insert_signature(MYSQL *connection, signature_t *sig)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return 0;

    if (mysql_stmt_prepare(statement, SIGNATURE_QUERY, strlen(SIGNATURE_QUERY))) return 1;

    MYSQL_BIND bind[24];
    int i = 0;

    //double storage
    for (i = 0; i < 9; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
        bind[i].length = 0;
        bind[i].is_null = 0;
    }

    //unsigned long long storage
    for (i = 9; i < 22; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
        bind[i].length = 0;
        bind[i].is_null = 0;
        bind[i].is_unsigned = 1;
    }

    //unsigned long storage
    bind[22].buffer_type = bind[23].buffer_type = MYSQL_TYPE_LONG;
    bind[22].length = bind[23].length = 0;
    bind[22].is_null = bind[23].is_null = 0;
    bind[22].is_unsigned = bind[23].is_unsigned = 1;

    //storage variables assignation
    bind[0].buffer = (char *)&sig->DC_power;
    bind[1].buffer = (char *)&sig->DRAM_power;
    bind[2].buffer = (char *)&sig->PCK_power;
    bind[3].buffer = (char *)&sig->EDP;
    bind[4].buffer = (char *)&sig->GBS;
    bind[5].buffer = (char *)&sig->TPI;
    bind[6].buffer = (char *)&sig->CPI;
    bind[7].buffer = (char *)&sig->Gflops;
    bind[8].buffer = (char *)&sig->time;
    bind[9].buffer = (char *)&sig->FLOPS[0];
    bind[10].buffer = (char *)&sig->FLOPS[1];
    bind[11].buffer = (char *)&sig->FLOPS[2];
    bind[12].buffer = (char *)&sig->FLOPS[3];
    bind[13].buffer = (char *)&sig->FLOPS[4];
    bind[14].buffer = (char *)&sig->FLOPS[5];
    bind[15].buffer = (char *)&sig->FLOPS[6];
    bind[16].buffer = (char *)&sig->FLOPS[7];
    bind[17].buffer = (char *)&sig->L1_misses;
    bind[18].buffer = (char *)&sig->L2_misses;
    bind[19].buffer = (char *)&sig->L3_misses;
    bind[20].buffer = (char *)&sig->instructions;
    bind[21].buffer = (char *)&sig->cycles;
    bind[22].buffer = (char *)&sig->avg_f;
    bind[23].buffer = (char *)&sig->def_f;

    if (mysql_stmt_bind_param(statement, bind)) return 0;

    if (mysql_stmt_execute(statement)) return 0;

    int id = mysql_stmt_insert_id(statement);

    my_ulonglong affected_rows = mysql_stmt_affected_rows(statement);

    if (affected_rows != 1) printf("ERROR: inserting signature failed.\n");

    if (mysql_stmt_close(statement)) return 0;

    return id;
}

int mysql_retrieve_signatures(MYSQL *connection, char *query, signature_t **sigs)
{
    signature_t *sig_aux = calloc(1, sizeof(signature_t));
    signature_t *sigs_aux;
    int num_signatures;
    int i = 0;
    int status = 0;

    MYSQL_BIND bind[25];
    memset(bind, 0, sizeof(bind));
    
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return 1;
    
    int id = 0;
    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    //id reciever
    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer_length= 4;
    bind[0].is_null = 0;
    bind[0].is_unsigned = 1;

    //double recievers
    for (i = 1; i < 10; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
        bind[i].buffer_length = 8;
        bind[i].is_null = 0;
    }

    //unsigned long long recievers
    for (i = 9; i < 22; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
        bind[i].buffer_length = 8;
        bind[i].is_null = 0;
        bind[i].is_unsigned = 1;
    }

    //unsigned long recievers
    bind[23].buffer_type = bind[24].buffer_type = MYSQL_TYPE_LONG;
    bind[23].buffer_length = bind[24].buffer_length = 4;
    bind[23].is_null = bind[24].is_null = 0;
    bind[23].is_unsigned = bind[24].is_unsigned = 1;

    //reciever variables assignation
    bind[0].buffer = &id;
    bind[1].buffer = &sig_aux->DC_power;
    bind[2].buffer = &sig_aux->DRAM_power;
    bind[3].buffer = &sig_aux->PCK_power;
    bind[4].buffer = &sig_aux->EDP;
    bind[5].buffer = &sig_aux->GBS;
    bind[6].buffer = &sig_aux->TPI;
    bind[7].buffer = &sig_aux->CPI;
    bind[8].buffer = &sig_aux->Gflops;
    bind[9].buffer = &sig_aux->time;
    bind[10].buffer = &sig_aux->FLOPS[0];
    bind[11].buffer = &sig_aux->FLOPS[1];
    bind[12].buffer = &sig_aux->FLOPS[2];
    bind[13].buffer = &sig_aux->FLOPS[3];
    bind[14].buffer = &sig_aux->FLOPS[4];
    bind[15].buffer = &sig_aux->FLOPS[5];
    bind[16].buffer = &sig_aux->FLOPS[6];
    bind[17].buffer = &sig_aux->FLOPS[7];
    bind[18].buffer = &sig_aux->L1_misses;
    bind[19].buffer = &sig_aux->L2_misses;
    bind[20].buffer = &sig_aux->L3_misses;
    bind[21].buffer = &sig_aux->instructions;
    bind[22].buffer = &sig_aux->cycles;
    bind[23].buffer = &sig_aux->avg_f;
    bind[24].buffer = &sig_aux->def_f;

    if (mysql_stmt_bind_result(statement, bind)) 
    {
        free(sig_aux);
        return mysql_statement_error(statement);
    }

    if (mysql_stmt_execute(statement)) 
    {
        free(sig_aux);
        return mysql_statement_error(statement);
    }

    if (mysql_stmt_store_result(statement)) 
    {
        free(sig_aux);
        return mysql_statement_error(statement);
    }

    num_signatures = mysql_stmt_num_rows(statement);

    if (num_signatures < 1)
    {
        mysql_stmt_close(statement);
        return 1;
    }

    sigs_aux = (signature_t*) calloc(num_signatures, sizeof(signature_t));
    i = 0; 
    
    //fetching and storing of signatures
    status = mysql_stmt_fetch(statement);
    while (status == 0 || status == MYSQL_DATA_TRUNCATED)
    {
        copy_signature(&sigs_aux[i], sig_aux);
        status = mysql_stmt_fetch(statement);
        i++;
    }

    *sigs = sigs_aux;


    if (mysql_stmt_close(statement)) return 1;

    free(sig_aux);

    return (i == 1) ? 0: i;
}

#endif