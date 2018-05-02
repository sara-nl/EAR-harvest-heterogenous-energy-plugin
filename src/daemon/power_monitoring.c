/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <float.h>
#include <errno.h>
#include <pthread.h>
#include <common/ear_verbose.h>
#include <common/types/generic.h>

#include <metrics/power_monitoring/ear_power_monitor.h>
#include <daemon/power_monitoring.h>

extern int eard_must_exit;
#define MAX_PATH_SIZE 256
extern char ear_tmp[MAX_PATH_SIZE];
static const char *__NAME__ = "powermon: ";

//  That constant is replicated. We must fix that
#define MAX_PATH_SIZE 256
#define NUM_SAMPLES_L1 	10 	// maximum number of samples saved every -frequency_monitoring- usecs
#define NUM_SAMPLES_L2	10		// Every NUM_SAMPLES_L1 of L1 we will compute 1 sample of L2, 

unsigned int f_monitoring;

int idleNode=1;

power_data_t *L1_samples,*L2_samples;
int current_L1=0;
int num_L1=0;
int current_L2=0;
int num_L2=0;

/* AVG, MAX, MIN for app */
int samples=0;
double max_dc=0,min_dc=DBL_MAX,avg_dc;
int current_app_id=-1;

extern char nodename[MAX_PATH_SIZE];
static int fd_powermon=-1;
static int fd_periodic=-1;

typedef struct powermon_app{
    int job_id;
	time_t begin_time;
	time_t end_time;
	time_t mpi_init_time;
	time_t mpi_finalize_time;
	uint   job_created;
	double avg_dc_power;
	double max_dc_power;
	double min_dc_power;
}powermon_app_t;

powermon_app_t current_ear_app;

#define max(X,Y) (((X) > (Y)) ? (X) : (Y))
#define min(X,Y) (((X) < (Y)) ? (X) : (Y))

void copy_powermon_app(powermon_app_t *dest,powermon_app_t *src)
{
	bcopy(src,dest,sizeof(powermon_app_t));
}


void reset_current_app()
{
	current_ear_app.job_id=-1;
	current_ear_app.job_created=0;
}

void mpi_init_powermon_app(int app_id)
{
	time(&current_ear_app.mpi_init_time);
}
void job_init_powermon_app(int app_id,uint from_mpi)
{
    current_ear_app.avg_dc_power=0;
    current_ear_app.max_dc_power=0;
    current_ear_app.min_dc_power=DBL_MAX;
    current_ear_app.job_id=app_id;
	current_ear_app.job_created=!from_mpi;
	current_ear_app.mpi_init_time=0;
	current_ear_app.mpi_finalize_time=0;
    time(&current_ear_app.begin_time);
}

void mpi_finalize_powermon_app(int app_id,int samples)
{
	time(&current_ear_app.mpi_finalize_time);
}

void job_end_powermon_app(int app_id,int samples)
{
    current_ear_app.avg_dc_power=current_ear_app.avg_dc_power/(double)samples;
    time(&current_ear_app.end_time);
}


void report_powermon_app(powermon_app_t *app)
{
	char buffer[1024];
    struct tm *current_t;
    char jbegin[64],jend[64],mpibegin[64],mpiend[64];

	// This function will report values to EAR_DB

    // We format the end time into localtime and string
    current_t=localtime(&(app->begin_time));
    strftime(jbegin, sizeof(jbegin), "%c", current_t);
    current_t=localtime(&(app->end_time));
    strftime(jend, sizeof(jend), "%c", current_t);
	if (app->mpi_init_time){
    	current_t=localtime(&(app->mpi_init_time));
    	strftime(mpibegin, sizeof(mpibegin), "%c", current_t);
    	current_t=localtime(&(app->mpi_finalize_time));
    	strftime(mpiend, sizeof(mpiend), "%c", current_t);
	}

	if (fd_powermon>=0){
		//"job_id;begin_time;end_time;mpi_init_time;mpi_finalize_time;avg_dc_power;max_dc_power;min_dc_power\n";
		if (app->mpi_init_time){
			sprintf(buffer,"%d;%s;%s;%s;%s;%.3lf;%.3lf;%.3lf\n",app->job_id,jbegin,jend,mpibegin,mpiend,app->avg_dc_power,app->max_dc_power,app->min_dc_power);
		}else{
			sprintf(buffer,"%d;%s;%s;not mpi;not mpi;%.3lf;%.3lf;%.3lf\n",app->job_id,jbegin,jend,app->avg_dc_power,app->max_dc_power,app->min_dc_power);
		}
		write(fd_powermon,buffer,strlen(buffer));
	}
	
}

// That functions controls the init/end of jobs
static pthread_mutex_t app_lock = PTHREAD_MUTEX_INITIALIZER;

void powermon_mpi_init(int appID)
{
	VERBOSE_N(1,"powermon_mpi_init %d\n",appID);
	// As special case, we will detect if not job init has been specified
	if (!current_ear_app.job_created){	// If the job is nt submitted through slurm, new_job would not be submitted 
		powermon_new_job(appID,1);
	}
	// MPI_init : It only changes mpi_init time, we don't need to acquire the lock
	mpi_init_powermon_app(appID);
}

void powermon_mpi_finalize(int appID)
{
	// MPI_finalize: we don't need to acquire the lock
	double lavg,lmax,lmin;
	powermon_app_t summary;
	int lsamples;
	char buffer[128];
	VERBOSE_N(0,"powermon_mpi_finalize %d\n",appID);
	mpi_finalize_powermon_app(appID,samples);
	if (!current_ear_app.job_created){  // If the job is nt submitted through slurm, end_job would not be submitted 
		powermon_end_job(appID);
	}
}

void powermon_new_job(int appID,uint from_mpi)
{
    // New application connected
	VERBOSE_N(0,"powermon_new_job %d\n",appID);
    while (pthread_mutex_trylock(&app_lock));
        idleNode=0;
        job_init_powermon_app(appID,from_mpi);
        samples=0;
    pthread_mutex_unlock(&app_lock);

}

void powermon_end_job(int appID)
{
    // Application disconnected
    double lavg,lmax,lmin;
    powermon_app_t summary;
    char buffer[128];
	VERBOSE_N(0,"powermon_end_job %d\n",appID);
    while (pthread_mutex_trylock(&app_lock));
        idleNode=1;
        job_end_powermon_app(appID,samples);
        copy_powermon_app(&summary,&current_ear_app);
        current_ear_app.job_id=-1;
		current_ear_app.job_created=0;
        lavg=current_ear_app.avg_dc_power;
        lmax=current_ear_app.max_dc_power;
        lmin=current_ear_app.min_dc_power;
    pthread_mutex_unlock(&app_lock);
    VERBOSE_N(0,"Application %d disconnected: DC node power metrics (avg. %lf max %lf min %lf)\n",appID,lavg,lmax,lmin);
    report_powermon_app(&summary);
}


// Each sample is processed by this function
void update_historic_info(power_data_t *my_current_power)
{
	while (pthread_mutex_trylock(&app_lock));
	if (current_ear_app.job_id>0){
			current_ear_app.max_dc_power=max(current_ear_app.max_dc_power,my_current_power->avg_dc);
			current_ear_app.min_dc_power=min(current_ear_app.min_dc_power,my_current_power->avg_dc);
			current_ear_app.avg_dc_power+=my_current_power->avg_dc;
			samples++;
	}
	pthread_mutex_unlock(&app_lock);
		
	if (current_ear_app.job_id!=-1)	printf("Application id %d: ",current_ear_app.job_id);
    	report_periodic_power(fd_periodic, my_current_power);

	return;
}



void create_powermon_out()
{
	char output_name[MAX_PATH_SIZE];
	char *header="job_id;begin_time;end_time;mpi_init_time;mpi_finalize_time;avg_dc_power;max_dc_power;min_dc_power\n";
	mode_t my_mask;
	// We are using EAR_TMP but this info will go to the DB
	my_mask=umask(0);	
	sprintf(output_name,"%s/%s.pm_data.csv",ear_tmp,nodename);
	fd_powermon=open(output_name,O_WRONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd_powermon<0){
		fd_powermon=open(output_name,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
		if (fd_powermon>=0) write(fd_powermon,header,strlen(header));
	}
	if (fd_powermon<0){ 
		VERBOSE_N(0,"Error creating output file %s\n",strerror(errno));
	}else{
		VERBOSE_N(0,"Created job power monitoring  file %s\n",output_name);
	}	
    sprintf(output_name,"%s/%s.pm_periodic_data.txt",ear_tmp,nodename);
    fd_periodic=open(output_name,O_WRONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd_periodic<0){
        fd_periodic=open(output_name,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    }
    if (fd_periodic<0){
        VERBOSE_N(0,"Error creating output file for periodic monitoring %s\n",strerror(errno));
    }else{
        VERBOSE_N(0,"Created power monitoring file for periodic information %s\n",output_name);
    }
	umask(my_mask);
}
	

// frequency_monitoring will be expressed in usecs
void *eard_power_monitoring(void *frequency_monitoring)
{
	unsigned int *f_monitoringp=(unsigned int *)frequency_monitoring;
	energy_data_t e_begin;
	energy_data_t e_end;	
	unsigned long t_ms;
	time_t t_begin,t_end; 
	double t_diff;
	power_data_t my_current_power;

	VERBOSE_N(0,"power monitoring thread created\n");
	if (init_power_ponitoring()!=EAR_SUCCESS) VERBOSE_N(0,"Error in init_power_ponitoring\n");
	f_monitoring=*f_monitoringp;
	t_ms=f_monitoring/1000;

	create_powermon_out();
	reset_current_app();

	// We will collect and report avg power until eard finishes
	// Get time and Energy
	read_enegy_data(&e_begin);
	while(!eard_must_exit)
	{
		// Wait for N usecs
		usleep(f_monitoring);
		// Get time and Energy
		read_enegy_data(&e_end);
		// Compute the power
		compute_power(&e_begin,&e_end,&my_current_power);
		// Save current power
		update_historic_info(&my_current_power);
		// Set values for next iteration
		copy_energy_data(&e_begin,&e_end);
		t_begin=t_end;
	}
	pthread_exit(0);
	//exit(0);
}
