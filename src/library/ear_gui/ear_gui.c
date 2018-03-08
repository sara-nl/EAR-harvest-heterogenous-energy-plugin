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
#include <papi.h>

#include <library/ear_gui/ear_gui.h>
#include <library/ear_states/ear_states.h>
#include <library/ear_frequency/ear_cpufreq.h>
#include <library/ear_metrics/ear_metrics.h>
#include <library/common/externs.h>
#include <common/types/generic.h>
#include <common/environment.h>
#include <common/ear_verbose.h>

// Move all the debug code here
// #define GLOBAL_MASTER 0
// #define LOCAL_MASTER 1

#ifdef EAR_GUI

#define DYNAIS_TRACE 0
#define APPLICATION_TRACE	1

static long long first_sample,sample_time;
static long long sample,last_sample=0;
static int fd_events;
static char ear_gui_buffer[2048];
static int trace_type=DYNAIS_TRACE;

#define CREATE_FLAGS S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

/*
Appinfo.txt
	Nodes			:
	MPI proc		:
	MPI proc/node	:
	Policy			:

Archinfo.txt
	Sockets			:
	Cores/socket	:
	Max freq		:
	Min freq		:
	Nom freq		:
	CPU model		:
	CPU name		:
*/

void get_hw_info(int *sockets, int *cores_socket, ulong *max_f, ulong *min_f, ulong *nom_f,
				 char *CPU_model, char *CPU_name)
{
	*nom_f = frequency_get_nominal_freq();
	hw_general = metrics_get_hw_info();

	*sockets = hw_general->sockets;
	*cores_socket = hw_general->cores;
	*max_f = hw_general->cpu_max_mhz;
	*min_f = hw_general->cpu_min_mhz;

	strcpy(CPU_model, hw_general->model_string);
	strcpy(CPU_name, hw_general->vendor_string);
}

void traces_init(int gwho,int lwho,char *appname,char *nodename,int nodes, int mpis,int ppnode)
{
	char *app_file="Appinfo.txt";
	char *arch_file="Archinfo.txt";
	char filenameApp[128];
	char filenameArch[128];
	char filenameEvents[128];
	char *ear_gui_path;
	char *my_trace_type;
	int sockets,cores_per_socket;
	unsigned long max_freq,min_freq,nom_freq;
	char vendor[128],model[128];
	int fd;

	ear_gui_path=get_ear_gui_pathname();
	ear_verbose(1,"EAR(%s) Using gui path %s\n",__FILE__,ear_gui_path);
	if (gwho==0){ // By the moment, only the global master, node events must be replicated based on DEBUG options
		my_trace_type=getenv("EAR_TRACE_TYPE");
		if (my_trace_type!=NULL){
			if (strcmp(my_trace_type,"DYNAIS")==0) trace_type=DYNAIS_TRACE;
			else trace_type=APPLICATION_TRACE;
		}
		sprintf(filenameApp,"%s/%s",ear_gui_path,app_file);
		sprintf(filenameArch,"%s/%s",ear_gui_path,arch_file);
		// APP info
		fd=open(filenameApp,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
		if (fd<0){
			ear_verbose(0,"EAR(%s) Creating EAR GUI app file %s (%s)\n",__FILE__,filenameApp,strerror(errno));
			exit(1);
		}
		sprintf(ear_gui_buffer,"%s\n%d\n%d\n%d\n%s\n",appname,nodes,mpis,ppnode, application.policy);
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
		get_hw_info(&sockets,&cores_per_socket,&max_freq,&min_freq,&nom_freq,model,vendor);
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
		if (trace_type==APPLICATION_TRACE){
        	sample_time=0;
			sprintf(ear_gui_buffer,"%llu;%u;%u\n%llu;%u;%u\n%llu;%u;%u\n",
			sample_time,PERIOD_ID,0,sample_time,PERIOD_LENGTH,0,sample_time,PERIOD_ITERATIONS,0);
        		write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
        		first_sample=PAPI_get_real_usec();
			last_sample=sample_time;
        }
	}
	
}

void traces_end(int gwho,int lwho,unsigned long int total_energy)
{
	if (trace_type==APPLICATION_TRACE){
	if (lwho==0){
		sample=PAPI_get_real_usec();
		sample_time=metrics_usecs_diff(sample,first_sample);
		sprintf(ear_gui_buffer,"%llu;%u;%llu\n",sample_time,APP_ENERGY,total_energy);
		write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
		close(fd_events);
	}
	}
}

void traces_new_period(int gwho,int lwho,int period_id)
{
}

void traces_new_n_iter(int gwho,int lwho,int period_id,int period_size, int iterations,int my_state)
{
	if (trace_type==APPLICATION_TRACE){
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
}
void traces_end_period(int gwho,int lwho)
{
	if (trace_type==APPLICATION_TRACE){
	if (lwho==0){
		sample=PAPI_get_real_usec();
		sample_time=metrics_usecs_diff(sample,first_sample);
		sprintf(ear_gui_buffer,"%llu;%u;%u\n%llu;%u;%u\n%llu;%u;%u\n",
			sample_time,PERIOD_ID,0,sample_time,PERIOD_LENGTH,0,sample_time,PERIOD_ITERATIONS,0);
		write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
		last_sample=sample_time;
	}
	}

}

void traces_new_signature(int gwho,int lwho,double seconds,double CPI,double TPI,double GBS,double POWER)
{
	if (trace_type==APPLICATION_TRACE){
	if (lwho==0){
                sample=PAPI_get_real_usec();
                sample_time=metrics_usecs_diff(sample,first_sample);
                sprintf(ear_gui_buffer,"%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n", 
			sample_time,PERIOD_TIME,seconds*1000000,sample_time,PERIOD_CPI,CPI,sample_time,PERIOD_TPI,TPI,sample_time,PERIOD_GBS,GBS,sample_time,PERIOD_POWER,POWER);
                write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
		last_sample=sample_time;
        }
	}
}

void traces_PP(int gwho,int lwho,double seconds,double CPI,double POWER)
{
	if (trace_type==APPLICATION_TRACE){
	if (lwho==0){
                sample=PAPI_get_real_usec();
                sample_time=metrics_usecs_diff(sample,first_sample);
                sprintf(ear_gui_buffer,"%llu;%u;%lf\n%llu;%u;%lf\n%llu;%u;%lf\n",      
                        sample_time,PERIOD_TIME_PROJ,seconds*1000000,sample_time,PERIOD_CPI_PROJ,CPI,sample_time,PERIOD_POWER_PROJ,POWER);
                write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
                last_sample=sample_time;
        }
	}
}

void traces_frequency(int gwho,int lwho, unsigned long f)
{
	if (trace_type==APPLICATION_TRACE){
	if (lwho==0){
                sample=PAPI_get_real_usec();
                sample_time=metrics_usecs_diff(sample,first_sample);
                sprintf(ear_gui_buffer,"%llu;%u;%u\n", sample_time,PERIOD_FREQ,f);
                write(fd_events,ear_gui_buffer,strlen(ear_gui_buffer));
                last_sample=sample_time;
	}
	}
}

void traces_mpi_call(int gwho,int lwho,ulong timestamp,ulong event,ulong arg1,ulong arg2,ulong arg3)
{
	ulong trace_data[5];
	if (trace_type==DYNAIS_TRACE){
    	if (gwho==0){
    		trace_data[0]=(unsigned long)arg1;
    		trace_data[1]=(unsigned long)arg2;
    		trace_data[2]=(unsigned long)arg3;
    		trace_data[3]=(unsigned long)event;
    		trace_data[4]=(unsigned long)timestamp;
    		write(fd_events,trace_data,sizeof(trace_data));
    	}
	}
}
#endif
