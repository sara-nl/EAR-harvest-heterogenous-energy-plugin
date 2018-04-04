/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
/*
typedef struct ear_event{
	uint job_id;
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
#include <common/types/log.h>

static int fd_log=-1;
static char my_log_buffer[1024];
static char log_name[128];
void init_log()
{
	mode_t my_mask;
	time_t curr_time;
    struct tm *current_t;
    char s[64];
	char nodename[128];

	if (fd_log>=0) return;
	time(&curr_time);
	my_mask=umask(0);	
	gethostname(nodename, sizeof(nodename));
	sprintf(log_name,"EAR.%s.log",nodename);
	fprintf(stderr,"Creating %s log file\n",log_name);
	fd_log=open(log_name,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd_log<0){
		fprintf(stderr,"EAR: Error creating EAR log file %s (%s)\n",log_name,strerror(errno));
	}
	umask(my_mask);
    current_t=localtime(&curr_time);
    strftime(s, sizeof(s), "%c", current_t);
	sprintf(my_log_buffer,"----------------------	EAR log created %s ------------------\n",s);
	write(fd_log,my_log_buffer,strlen(my_log_buffer));
}
void end_log()
{
	time_t curr_time;
    struct tm *current_t;
    char s[64];
	if (fd_log<0) return;
	time(&curr_time);
    current_t=localtime(&curr_time);
    strftime(s, sizeof(s), "%c", current_t);
	sprintf(my_log_buffer,"----------------------	EAR log closed %s ------------------\n",s);
	write(fd_log,my_log_buffer,strlen(my_log_buffer));
	close(fd_log);
}
void report_new_event(ear_event_t *event)
{
    time_t curr_time;
    struct tm *current_t;
    char s[64];
    if (fd_log<0) return;
    time(&curr_time);
    current_t=localtime(&curr_time);
    strftime(s, sizeof(s), "%c", current_t);
	switch(event->event){
		case ENERGY_POLICY_NEW_FREQ:
    		sprintf(my_log_buffer,"%s : job_id %u --> Frequency changed to %lu because of energy policy\n",s,event->job_id,event->freq);
			break;
		case GLOBAL_ENERGY_POLICY:
    		sprintf(my_log_buffer,"%s : job_id %u --> Frequency changed to %lu because of global Energy Budget\n",s,event->job_id,event->freq);
			break;
		case ENERGY_POLICY_FAILS:
    		sprintf(my_log_buffer,"%s : job_id %u --> Frequency changed to %lu because of policy projections failure\n",s,event->job_id,event->freq);
			break;
		case DYNAIS_OFF:
    		sprintf(my_log_buffer,"%s : job_id %u --> DynAIS off because of overhead\n",s,event->job_id);
			break;
	}

    write(fd_log,my_log_buffer,strlen(my_log_buffer));

}

void log_report_new_freq(int job,ulong newf)
{
    ear_event_t new_event;
    new_event.event=ENERGY_POLICY_NEW_FREQ;
    new_event.job_id=job;
    new_event.freq=newf ;
    report_new_event(&new_event);

}


void log_report_dynais_off(int job)
{
    ear_event_t new_event;
    new_event.event=DYNAIS_OFF;
    new_event.job_id=job;
    report_new_event(&new_event);

}



void log_report_max_tries(int job,ulong newf)
{
    ear_event_t new_event;
    new_event.event=ENERGY_POLICY_FAILS;
    new_event.freq=newf;
    new_event.job_id=job;
    report_new_event(&new_event);
}

void log_report_global_policy_freq(int job,ulong newf)
{
    ear_event_t new_event;
    new_event.event=GLOBAL_ENERGY_POLICY;
    new_event.freq=newf;
    new_event.job_id=job;
    report_new_event(&new_event);
}

