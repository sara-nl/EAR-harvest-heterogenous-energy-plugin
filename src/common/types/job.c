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