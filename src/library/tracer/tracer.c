/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <papi.h>

#include <control/frequency.h>
#include <metrics/papi/generics.h>
#include <library/tracer/tracer.h>
#include <library/ear_states/ear_states.h>
#include <library/ear_metrics/ear_metrics.h>
#include <library/common/externs.h>
#include <common/types/generic.h>
#include <common/environment.h>
#include <common/ear_verbose.h>

#ifdef EAR_GUI

#define DYNAIS_TRACE		0
#define APPLICATION_TRACE	1
#define APP_INFO_FILE		"appinfo.txt"
#define ARCH_INFO_FILE		"archinfo.txt"

static const char *__NAME__ = "TRACER";

static char write_buffer[4096];
static long long first_sample;
static long long sample_time;
static long long last_sample;
static long long sample;
static int trace_type = DYNAIS_TRACE;
static int fd_evs_app;
static int fd_evs_dyn;

#define CREATE_FLAGS S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

#define OPEN_FILE(_filename, _fd) 															 \
	if ((_fd = open(_filename, O_WRONLY | O_CREAT | O_TRUNC, CREATE_FLAGS)) < 0) {	         \
		VERBOSE_N(0, "ERROR while creating trace file %s (%s)", _filename, strerror(errno)); \
		exit(1);																			 \
	}

static void write_archinfo_info_file(char *pathname)
{
	char filename[PATH_MAX], vendor[128], model[128];
	const PAPI_hw_info_t *hw_general = NULL;
	unsigned long max_f, min_f, nom_f;
	int sockets, cores_per_socket;
	int fd;

	sprintf(filename, "%s/%s", pathname, ARCH_INFO_FILE);
	OPEN_FILE(filename, fd);

	//
	nom_f = frequency_get_nominal_freq();

	//
	hw_general = metrics_get_hw_info();
	sockets = hw_general->sockets;
	cores_per_socket = hw_general->cores;
	max_f = hw_general->cpu_max_mhz;
	min_f = hw_general->cpu_min_mhz;

	//
	strcpy(vendor, hw_general->vendor_string);
	strcpy(model, hw_general->model_string);

	//
	sprintf(write_buffer, "%d\n%d\n%u\n%u\n%u\n%s\n%s\n",
		sockets, cores_per_socket, max_f * 1000, min_f * 1000, nom_f, vendor, model);

	//
	if (write(fd, write_buffer, strlen(write_buffer)) != strlen(write_buffer)) {
		VERBOSE_N(0, "ERROR while writting trace file %s (%s)", filename, strerror(errno));
		exit(1);
	}

	close(fd);
}

static void write_app_info_file(char *pathname, int nodes, int mpis, int ppn)
{
	char filename[PATH_MAX];
	int fd;

	sprintf(filename, "%s/%s", pathname, APP_INFO_FILE);
	OPEN_FILE(filename, fd);

	//
	sprintf(write_buffer, "%s\n%d\n%d\n%d\n%s\n", application.app_id, nodes, mpis, ppn, application.policy);

	//
	if (write(fd, write_buffer, strlen(write_buffer)) != strlen(write_buffer)) {
		VERBOSE_N(0, "ERROR while writting trace file %s (%s)", filename, strerror(errno));
		exit(1);
	}

	close(fd);
}

static void open_event_file_application(char *pathname, int global_rank)
{
	char filename[PATH_MAX];

	sprintf(filename, "%s/%d.%s.txt", pathname, global_rank, application.node_id);
	OPEN_FILE(filename, fd_evs_app);

	sample_time = 0;

	sprintf(write_buffer,"%llu;%u;%u\n%llu;%u;%u\n%llu;%u;%u\n", sample_time, PERIOD_ID, 0,
		sample_time, PERIOD_LENGTH, 0, sample_time, PERIOD_ITERATIONS, 0);

	write(fd_evs_app, write_buffer, strlen(write_buffer));
	first_sample = PAPI_get_real_usec();
	last_sample = sample_time;
}

static void open_event_file_dynais(char *pathname)
{
	char filename[PATH_MAX];

	sprintf(filename, "%s/%s.dyn", pathname, application.node_id);
	OPEN_FILE(filename, fd_evs_dyn);
}

// ear_api.c
void traces_init(int global_rank, int local_rank, int nodes, int mpis, int ppn)
{
	char *pathname;

	pathname = get_ear_gui_pathname();
	VERBOSE_N(1, "using gui path %s\n", pathname);

	if (global_rank == 0)
	{
		write_app_info_file(pathname, nodes, mpis, ppn);
		write_archinfo_info_file(pathname);
	}

	if (local_rank == 0 && trace_type == APPLICATION_TRACE)
	{
		open_event_file_application(pathname, global_rank);
	}

	if (local_rank == 0 && trace_type == DYNAIS_TRACE)
	{
		open_event_file_dynais(pathname);
	}
}

#define WRITE_TRACE(_fd, format, ...)						\
{ 															\
	sample = PAPI_get_real_usec();							\
	sample_time = metrics_usecs_diff(sample, first_sample);	\
	sprintf(write_buffer, format, __VA_ARGS__);				\
	write(_fd, write_buffer, strlen(write_buffer));			\
	last_sample = sample_time;								\
}

// ear_api.c
void traces_end(int global_rank, int local_rank, unsigned long total_energy)
{
    if (local_rank != 0 || trace_type != APPLICATION_TRACE) {
        return; 
    }

	WRITE_TRACE(fd_evs_app, "%llu;%u;%llu\n", sample_time, APP_ENERGY, total_energy);
}

// ear_states.c
void traces_new_period(int global_rank, int local_rank, int loop_id)
{
}

// ear_api.c
void traces_new_n_iter(int global_rank, int local_rank, int period_id, int loop_size, int iterations)
{
	if (local_rank != 0 || trace_type != APPLICATION_TRACE) {
        return; 
    }

	//
	sample = PAPI_get_real_usec();
	sample_time = metrics_usecs_diff(sample,first_sample);

	// we report the loop after 1 iteration to compute the loop size
	if (iterations == 1 || (sample_time - last_sample) > MIN_FREQ_FOR_SAMPLING)
	{
		sprintf(write_buffer,"%llu;%u;%u\n%llu;%u;%u\n%llu;%u;%u\n",
			sample_time, PERIOD_ID, period_id, sample_time, PERIOD_LENGTH,
			loop_size, sample_time, PERIOD_ITERATIONS, iterations);

		write(fd_evs_app, write_buffer, strlen(write_buffer));
		last_sample = sample_time;
	}

	last_sample = sample_time;
}

// ear_api.c
void traces_end_period(int global_rank, int local_rank)
{
    if (local_rank != 0 || trace_type != APPLICATION_TRACE) {
        return; 
    }

	WRITE_TRACE(fd_evs_app, "%llu;%u;%u\n%llu;%u;%u\n%llu;%u;%u\n", sample_time,PERIOD_ID, 0,
		sample_time, PERIOD_LENGTH,0,sample_time,PERIOD_ITERATIONS, 0);
}

// ear_states.c
void traces_new_signature(int global_rank, int local_rank, double seconds,
	double cpi, double tpi, double gbs, double power)
{
    if (local_rank != 0 || trace_type != APPLICATION_TRACE) {
        return; 
    }

	WRITE_TRACE(fd_evs_app, "%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n",
		sample_time, PERIOD_TIME, seconds*1000000, sample_time, PERIOD_CPI, cpi, sample_time,
		PERIOD_TPI, tpi, sample_time, PERIOD_GBS, gbs, sample_time, PERIOD_POWER, power);
}

// ear_states.c
void traces_PP(int global_rank, int local_rank, double seconds, double cpi, double power)
{
    if (local_rank != 0 || trace_type != APPLICATION_TRACE) {
        return; 
    }

	WRITE_TRACE(fd_evs_app, "%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n",
		sample_time, PERIOD_TIME_PROJ, seconds * 1000000, sample_time,
		PERIOD_CPI_PROJ, cpi, sample_time, PERIOD_POWER_PROJ, power);
}

// ear_api.c, ear_states.c
void traces_frequency(int global_rank, int local_rank, unsigned long f)
{
	if (local_rank != 0 || trace_type != APPLICATION_TRACE) {
		return;
	}

	WRITE_TRACE(fd_evs_app, "%llu;%u;%u\n", sample_time, PERIOD_FREQ, f);
}

// ear_api.c
void traces_mpi_call(int global_rank, int local_rank, ulong time, ulong ev, ulong a1, ulong a2, ulong a3)
{
	ulong trace_data[5];

	if (global_rank != 0 || trace_type != DYNAIS_TRACE) {
		return;
	}
	
	trace_data[0] = (unsigned long) a1;
	trace_data[1] = (unsigned long) a2;
	trace_data[2] = (unsigned long) a3;
	trace_data[3] = (unsigned long) ev;
	trace_data[4] = (unsigned long) time;
	write(fd_evs_dyn, trace_data, sizeof(trace_data));
}
#endif
