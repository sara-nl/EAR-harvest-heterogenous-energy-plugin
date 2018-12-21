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
#include <common/output/verbose.h>
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
#define TRA_DYN		60014
#define TRA_STA		60015
#define TRA_MOD		60016
#define TRA_VPI		60017
#define TRA_REC		60018

static const char *__NAME__ = "EARL";
extern char *__HOST__ ;


static char buffer1[SZ_BUFF_BIG];
static char buffer2[SZ_BUFF_BIG];

static long long time_sta;
static long long time_end;

static int edit_time_header;
static int edit_time_states;
static int file_prv;
static int file_pcf;
static int file_row;
static int enabled;
static int working;


static int my_trace_rank=0;
static int my_num_nodes=0;
static char my_app[GENERIC_NAME];
static char  hostname[256];
static char *pathname;

#define PARAVER_EVENTS  400 
#define EVENTS_IN_BUFFER 200 
typedef struct paraver_events{
  long long t;
  int event; 
  ullong value;
}paraver_events_t;
static paraver_events_t events_list[PARAVER_EVENTS];
static int num_events=0;


#define TRA_VER	2

static void row_file_create(char *pathname, char *hostname)
{
	sprintf(buffer1, "%s/%d_%s.%d.row", pathname, my_trace_rank,my_app,getpid());
	if (my_trace_rank==1){
		file_row=open(buffer1,
               O_WRONLY | O_CREAT | O_TRUNC,
               S_IRUSR  | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (file_row<0) return;
		sprintf(buffer1,"LEVEL TASK SIZE %d\n",my_num_nodes);
		write(file_row, buffer1, strlen(buffer1));
	}else{
		file_row=open(buffer1,
               O_WRONLY | O_CREAT | O_APPEND,
               S_IRUSR  | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (file_row<0) return;
	}
	sprintf(buffer1,"%s %d\n",hostname,my_trace_rank);
	write(file_row, buffer1, strlen(buffer1));
	close(file_row);
	

}

static void config_file_create(char *pathname, char* hostname)
{
	//
	if (my_trace_rank>1) return;
	sprintf(buffer1, "%s/%d_%s.%d.pcf", pathname, my_trace_rank,my_app,getpid());

	//
	file_pcf = open(buffer1,
			   O_WRONLY | O_CREAT | O_TRUNC,
			   S_IRUSR  | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (file_pcf<0) return;

	sprintf(buffer1, "GRADIENT_COLOR\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "0\t{255,255,255}\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "GRADIENT_NAMES\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "0\twhite\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t1\tRUN\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60001\tPERIOD_ID\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60002\tPERIOD_LENGTH\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60003\tPERIOD_ITERATIONS\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60004\tPERIOD_TIME\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60005\tPERIOD_CPI\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60006\tPERIOD_TPI\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60007\tPERIOD_GBS\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60008\tPERIOD_POWER\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60009\tPERIOD_TIME_PROJECTION\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60010\tPERIOD_CPI_PROJECTION\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60011\tPERIOD_POWER_PROJECTION\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60012\tFREQUENCY\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60013\tENERGY\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60014\tDYNAIS_ON_OFF\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60015\tEARL_STATE\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "VALUES\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1,"2\tEVALUATING_POLICY\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1,"3\tVALIDATING_POLICY\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1,"4\tPOLICY_ERROR\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60016\tEARL_MODE\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "VALUES\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1,"1\tDYNAIS\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1,"2\tPERIODIC\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60017\tVPI\n\n");
	write(file_pcf, buffer1, strlen(buffer1));
	sprintf(buffer1, "EVENT_TYPE\n0\t60018\tRECONFIGURATION\n\n");
	write(file_pcf, buffer1, strlen(buffer1));



	close(file_pcf);
}

static void trace_file_open(char *pathname, char *hostname)
{
	//
	sprintf(buffer1, "%s/%d_%s.%d.prv", pathname, my_trace_rank,my_app,getpid());
	verbose(TRA_VER,"Generating trace file %s\n",buffer1);

	//
	file_prv = open(buffer1,
		O_WRONLY | O_CREAT | O_TRUNC,
		S_IRUSR  | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	//printf("FD: %s %d %s %s %s\n", buffer1, file_prv, strerror(errno), pathname, hostname);
}

static void trace_file_init(int n_nodes)
{
	char *buffer = buffer1;
	char *b;
	int i, j;
   

	time_t curr_time;
    	struct tm *current_t;
    	char s[256];


	if (!enabled) {
		return;
	}

	time(&curr_time);
    	current_t = localtime(&curr_time);
	strftime(s, 256, "%d/%m/%Y at %H:%M", current_t);

	// Buffer position 0
	// Trace file header only generated by master_rank 0
	if (my_trace_rank==1){	
	i = sprintf(buffer, "#Paraver (%s):%020llu:%d(",s,
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
	}
	// 1:cpu:app:task:thread:b_time:e_time:state
	i = sprintf(buffer, "1:%d:1:%d:1:0:", my_trace_rank, my_trace_rank);
	write(file_prv, buffer, strlen(buffer));

	//
	edit_time_states = lseek(file_prv, 0, SEEK_CUR);

	//
	sprintf(buffer, "%020llu:1\n", (unsigned long long) 0);
	write(file_prv, buffer, strlen(buffer));
}

static void trace_file_write_in_file()
{
  int i;
  int events,pendings=num_events,ready=0;
  while(pendings>0){
    if (pendings<EVENTS_IN_BUFFER) events=pendings;
    else events=EVENTS_IN_BUFFER;
    sprintf(buffer2,"");
    for (i=0;i<events;i++){
      sprintf(buffer1,"2:%d:1:%d:1:%llu:%d:%llu\n", my_trace_rank, my_trace_rank,
      events_list[ready+i].t,events_list[ready+i].event,events_list[ready+i].value);
      strcat(buffer2,buffer1);
    }
    write(file_prv, buffer2, strlen(buffer2));
    ready+=events;
    pendings-=events;
  }
  num_events=0;
}

static void trace_file_write(int event, ullong value)
{
  if (num_events==PARAVER_EVENTS){
    trace_file_write_in_file();
  }
  long long my_time = metrics_usecs_diff(PAPI_get_real_usec(), time_sta);
  events_list[num_events].t=my_time;
  events_list[num_events].event=event;
  events_list[num_events].value=value;
  num_events++;

}
static void trace_file_write_simple_event(int event)
{
  if (num_events==PARAVER_EVENTS-2){
    trace_file_write_in_file();
  }
  long long my_time = metrics_usecs_diff(PAPI_get_real_usec(), time_sta);
  events_list[num_events].t=my_time;
  events_list[num_events].event=event;
  events_list[num_events].value=1;
  num_events++;
  events_list[num_events].t=my_time+10;
  events_list[num_events].event=event;
  events_list[num_events].value=0;
  num_events++;

}

/*
 *
 *
 * ear_api.c
 *
 *
 */

void traces_start()
{
	verbose(TRA_VER,"traces start");
	working = 1;
}

void traces_stop()
{
	verbose(TRA_VER,"traces stop");
	working = 0;
}

void traces_init(char *app,int global_rank, int local_rank, int nodes, int mpis, int ppn)
{
	pathname = getenv("EAR_TRACE_PATH");

	//
	file_prv = -1;
	file_pcf = -1;
	file_row = -1;

	if (pathname == NULL) {
		enabled = 0;
		return;
	}
	my_trace_rank = global_rank+1;
	strcpy(my_app,app);

	//
	time_sta = PAPI_get_real_usec();

	//
	gethostname(hostname, SZ_BUFF_BIG);
	strtok(hostname,".");

	//
	trace_file_open(pathname, hostname);
	enabled = (file_prv >= 0);

	//
	my_num_nodes=nodes;
	trace_file_init(nodes);

	if(!enabled) {
		return;
	}

	//
	config_file_create(pathname, hostname);
	if (my_trace_rank==1) row_file_create(pathname, hostname);
}

// ear_api.c
void traces_end(int global_rank, int local_rank, unsigned long total_energy)
{
	//
	time_end = metrics_usecs_diff(PAPI_get_real_usec(), time_sta);

	//
	trace_file_write(TRA_ENE, total_energy);	
	trace_file_write_in_file();

	// Post process
	sprintf(buffer1, "%020llu", time_end);
	
	if (my_trace_rank==1){
	lseek(file_prv, edit_time_header, SEEK_SET);
	write(file_prv, buffer1, 20);
	}

	lseek(file_prv, edit_time_states, SEEK_SET);
	write(file_prv, buffer1, 20);

	//
	close(file_prv);


	if (my_trace_rank>1) row_file_create(pathname, hostname);

	//
	enabled = 0;
	working = 0;
}

// ear_states.c
void traces_new_period(int global_rank, int local_rank, ullong loop_id)
{
}

// ear_api.c
void traces_new_n_iter(int global_rank, int local_rank, ullong loop_id, int loop_size, int iterations)
{
	#if 0
	if (!enabled || !working) {
		return;
	}

	trace_file_write(TRA_ID, (ullong) loop_id);
	trace_file_write(TRA_LEN, (ullong) loop_size);
	trace_file_write(TRA_ITS, (ullong) iterations);
	#endif
}

// ear_api.c
void traces_end_period(int global_rank, int local_rank)
{
	if (!enabled || !working) {
		return;
	}

	trace_file_write(TRA_ID, 0ll);
	trace_file_write(TRA_LEN, 0ll);
	trace_file_write(TRA_ITS, 0ll);
}

// ear_states.c
void traces_new_signature(int global_rank, int local_rank, double seconds,
	double cpi, double tpi, double gbs, double power, double vpi)
{
	ullong lsec;
	ullong lcpi;
	ullong ltpi;
	ullong lgbs;
	ullong lpow;
    ullong lvpi;

	verbose(TRA_VER,"traces new signature");
	if (!enabled || !working) {
		return;
	}

    lsec = (ullong) (seconds * 1000000.0);
    lcpi = (ullong) (cpi * 1000.0);
    ltpi = (ullong) (tpi * 1000.0);
    lgbs = (ullong) (gbs * 1000.0);
    lpow = (ullong) (power);
    lvpi = (ullong) (vpi * 1000.0);

	trace_file_write(TRA_TIM, lsec);
	trace_file_write(TRA_CPI, lcpi);
	trace_file_write(TRA_TPI, ltpi);
	trace_file_write(TRA_GBS, lgbs);
	trace_file_write(TRA_POW, lpow);
	trace_file_write(TRA_VPI, lvpi);
}

// ear_states.c
void traces_PP(int global_rank, int local_rank, double seconds, double power)
{
	ullong lpsec;
    ullong lpcpi;
    ullong lppow;

	if (!enabled || !working) {
		return;
	}

	lpsec = (ullong) (seconds * 1000000.0);
    lppow = (ullong) (power);

	trace_file_write(TRA_PTI, lpsec);
	trace_file_write(TRA_PPO, lppow);
}

// ear_api.c, ear_states.c
void traces_frequency(int global_rank, int local_rank, unsigned long f)
{
	if (!enabled || !working) {
		return;
	}
	verbose(TRA_VER,"traces frequency");

	trace_file_write(TRA_FRQ, f);
}

// ear_api.c
void traces_mpi_call(int global_rank, int local_rank, ulong time, ulong ev, ulong a1, ulong a2, ulong a3)
{
}


void traces_policy_state(int global_rank, int local_rank, int state)
{
    if (!enabled || !working) {
        return;
	}
	trace_file_write(TRA_STA,state);
}
void traces_dynais(int global_rank, int local_rank, int state)
{
    if (!enabled || !working) {
        return;
	}
}
void traces_earl_mode_dynais(int global_rank, int local_rank)
{
    if (!enabled || !working) {
        return;
	}
}
void traces_earl_mode_periodic(int global_rank, int local_rank)
{
    if (!enabled || !working) {
        return;
	}
}

void traces_reconfiguration(int global_rank, int local_rank)
{
    if (!enabled || !working) {
        return;
    }

    trace_file_write_simple_event(TRA_REC);
}

int traces_are_on()
{
	return enabled;
}

#endif
