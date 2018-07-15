/********************************************************
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

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

static int print_application_fd(int fd, application_t *app, int new_line)
{
	print_job_fd(fd, &app->job);
	dprintf(fd, ";");
	print_signature_fd(fd, &app->signature);

	if (new_line) {
		dprintf(fd, "\n");
	}

	return EAR_SUCCESS;
}

int append_application_text_file(char *path, application_t *app)
{
    //lacking: NODENAME(node_id in loop_t), not linked to any loop
	static char *HEADER = "USERNAME;JOB_ID;APPNAME;POLICY;POLICY_TH;AVG.FREQ;DEF.FREQ;TIME;"\
    "CPI;TPI;GBS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;CYCLES;INSTRUCTIONS;L1_MISSES;"     \
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

    //print_job_fd(fd, &app->job);
    //print_signature_fd(fd, &app->signature);
    print_application_fd(fd, app, 1);

    close(fd);

    if (ret < 0) return EAR_ERROR;
    return EAR_SUCCESS;
}

void report_application_data(application_t *app)
{
	float avg_f = ((double) app->signature.avg_f) / 1000000.0;
	float def_f = ((double) app->job.def_f) / 1000000.0;
	float pavg_f = ((double) app->power_sig.avg_f) / 1000000.0;
	float pdef_f = ((double) app->power_sig.def_f) / 1000000.0;
	char st[64],et[64];
	char stmpi[64],etmpi[64];
	struct tm *tmp;
	tmp=localtime(&app->job.start_time);
	strftime(st, sizeof(st), "%c", tmp);
	tmp=localtime(&app->job.end_time);
    strftime(et, sizeof(et), "%c", tmp);
	tmp=localtime(&app->job.start_mpi_time);
	strftime(stmpi, sizeof(stmpi), "%c", tmp);
	tmp=localtime(&app->job.end_mpi_time);
    strftime(etmpi, sizeof(etmpi), "%c", tmp);


	fprintf(stderr,"----------------------------------- Application Summary[%s] --\n",app->node_id);
	fprintf(stderr,"-- App id: %s, user id: %s, job id: %lu", app->job.app_id, app->job.user_id, app->job.id);
	fprintf(stderr,"   procs: %lu (s) acc %s\n", app->job.procs,app->job.user_acc);
	fprintf(stderr,"   start time %s end time %s start mpi %s end mpi %s\n",st,et,stmpi,etmpi);
	fprintf(stderr,"-- power_sig: E. time: %0.3lf (s), nom freq: %0.3f (MHz), avg freq: %0.3f (MHz), ", app->power_sig.time, pdef_f, pavg_f);
	fprintf(stderr,"DC/DRAM/PCK power: %0.3lf/%0.3lf/%0.3lf (W)\n", app->power_sig.DC_power, app->power_sig.DRAM_power, 
                                                               	app->power_sig.PCK_power);
	fprintf(stderr,"\tmax_DC_power/min_DC_power: %0.3lf/%0.3lf (W)\n",app->power_sig.max_DC_power,app->power_sig.min_DC_power);
	if (app->is_mpi){

		fprintf(stderr,"-- mpi_sig: E. time: %0.3lf (s), nom freq: %0.3f (MHz), avg freq: %0.3f (MHz), ", app->signature.time, def_f, avg_f);
		fprintf(stderr,"   procs: %lu (s)\n", app->job.procs);
		fprintf(stderr,"\tCPI/TPI: %0.3lf/%0.3lf, GB/s: %0.3lf, GFLOPS: %0.3lf, ", app->signature.CPI, app->signature.TPI,
                                                                        	app->signature.GBS, app->signature.Gflops);
		fprintf(stderr,"  DC/DRAM/PCK power: %0.3lf/%0.3lf/%0.3lf (W)\n", app->signature.DC_power, app->signature.DRAM_power, 
                                                               	app->signature.PCK_power);
	}
	fprintf(stderr,"-----------------------------------------------------------------------------------------------\n");
}

void report_mpi_application_data(application_t *app)
{
    float avg_f = ((double) app->signature.avg_f) / 1000000.0;
    float def_f = ((double) app->job.def_f) / 1000000.0;
    float pavg_f = ((double) app->power_sig.avg_f) / 1000000.0;
    float pdef_f = ((double) app->power_sig.def_f) / 1000000.0;

    fprintf(stderr,"---------------------------------------------- Application Summary [%s] --\n",app->node_id);
    fprintf(stderr,"-- App id: %s, user id: %s, job id: %lu", app->job.app_id, app->job.user_id, app->job.id);
    fprintf(stderr,"   acc %s\n", app->job.user_acc);
    if (app->is_mpi){

        fprintf(stderr,"-- mpi_sig: E. time: %0.3lf (s), nom freq: %0.3f (MHz), avg freq: %0.3f (MHz), ", app->signature.time, def_f, avg_f);
        fprintf(stderr,"   procs: %lu (s)\n", app->job.procs);
        fprintf(stderr,"\tCPI/TPI: %0.3lf/%0.3lf, GB/s: %0.3lf, GFLOPS: %0.3lf, ", app->signature.CPI, app->signature.TPI,
                                                                            app->signature.GBS, app->signature.Gflops);
        fprintf(stderr,"  DC/DRAM/PCK power: %0.3lf/%0.3lf/%0.3lf (W) GFlops/Watts %.3lf\n", app->signature.DC_power, app->signature.DRAM_power,
                                                                app->signature.PCK_power,app->signature.Gflops/app->signature.DC_power);
    }
    fprintf(stderr,"-----------------------------------------------------------------------------------------------\n");
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
	ret = fscanf(fd, "%[^;];%lu;%[^;];" \
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
