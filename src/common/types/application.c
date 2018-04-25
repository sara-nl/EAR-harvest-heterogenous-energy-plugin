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

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <common/states.h>

#include <common/types/application.h>


#define PERMISSION S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define OPTIONS O_WRONLY | O_CREAT | O_TRUNC | O_APPEND

void init_application(application_t *app)
{
    memset(app, 0, sizeof(application_t));
}

void copy_application(application_t *destiny, application_t *source)
{
	memcpy(destiny, source, sizeof(application_t));
}

int append_application_text_file(char *path, application_t *app)
{
    //lacking: NODENAME(node_id in loop_t), not linked to any loop
	static char *HEADER = "USERNAME;JOB_ID;APPNAME;AVG.FREQ;DEF.FREQ;TIME;CPI;TPI;GBS;" \
	"DC-NODE-POWER;DRAM-POWER;PCK-POWER;POLICY;POLICY_TH;CYCLES;INSTRUCTIONS;L1_MISSES;"     \
	"L2_MISSES;L3_MISSES;GFLOPS;SP_SINGLE;SP_128;SP_256;SP_512;DP_SINGLE;DP_128;DP_256;DP_512";
	int fd, ret;

	if (path == NULL) {
		return EAR_ERROR;
	}

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

    print_job_fd(fd, &app->job);
    print_signature_fd(fd, &app->signature);

    close(fd);

    if (ret < 0) return EAR_ERROR;
    return EAR_SUCCESS;
}

void report_application_data(application_t *app)
{
	float avg_f = ((double) app->signature.avg_f) / 1000000.0;
	float def_f = ((double) app->signature.def_f) / 1000000.0;

	printf("------------------------------------------------------------------------ Application Summary --\n");
	printf("-- App id: %s, user id: %s, job id: %u\n", app->job.app_id, app->job.user_id, app->job.id);
	printf("-- E. time: %0.3lf (s), nom freq: %0.3f (MHz), avg freq: %0.3f (MHz), ", app->signature.time, def_f, avg_f);
	printf(   "procs: %lu (s)\n", app->job.procs);
	printf("-- CPI/TPI: %0.3lf/%0.3lf, GB/s: %0.3lf, GFLOPS: %0.3lf, ", app->signature.CPI, app->signature.TPI,
                                                                        app->signature.GBS, app->signature.Gflops);
	printf(   "DC/DRAM/PCK power: %0.3lf/%0.3lf/%0.3lf (W)\n", app->signature.DC_power, app->signature.DRAM_power, 
                                                               app->signature.PCK_power);
	printf("-----------------------------------------------------------------------------------------------\n");
}

int print_application_fd(int fd, application_t *app, int new_line)
{
    print_job_fd(fd, &app->job);
    print_signature_fd(fd, &app->signature);

	if (new_line) {
		dprintf(fd, "\n");
	}

	return EAR_SUCCESS;
}

int print_application(application_t *app)
{
    return print_application_fd(STDOUT_FILENO, app, 1);
}

int scan_application_fd(FILE *fd, application_t *app)
{
    #define APP_TEXT_FILE_FIELDS 28

	application_t *a;
	int ret;

	a = app;
	ret = fscanf(fd, "%[^;];%u;%[^;];" \
			 "%[^;];%lf;" \
			 "%lu;%lu;" \
			 "%lf;%lf;%lf;%lf;" \
			 "%lf;%lf;%lf;" \
			 "%llu;%llu;" \
			 "%llu;%llu;%llu;" \
			 "%lf;%llu;%llu;%llu;%llu;"	\
			 "%llu;%llu;%llu;%llu\n",
			 a->job.user_id, &a->job.id, a->job.app_id,
			 a->job.policy, &a->job.th,
			 &a->signature.avg_f, &a->signature.def_f,
			 &a->signature.time, &a->signature.CPI, &a->signature.TPI, &a->signature.GBS,
			 &a->signature.DC_power, &a->signature.DRAM_power, &a->signature.PCK_power,
			 &a->signature.cycles, &a->signature.instructions,
			 &a->signature.L1_misses, &a->signature.L2_misses, &a->signature.L3_misses,
			 &a->signature.Gflops, &a->signature.FLOPS[0], &a->signature.FLOPS[1], &a->signature.FLOPS[2], &a->signature.FLOPS[3],
			 &a->signature.FLOPS[4], &a->signature.FLOPS[5], &a->signature.FLOPS[6], &a->signature.FLOPS[7]);
	
	return ret;
}

int read_application_text_file(char *path, application_t **apps)
{
	char line[PIPE_BUF];
	application_t *apps_aux, *a;
	int lines, i, ret;
	FILE *fd;

	if (path == NULL) {
		return EAR_ERROR;
	}

    if ((fd = fopen(path, "r")) == NULL) {
        return EAR_FILE_NOT_FOUND;
    }

    // Reading the header
    fscanf(fd, "%s\n", line);
    lines = 0;

    while(fscanf(fd, "%s\n", line) > 0) {
        lines += 1;
    }

    // Allocating memory
    apps_aux = (application_t *) malloc(lines * sizeof(application_t));
    memset(apps_aux, 0, sizeof(application_t));

    // Rewind
    rewind(fd);
    fscanf(fd, "%s\n", line);

    i = 0;
    a = apps_aux;
 
    while((ret = scan_application_fd(fd, a)) == APP_TEXT_FILE_FIELDS)
    {
        i += 1;
        a = &apps_aux[i];
    }

    fclose(fd);

	if (ret >= 0 && ret < APP_TEXT_FILE_FIELDS) {
		free(apps_aux);
		return EAR_ERROR;
	}

    *apps = apps_aux;
    return i;
}

int append_application_binary_file(char *path, application_t *app)
{
	int fd, ret;

	if (path == NULL) {
		return EAR_ERROR;
	}


    fd = open(path, O_WRONLY | O_APPEND);

    if (fd < 0)
    {
        if (errno == ENOENT)
        {
            fd = open(path, OPTIONS, PERMISSION);
        }
    }

    if (fd < 0) {
        return EAR_ERROR;
    }

    ret = write(fd, app, sizeof(application_t));
    close(fd);

    if (ret != sizeof(application_t)) return EAR_ERROR;
    return EAR_SUCCESS;
}

int read_application_binary_file(char *path, application_t **apps)
{
	application_t *apps_aux, *a;
	int fd, lines, i;

	if (path == NULL) {
		return EAR_ERROR;
	}

    if ((fd = open(path, O_RDONLY)) < 0) {
        return EAR_FILE_NOT_FOUND;
    }

    // Getting the number
    lines = (lseek(fd, 0, SEEK_END) / sizeof(application_t));

    // Allocating memory
    apps_aux = (application_t *) malloc(lines * sizeof(application_t));
    memset(apps_aux, 0, sizeof(application_t));

    // Returning to the begining
    lseek(fd, 0, SEEK_SET);
    i = 0;

    while (read(fd, &apps_aux[i], sizeof(application_t)) > 0)
    {
        i += 1;
        a = &apps_aux[i];
    }

    //
    close(fd);

    *apps = apps_aux;
    return i;
}


#if DB_MYSQL

#include <mysql.h>


#define APPLICATION_QUERY   "INSERT INTO Applications (job_id, step_id, node_id, signature_id) VALUES \
                            (?, ?, ?, ?)"


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


#endif