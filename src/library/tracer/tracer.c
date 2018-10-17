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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <common/sizes.h>
#include <common/config.h>
#include <library/tracer/tracer.h>

#ifdef EAR_GUI

#define TRA_ID		60001
#define TRA_LEN		60002
#define TRA_ITS		60003
#define TRA_TIM		60004
#define TRA_CPI		60005
#define TRA_TPI		60006
#define TRA_GBS		60007
#define TRA_POW		60008
#define TRA_PTI		60009
#define TRA_PCP		60010
#define TRA_PPO		60011
#define TRA_FRQ		60012
#define TRA_ENE		60013

static char buffer1[SZ_BUFF_BIG];
static char buffer2[SZ_BUFF_BIG];
static int edit_time_header;
static int edit_time_states;
static long long time_sta;
static long long time_end;
static int file_prv;
static int file_pcf;
static int file_row;
static int r_master;
static int n_nodes;
static int enabled;

/*
static long long metrics_usecs_diff(long long end, long long init)
{
	long long to_max;

	if (end < init)
	{
		to_max = 9223372036854775807LL - init;
		return (to_max + end);
	}

	return (end - init);
}

static long long PAPI_get_real_usec()
{
	return 0;
}
*/

static void config_file_create(char *pathname, char* hostname)
{
	//
	sprintf(buffer1, "%s/%d.pcf", pathname, getpid());

	//
	file_pcf = open(buffer1,
			   O_WRONLY | O_CREAT | O_TRUNC,
			   S_IRUSR  | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	close(file_pcf);
}

static void trace_file_open(char *pathname, char *hostname)
{
	//
	sprintf(buffer1, "%s/%d.prv", pathname, getpid());

	//
	file_prv = open(buffer1,
		O_WRONLY | O_CREAT | O_TRUNC,
		S_IRUSR  | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	//printf("FD: %s %d %s %s %s\n", buffer1, file_prv, strerror(errno), pathname, hostname);
}

static void trace_file_init()
{
	char *buffer = buffer1;
	char *b;
	int i, j;

	if (!enabled) {
		return;
	}

	// Buffer position 0
	i = sprintf(buffer, "#Paraver (01/01/1970 at 00:00):%020llu:%d(",
				(unsigned long long) 0, n_nodes);
	edit_time_header = 31;

	//
	for (b = &buffer[i], i = 0, j = 0; i < n_nodes; ++i, j += 2)
	{
		b[j+0] = '1';
		b[j+1] = ',';
	}

	// Buffer position 2
	b = &b[j-1];
	i = sprintf(b, "):1:%d(", n_nodes);

	// Buffer position 3
	for (b = &b[i], i = 0, j = 1; i < n_nodes; ++i, ++j)
	{
		sprintf(b, "1:%d,", j);
		b = &b[strlen(b)];
	}

	b = &b[strlen(b)-1];
	b[0] = ')';
	b[1] = '\n';
	b[2] = '\0';

	write(file_prv, buffer, strlen(buffer));

	// 1:cpu:app:task:thread:b_time:e_time:state
	i = sprintf(buffer, "1:%d:1:%d:1:0:", 1, 1);
	write(file_prv, buffer, strlen(buffer));

	//
	edit_time_states = lseek(file_prv, 0, SEEK_CUR);

	//
	sprintf(buffer, "%020llu:1\n", (unsigned long long) 0);
	write(file_prv, buffer, strlen(buffer));
}

static void trace_file_write(int event, ullong value)
{
	long long time = metrics_usecs_diff(PAPI_get_real_usec(), time_sta);
	char *buffer = buffer1;

	// 2:cpu:app:task:thread:time:event:value
	sprintf(buffer, "2:%d:1:%d:1:%llu:%d:%llu\n", 1, 1, time, event, value);
	//fprintf(stderr, "%d: %s");
	write(file_prv, buffer, strlen(buffer));
}

/*
 *
 *
 * ear_api.c
 *
 *
 */

void traces_init(int global_rank, int local_rank, int nodes, int mpis, int ppn)
{
	char *pathname = getenv("EAR_TRACE_PATH");
	char  hostname[256];

	//
	file_prv = -1;
	file_pcf = -1;
	file_row = -1;

	if (pathname == NULL) {
		enabled = 0;
		return;
	}

	//
	time_sta = PAPI_get_real_usec();
	r_master = 1;
	n_nodes  = 1;

	//
	gethostname(hostname, SZ_BUFF_BIG);

	//
	trace_file_open(pathname, hostname);
	enabled = (file_prv >= 0);

	//
	trace_file_init();

	//
	config_file_create(pathname, hostname);
	enabled = enabled && (file_pcf >= 0);
}

// ear_api.c
void traces_end(int global_rank, int local_rank, unsigned long total_energy)
{
	//
	time_end = metrics_usecs_diff(PAPI_get_real_usec(), time_sta);
	//time_end = 1000;

	//
	trace_file_write(TRA_ENE, total_energy);

	// Post process
	sprintf(buffer1, "%020llu", time_end);

	lseek(file_prv, edit_time_header, SEEK_SET);
	write(file_prv, buffer1, 20);

	lseek(file_prv, edit_time_states, SEEK_SET);
	write(file_prv, buffer1, 20);

	//
	close(file_prv);
}

// ear_states.c
void traces_new_period(int global_rank, int local_rank, ullong loop_id)
{
	if (!enabled) {
		return;
	}
}

// ear_api.c
void traces_new_n_iter(int global_rank, int local_rank, ullong loop_id, int loop_size, int iterations)
{
	if (!enabled) {
		return;
	}

	trace_file_write(TRA_ID, (ullong) loop_id);
	trace_file_write(TRA_LEN, (ullong) loop_size);
	trace_file_write(TRA_ITS, (ullong) iterations);
}

// ear_api.c
void traces_end_period(int global_rank, int local_rank)
{
	if (!enabled) {
		return;
	}

	trace_file_write(TRA_ID, 0ll);
	trace_file_write(TRA_LEN, 0ll);
	trace_file_write(TRA_ITS, 0ll);
}

// ear_states.c
void traces_new_signature(int global_rank, int local_rank, double seconds,
	double cpi, double tpi, double gbs, double power)
{
	ullong lsec;
	ullong lcpi;
	ullong ltpi;
	ullong lgbs;
	ullong lpow;

	if (!enabled) {
		return;
	}

    lsec = (ullong) (seconds * 1000000.0);
    lcpi = (ullong) (cpi * 1000.0);
    ltpi = (ullong) (tpi * 1000.0);
    lgbs = (ullong) (gbs * 1000.0);
    lpow = (ullong) (power);

	trace_file_write(TRA_TIM, lsec);
	trace_file_write(TRA_CPI, lcpi);
	trace_file_write(TRA_TPI, ltpi);
	trace_file_write(TRA_GBS, lgbs);
	trace_file_write(TRA_POW, lpow);
}

// ear_states.c
void traces_PP(int global_rank, int local_rank, double seconds, double cpi, double power)
{
	ullong lpsec;
    ullong lpcpi;
    ullong lppow;

	if (!enabled) {
		return;
	}

	lpsec = (ullong) (seconds * 1000000.0);
    lpcpi = (ullong) (cpi * 1000.0);
    lppow = (ullong) (power);

	trace_file_write(TRA_PTI, lpsec);
	trace_file_write(TRA_PCP, lpcpi);
	trace_file_write(TRA_PPO, lppow);
}

// ear_api.c, ear_states.c
void traces_frequency(int global_rank, int local_rank, unsigned long f)
{
	if (!enabled) {
		return;
	}

	trace_file_write(TRA_FRQ, f);
}

// ear_api.c
void traces_mpi_call(int global_rank, int local_rank, ulong time, ulong ev, ulong a1, ulong a2, ulong a3)
{
}

/*
int main(int argc, char *argv[])
{
	traces_init(0, 0, 0, 0, 0);
	traces_end_period(0, 0);
	traces_PP(0, 0, 0, 0, 0);
	traces_new_signature(0, 0, 0, 0, 0, 0, 0);
	traces_end(0, 0, 0);

	return 0;
}
*/
#endif
