#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/types/application.h>
#include <common/string_enhanced.h>
#include <common/states.h>

#define PERMISSION S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define OPTIONS O_WRONLY | O_CREAT | O_TRUNC | O_APPEND

void copy_application(application_t *destiny, application_t *source)
{
	memcpy(destiny, source, sizeof(application_t));
}

static int print_loop_fd(int fd, loop_t *loop)
{
	dprintf(fd, "%lu;%u;%u;%u;", first_event, a->level, a->iterations, a->size);

	return EAR_SUCCESS;
}

static int print_application_fd(int fd, application_t *app, int new_line)
{
	application_t *a;
	int i;

	a = app;
	dprintf(fd, "%s;%s;%s;%s;", a->user_id, a->job_id, a->node_id, a->app_id);
	dprintf(fd, "%u;%u;", a->avg_f, a->def_f);
	dprintf(fd, "%lf;%lf;%lf;%lf;", a->time, a->CPI, a->TPI, a->GBS);
	dprintf(fd, "%lf;%lf;%lf;", a->DC_power, a->DRAM_power, a->PCK_power);
	dprintf(fd, "%s;%.3lf;", a->policy, a->policy_th);
	dprintf(fd, "%llu;%llu;", a->cycles, a->instructions);
	dprintf(fd, "%llu;%llu;%llu;", a->L1_misses, a->L2_misses, a->L3_misses);
	dprintf(fd, "%lf;%llu", a->Gflops, app->FLOPS[0]);

	for (i = 1; i < FLOPS_EVENTS; ++i) {
		dprintf(fd, ";%llu", app->FLOPS[i]);
	}

	if (new_line) {
		dprintf(fd, "\n");
	}

	return EAR_SUCCESS;
}

int append_loop_text_file(char *path, loop_t *loop)
{
	static char *HEADER = "USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;DEF.FREQ;TIME;CPI;TPI;GBS;" \
        "DC-NODE-POWER;DRAM-POWER;PCK-POWER;POLICY;POLICY_TH;CYCLES;INSTRUCTIONS;L1_MISSES;"     \
        "L2_MISSES;L3_MISSES;GFLOPS;SP_SINGLE;SP_128;SP_256;SP_512;DP_SINGLE;DP_128;DP_256;"     \
        "DP_512;FIRST_EVENT;ITERATIONS;LEVEL;SIZE";
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

	print_application_fd(fd, loop->signature, 0);
	print_loop_fd(fd, app);
	close(fd);

	if (ret < 0) return EAR_ERROR;
	return EAR_SUCCESS;
}

void init_application(application_t *app)
{
    memset(app, 0, sizeof(application_t));
}

void report_application_data(application_t *app)
{
	float avg_f = ((double) app->avg_f) / 1000000.0;
	float def_f = ((double) app->def_f) / 1000000.0;

	printf("------------------------------------------------------------------------ Application Summary --\n");
	printf("-- App id: %s, node id: %s, user id: %s, job id: %s\n", app->app_id, app->node_id, app->user_id, app->job_id);
	printf("-- E. time: %0.3lf (s), nom freq: %0.3f (MHz), avg freq: %0.3f (MHz), ", app->time, def_f, avg_f);
	printf(   "procs: %u (s)\n", app->procs);
	printf("-- CPI/TPI: %0.3lf/%0.3lf, GB/s: %0.3lf, GFLOPS: %0.3lf, ", app->CPI, app->TPI, app->GBS, app->Gflops);
	printf(   "DC/DRAM/PCK power: %0.3lf/%0.3lf/%0.3lf (W)\n", app->DC_power, app->DRAM_power, app->PCK_power);
	printf("-----------------------------------------------------------------------------------------------\n");
}

int append_application_binary_file(char *path, application_t *app)
{
    int fd, ret;

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

int print_application(application_t *app)
{
    return print_application_fd(STDOUT_FILENO, app, 1);
}

int append_application_text_file(char *path, application_t *app)
{
	static char *HEADER = "USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;DEF.FREQ;TIME;CPI;TPI;GBS;" \
        "DC-NODE-POWER;DRAM-POWER;PCK-POWER;POLICY;POLICY_TH;CYCLES;INSTRUCTIONS;L1_MISSES;"     \
        "L2_MISSES;L3_MISSES;GFLOPS;SP_SINGLE;SP_128;SP_256;SP_512;DP_SINGLE;DP_128;DP_256;DP_512";
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

    print_application_fd(fd, app, 1);
    close(fd);

    if (ret < 0) return EAR_ERROR;
    return EAR_SUCCESS;
}

int read_application_binary_file(char *path, application_t **apps)
{
    application_t *apps_aux, *a;
    int fd, lines, i;

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

int scan_application_fd(FILE *fd, application_t *app)
{
	application_t *a;
	int ret;

	a = app;
	ret = fscanf(fd, "%[^;];%[^;];%[^;];%[^;];" \
			 "%u;%u;" \
			 "%lf;%lf;%lf;%lf;" \
			 "%lf;%lf;%lf;" \
			 "%[^;];%lf;" \
			 "%llu;%llu;" \
			 "%llu;%llu;%llu;" \
			 "%lf;%llu;%llu;%llu;%llu;"	\
			 "%llu;%llu;%llu;%llu\n",
			 a->user_id, a->job_id, a->node_id, a->app_id,
			 &a->avg_f, &a->def_f,
			 &a->time, &a->CPI, &a->TPI, &a->GBS,
			 &a->DC_power, &a->DRAM_power, &a->PCK_power,
			 a->policy, &a->policy_th,
			 &a->cycles, &a->instructions,
			 &a->L1_misses, &a->L2_misses, &a->L3_misses,
			 &a->Gflops, &a->FLOPS[0], &a->FLOPS[1], &a->FLOPS[2], &a->FLOPS[3],
			 &a->FLOPS[4], &a->FLOPS[5], &a->FLOPS[6], &a->FLOPS[7]);

	return ret;
}

int read_application_text_file(char *path, application_t **apps)
{
    char line[PIPE_BUF];
    application_t *apps_aux, *a;
    int lines, i;
    FILE *fd;

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

    while(scan_application_fd(fd, a) > 0)
    {
        i += 1;
        a = &apps_aux[i];
    }

    fclose(fd);

    *apps = apps_aux;
    return i;
}
