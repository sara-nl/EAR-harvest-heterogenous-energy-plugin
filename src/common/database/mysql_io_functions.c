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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/config.h>
#include <common/states.h>
#include <common/ear_verbose.h>
#include <common/database/mysql_io_functions.h>

#define APPLICATION_QUERY   "INSERT INTO Applications (job_id, step_id, node_id, signature_id, power_signature_id) VALUES" \
                            "(?, ?, ?, ?, ?)"


#define LOOP_QUERY              "INSERT INTO Loops (event, size, level, job_id, step_id,  node_id, total_iterations," \
                                "signature_id) VALUES (?, ?, ?, ?, ?, ?, ? ,?)"


#define JOB_QUERY               "INSERT IGNORE INTO Jobs (id, step_id, user_id, app_id, start_time, end_time, start_mpi_time," \
                                "end_mpi_time, policy, threshold, procs, job_type, def_f, user_acc) VALUES" \
                                "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

#define SIGNATURE_QUERY         "INSERT INTO Signatures (DC_power, DRAM_power, PCK_power, EDP,"\
                                "GBS, TPI, CPI, Gflops, time, FLOPS1, FLOPS2, FLOPS3, FLOPS4, "\
                                "FLOPS5, FLOPS6, FLOPS7, FLOPS8,"\
                                "instructions, cycles, avg_f, def_f) VALUES (?, ?, ?, ?, ?, ?, ?, ?, "\
                                "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

#define POWER_SIGNATURE_QUERY   "INSERT INTO Power_signatures (DC_power, DRAM_power, PCK_power, EDP, max_DC_power, min_DC_power, "\
                                "time, avg_f, def_f) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
#if DEMO
#define PERIODIC_METRIC_QUERY   "INSERT INTO Periodic_metrics (start_time, end_time, dc_energy, node_id, job_id, step_id, avg_f)"\
                                "VALUES (?, ?, ?, ?, ?, ?, ?)"
#else
#define PERIODIC_METRIC_QUERY   "INSERT INTO Periodic_metrics (start_time, end_time, dc_energy, node_id, job_id, step_id)"\
                                "VALUES (?, ?, ?, ?, ?, ?)"
#endif

#define PERIODIC_AGGREGATION_QUERY "INSERT INTO Periodic_aggregations (DC_energy, start_time, end_time) VALUES (?, ?, ?)"

#define EAR_EVENT_QUERY         "INSERT INTO Events (timestamp, event_type, job_id, step_id, freq) VALUES (?, ?, ?, ?, ?)"

#define EAR_WARNING_QUERY       "INSERT INTO Warnings (energy_percent, warning_level, inc_th, p_state) VALUES (?, ?, ?, ?)"

//Learning_phase insert queries
#define LEARNING_APPLICATION_QUERY  "INSERT INTO Learning_applications (job_id, step_id, node_id, "\
                                    "signature_id, power_signature_id) VALUES (?, ?, ?, ?, ?)"

#define LEARNING_SIGNATURE_QUERY    "INSERT INTO Learning_signatures (DC_power, DRAM_power,"\
                                    "PCK_power, EDP, GBS, TPI, CPI, Gflops, time, FLOPS1, FLOPS2, FLOPS3, FLOPS4, "\
                                    "FLOPS5, FLOPS6, FLOPS7, FLOPS8,"\
                                    "instructions, cycles, avg_f, def_f) VALUES (?, ?, ?, ?, ?, ?, ?, ?, "\
                                    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

#define LEARNING_JOB_QUERY          "INSERT IGNORE INTO Learning_jobs (id, step_id, user_id, app_id, start_time, end_time, "\
                                    "start_mpi_time, end_mpi_time, policy, threshold, procs, job_type, def_f, user_acc) VALUES" \
                                    "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

//number of arguments inserted into periodic_metrics
#if DEMO
#define PERIODIC_METRIC_ARGS 7
#else
#define PERIODIC_METRIC_ARGS 6
#endif

#define EAR_EVENTS_ARGS 5

#define SIGNATURE_ARGS 21

#define POWER_SIGNATURE_ARGS 9

#define APPLICATION_ARGS 5

int mysql_statement_error(MYSQL_STMT *statement)
{
    fprintf(stderr, "MYSQL statement error (%d): %s\n", mysql_stmt_errno(statement), mysql_stmt_error(statement));
    mysql_stmt_close(statement);
    return EAR_MYSQL_STMT_ERROR;
}

int mysql_insert_application(MYSQL *connection, application_t *app)
{
    char is_learning = app->is_learning;
    char is_mpi = app->is_mpi;
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;
    
    if (!is_learning)
    {
        if (mysql_stmt_prepare(statement, APPLICATION_QUERY, strlen(APPLICATION_QUERY))) return mysql_statement_error(statement);
    }
    else
    {
        if (mysql_stmt_prepare(statement, LEARNING_APPLICATION_QUERY, strlen(LEARNING_APPLICATION_QUERY))) return mysql_statement_error(statement);
    }
    
    MYSQL_BIND bind[5];
    memset(bind, 0, sizeof(bind));
   
    mysql_insert_job(connection, &app->job, is_learning);
    //integer types
    bind[0].buffer_type = bind[1].buffer_type = bind[3].buffer_type = bind[4].buffer_type = MYSQL_TYPE_LONG;
    bind[0].is_unsigned = bind[1].is_unsigned = bind[3].is_unsigned = bind[4].is_unsigned = 1;

    int pow_sig_id = 0;
    int sig_id = 0;

    pow_sig_id = mysql_insert_power_signature(connection, &app->power_sig);
    
    if (pow_sig_id < 0)
    {
        if (pow_sig_id == EAR_MYSQL_ERROR) fprintf(stderr, "MYSQL error when writing power_signature to database.\n");
        else if (pow_sig_id == EAR_MYSQL_STMT_ERROR) fprintf(stderr, "STMT error when writing power_signature to database.\n");
        else fprintf(stderr,"Unknown error when writing power_signature to database.\n");
    }

    if (is_mpi)
    {
        sig_id = mysql_insert_signature(connection, &app->signature, is_learning);

        if (sig_id < 0)
        {
            if (sig_id == EAR_MYSQL_ERROR) fprintf(stderr, "MYSQL error when writing signature to database.\n");
            else if (sig_id == EAR_MYSQL_STMT_ERROR) fprintf(stderr, "STMT error when writing signature to database.\n");
            else fprintf(stderr,"Unknown error when writing signature to database.\n");
        }

    }
    else 
    {
        bind[3].buffer_type = MYSQL_TYPE_NULL;
        bind[3].is_null = (my_bool*) 1;
    }

    //string types
    bind[2].buffer_type = MYSQL_TYPE_VARCHAR;
    bind[2].buffer_length = strlen(app->node_id);

    //storage variable assignation
    bind[0].buffer = (char *)&app->job.id;
    bind[1].buffer = (char *)&app->job.step_id;
    bind[2].buffer = (char *)&app->node_id;
    bind[4].buffer = (char *)&pow_sig_id;
    
    if (is_mpi) bind[3].buffer = (char *)&sig_id;
    else bind[3].buffer = (char *) NULL;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return EAR_SUCCESS;
}

int mysql_batch_insert_application(MYSQL *connection, application_t *app, int num_apps)
{
    char is_learning = app->is_learning;
    char is_mpi = app->is_mpi;
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    char *params = ", (?, ?, ?, ?, ?)";
    char *query;

    if (!is_learning)
    {
        query = malloc(strlen(APPLICATION_QUERY)+strlen(params)*num_apps);
        strcpy(query, APPLICATION_QUERY);
    }
    else
    {
        query = malloc(strlen(LEARNING_APPLICATION_QUERY)+strlen(params)*num_apps);
        strcpy(query, LEARNING_APPLICATION_QUERY);
    }
    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    MYSQL_BIND *bind = calloc(num_apps*APPLICATION_ARGS, sizeof(bind));
   
    //job only needs to be inserted once
    mysql_insert_job(connection, &app->job, is_learning);
    int pow_sig_id = 0;
    int sig_id = 0;
    int *pow_sigs_ids = calloc(num_apps, sizeof(int));
    int *sigs_ids = calloc(num_apps, sizeof(int));
    
    //inserting all powersignatures (always present)
    pow_sig_id = mysql_batch_insert_power_signatures(connection, app, num_apps);
    
    if (pow_sig_id < 0)
        fprintf(stderr,"Unknown error when writing power_signature to database.\n");

    int i;
    for (i = 0; i < num_apps; i++)
        pow_sigs_ids[i] = pow_sig_id + i;


    //inserting signatures (if the application is mpi)
    if (is_mpi)
    {
        sig_id = mysql_batch_insert_signatures(connection, app, is_learning, num_apps);

        if (sig_id < 0)
            fprintf(stderr,"Unknown error when writing signature to database.\n");

        for (i = 0; i < num_apps; i++)
            sigs_ids[i] = sig_id + i;

    }

    //binding preparations
    for (i = 0; i < num_apps; i++)
    {
        int offset = i*APPLICATION_ARGS;
        //integer types
        bind[0+offset].buffer_type = bind[1+offset].buffer_type = bind[3+offset].buffer_type = bind[4+offset].buffer_type = MYSQL_TYPE_LONG;
        bind[0+offset].is_unsigned = bind[1+offset].is_unsigned = bind[3+offset].is_unsigned = bind[4+offset].is_unsigned = 1;

        if (!is_mpi) 
        {
            bind[3+offset].buffer_type = MYSQL_TYPE_NULL;
            bind[3+offset].is_null = (my_bool*) 1;
        }

        //string types
        bind[2+offset].buffer_type = MYSQL_TYPE_VARCHAR;
        bind[2+offset].buffer_length = strlen(app->node_id);

        //storage variable assignation
        bind[0+offset].buffer = (char *)&app[i].job.id;
        bind[1+offset].buffer = (char *)&app[i].job.step_id;
        bind[2+offset].buffer = (char *)&app[i].node_id;
        bind[4+offset].buffer = (char *)&pow_sigs_ids[i]; 
        
        if (is_mpi) bind[3+offset].buffer = (char *)&sigs_ids[i];
        else bind[3+offset].buffer = (char *) NULL;

    }

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    free(bind);
    free(query);
    free(pow_sigs_ids);
    free(sigs_ids);

    return EAR_SUCCESS;
}

int mysql_retrieve_applications(MYSQL *connection, char *query, application_t **apps, char is_learning)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;
    application_t *app_aux = calloc(1, sizeof(application_t));
    application_t *apps_aux;
    int status = 0;
    int num_jobs;

    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);
    MYSQL_BIND bind[5];
    unsigned long job_id, step_id, sig_id, pow_sig_id;
    int num_apps;
    memset(bind, 0, sizeof(bind));

    //integer types
    bind[0].buffer_type = bind[1].buffer_type = bind[3].buffer_type = bind[4].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].is_unsigned = bind[1].is_unsigned = bind[3].is_unsigned = bind[4].is_unsigned = 1;

    //string types
    bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[2].buffer_length = 256;

    //reciever variables assignation
    bind[0].buffer = &job_id;
    bind[1].buffer = &step_id;
    bind[2].buffer = &app_aux->node_id;
    bind[3].buffer = &sig_id;
    bind[4].buffer = &pow_sig_id;

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
        return 0;
    }
    apps_aux = (application_t*) calloc(num_apps, sizeof(application_t));
    int i = 0;
    char job_query[128];
    char sig_query[128];
    char pow_sig_query[128];
    power_signature_t *pow_sig_aux;

    job_t *job_aux;
    signature_t *sig_aux;
    //fetching and storing of jobs    
    status = mysql_stmt_fetch(statement);

    char is_mpi = 1; //we take mpi as the default type
    if (sig_id < 1 || bind[3].is_null)
        is_mpi = 0;
    
    while (status == 0 || status == MYSQL_DATA_TRUNCATED)
    {
        if (is_learning)
            sprintf(job_query, "SELECT * FROM Learning_jobs WHERE id=%d AND step_id=%d", job_id, step_id);
        else
            sprintf(job_query, "SELECT * FROM Jobs WHERE id=%d AND step_id=%d", job_id, step_id);
        num_jobs = mysql_retrieve_jobs(connection, job_query, &job_aux);
        copy_job(&app_aux->job, job_aux);
        free(job_aux);

        if (is_mpi)
        {
            if (is_learning)
                sprintf(sig_query, "SELECT * FROM Learning_jobs WHERE id=%d AND step_id=%d", job_id, step_id);
            else
                sprintf(sig_query, "SELECT * FROM Signatures WHERE id=%d", sig_id);
            int num_sigs = mysql_retrieve_signatures(connection, sig_query, &sig_aux);
            if (num_sigs > 0) {
                copy_signature(&app_aux->signature, sig_aux);
                free(sig_aux);
            }
            app_aux->is_mpi = 1;
        }
        else app_aux->is_mpi = 0;

        sprintf(pow_sig_query, "SELECT * FROM Power_signatures WHERE id=%d", pow_sig_id);
        int num_pow_sigs = mysql_retrieve_power_signatures(connection, pow_sig_query, &pow_sig_aux);
        if (num_pow_sigs > 0) copy_power_signature(&app_aux->power_sig, pow_sig_aux);
        free(pow_sig_aux);

        copy_application(&apps_aux[i], app_aux);
        status = mysql_stmt_fetch(statement);
        i++;
    }
    *apps = apps_aux;

    free(app_aux);

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return num_apps;
}

int mysql_insert_loop(MYSQL *connection, loop_t *loop)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;
    int i;

    if (mysql_stmt_prepare(statement, LOOP_QUERY, strlen(LOOP_QUERY))) return mysql_statement_error(statement);

    MYSQL_BIND bind[8];
    memset(bind, 0, sizeof(bind));

//    mysql_insert_job(connection, loop->job, 0);
    
    int sig_id = mysql_insert_signature(connection, &loop->signature, 0);

    //integer types
    for (i = 0; i < 8; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
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

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return EAR_SUCCESS;
}

int mysql_retrieve_loops(MYSQL *connection, char *query, loop_t **loops)
{
    
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;
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
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
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
        return EAR_ERROR;
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
    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return num_loops;
}


int mysql_insert_job(MYSQL *connection, job_t *job, char is_learning)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    if (!is_learning)
    {
        if (mysql_stmt_prepare(statement, JOB_QUERY, strlen(JOB_QUERY))) return mysql_statement_error(statement);
    }
    else
    {
        if (mysql_stmt_prepare(statement, LEARNING_JOB_QUERY, strlen(LEARNING_JOB_QUERY))) return mysql_statement_error(statement);
    }

    MYSQL_BIND bind[14];
    memset(bind, 0, sizeof(bind));

    //integer types
    bind[0].buffer_type = bind[4].buffer_type = bind[5].buffer_type = bind[12].buffer_type
    = bind[6].buffer_type = bind[7].buffer_type = bind[10].buffer_type = bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].is_unsigned = bind[10].is_unsigned = 1;

    //string types
    bind[2].buffer_type = bind[3].buffer_type = bind[8].buffer_type = 
    bind[13].buffer_type = MYSQL_TYPE_VARCHAR;

    bind[2].buffer_length = strlen(job->user_id);
    bind[3].buffer_length = strlen(job->app_id);
    bind[8].buffer_length = strlen(job->policy);
    bind[13].buffer_length = strlen(job->user_acc);

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
    bind[13].buffer = (char *)&job->user_acc;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return EAR_SUCCESS;
}

int mysql_retrieve_jobs(MYSQL *connection, char *query, job_t **jobs)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;
    job_t *job_aux = calloc(1, sizeof(job_t));
    job_t *jobs_aux;
    int status = 0;
    int num_jobs;

    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    MYSQL_BIND bind[14];
    memset(bind, 0, sizeof(bind));
    //integer types
    bind[0].buffer_type = bind[4].buffer_type = bind[5].buffer_type = bind[12].buffer_type
    = bind[6].buffer_type = bind[7].buffer_type = bind[10].buffer_type = bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].is_unsigned = bind[10].is_unsigned = 1;

    //string types
    bind[2].buffer_type = bind[3].buffer_type = bind[8].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[2].buffer_length = bind[3].buffer_length = bind[8].buffer_length = 256;

    //double types
    bind[9].buffer_type = MYSQL_TYPE_DOUBLE;

    //varchar types
    bind[13].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[13].buffer_length = 256;


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
    bind[13].buffer = &job_aux->user_acc;

    if (mysql_stmt_bind_result(statement, bind)) return mysql_statement_error(statement);
    
    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);
    
    if (mysql_stmt_store_result(statement)) return mysql_statement_error(statement);
    
    num_jobs = mysql_stmt_num_rows(statement);
    if (num_jobs < 1)
    {
        mysql_stmt_close(statement);
        return EAR_ERROR;
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

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;
    free(job_aux);

    return num_jobs;

}

//returns id of the inserted signature
int mysql_insert_signature(MYSQL *connection, signature_t *sig, char is_learning)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    if (!is_learning)
    {
        if (mysql_stmt_prepare(statement, SIGNATURE_QUERY, strlen(SIGNATURE_QUERY))) return mysql_statement_error(statement);
    }
    else
    {
        if (mysql_stmt_prepare(statement, LEARNING_SIGNATURE_QUERY, strlen(LEARNING_SIGNATURE_QUERY))) return mysql_statement_error(statement);
    }

    MYSQL_BIND bind[21];
    int i = 0;

    //double storage
    for (i = 0; i < 9; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
        bind[i].length = 0;
        bind[i].is_null = 0;
    }

    //unsigned long long storage
    for (i = 9; i < 21; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
        bind[i].length = 0;
        bind[i].is_null = 0;
        bind[i].is_unsigned = 1;
    }

    //unsigned long storage
    // bind[24].buffer_type = bind[25].buffer_type = MYSQL_TYPE_LONGLONG;
    // bind[24].length = bind[25].length = 0;
    // bind[24].is_null = bind[25].is_null = 0;
    // bind[24].is_unsigned = bind[25].is_unsigned = 1;

    //storage variables assignation
    bind[0].buffer = (char *)&sig->DC_power;
    //bind[1].buffer = (char *)&sig->max_DC_power;
    //bind[2].buffer = (char *)&sig->min_DC_power;
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
    //bind[19].buffer = (char *)&sig->L1_misses;
    //bind[20].buffer = (char *)&sig->L2_misses;
    //bind[21].buffer = (char *)&sig->L3_misses;
    bind[17].buffer = (char *)&sig->instructions;
    bind[18].buffer = (char *)&sig->cycles;
    bind[19].buffer = (char *)&sig->avg_f;
    bind[20].buffer = (char *)&sig->def_f;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    int id = mysql_stmt_insert_id(statement);

    my_ulonglong affected_rows = mysql_stmt_affected_rows(statement);

    if (affected_rows != 1) printf("ERROR: inserting signature failed.\n");

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return id;
}

//returns id of the first inserted signature
int mysql_batch_insert_signatures(MYSQL *connection, application_t *sig, char is_learning, int num_sigs)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;
    char *params = ", (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    char *query;

    if (!is_learning)
    {
        query = malloc(strlen(SIGNATURE_QUERY)+num_sigs*strlen(params)+1);
        strcpy(query, SIGNATURE_QUERY);
    }
    else
    {
        query = malloc(strlen(LEARNING_SIGNATURE_QUERY)+num_sigs*strlen(params)+1);
        strcpy(query, LEARNING_JOB_QUERY);
    }

    int i, j;
    for (i = 1; i < num_sigs; i++)
        strcat(query, params);

    
    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);


    MYSQL_BIND *bind = calloc(num_sigs*SIGNATURE_ARGS, sizeof(MYSQL_BIND));

    for (i = 0; i < num_sigs; i++)
    {
        int offset = i*SIGNATURE_ARGS;
        //double storage
        for (j = 0; i < 9; j++)
        {
            bind[offset+j].buffer_type = MYSQL_TYPE_DOUBLE;
            bind[offset+j].length = 0;
        }

        //unsigned long long storage
        for (j = 9; j < 21; j++)
        {
            bind[offset+j].buffer_type = MYSQL_TYPE_LONGLONG;
            bind[offset+j].length = 0;
            bind[offset+j].is_null = 0;
            bind[offset+j].is_unsigned = 1;
        }


        //storage variables assignation
        bind[0+offset].buffer = (char *)&sig[i].signature.DC_power;
        bind[1+offset].buffer = (char *)&sig[i].signature.DRAM_power;
        bind[2+offset].buffer = (char *)&sig[i].signature.PCK_power;
        bind[3+offset].buffer = (char *)&sig[i].signature.EDP;
        bind[4+offset].buffer = (char *)&sig[i].signature.GBS;
        bind[5+offset].buffer = (char *)&sig[i].signature.TPI;
        bind[6+offset].buffer = (char *)&sig[i].signature.CPI;
        bind[7+offset].buffer = (char *)&sig[i].signature.Gflops;
        bind[8+offset].buffer = (char *)&sig[i].signature.time;
        bind[9+offset].buffer = (char *)&sig[i].signature.FLOPS[0];
        bind[10+offset].buffer = (char *)&sig[i].signature.FLOPS[1];
        bind[11+offset].buffer = (char *)&sig[i].signature.FLOPS[2];
        bind[12+offset].buffer = (char *)&sig[i].signature.FLOPS[3];
        bind[13+offset].buffer = (char *)&sig[i].signature.FLOPS[4];
        bind[14+offset].buffer = (char *)&sig[i].signature.FLOPS[5];
        bind[15+offset].buffer = (char *)&sig[i].signature.FLOPS[6];
        bind[16+offset].buffer = (char *)&sig[i].signature.FLOPS[7];
        bind[17+offset].buffer = (char *)&sig[i].signature.instructions;
        bind[18+offset].buffer = (char *)&sig[i].signature.cycles;
        bind[19+offset].buffer = (char *)&sig[i].signature.avg_f;
        bind[20+offset].buffer = (char *)&sig[i].signature.def_f;

    }


    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    int id = mysql_stmt_insert_id(statement);

    my_ulonglong affected_rows = mysql_stmt_affected_rows(statement);

    if (affected_rows != num_sigs) fprintf(stderr, "ERROR: inserting batch signature failed (affected rows does not match num_sigs).\n");

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    free(bind);
    free(query);

    return id;
}

int mysql_retrieve_signatures(MYSQL *connection, char *query, signature_t **sigs)
{
    signature_t *sig_aux = calloc(1, sizeof(signature_t));
    signature_t *sigs_aux;
    int num_signatures;
    int i = 0;
    int status = 0;

    MYSQL_BIND bind[22];
    memset(bind, 0, sizeof(bind));
    
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;
    
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
    }

    //unsigned long long recievers
    for (i = 10; i < 22; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
        bind[i].buffer_length = 8;
        bind[i].is_unsigned = 1;
    }

    //unsigned long recievers
    // bind[24].buffer_type = bind[25].buffer_type = MYSQL_TYPE_LONGLONG;
    // bind[24].buffer_length = bind[25].buffer_length = 4;
    // bind[24].is_null = bind[25].is_null = 0;
    // bind[24].is_unsigned = bind[25].is_unsigned = 1;

    //reciever variables assignation
    bind[0].buffer = &id;
    bind[1].buffer = &sig_aux->DC_power;
    //bind[2].buffer = &sig_aux->max_DC_power;
    //bind[3].buffer = &sig_aux->min_DC_power;
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
    // bind[20].buffer = &sig_aux->L1_misses;
    // bind[21].buffer = &sig_aux->L2_misses;
    // bind[22].buffer = &sig_aux->L3_misses;
    bind[18].buffer = &sig_aux->instructions;
    bind[19].buffer = &sig_aux->cycles;
    bind[20].buffer = &sig_aux->avg_f;
    bind[21].buffer = &sig_aux->def_f;

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


    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    free(sig_aux);

    return num_signatures;
}

int mysql_insert_power_signature(MYSQL *connection, power_signature_t *pow_sig)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    if (mysql_stmt_prepare(statement, POWER_SIGNATURE_QUERY, strlen(POWER_SIGNATURE_QUERY))) return mysql_statement_error(statement);

    MYSQL_BIND bind[9];
    memset(bind, 0, sizeof(bind));

    //double types
    int i;
    for (i = 0; i < 7; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
    }

    //integer types
    bind[7].buffer_type = bind[8].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[7].is_unsigned = bind[8].is_unsigned = 1;

    //storage variable assignation
    bind[0].buffer = (char *)&pow_sig->DC_power;
    bind[1].buffer = (char *)&pow_sig->DRAM_power;
    bind[2].buffer = (char *)&pow_sig->PCK_power;
    bind[3].buffer = (char *)&pow_sig->EDP;
    bind[4].buffer = (char *)&pow_sig->max_DC_power;
    bind[5].buffer = (char *)&pow_sig->min_DC_power;
    bind[6].buffer = (char *)&pow_sig->time;
    bind[7].buffer = (char *)&pow_sig->avg_f;
    bind[8].buffer = (char *)&pow_sig->def_f;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    int id = mysql_stmt_insert_id(statement);
    
    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return id;
}

int mysql_batch_insert_power_signatures(MYSQL *connection, application_t *pow_sig, int num_sigs)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    char *params = ", (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    char *query = malloc(strlen(POWER_SIGNATURE_QUERY + strlen(params)*num_sigs));
    strcpy(query, POWER_SIGNATURE_QUERY);
    int i, j;
    for (i = 1; i < num_sigs; i++)
        strcat(query, params);

    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    MYSQL_BIND *bind = calloc(num_sigs*POWER_SIGNATURE_ARGS, sizeof(MYSQL_BIND));

    for (i = 0; i < num_sigs;  i++)
    {
        int offset = i*POWER_SIGNATURE_ARGS;
        //double types
        for (j = 0; j < 7; j++)
        {
            bind[j+offset].buffer_type = MYSQL_TYPE_DOUBLE;
        }

        //integer types
        bind[7+offset].buffer_type = bind[8+offset].buffer_type = MYSQL_TYPE_LONGLONG;
        bind[7+offset].is_unsigned = bind[8+offset].is_unsigned = 1;

        //storage variable assignation
        bind[0+offset].buffer = (char *)&pow_sig[i].power_sig.DC_power;
        bind[1+offset].buffer = (char *)&pow_sig[i].power_sig.DRAM_power;
        bind[2+offset].buffer = (char *)&pow_sig[i].power_sig.PCK_power;
        bind[3+offset].buffer = (char *)&pow_sig[i].power_sig.EDP;
        bind[4+offset].buffer = (char *)&pow_sig[i].power_sig.max_DC_power;
        bind[5+offset].buffer = (char *)&pow_sig[i].power_sig.min_DC_power;
        bind[6+offset].buffer = (char *)&pow_sig[i].power_sig.time;
        bind[7+offset].buffer = (char *)&pow_sig[i].power_sig.avg_f;
        bind[8+offset].buffer = (char *)&pow_sig[i].power_sig.def_f;
    }

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    int id = mysql_stmt_insert_id(statement);
    
    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    free(bind);
    free(query);

    return id;
}

int mysql_retrieve_power_signatures(MYSQL *connection, char *query, power_signature_t **pow_sigs)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;
    power_signature_t *pow_sig_aux = calloc(1, sizeof(power_signature_t));
    power_signature_t *pow_sigs_aux;
    int status = 0;
    int num_pow_sigs;
    unsigned long id;

    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    MYSQL_BIND bind[10];
    memset(bind, 0, sizeof(bind));

    //double types
    int i;
    for (i = 1; i < 8; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
    }

    //integer types
    bind[0].buffer_type = bind[8].buffer_type = bind[9].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].is_unsigned = bind[8].is_unsigned = bind[9].is_unsigned = 1;

    //storage variable assignation
    bind[0].buffer = &id;
    bind[1].buffer = &pow_sig_aux->DC_power;
    bind[2].buffer = &pow_sig_aux->DRAM_power;
    bind[3].buffer = &pow_sig_aux->PCK_power;
    bind[4].buffer = &pow_sig_aux->EDP;
    bind[5].buffer = &pow_sig_aux->max_DC_power;
    bind[6].buffer = &pow_sig_aux->min_DC_power;
    bind[7].buffer = &pow_sig_aux->time;
    bind[8].buffer = &pow_sig_aux->avg_f;
    bind[9].buffer = &pow_sig_aux->def_f;

    if (mysql_stmt_bind_result(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    if (mysql_stmt_store_result(statement)) return mysql_statement_error(statement);

    num_pow_sigs = mysql_stmt_num_rows(statement);
    if (num_pow_sigs < 1)
    {
        mysql_stmt_close(statement);
        return EAR_ERROR;
    }
    
    pow_sigs_aux = (power_signature_t *) calloc(num_pow_sigs, sizeof(power_signature_t));
    i = 0;

    //fetching and storing of power_signatures
    status = mysql_stmt_fetch(statement);
    while (status == 0 || status == MYSQL_DATA_TRUNCATED)
    {
        copy_power_signature(&pow_sigs_aux[i], pow_sig_aux);
        status = mysql_stmt_fetch(statement);
        i++;
    }

    *pow_sigs = pow_sigs_aux;

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    free(pow_sig_aux);

    return num_pow_sigs;

}

int mysql_insert_periodic_metric(MYSQL *connection, periodic_metric_t *per_met)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    if (mysql_stmt_prepare(statement, PERIODIC_METRIC_QUERY, strlen(PERIODIC_METRIC_QUERY))) return mysql_statement_error(statement);

#if DEMO
    MYSQL_BIND bind[7];
#else
    MYSQL_BIND bind[6];
#endif
    memset(bind, 0, sizeof(bind));

    //integer types
    int i;
    for (i = 0; i < 3; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
        bind[i].is_unsigned = 1;
    }
    bind[4].buffer_type = bind[5].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[4].is_unsigned = bind[5].is_unsigned = 1;

    //varchar types
    bind[3].buffer_type = MYSQL_TYPE_VARCHAR;
    bind[3].buffer_length = strlen(per_met->node_id);

    //storage variable assignation
    bind[0].buffer = (char *)&per_met->start_time;
    bind[1].buffer = (char *)&per_met->end_time;
    bind[2].buffer = (char *)&per_met->DC_energy;
    bind[3].buffer = (char *)&per_met->node_id;
    bind[4].buffer = (char *)&per_met->job_id;
    bind[5].buffer = (char *)&per_met->step_id;
#if DEMO
    bind[6].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[6].is_unsigned = 1;
    bind[6].buffer = (char *)&per_met->avg_f;
#endif

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    int id = mysql_stmt_insert_id(statement);
    
    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return id;
}

int mysql_insert_periodic_aggregation(MYSQL *connection, periodic_aggregation_t *per_agg)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    if (mysql_stmt_prepare(statement, PERIODIC_AGGREGATION_QUERY, strlen(PERIODIC_AGGREGATION_QUERY))) return mysql_statement_error(statement);

    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));

    //integer types
    int i;
    for (i = 0; i < 3; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
    }

    //storage variable assignation
    bind[0].buffer = (char *)&per_agg->DC_energy;
    bind[1].buffer = (char *)&per_agg->start_time;
    bind[2].buffer = (char *)&per_agg->end_time;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    int id = mysql_stmt_insert_id(statement);
    
    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return id;

}

int mysql_batch_insert_periodic_metrics(MYSQL *connection, periodic_metric_t *per_mets, int num_mets)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

#if DEMO
    char *params = ", (?, ?, ?, ?, ?, ?, ?)";
#else
    char *params = ", (?, ?, ?, ?, ?, ?)";
#endif
    char *query = malloc(strlen(PERIODIC_METRIC_QUERY)+(num_mets)*strlen(params)+1);
    strcpy(query, PERIODIC_METRIC_QUERY);

    int i, j;
    for (i = 1; i < num_mets; i++)
    {
        strcat(query, params);
    }


    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement); 
    

    MYSQL_BIND *bind = calloc(num_mets*PERIODIC_METRIC_ARGS, sizeof(MYSQL_BIND));

    for (i = 0; i < num_mets; i++)
    {
        int offset = i*PERIODIC_METRIC_ARGS;
        for (j = 0; j < PERIODIC_METRIC_ARGS; j++)
        {
            bind[offset+j].buffer_type = MYSQL_TYPE_LONGLONG;
            bind[offset+j].is_unsigned = 1;
        }
        bind[offset+3].buffer_type = MYSQL_TYPE_VARCHAR;
        bind[offset+3].buffer_length = strlen(per_mets[i].node_id);

        bind[0+offset].buffer = (char *)&per_mets[i].start_time;
        bind[1+offset].buffer = (char *)&per_mets[i].end_time;
        bind[2+offset].buffer = (char *)&per_mets[i].DC_energy;
        bind[3+offset].buffer = (char *)&per_mets[i].node_id;
        bind[4+offset].buffer = (char *)&per_mets[i].job_id;
        bind[5+offset].buffer = (char *)&per_mets[i].step_id;
#if DEMO
        bind[6+offset].buffer_type = MYSQL_TYPE_LONGLONG;
        bind[6+offset].is_unsigned = 1;
        bind[6+offset].buffer = (char *)&per_mets[i].avg_f;
#endif
    }

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    mysql_stmt_close(statement);
    free(bind);
    free(query);
    return EAR_SUCCESS;
}

int mysql_insert_ear_event(MYSQL *connection, ear_event_t *ear_ev)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    if (mysql_stmt_prepare(statement, EAR_EVENT_QUERY, strlen(EAR_EVENT_QUERY))) return mysql_statement_error(statement);

    MYSQL_BIND bind[5];
    memset(bind, 0, sizeof(bind));

    time_t timestamp = time(NULL);

    //integer types
    int i;
    for (i = 0; i < 5; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
    }

    //storage variable assignation
    bind[0].buffer = (char *)&timestamp;
    bind[1].buffer = (char *)&ear_ev->event;
    bind[2].buffer = (char *)&ear_ev->jid;
    bind[3].buffer = (char *)&ear_ev->step_id;
    bind[4].buffer = (char *)&ear_ev->freq;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    int id = mysql_stmt_insert_id(statement);
    
    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return id;

}

int mysql_batch_insert_ear_events(MYSQL *connection, ear_event_t *ear_ev, int num_evs)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    char *params = ", (?, ?, ?, ?, ?)";
    char *query = malloc(strlen(EAR_EVENT_QUERY)+(num_evs)*strlen(params)+1);
    strcpy(query, EAR_EVENT_QUERY);

    int i, j, offset;
    for (i = 1; i < num_evs; i++)
    {
        strcat(query, params);
    }

    if (mysql_stmt_prepare(statement, query, strlen(query))) return mysql_statement_error(statement);

    MYSQL_BIND *bind = calloc(num_evs*EAR_EVENTS_ARGS, sizeof(MYSQL_BIND));

    time_t timestamp = time(NULL);

    for (i = 0; i < num_evs; i++)
    {
        offset = i*EAR_EVENTS_ARGS;

        for (j = 0; j < EAR_EVENTS_ARGS; j++)
        {
            bind[offset+j].buffer_type = MYSQL_TYPE_LONGLONG;
        }

        //storage variable assignation
        bind[0+offset].buffer = (char *)&timestamp;
        bind[1+offset].buffer = (char *)&ear_ev[i].event;
        bind[2+offset].buffer = (char *)&ear_ev[i].jid;
        bind[3+offset].buffer = (char *)&ear_ev[i].step_id;
        bind[4+offset].buffer = (char *)&ear_ev[i].freq;
    }

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    mysql_stmt_close(statement);
    free(bind);
    free(query);

    return EAR_SUCCESS;

}

int mysql_insert_gm_warning(MYSQL *connection, gm_warning_t *warning)
{
    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement) return EAR_MYSQL_ERROR;

    if (mysql_stmt_prepare(statement, EAR_WARNING_QUERY, strlen(EAR_WARNING_QUERY))) return mysql_statement_error(statement);

    MYSQL_BIND bind[4];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = bind[2].buffer_type = MYSQL_TYPE_DOUBLE;
    bind[1].buffer_type = bind[3].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[1].is_unsigned = 1;

    bind[0].buffer = (char *)&warning->energy_percent;
    bind[1].buffer = (char *)&warning->level;
    bind[2].buffer = (char *)&warning->inc_th;
    bind[3].buffer = (char *)&warning->new_p_state;

    if (mysql_stmt_bind_param(statement, bind)) return mysql_statement_error(statement);

    if (mysql_stmt_execute(statement)) return mysql_statement_error(statement);

    if (mysql_stmt_close(statement)) return EAR_MYSQL_ERROR;

    return EAR_SUCCESS;

}
