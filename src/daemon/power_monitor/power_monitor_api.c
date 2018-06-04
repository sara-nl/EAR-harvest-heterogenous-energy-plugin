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
#include <stdio.h>
#include <fcntl.h>
#include <float.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <control/frequency.h>
#include <database_cache/eardbd_api.h>
#include <daemon/shared_configuration.h>
#include <daemon/power_monitor/power_monitor.h>
#include <daemon/power_monitor/power_monitor_api.h>
#include <common/types/periodic_metric.h>
#include <common/types/application.h>
#include <common/types/cluster_conf.h>
#include <common/types/generic.h>
#include <common/ear_verbose.h>
#include <common/config.h>

#if DB_MYSQL
#include <common/database/db_helper.h>
#endif

#define MAX_PATH_SIZE 256
extern int eard_must_exit;
extern char ear_tmp[MAX_PATH_SIZE];
extern node_conf_t     *my_node_conf;
extern cluster_conf_t my_cluster_conf;
static char *__NAME__="powermon: ";

unsigned int f_monitoring;
extern ulong current_node_freq;
int idleNode=1;



extern char nodename[MAX_PATH_SIZE];
static int fd_powermon=-1;
static int fd_periodic=-1;
extern ear_conf_t *dyn_conf;

typedef struct powermon_app{
	application_t app;
	uint job_created;
	energy_data_t energy_init;
}powermon_app_t;


powermon_app_t current_ear_app;
periodic_metric_t current_sample;

static void PM_my_sigusr1(int signun)
{
    VERBOSE_N(1," thread %u receives sigusr1\n",(uint)pthread_self());
    pthread_exit(0);
}

static void PM_set_sigusr1()
{
    sigset_t set;
    struct  sigaction sa;
    sigemptyset(&set);
    sigaddset(&set,SIGUSR1);
    pthread_sigmask(SIG_UNBLOCK,&set,NULL); // unblocking SIGUSR1 for this thread
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = PM_my_sigusr1;
    sa.sa_flags=0;
    if (sigaction(SIGUSR1, &sa, NULL) < 0)
        ear_verbose(0,"eard doing sigaction of signal s=%d, %s\n",SIGUSR1,strerror(errno));

}



void reset_current_app()
{
	memset(&current_ear_app,0,sizeof(application_t));
}



/*
*
*
*	BASIC FUNCTIONS
*
*/
void job_init_powermon_app(application_t *new_app,uint from_mpi)
{
	energy_data_t c_energy;
	current_ear_app.job_created=!from_mpi;
	copy_application(&current_ear_app.app,new_app);	
	time(&current_ear_app.app.job.start_time);	
	current_ear_app.app.job.start_mpi_time=0;
	current_ear_app.app.job.end_mpi_time=0;
	current_ear_app.app.job.end_time=0;
	// reset signature
	init_signature(&current_ear_app.app.signature);
	init_power_signature(&current_ear_app.app.power_sig);
	current_ear_app.app.power_sig.max_DC_power=0;
	current_ear_app.app.power_sig.min_DC_power=500;
	// Initialize energy
	read_enegy_data(&c_energy);
	copy_energy_data(&current_ear_app.energy_init,&c_energy);
}


void job_end_powermon_app()
{
	energy_data_t c_energy;
	node_data_t app_total;
	power_data_t app_power;
	double exec_time;
	time(&current_ear_app.app.job.end_time);
	// Get the energy
	read_enegy_data(&c_energy);
	compute_power(&current_ear_app.energy_init,&c_energy,&app_power);
	
    current_ear_app.app.power_sig.DC_power=app_power.avg_dc;
    current_ear_app.app.power_sig.DRAM_power=app_power.avg_dram[0]+app_power.avg_dram[1];
    current_ear_app.app.power_sig.PCK_power=app_power.avg_cpu[0]+app_power.avg_cpu[1];
    current_ear_app.app.power_sig.time=difftime(app_power.end,app_power.begin);

	// nominal, avgf, edp is still pending

	// Metrics are not reported in this function
}


void report_powermon_app(powermon_app_t *app)
{
	// We can write here power information for this job
	report_application_data(&app->app);
#if DB_MYSQL
    if (!db_insert_application(&app->app)) DEBUG_F(1, "Application signature correctly written");
#endif
	
}

/*
*
*
*	MPI PART
*
*/
// That functions controls the init/end of jobs
static pthread_mutex_t app_lock = PTHREAD_MUTEX_INITIALIZER;

void powermon_mpi_init(application_t * appID)
{
	VERBOSE_N(2,"powermon_mpi_init job_id %d step_id %d (is_mpi %u)\n",appID->job.id,appID->job.step_id,appID->is_mpi);
	// As special case, we will detect if not job init has been specified
	if (!current_ear_app.job_created){	// If the job is nt submitted through slurm, new_job would not be submitted 
		powermon_new_job(appID,1);
	}
	// MPI_init : It only changes mpi_init time, we don't need to acquire the lock
	start_mpi(&current_ear_app.app.job);
	current_ear_app.app.is_mpi=1;
}

void powermon_mpi_finalize()
{
	VERBOSE_N(2,"powermon_mpi_finalize %d[%d]\n",current_ear_app.app.job.id,current_ear_app.app.job.step_id);
	end_mpi(&current_ear_app.app.job);
	if (!current_ear_app.job_created){  // If the job is not submitted through slurm, end_job would not be submitted 
		powermon_end_job(current_ear_app.app.job.id,current_ear_app.app.job.step_id);
	}
}

/*
*
*
*   JOB PART
*
*
/


/* This function is called by dynamic_configuration thread when a new_job command arrives */

void powermon_new_job(application_t* appID,uint from_mpi)
{
    // New application connected
	VERBOSE_N(2,"powermon_new_job (%d,%d)\n",appID->job.id,appID->job.step_id);
	frequency_save_previous_frequency();
	frequency_save_previous_policy();
	frequency_set_userspace_governor_all_cpus();
	// At this point, we assume MIN_TIME or MONITORING. It is pending to check about privileges and special cases
	frequency_set_all_cpus(dyn_conf->def_freq);
	current_node_freq=dyn_conf->def_freq;
	appID->job.def_f=dyn_conf->def_freq;	
    while (pthread_mutex_trylock(&app_lock));
        idleNode=0;
        job_init_powermon_app(appID,from_mpi);
		// We must report the energy beforesetting the job_id: PENDING
		new_job_for_period(&current_sample,appID->job.id,appID->job.step_id);
    pthread_mutex_unlock(&app_lock);
	VERBOSE_N(1,"Job created jid %u sid %u is_mpi %d\n",current_ear_app.app.job.id,current_ear_app.app.job.step_id,current_ear_app.app.is_mpi);

}
/* This function is called by dynamic_configuration thread when a end_job command arrives */

void powermon_end_job(job_id jid,job_id sid)
{
    // Application disconnected
    powermon_app_t summary;
    char buffer[128];
	if ((jid!=current_ear_app.app.job.id) || (sid!=current_ear_app.app.job.step_id)){ 
		VERBOSE_N(0,"powermon_end_job inicorrect jid %d\n",jid);
		return;
	}
	VERBOSE_N(1,"powermon_end_job %d[%d]\n",current_ear_app.app.job.id,current_ear_app.app.job.step_id);
    while (pthread_mutex_trylock(&app_lock));
        idleNode=1;
        job_end_powermon_app();
		// After that function, the avg power is computed
		bcopy(&current_ear_app,&summary,sizeof(powermon_app_t));
        current_ear_app.app.job.id=0;
		current_ear_app.job_created=0;
		// we must report the current period for that job before the reset:PENDING
		end_job_for_period(&current_sample);
    pthread_mutex_unlock(&app_lock);
    report_powermon_app(&summary);
	reset_current_app();
    frequency_recover_previous_policy();
    frequency_recover_previous_frequency();
	current_node_freq=frequency_get_cpu_freq(0);	

}

void powermon_new_max_freq(ulong maxf)
{
	if (current_ear_app.app.is_mpi==0){
		if (maxf<current_node_freq){
			VERBOSE_N(1,"MaxFreq: Application is not mpi, automatically changing freq from %lu to %lu\n",current_node_freq,maxf);
			frequency_set_all_cpus(maxf);
			current_node_freq=maxf;		
		}
	}
}

void powermon_new_def_freq(ulong def)
{
    if (current_ear_app.app.is_mpi==0){
        if (def<current_node_freq){
            VERBOSE_N(1,"DefFreq: Application is not mpi, automatically changing freq from %lu to %lu\n",current_node_freq,def);
            frequency_set_all_cpus(def);
            current_node_freq=def;
        }
    }
}

void powermon_red_freq(ulong max_freq,ulong def_freq)
{
	if (current_ear_app.app.is_mpi==0){
		if (def_freq<current_node_freq){
			VERBOSE_N(1,"RedFreq: Application is not mpi, automatically changing freq from %lu to %lu\n",current_node_freq,def_freq);
			frequency_set_all_cpus(def_freq);
			current_node_freq=def_freq;		
		}
	}
}

// Each sample is processed by this function
void update_historic_info(power_data_t *my_current_power,ulong avg_f)
{
	VERBOSE_N(0,"ID %u MPI=%u agv_f %lu Current power %lf max %lf min %lf\n",
		current_ear_app.app.job.id,current_ear_app.app.is_mpi,avg_f,my_current_power->avg_dc,
		current_ear_app.app.power_sig.max_DC_power, current_ear_app.app.power_sig.min_DC_power);
	
	while (pthread_mutex_trylock(&app_lock));
	
	if (current_ear_app.app.job.id>0){
		if (my_current_power->avg_dc>current_ear_app.app.power_sig.max_DC_power) 
			current_ear_app.app.power_sig.max_DC_power=my_current_power->avg_dc;
		if (my_current_power->avg_dc<current_ear_app.app.power_sig.min_DC_power)
			current_ear_app.app.power_sig.min_DC_power=my_current_power->avg_dc;
	}

	pthread_mutex_unlock(&app_lock);
		
    report_periodic_power(fd_periodic, my_current_power);

	current_sample.start_time=my_current_power->begin;
	current_sample.end_time=my_current_power->end;
	current_sample.DC_energy=my_current_power->avg_dc*(ulong)difftime(my_current_power->end,my_current_power->begin);
	
	#if DEMO
	current_sample.avg_f=avg_f;
	#endif	

	#if DB_MYSQL
	/* current sample reports the value of job_id and step_id active at this moment */
	/* If we want to be strict, we must report intermediate samples at job start and job end */
    if (!db_insert_periodic_metric(&current_sample)) DEBUG_F(1, "Periodic power monitoring sample correctly written");
	#endif

	eardbd_send_periodic_metric(&current_sample);

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
		VERBOSE_N(0," Error creating output file %s\n",strerror(errno));
	}else{
		VERBOSE_N(2," Created job power monitoring  file %s\n",output_name);
	}	
    sprintf(output_name,"%s/%s.pm_periodic_data.txt",ear_tmp,nodename);
    fd_periodic=open(output_name,O_WRONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd_periodic<0){
        fd_periodic=open(output_name,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    }
    if (fd_periodic<0){
        VERBOSE_N(0," Error creating output file for periodic monitoring %s\n",strerror(errno));
    }else{
        VERBOSE_N(2," Created power monitoring file for periodic information %s\n",output_name);
    }
	umask(my_mask);
}

void powermon_mpi_signature(application_t *app)
{
    copy_signature(&current_ear_app.app.signature,&app->signature);
    current_ear_app.app.job.def_f=app->job.def_f;
    current_ear_app.app.job.th=app->job.th;
    current_ear_app.app.job.procs=app->job.procs;
    strcpy(current_ear_app.app.job.policy,app->job.policy);
    strcpy(current_ear_app.app.job.app_id,app->job.app_id);
}

/*
*	MAIN POWER MONITORING THREAD
*/
	

// frequency_monitoring will be expressed in usecs
void *eard_power_monitoring(void *frequency_monitoring)
{
	unsigned int *f_monitoringp=(unsigned int *)frequency_monitoring;
	ulong avg_f;
	energy_data_t e_begin;
	energy_data_t e_end;	
	unsigned long t_ms;
	time_t t_begin,t_end; 
	double t_diff;
	power_data_t my_current_power;

	PM_set_sigusr1();

	VERBOSE_N(2," power monitoring thread created\n");
	if (init_power_ponitoring()!=EAR_SUCCESS) VERBOSE_N(0," Error in init_power_ponitoring\n");
	f_monitoring=*f_monitoringp;
	t_ms=f_monitoring/1000;
	// current_sample is the current powermonitoring period
	init_periodic_metric(&current_sample);
		
	create_powermon_out();
	reset_current_app();

	// We will collect and report avg power until eard finishes
	// Get time and Energy
	read_enegy_data(&e_begin);
	aperf_periodic_avg_frequency_init_all_cpus();

	// Database cache daemon
	eardbd_connect("cae2306.hpc.eu.lenovo.com", UDP);

	while(!eard_must_exit)
	{
		// Wait for N usecs
		usleep(f_monitoring);
		
		// Get time and Energy
		read_enegy_data(&e_end);
		avg_f=aperf_periodic_avg_frequency_end_all_cpus();
		
		// Compute the power
		compute_power(&e_begin,&e_end,&my_current_power);
		
		// Save current power
		update_historic_info(&my_current_power,avg_f);


		// Set values for next iteration
		copy_energy_data(&e_begin,&e_end);
		
		t_begin=t_end;
	}

	// Database cache daemon disconnect
	eardbd_disconnect();

	pthread_exit(0);
	//exit(0);
}
