#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../states.h"
#include "application.h"

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

int append_application_text_file(char *path, application_t *app)
{
    static char *HEADER = "USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;" \
        "GBS;GFLOPS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;POLICY;POLICY_TH";
    application_t *a;
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

    a = app;
    ret = dprintf(fd, "%s;%s;%s;%s;%lu;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;%s;%.3lf\n",
           a->user_id, a->job_id, a->node_id, a->app_id, a->avg_f, a->iter_time, a->CPI,
           a->TPI, a->GBS, a->Gflops, a->DC_power, a->DRAM_power, a->PCK_power,
           a->def_f, a->policy, a->policy_th);
    close(fd);

    if (ret < 0) return EAR_ERROR;
    return ret;
}

int read_summary_file(char *path, application_t **apps)
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
          a->user_id, a->job_id, a->node_id, a->app_id, &a->avg_f, &a->iter_time, &a->CPI, &a->TPI, \
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