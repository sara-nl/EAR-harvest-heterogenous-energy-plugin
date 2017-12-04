/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ear_verbose.h>
#include <ear_configuration.h>
#include <ear_states/ear_states.h>
#include <ear_gui/ear_gui.h>
#include <ear_metrics/ear_papi.h>
#include <ear_frequency/ear_cpufreq.h>

extern char *ear_policy_name;



// Move all the debug code here
//#define GLOBAL_MASTER 0
//#define LOCAL_MASTER 1

#ifdef EAR_GUI

long long first_sample,sample_time;
long long sample,last_sample=0;
int fd_events;

#define CREATE_FLAGS S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
/*
Appinfo.txt
	Nodes			:
	MPI proc		:
	MPI proc/node		:
	Policy			:

Archinfo.txt
	Sockets			:
	Cores/socket		:
	Max freq		:
	Min freq		:
	Nom freq		:
	CPU model		:
	CPU name		:
*/

char ear_gui_buffer[2048];

void gui_init(int gwho,int lwho,char *appname,char *nodename,int nodes, int mpis,int ppnode)
{
	char *app_file="Appinfo.txt";
	char *arch_file="Archinfo.txt";
	char filenameApp[128];
	char filenameArch[128];
	char filenameEvents[128];
	char *ear_gui_path;



	int sockets,cores_per_socket;
	unsigned long max_freq,min_freq,nom_freq;
	char vendor[128],model[128];

	int fd;	
    ear_gui_path=get_ear_gui_pathname();
	ear_verbose(1,"EAR(%s) Using gui path %s\n",__FILE__,ear_gui_path);
	if (gwho==0){ // By the moment, only the global master, node events must be replicated based on DEBUG options
		sprintf(filenameApp,"%s/%s",ear_gui_path,app_file);
		sprintf(filenameArch,"%s/%s",ear_gui_path,arch_file);
		// APP info
		fd=open(filenameApp,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
		if (fd<0){
			ear_verbose(0,"EAR(%s) Creating EAR GUI app file %s (%s)\n",__FILE__,filenameApp,strerror(errno));
			exit(1);
		}
		sprintf(ear_gui_buffer,"%s\n%d\n%d\n%d\n%s\n",appname,nodes,mpis,ppnode,ear_policy_name);
		if (write(fd,ear_gui_buffer,strlen(ear_gui_buffer))!=strlen(ear_gui_buffer)){
			ear_verbose(0,"EAR(%s) error writting App info (%s) \n",__FILE__,strerror(errno));
			exit(1);
		}
		close(fd);
		// Arch info
        fd=open(filenameArch,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
        if (fd<0){
                        ear_verbose(0,"EAR(%s)Creating EAR GUI arch file %s (%s)\n",__FILE__,filenameArch,strerror(errno));
                        exit(1);
        }
		metrics_get_hw_info(&sockets,&cores_per_socket,&max_freq,&min_freq,&nom_freq,model,vendor);
		sprintf(ear_gui_buffer,"%d\n%d\n%u\n%u\n%u\n%s\n%s\n",
			sockets,cores_per_socket,max_freq*1000,min_freq*1000,nom_freq,vendor,model);
        if (write(fd,ear_gui_buffer,strlen(ear_gui_buffer))!=strlen(ear_gui_buffer)){
                        ear_verbose(0,"EAR(%s) error writting Arch info (%s)\n",__FILE__,strerror(errno));
                        exit(1);
        }
        close(fd);
	}
	if (lwho==0){
		sprintf(filenameEvents,"%s/%d.%s.txt",ear_gui_path,gwho,nodename);
		// Event info
        fd_events=open(filenameEvents,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
        if (fd_events<0){
                        ear_verbose(0,"EAR(%s) Error creating EAR GUI fd_events %s (%s)",__FILE__,filenameEvents,strerror(errno));
                        exit(1);
        }
		
        sample_time=0;
		sprintf(ear_gui_buffer,"%llu;%u;%u\n%llu;%u;%u\n%llu;%u;%u\n",
		sample_time,PERIOD_ID,0,sample_time,PERIOD_LENGTH,0,sample_time,PERIOD_ITERATIONS,0);
        write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
        first_sample=PAPI_get_real_usec();
		last_sample=sample_time;
        }
	
}

void gui_end(int gwho,int lwho,unsigned long int total_energy)
{
	if (lwho==0){
		sample=PAPI_get_real_usec();
		sample_time=metrics_usecs_diff(sample,first_sample);
		sprintf(ear_gui_buffer,"%llu;%u;%llu\n",sample_time,APP_ENERGY,total_energy);
		write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
		close(fd_events);
	}
}

void gui_new_period(int gwho,int lwho,int period_id)
{

}


void gui_new_n_iter(int gwho,int lwho,int period_id,int period_size, int iterations,int my_state)
{
	if (lwho==0){
		sample=PAPI_get_real_usec();
		sample_time=metrics_usecs_diff(sample,first_sample);
		if (iterations==1){ // we report the loop after 1 iteration to compute the loop size
			sprintf(ear_gui_buffer,"%llu;%u;%u\n%llu;%u;%u\n%llu;%u;%u\n",
			sample_time,PERIOD_ID,period_id,sample_time,PERIOD_LENGTH,period_size,sample_time,PERIOD_ITERATIONS,iterations);
			write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
			last_sample=sample_time;
		}else{
			if ((sample_time-last_sample)>MIN_FREQ_FOR_SAMPLING){
				sprintf(ear_gui_buffer,"%llu;%u;%u\n%llu;%u;%u\n%llu;%u;%u\n",
				sample_time,PERIOD_ID,period_id,sample_time,PERIOD_LENGTH,period_size,sample_time,PERIOD_ITERATIONS,iterations);
				write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
				last_sample=sample_time;
			}
		}
		last_sample=sample_time;
	}
}
void gui_end_period(int gwho,int lwho)
{
	if (lwho==0){
		sample=PAPI_get_real_usec();
		sample_time=metrics_usecs_diff(sample,first_sample);
		sprintf(ear_gui_buffer,"%llu;%u;%u\n%llu;%u;%u\n%llu;%u;%u\n",
			sample_time,PERIOD_ID,0,sample_time,PERIOD_LENGTH,0,sample_time,PERIOD_ITERATIONS,0);
		write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
		last_sample=sample_time;
	}

}

void gui_new_signature(int gwho,int lwho,double seconds,double CPI,double TPI,double GBS,double POWER)
{
	if (lwho==0){
                sample=PAPI_get_real_usec();
                sample_time=metrics_usecs_diff(sample,first_sample);
                sprintf(ear_gui_buffer,"%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n", 
			sample_time,PERIOD_TIME,seconds*1000000,sample_time,PERIOD_CPI,CPI,sample_time,PERIOD_TPI,TPI,sample_time,PERIOD_GBS,GBS,sample_time,PERIOD_POWER,POWER);
                write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
		last_sample=sample_time;
        }
}

void gui_PP(int gwho,int lwho,double seconds,double CPI,double POWER)
{
	if (lwho==0){
                sample=PAPI_get_real_usec();
                sample_time=metrics_usecs_diff(sample,first_sample);
                sprintf(ear_gui_buffer,"%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n",      
                        sample_time,PERIOD_TIME_PROJ,seconds*1000000,sample_time,PERIOD_CPI_PROJ,CPI,sample_time,PERIOD_POWER_PROJ,POWER);
                write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
                last_sample=sample_time;
        }

}

void gui_frequency(int gwho,int lwho, unsigned long f)
{
	if (lwho==0){
                sample=PAPI_get_real_usec();
                sample_time=metrics_usecs_diff(sample,first_sample);
                sprintf(ear_gui_buffer,"%llu;%u;%u\n", sample_time,PERIOD_FREQ,f);
                write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
                last_sample=sample_time;
	}

}
#endif
