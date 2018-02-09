#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <states.h>
#include <types/application.h>

#define PERMISSION S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define OPTIONS O_WRONLY | O_CREAT | O_TRUNC | O_APPEND

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

static int print_application_fd(int fd, application_t *app)
{
    application_t *a;

    a = app;
    dprintf(fd, "%s;%s;%s;%s;", a->user_id, a->job_id, a->node_id, a->app_id);
    dprintf(fd, "%lu;%lu;", a->avg_f, a->def_f);
    dprintf(fd, "%lf;%lf;%lf;%lf;", a->time, a->CPI, a->TPI, a->GBS);
    dprintf(fd, "%lf;%lf;%lf;", a->DC_power, a->DRAM_power, a->PCK_power);
    dprintf(fd, "%s;%.3lf;", a->policy, a->policy_th);
    dprintf(fd, "%llu;%llu;", a->cycles, a->instructions);
    dprintf(fd, "%llu;%llu;%llu;", a->L1_misses, a->L2_misses, a->L3_misses);
    dprintf(fd, "%lf;%llu;%llu", a->Gflops, (ull) 0,(ull) 0,(ull) 0,(ull) 0);
    dprintf(fd, "\n");

    return EAR_SUCCESS;
}


int print_application(application_t *app)
{
    return print_application_fd(STDOUT_FILENO, app);
}

int append_application_text_file(char *path, application_t *app)
{
	static char *HEADER = "USERNAME;JOB_ID;NODENAME;APPNAME;DEF.FREQ;AVG.FREQ;TIME;CPI;TPI;GBS;" \
        "DC-NODE-POWER;DRAM-POWER;PCK-POWER;POLICY;POLICY_TH;CYCLES;INSTRUCTIONS;L1_MISSES;"     \
        "L2_MISSES;L3_MISSES;GFLOPS;DPSINGLE_OPS;DP128_OPS;DP256_OPS;DP512_OPS";
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

    print_application_fd(fd, app);
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

int read_application_text_file(char *path, application_t **apps)
{
    char line[PIPE_BUF];
    application_t *apps_aux, *a;
    int ret, lines, i;
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

    #define READ_SUMMARY()                                                                          \
    ret = fscanf(fd, "%[^;];%[^;];%[^;];%[^;];%lu;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;%[^;];%lf\n",  \
          a->user_id, a->job_id, a->node_id, a->app_id, &a->avg_f, &a->time, &a->CPI, &a->TPI, \
          &a->GBS, &a->Gflops, &a->DC_power, &a->DRAM_power, &a->PCK_power, &a->def_f, a->policy,   \
          &a->policy_th)

    while((READ_SUMMARY()) > 0)
    {
        i += 1;
        a = &apps_aux[i];
    }

    fclose(fd);

    *apps = apps_aux;
    return i;
}
