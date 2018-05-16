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

#include <common/config.h>
#include <common/ear_verbose.h>
#include <common/types/generic.h>
#include <common/types/application.h>
#include <metrics/power_monitoring/ear_power_monitor.h>
#include <daemon/power_monitoring.h>

extern int eard_must_exit;
#define MAX_PATH_SIZE 256
extern char ear_tmp[MAX_PATH_SIZE];
static const char *__NAME__ = "powermon: ";

//  That constant is replicated. We must fix that
#define MAX_PATH_SIZE 256

unsigned int f_monitoring;

int idleNode=1;


/* AVG, MAX, MIN for app */
int samples=0;
double max_dc=0,min_dc=DBL_MAX,avg_dc;
int current_app_id=-1;

extern char nodename[MAX_PATH_SIZE];
static int fd_powermon=-1;
static int fd_periodic=-1;

typedef struct powermon_app{
	application_t app;
	uint   job_created;
	node_data_t DC_energy_init,DC_energy_end;
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
	current_ear_app.app.job.id=-1;
	current_ear_app.job_created=0;
}

void mpi_init_powermon_app()
{
	start_mpi(&current_ear_app.app.job);
	
}
void job_init_powermon_app(job_t *new_job,uint from_mpi)
{
	energy_data_t c_energy;
	current_ear_app.job_created=!from_mpi;
	copy_job(&current_ear_app.app.job,new_job);	
	current_ear_app.app.signature.max_DC_power=0;
	current_ear_app.app.signature.min_DC_power=DBL_MAX;
	time(&current_ear_app.app.job.start_time);	
	current_ear_app.app.job.start_mpi_time=0;
	current_ear_app.app.job.end_mpi_time=0;
	current_ear_app.app.job.end_time=0;
	// reset signature
	init_signature(&current_ear_app.app.signature);
	init_power_signature(&current_ear_app.app.power_sig);
	// Initialize energy
	read_enegy_data(&c_energy);
	current_ear_app.DC_energy_init=c_energy.DC_node_energy;	
}

void mpi_finalize_powermon_app()
{
	end_mpi(&current_ear_app.app.job);
	
}

void job_end_powermon_app()
{
	energy_data_t c_energy;
	node_data_t app_total;
	double exec_time;
	time(&current_ear_app.app.job.end_time);
	// Get the energy
	read_enegy_data(&c_energy);
	current_ear_app.DC_energy_end=c_energy.DC_node_energy;
	// Compute the avg power	
	app_total=diff_node_energy(current_ear_app.DC_energy_end,current_ear_app.DC_energy_init);	
	exec_time=difftime(current_ear_app.app.job.end_time,current_ear_app.app.job.start_time);
	current_ear_app.app.power_sig.DC_power=(double)app_total/(exec_time*1000);	
	// Metrics are not reported in this function
}


void report_powermon_app(powermon_app_t *app)
{
	char buffer[1024];
    struct tm *current_t;
    char jbegin[64],jend[64],mpibegin[64],mpiend[64];

	// This function will report values to EAR_DB

    // We format the end time into localtime and string
    current_t=localtime(&(app->app.job.start_time));
    strftime(jbegin, sizeof(jbegin), "%c", current_t);
    current_t=localtime(&(app->app.job.end_time));
    strftime(jend, sizeof(jend), "%c", current_t);
	if (app->app.job.start_mpi_time){
    	current_t=localtime(&(app->app.job.start_mpi_time));
    	strftime(mpibegin, sizeof(mpibegin), "%c", current_t);
    	current_t=localtime(&(app->app.job.end_mpi_time));
    	strftime(mpiend, sizeof(mpiend), "%c", current_t);
	}

	// We can write here power information for this job
	report_application_data(&app->app);
	
}

// That functions controls the init/end of jobs
static pthread_mutex_t app_lock = PTHREAD_MUTEX_INITIALIZER;

void powermon_mpi_init(job_t * appID)
{
	VERBOSE_N(1,"powermon_mpi_init %d\n",appID);
	// As special case, we will detect if not job init has been specified
	if (!current_ear_app.job_created){	// If the job is nt submitted through slurm, new_job would not be submitted 
		powermon_new_job(appID,1);
	}
	// MPI_init : It only changes mpi_init time, we don't need to acquire the lock
	mpi_init_powermon_app();
}

void powermon_mpi_finalize()
{
	VERBOSE_N(0,"powermon_mpi_finalize %d[%d]\n",current_ear_app.app.job.id,current_ear_app.app.job.step_id);
	mpi_finalize_powermon_app();
	if (!current_ear_app.job_created){  // If the job is not submitted through slurm, end_job would not be submitted 
		powermon_end_job(current_ear_app.app.job.id,current_ear_app.app.job.step_id);
	}
}

void powermon_new_job(job_t* appID,uint from_mpi)
{
    // New application connected
	VERBOSE_N(0,"powermon_new_job %d\n",appID);
    while (pthread_mutex_trylock(&app_lock));
        idleNode=0;
        job_init_powermon_app(appID,from_mpi);
    pthread_mutex_unlock(&app_lock);

}

void powermon_end_job(job_id jid,job_id sid)
{
    // Application disconnected
    double lavg,lmax,lmin;
    powermon_app_t summary;
    char buffer[128];
	if ((jid!=current_ear_app.app.job.id) || (sid!=current_ear_app.app.job.step_id)){ 
		VERBOSE_N(0,"powermon_end_job inicorrect jid %d\n",jid);
		return;
	}
	VERBOSE_N(0,"powermon_end_job %d[%d]\n",current_ear_app.app.job.id,current_ear_app.app.job.step_id);
    while (pthread_mutex_trylock(&app_lock));
        idleNode=1;
        job_end_powermon_app();
		// After that function, the avg power is computed
        copy_powermon_app(&summary,&current_ear_app);
        current_ear_app.app.job.id=-1;
		current_ear_app.job_created=0;
    pthread_mutex_unlock(&app_lock);
    report_powermon_app(&summary);
}


// Each sample is processed by this function
void update_historic_info(power_data_t *my_current_power)
{
	while (pthread_mutex_trylock(&app_lock));
	if (current_ear_app.app.job.id>0){
			current_ear_app.app.power_sig.max_DC_power=max(current_ear_app.app.power_sig.max_DC_power,my_current_power->avg_dc);
			current_ear_app.app.power_sig.min_DC_power=min(current_ear_app.app.power_sig.min_DC_power,my_current_power->avg_dc);
	}
	pthread_mutex_unlock(&app_lock);
		
	if (current_ear_app.app.job.id!=-1)	printf("Application id %d[%d]: ",current_ear_app.app.job.id,current_ear_app.app.job.step_id);
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
void powermon_mpi_signature(application_t *app)
{

#if DB_MYSQL

#include <common/database/db_helper.h>

	if (!db_insert_application(app, 0)) DEBUG_F(1, "Application signature correctly written");

#endif
}
