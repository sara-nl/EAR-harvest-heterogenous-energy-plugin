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

/*
typedef struct ear_event{
	job_id jid,step_id;
	uint event;
	ulong freq;
}ear_event_t;

#define ENERGY_POLICY_NEW_FREQ	0
#define GLOBAL_ENERGY_POLICY	1
#define ENERGY_POLICY_FAILS		2
*/

#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <common/config.h>
#include <common/types/log.h>
#include <common/ear_verbose.h>
#include <daemon/eard_api.h>

#define LOG_FILE 1

const char *__NAME__ = "LOG";

static int fd_log=-1;
static char my_log_buffer[1024];
static char log_name[128];
static char log_nodename[GENERIC_NAME];

void init_log()
{
#if LOG_FILE
#if DB_FILES
	mode_t my_mask;
	time_t curr_time;
    struct tm *current_t;
	char nodename[GENERIC_NAME];
    char s[64];
	if (fd_log>=0) return;
	time(&curr_time);
	my_mask=umask(0);	
	gethostname(nodename, sizeof(nodename));
	strtok(nodename, ".");
	sprintf(log_name,"EAR.%s.log",nodename);
	VERBOSE_N(2, "creating %s log file", log_name);
	fd_log=open(log_name,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd_log<0){
		VERBOSE_N(0, "ERROR while creating EAR log file %s (%s)", log_name, strerror(errno));
	}
	umask(my_mask);
    current_t=localtime(&curr_time);
    strftime(s, sizeof(s), "%c", current_t);
	sprintf(my_log_buffer,"----------------------	EAR log created %s ------------------\n",s);
	write(fd_log,my_log_buffer,strlen(my_log_buffer));
#endif
#if DB_MYSQL
	gethostname(log_nodename, sizeof(log_nodename));
	strtok(log_nodename, ".");
#endif
#endif
}
void end_log()
{
	time_t curr_time;
    struct tm *current_t;
    char s[64];
#if LOG_FILE
#if DB_FILES
	if (fd_log<0) return;
	time(&curr_time);
    current_t=localtime(&curr_time);
    strftime(s, sizeof(s), "%c", current_t);
	sprintf(my_log_buffer,"----------------------	EAR log closed %s ------------------\n",s);
	write(fd_log,my_log_buffer,strlen(my_log_buffer));
	close(fd_log);
#endif
#endif
}
void report_new_event(ear_event_t *event)
{
#if LOG_FILE
    time_t curr_time;
    struct tm *current_t;
    char s[64];
#if DB_FILES
    if (fd_log<0) return;
    time(&curr_time);
    current_t=localtime(&curr_time);
    strftime(s, sizeof(s), "%c", current_t);
	switch(event->event){
		case ENERGY_POLICY_NEW_FREQ:
    		sprintf(my_log_buffer,"%s : job_id %u --> Frequency changed to %lu because of energy policy\n",s,event->jid,event->freq);
			break;
		case GLOBAL_ENERGY_POLICY:
    		sprintf(my_log_buffer,"%s : job_id %u --> Frequency changed to %lu because of global Energy Budget\n",s,event->jid,event->freq);
			break;
		case ENERGY_POLICY_FAILS:
    		sprintf(my_log_buffer,"%s : job_id %u --> Frequency changed to %lu because of policy projections failure\n",s,event->jid,event->freq);
			break;
		case DYNAIS_OFF:
    		sprintf(my_log_buffer,"%s : job_id %u --> DynAIS off because of overhead\n",s,event->jid);
			break;
	}

    write(fd_log,my_log_buffer,strlen(my_log_buffer));
#endif
#if DB_MYSQL
	/* we request the daemon to write the event in the DB */
	event->timestamp=time(NULL);
	strcpy(event->node_id,log_nodename);
	eards_write_event(event);
	//db_insert_ear_event(event);
#endif
#endif
}

void log_report_new_freq(job_id job,job_id step_id,ulong newf)
{
#if LOG_FILE
    ear_event_t new_event;
    new_event.event=ENERGY_POLICY_NEW_FREQ;
    new_event.jid=job;
    new_event.step_id=step_id;
    new_event.freq=newf ;
    report_new_event(&new_event);
#endif
}


void log_report_dynais_off(job_id job,job_id sid)
{
#if LOG_FILE
    ear_event_t new_event;
    new_event.event=DYNAIS_OFF;
    new_event.jid=job;
    new_event.step_id=sid;
    report_new_event(&new_event);
#endif
}



void log_report_max_tries(job_id job,job_id sid,ulong newf)
{
#if LOG_FILE
    ear_event_t new_event;
    new_event.event=ENERGY_POLICY_FAILS;
    new_event.jid=job;
    new_event.step_id=sid;
    new_event.freq=newf;
    report_new_event(&new_event);
#endif
}

void log_report_global_policy_freq(job_id job,job_id sid,ulong newf)
{
#if LOG_FILE
    ear_event_t new_event;
    new_event.event=GLOBAL_ENERGY_POLICY;
    new_event.freq=newf;
    new_event.jid=job;
	new_event.step_id=sid;
    report_new_event(&new_event);
#endif
}

