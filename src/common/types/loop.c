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

#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <common/types/loop.h>
#include <common/states.h>
#include <library/common/externs.h>


#define PERMISSION S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define OPTIONS O_WRONLY | O_CREAT | O_TRUNC | O_APPEND


loop_id_t *create_loop_id(ulong event, ulong size, ulong level)
{
    loop_id_t *loop_id = malloc(sizeof(loop_id_t));
    loop_id->event = event;
    loop_id->size = size;
    loop_id->level = level;

    return loop_id;
}

loop_t *create_loop(loop_id_t loop_id)
{
    loop_t *loop = calloc(1, sizeof(loop));
    loop->id = loop_id;
    return loop;
}

void loop_init(loop_t *loop)
{
    memset(loop, 0, sizeof(loop_t));
    loop->job = &application.job;
}


void add_loop_signature(loop_t *loop,  signature_t *sig)
{
    loop->signature = *sig;
}

void end_loop(loop_t *loop, ulong iterations)
{
    loop->total_iterations = iterations;
}

void copy_loop(loop_t *destiny, loop_t *source)
{
    memcpy(destiny, source, sizeof(loop_t));
}

void print_loop_id_fd(int fd, loop_id_t *loop_id)
{
    dprintf(fd, "%lu;%lu;%lu;", loop_id->event, loop_id->level, loop_id->size);
}

void print_loop_fd(int fd, loop_t *loop)
{
    //tbd
}

int append_loop_text_file(char *path, loop_t *loop)
{
    if (path == NULL) {
        return EAR_ERROR;
    }

    char *HEADER = "NODENAME;USERNAME;JOB_ID;APPNAME;POLICY;POLICY_TH;AVG.FREQ;DEF.FREQ;" \
        "TIME;CPI;TPI;GBS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;CYCLES;INSTRUCTIONS;L1_MISSES;" \
        "L2_MISSES;L3_MISSES;GFLOPS;SP_SINGLE;SP_128;SP_256;SP_512;DP_SINGLE;DP_128;DP_256;" \
        "DP_512;FIRST_EVENT;LEVEL;SIZE;ITERATIONS";
	
    int fd, ret;

    fd = open(path, O_WRONLY | O_APPEND);

    if (fd < 0) 
    {
		if (errno == ENOENT)
		{
			fd = open(path, OPTIONS, PERMISSION);

			// Write header
			if (fd >= 0) {
				ret = dprintf(fd, "%s\n", HEADER);
			}
		}
	}

	if (fd < 0) {
		return EAR_ERROR;
	}

    dprintf(fd, "%s;", loop->node_id);
    print_job_fd(fd, loop->job);
    print_signature_fd(fd, &loop->signature);
    print_loop_id_fd(fd, &loop->id);
    dprintf(fd, "%lu\n", loop->total_iterations);

	close(fd);

	if (ret < 0) return EAR_ERROR;
	return EAR_SUCCESS;
    return 0;
}

#if DB_MYSQL

#include <mysql.h>


#define LOOP_QUERY          "INSERT INTO Loops (event, size, level, job_id, step_id,  node_id, total_iterations, \
                            signature_id) VALUES (?, ?, ?, ?, ?, ?, ? ,?)"


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

#endif
