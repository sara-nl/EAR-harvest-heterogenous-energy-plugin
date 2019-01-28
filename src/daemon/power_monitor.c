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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <float.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <common/config.h>
#include <common/sockets.h>
#include <common/output/debug.h>
#include <common/output/verbose.h>
#include <common/types/generic.h>
#include <common/types/application.h>
#include <common/types/periodic_metric.h>
#include <common/types/configuration/cluster_conf.h>
#include <metrics/custom/frequency.h>
#include <metrics/custom/frequency_uncore.h>
#include <metrics/power_metrics/power_metrics.h>
#include <control/frequency.h>
#include <daemon/power_monitor.h>
#include <daemon/eard_conf_rapi.h>
#include <daemon/eard_checkpoint.h>
#include <daemon/shared_configuration.h>

#if SYSLOG_MSG
#include <syslog.h>
#endif

#if DB_MYSQL
#include <database_cache/eardbd_api.h>
#include <common/database/db_helper.h>
#endif

#define REPORT_UNCORE	1

uint64_t uncore_freq[2]={0,0};
extern int eard_must_exit;
extern char ear_tmp[MAX_PATH_SIZE];
extern my_node_conf_t     *my_node_conf;
extern cluster_conf_t my_cluster_conf;
extern eard_dyn_conf_t eard_dyn_conf;
extern policy_conf_t default_policy_context,energy_tag_context,authorized_context;
extern int ear_ping_fd;
extern int ear_fd_ack[1];
extern int application_id;

/* This variable controls the frequency for periodic power monitoring */
extern uint f_monitoring;
extern ulong current_node_freq;
int idleNode=1;



extern char nodename[MAX_PATH_SIZE];
static int fd_powermon=-1;
static int fd_periodic=-1;
extern settings_conf_t *dyn_conf;
extern resched_t *resched_conf;
static int sig_reported=0;

#define MAX_NESTED_LEVELS 10
powermon_app_t *current_ear_app[MAX_NESTED_LEVELS];
powermon_app_t default_app;
int ccontext=-1;
periodic_metric_t current_sample;
double last_power_reported=0;


powermon_app_t *get_powermon_app()
{
	if (ccontext>0) return current_ear_app[ccontext];
	else return &default_app;
}
static void PM_my_sigusr1(int signun)
{
    verbose(1," thread %u receives sigusr1\n",(uint)pthread_self());
    pthread_exit(0);
}

static void PM_set_sigusr1()
{
    sigset_t set;
    struct  sigaction sa;
    sigfillset(&set);
    sigdelset(&set,SIGUSR1);
    sigdelset(&set,SIGALRM);
    pthread_sigmask(SIG_SETMASK,&set,NULL); // unblocking SIGUSR1 and SIGALRM for this thread
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = PM_my_sigusr1;
    sa.sa_flags=0;
    if (sigaction(SIGUSR1, &sa, NULL) < 0)
        verbose(0," doing sigaction of signal s=%d, %s\n",SIGUSR1,strerror(errno));

}

void reset_shared_memory()
{
	policy_conf_t *my_policy;
    my_policy=get_my_policy_conf(my_node_conf,my_cluster_conf.default_policy);
    dyn_conf->user_type=NORMAL;
	dyn_conf->learning=0;
    dyn_conf->lib_enabled=1;
    dyn_conf->policy=my_cluster_conf.default_policy;
    dyn_conf->def_freq=frequency_pstate_to_freq(my_policy->p_state);
	dyn_conf->def_p_state=my_policy->p_state;
    dyn_conf->th=my_policy->th;
}

void clean_job_environment(int id,int step_id)
{
	char ear_ping[MAX_PATH_SIZE],fd_lock_filename[MAX_PATH_SIZE],ear_commack[MAX_PATH_SIZE];
	uint pid=create_ID(id,step_id);

	if (ear_ping_fd>=0){
		close(ear_ping_fd);
		ear_ping_fd=-1;
	}
	if (ear_fd_ack[0]>=0){
		close(ear_fd_ack[0]);
		ear_fd_ack[0]=-1;
	}
	application_id = -1;
	sprintf(ear_ping,"%s/.ear_comm.ping.%u",ear_tmp,pid);
    sprintf(fd_lock_filename, "%s/.ear_app_lock.%u", ear_tmp, pid);
    sprintf(ear_commack, "%s/.ear_comm.ack_0.%u",ear_tmp, pid);
	verbose(0,"Cleanning the environment");
	verbose(0,"Releasing %s - %s - %s ",ear_ping,fd_lock_filename,ear_commack);
	unlink(ear_ping);
	unlink(fd_lock_filename);
	unlink(ear_commack);
	#if SYSLOG_MSG
	closelog();
	#endif

	

}

void copy_powermon_app(powermon_app_t *dest,powermon_app_t *src)
{
	dest->job_created=src->job_created;
	dest->energy_init=src->energy_init;
	copy_application(&(dest->app),&(src->app));
	dest->governor.min=src->governor.min;
	dest->governor.max=src->governor.max;
	dest->governor.governor=(char *)malloc(strlen(src->governor.governor)+1);
	strcpy(dest->governor.governor,src->governor.governor);	
	dest->current_freq=src->current_freq;

}


#if DB_FILES
char database_csv_path[PATH_MAX];

void report_application_in_file(application_t *app)
{
	int ret1,ret2;
	ret2 = append_application_text_file(database_csv_path, app,app->is_mpi);
	if (ret1 == EAR_SUCCESS && ret2 == EAR_SUCCESS)
	{
		debug( "application signature correctly written");
	} else {
		verbose(1, "ERROR while application signature writing");
	}
}

void form_database_paths()
{
	char island[PATH_MAX];
	mode_t mode,old_mode;

	old_mode=umask(0);	
	mode=S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;
	sprintf(island,"%s/island%d",my_cluster_conf.DB_pathname,my_node_conf->island);
	if ((mkdir(island,mode)<0 ) && (errno!=EEXIST)){
		verbose(0,"DB island cannot be created for %s node %s\n",island,nodename);
		sprintf(database_csv_path, "/dev/null");
	}else{
		sprintf(database_csv_path, "%s/%s.csv", island, nodename);
	}
	umask(old_mode);

	verbose(0, "Using DB plain-text file: %s", database_csv_path);
}

#else
#define form_database_paths()
#define report_application_in_file(x)
#endif




/*
*
*
*	BASIC FUNCTIONS 
*
*/

void job_init_powermon_app(application_t *new_app,uint from_mpi)
{
	energy_data_t c_energy;
	current_ear_app[ccontext]->job_created=!from_mpi;
	copy_application(&current_ear_app[ccontext]->app,new_app);	
	strcpy(current_ear_app[ccontext]->app.node_id,nodename);
	time(&current_ear_app[ccontext]->app.job.start_time);	
	current_ear_app[ccontext]->app.job.start_mpi_time=0;
	current_ear_app[ccontext]->app.job.end_mpi_time=0;
	current_ear_app[ccontext]->app.job.end_time=0;
	// reset signature
	signature_init(&current_ear_app[ccontext]->app.signature);
	init_power_signature(&current_ear_app[ccontext]->app.power_sig);
	current_ear_app[ccontext]->app.power_sig.max_DC_power=0;
	current_ear_app[ccontext]->app.power_sig.min_DC_power=500;
	current_ear_app[ccontext]->app.power_sig.def_f=dyn_conf->def_freq;
	// Initialize energy
	read_enegy_data(&c_energy);
	copy_energy_data(&current_ear_app[ccontext]->energy_init,&c_energy);
	aperf_job_avg_frequency_init_all_cpus();
}


void job_end_powermon_app()
{
	energy_data_t c_energy;
	node_data_t app_total;
	power_data_t app_power;
	double exec_time;
	time(&current_ear_app[ccontext]->app.job.end_time);
	// Get the energy
	read_enegy_data(&c_energy);
	compute_power(&current_ear_app[ccontext]->energy_init,&c_energy,&app_power);
	
    current_ear_app[ccontext]->app.power_sig.DC_power=app_power.avg_dc;
	if (app_power.avg_dc>current_ear_app[ccontext]->app.power_sig.max_DC_power) current_ear_app[ccontext]->app.power_sig.max_DC_power=app_power.avg_dc;
	if (app_power.avg_dc<current_ear_app[ccontext]->app.power_sig.min_DC_power) current_ear_app[ccontext]->app.power_sig.min_DC_power=app_power.avg_dc;
    current_ear_app[ccontext]->app.power_sig.DRAM_power=app_power.avg_dram[0]+app_power.avg_dram[1];
    current_ear_app[ccontext]->app.power_sig.PCK_power=app_power.avg_cpu[0]+app_power.avg_cpu[1];
    current_ear_app[ccontext]->app.power_sig.time=difftime(app_power.end,app_power.begin);
	current_ear_app[ccontext]->app.power_sig.avg_f=aperf_job_avg_frequency_end_all_cpus();

	// nominal is still pending

	// Metrics are not reported in this function
}

void report_powermon_app(powermon_app_t *app)
{
	// We can write here power information for this job
	if (sig_reported==0){
		verbose(0,"Reporting not mpi application");
		app->app.is_mpi=0;
	}

	report_application_data(&app->app);
	report_application_in_file(&app->app);
	
	#if DB_MYSQL
    if (my_cluster_conf.eard.use_mysql)
    {
		if (!my_cluster_conf.eard.use_eardbd) {
			if (!db_insert_application(&app->app))
				debug( "Application signature correctly written");
		}
		else {
			edb_state_t state = eardbd_send_application(&app->app);

			if (edb_state_fail(state)) {
				verbose(0, "Error when sending application to eardb");
				eardbd_reconnect(&my_cluster_conf, my_node_conf, state);
			}
		}
	}
	#endif
}

policy_conf_t per_job_conf;
policy_conf_t *  configure_context(uint user_type, energy_tag_t *my_tag,application_t * appID)
{
	policy_conf_t * my_policy;
	int p_id;
	verbose(0,"configuring policy for user %u policy %s freq %lu th %lf is_learning %u\n",user_type,appID->job.policy,appID->job.def_f,appID->job.th,appID->is_learning);
	switch (user_type){
	case NORMAL:
		appID->is_learning=0;
        	p_id=policy_name_to_id(appID->job.policy);
        	/* Use cluster conf function */
        	if (p_id!=EAR_ERROR){
            	my_policy=get_my_policy_conf(my_node_conf,p_id);
		if (!my_policy->is_available){
				verbose(0,"User type %d is not alloweb to use policy %s",user_type,appID->job.policy);
				my_policy=get_my_policy_conf(my_node_conf,my_cluster_conf.default_policy);
			}
			copy_policy_conf(&per_job_conf,my_policy);
			my_policy=&per_job_conf;
        	}else{
			verbose(0,"Invalid policy %s ",appID->job.policy);
            		my_policy=get_my_policy_conf(my_node_conf,my_cluster_conf.default_policy);
			if (my_policy==NULL){
				verbose(0,"Error Default policy configuration returns NULL,invalid policy, check ear.conf (setting MONITORING)");
				authorized_context.p_state=1;
				authorized_context.policy=MONITORING_ONLY;
				authorized_context.th=0;
			}else{
				copy_policy_conf(&authorized_context,my_policy);
			}
			my_policy=&authorized_context;
        	}
        	if (my_policy==NULL){
        	    verbose(0,"Default policy configuration returns NULL,invalid policy, check ear.conf");
       	     	    my_policy=&default_policy_context;
        	}
		break;

	case AUTHORIZED:
		if (appID->is_learning){
			authorized_context.policy=MONITORING_ONLY;
			if (appID->job.def_f){ 
				if (frequency_is_valid_frequency(appID->job.def_f)) authorized_context.p_state=frequency_freq_to_pstate(appID->job.def_f);
				else authorized_context.p_state=1;
			} else authorized_context.p_state=1;
			
			authorized_context.th=0;
			my_policy=&authorized_context;
		}else{
			p_id=policy_name_to_id(appID->job.policy);
			if (p_id!=EAR_ERROR){
            		my_policy=get_my_policy_conf(my_node_conf,p_id);
			authorized_context.policy=p_id;
			if (appID->job.def_f){ 
					verbose(0,"Setting freq to NOT default policy p_state \n");
					if (frequency_is_valid_frequency(appID->job.def_f)) authorized_context.p_state=frequency_freq_to_pstate(appID->job.def_f);
					else authorized_context.p_state=my_policy->p_state;
				}else{ 
					verbose(0,"Setting freq to default policy p_state %u\n",my_policy->p_state);
					authorized_context.p_state=my_policy->p_state;	
				}
				if (appID->job.th>0) authorized_context.th=appID->job.th;
				else authorized_context.th=my_policy->th;
				my_policy=&authorized_context;
			}else{
				verbose(0,"Authorized user is executing not defined/invalid policy using default %d",my_cluster_conf.default_policy);
				my_policy=get_my_policy_conf(my_node_conf,my_cluster_conf.default_policy);
				print_policy_conf(my_policy);		
				if (my_policy==NULL){
					verbose(0,"Error Default policy configuration returns NULL,invalid policy, check ear.conf (setting MONITORING)");
					authorized_context.p_state=1;
					authorized_context.policy=MONITORING_ONLY;
					authorized_context.th=0;
				}else{
					copy_policy_conf(&authorized_context,my_policy);
				}
				my_policy=&authorized_context;
			}
		}
		break;
	case ENERGY_TAG:
		appID->is_learning=0;
		energy_tag_context.policy=MONITORING_ONLY;
		energy_tag_context.p_state=my_tag->p_state;
		energy_tag_context.th=0;
		my_policy=&energy_tag_context;
		break;
	}
	if ((!appID->is_mpi) && (!my_cluster_conf.eard.force_frequencies)){
		my_policy->p_state=frequency_freq_to_pstate(frequency_get_cpu_freq(0));
		verbose(0,"Application is not using ear and force_frequencies=off, frequencies are not changed pstate=%u\n",my_policy->p_state);
		
	}
	appID->is_mpi=0;
	return my_policy;
}


/*
*
*
*	MPI PART
*
*/
// That functions controls the mpi init/end of jobs . These functions are called by eard when application executes mpi_init/mpi_finalized and contacts eard
static pthread_mutex_t app_lock = PTHREAD_MUTEX_INITIALIZER;

void powermon_mpi_init(application_t * appID)
{
	verbose(2,"powermon_mpi_init job_id %d step_id %d (is_mpi %u)\n",appID->job.id,appID->job.step_id,appID->is_mpi);
	// As special case, we will detect if not job init has been specified
	if (!current_ear_app[ccontext]->job_created){	// If the job is nt submitted through slurm, new_job would not be submitted 
		powermon_new_job(appID,1);
	}
	// MPI_init : It only changes mpi_init time, we don't need to acquire the lock
	start_mpi(&current_ear_app[ccontext]->app.job);
	current_ear_app[ccontext]->app.is_mpi=1;
	save_eard_conf(&eard_dyn_conf);	
}

void powermon_mpi_finalize()
{
	verbose(2,"powermon_mpi_finalize %d[%d]\n",current_ear_app[ccontext]->app.job.id,current_ear_app[ccontext]->app.job.step_id);
	end_mpi(&current_ear_app[ccontext]->app.job);
	if (!current_ear_app[ccontext]->job_created){  // If the job is not submitted through slurm, end_job would not be submitted 
		powermon_end_job(current_ear_app[ccontext]->app.job.id,current_ear_app[ccontext]->app.job.step_id);
	}
}

/*
*
*
*   JOB PART
*
*
/


/* This functiono is called by dynamic_configuration thread when a new_job command arrives */

void new_batch()
{
	ccontext++;
	if (ccontext==MAX_NESTED_LEVELS){
		verbose(0,"Panic: Maximum number of levels reached in new_job %d\n",ccontext);
	}
	current_ear_app[ccontext]=(powermon_app_t*)malloc(sizeof(powermon_app_t));
	if (current_ear_app[ccontext]==NULL){
		verbose(0,"Panic: malloc returns NULL for current context");
	}
	memset(current_ear_app[ccontext],0,sizeof(powermon_app_t));
	verbose(0,"New context created %d\n",ccontext);
}
void end_batch()
{
	verbose(0,"Releasing context %d\n",ccontext);
	free(current_ear_app[ccontext]->governor.governor);
	free(current_ear_app[ccontext]);
	current_ear_app[ccontext]=NULL;
	ccontext--;
}

void powermon_new_job(application_t* appID,uint from_mpi)
{
    // New application connected
	int p_id;
	uint new_app_id=create_ID(appID->job.id,appID->job.step_id);
	energy_tag_t *my_tag;
	policy_conf_t *my_policy;
	ulong f;
	uint user_type;
	verbose(2,"powermon_new_job (%d,%d)\n",appID->job.id,appID->job.step_id);
	new_batch();
	/* Saving the context */
	current_ear_app[ccontext]->current_freq=frequency_get_cpu_freq(0);
	get_governor(&current_ear_app[ccontext]->governor);
	verbose(0,"Saving governor %s\n",current_ear_app[ccontext]->governor.governor);
	/* Setting userspace */
	frequency_set_userspace_governor_all_cpus();
	user_type=get_user_type(&my_cluster_conf,appID->job.energy_tag,appID->job.user_id,appID->job.group_id,appID->job.user_acc,&my_tag);
	verbose(0,"New job USER type is %u",user_type);
	if (my_tag!=NULL) print_energy_tag(my_tag);
	/* Given a user type, application, and energy_tag, my_policy is the cofiguration for this user and application */
	my_policy=configure_context(user_type, my_tag, appID);
	verbose(1,"Node configuration for policy %u p_state %d th %lf",my_policy->policy,my_policy->p_state,my_policy->th);
	/* Updating info in shared memory region */
	f=frequency_pstate_to_freq(my_policy->p_state);
	dyn_conf->id=new_app_id;
	dyn_conf->user_type=user_type;
	if (user_type==AUTHORIZED) dyn_conf->learning=appID->is_learning;
	else dyn_conf->learning=0;
	dyn_conf->lib_enabled=(user_type!=ENERGY_TAG);
	dyn_conf->policy=my_policy->policy;
	dyn_conf->def_freq=f;
	dyn_conf->def_p_state=my_policy->p_state;
	dyn_conf->th=my_policy->th;
	/* End app configuration */
	frequency_set_all_cpus(f);
	current_node_freq=f;
	appID->job.def_f=dyn_conf->def_freq;	
    	while (pthread_mutex_trylock(&app_lock));
    	idleNode=0;
    	job_init_powermon_app(appID,from_mpi);
	/* We must report the energy beforesetting the job_id: PENDING */
	new_job_for_period(&current_sample,appID->job.id,appID->job.step_id);
    pthread_mutex_unlock(&app_lock);
	save_eard_conf(&eard_dyn_conf);	
	verbose(1,"Job created jid %u sid %u is_mpi %d\n",current_ear_app[ccontext]->app.job.id,current_ear_app[ccontext]->app.job.step_id,current_ear_app[ccontext]->app.is_mpi);
	verbose(1,"*******************\n");
	sig_reported=0;

}

/* This function is called by dynamic_configuration thread when a end_job command arrives */

void powermon_end_job(job_id jid,job_id sid)
{
    // Application disconnected
    powermon_app_t summary;
    char buffer[128];
	if ((jid!=current_ear_app[ccontext]->app.job.id) || (sid!=current_ear_app[ccontext]->app.job.step_id)){ 
		verbose(0,"powermon_end_job inicorrect jid %d\n",jid);
		return;
	}
	verbose(1,"powermon_end_job %d[%d]\n",current_ear_app[ccontext]->app.job.id,current_ear_app[ccontext]->app.job.step_id);
    while (pthread_mutex_trylock(&app_lock));
        idleNode=1;
        job_end_powermon_app();
		// After that function, the avg power is computed
		memcpy(&summary,current_ear_app[ccontext],sizeof(powermon_app_t));
		// we must report the cur.rent period for that job before the reset:PENDING
		end_job_for_period(&current_sample);
    pthread_mutex_unlock(&app_lock);
    report_powermon_app(&summary);
    save_eard_conf(&eard_dyn_conf);
	/* RESTORE FREQUENCY */
	verbose(0,"restoring governor %s\n",current_ear_app[ccontext]->governor.governor);
	set_governor(&current_ear_app[ccontext]->governor);
	if (strcmp(current_ear_app[ccontext]->governor.governor,"userspace")==0){
		frequency_set_all_cpus(current_ear_app[ccontext]->current_freq);
	}
	current_node_freq=frequency_get_cpu_freq(0);	
	clean_job_environment(jid,sid);
	reset_shared_memory();
	end_batch();
}

/*
* These functions are called by dynamic_configuration thread: Used to notify when a configuracion setting is changed
*
*/
void powermon_inc_th(double th)
{
    policy_conf_t *min_time_p;
    min_time_p=get_my_policy_conf(my_node_conf,MIN_TIME_TO_SOLUTION);
    if (min_time_p==NULL){
        verbose(1,"MIN_TIME_TO_SOLUTION not supported, th setting has no effect");
    }else{
		if (((min_time_p->th+th)>0) && ((min_time_p->th+th)<=1.0)){
        	min_time_p->th=min_time_p->th+th;
		}
    }
    save_eard_conf(&eard_dyn_conf);
}

void powermon_set_th(double th)
{
	policy_conf_t *min_time_p;
	min_time_p=get_my_policy_conf(my_node_conf,MIN_TIME_TO_SOLUTION);
	if (min_time_p==NULL){
		verbose(1,"MIN_TIME_TO_SOLUTION not supported, th setting has no effect");
	}else{
		min_time_p->th=th;
	}
	save_eard_conf(&eard_dyn_conf);
}
/* Sets the maximum frequency in the node */
void powermon_new_max_freq(ulong maxf)
{
	uint ps;
	/* Job running and not EAR-aware */
	if ((ccontext>0) && (current_ear_app[ccontext]->app.job.id>0) && (current_ear_app[ccontext]->app.is_mpi==0)){
		if (maxf<current_node_freq){
			verbose(1,"MaxFreq: Application is not mpi, automatically changing freq from %lu to %lu\n",current_node_freq,maxf);
			frequency_set_all_cpus(maxf);
			current_node_freq=maxf;		
		}
	}
	ps=frequency_freq_to_pstate(maxf);
	verbose(1,"Max pstate set to %u freq=%lu",ps,maxf);
	my_node_conf->max_pstate=ps;
	save_eard_conf(&eard_dyn_conf);
}

void powermon_new_def_freq(uint p_id,ulong def)
{
	int nump;
	uint ps;
	ps=frequency_freq_to_pstate(def);
    if ((ccontext>0) && (current_ear_app[ccontext]->app.is_mpi==0)){
        if (def<current_node_freq){
            verbose(1,"DefFreq: Application is not mpi, automatically changing freq from %lu to %lu\n",current_node_freq,def);
            frequency_set_all_cpus(def);
            current_node_freq=def;
        }
    }
	if (is_valid_policy(p_id)){
		verbose(1,"New default pstate %u for policy %u freq=%lu",ps,my_node_conf->policies[p_id].policy,def);
		my_node_conf->policies[p_id].p_state=ps;
	}else{
		verbose(0,"Invalid policy %u",p_id);
	}
	save_eard_conf(&eard_dyn_conf);
}

/** Reduces the current freq (default and max) based on new values. If application is not mpi, automatically chages the node freq if needed */

void powermon_red_freq(ulong max_freq,ulong def_freq)
{
	int nump;
    uint ps_def,ps_max;
	ps_def=frequency_freq_to_pstate(def_freq);
	ps_max=frequency_freq_to_pstate(max_freq);
	if ((ccontext>0) && (current_ear_app[ccontext]->app.is_mpi==0)){
		if (def_freq<current_node_freq){
			verbose(1,"RedFreq: Application is not mpi, automatically changing freq from %lu to %lu\n",current_node_freq,def_freq);
			frequency_set_all_cpus(def_freq);
			current_node_freq=def_freq;		
		}
	}
	my_node_conf->max_pstate=ps_max;
    for (nump=0;nump<my_node_conf->num_policies;nump++){
        verbose(1,"New default pstate %u for policy %u freq=%lu",ps_def,my_node_conf->policies[nump].policy,def_freq);
        my_node_conf->policies[nump].p_state=ps_def;
    }
	save_eard_conf(&eard_dyn_conf);
}

/** Sets temporally the default and max frequency to the same value. When application is not mpi, automatically chages the node freq if needed */

void powermon_set_freq(ulong freq)
{
    int nump;
    uint ps;
    ps=frequency_freq_to_pstate(freq);
    if ((ccontext>0) && (current_ear_app[ccontext]->app.is_mpi==0)){
        if (freq!=current_node_freq){
            verbose(1,"SetFreq: Application is not mpi, automatically changing freq from %lu to %lu\n",current_node_freq,freq);
            frequency_set_all_cpus(freq);
            current_node_freq=freq;
        }
    }
    my_node_conf->max_pstate=ps;
    for (nump=0;nump<my_node_conf->num_policies;nump++){
        verbose(1,"New default pstate %u for policy %u freq=%lu",ps,my_node_conf->policies[nump].policy,freq);
        my_node_conf->policies[nump].p_state=ps;
    }
	save_eard_conf(&eard_dyn_conf);
}

/** it reloads ear.conf */
void powermon_reload_conf()
{
}


// Each sample is processed by this function
void update_historic_info(power_data_t *my_current_power,ulong avg_f)
{
	ulong jid,mpi;
	double maxpower,minpower;
	if (ccontext>0){ 
		jid=current_ear_app[ccontext]->app.job.id;
		mpi=current_ear_app[ccontext]->app.is_mpi;
		maxpower=current_ear_app[ccontext]->app.power_sig.max_DC_power;
		minpower=current_ear_app[ccontext]->app.power_sig.min_DC_power;
	} else{ 
		jid=0;
		mpi=0;
		maxpower=minpower=0;
	}
	verbose(0,"ID %u MPI=%u agv_f %lu Current power %lf max %lf min %lf uncore_freqs(%.2lf,%.2lf)\n",
		jid,mpi,avg_f,my_current_power->avg_dc,maxpower,minpower,
		((double)uncore_freq[0]/(double)(f_monitoring*2400000000)),
		((double)uncore_freq[1]/(double)(f_monitoring*2400000000)));
	
	while (pthread_mutex_trylock(&app_lock));
	
	if ((ccontext>0) && (current_ear_app[ccontext]->app.job.id>0)){
		if (my_current_power->avg_dc>maxpower) 
			current_ear_app[ccontext]->app.power_sig.max_DC_power=my_current_power->avg_dc;
		if (my_current_power->avg_dc<minpower)
			current_ear_app[ccontext]->app.power_sig.min_DC_power=my_current_power->avg_dc;
	}

	pthread_mutex_unlock(&app_lock);
		
    report_periodic_power(fd_periodic, my_current_power);

	current_sample.start_time=my_current_power->begin;
	current_sample.end_time=my_current_power->end;
	current_sample.DC_energy=my_current_power->avg_dc*(ulong)difftime(my_current_power->end,my_current_power->begin);
	last_power_reported=my_current_power->avg_dc;
	
	#if DEMO
	current_sample.avg_f=avg_f;
	#endif	

	#if SYSLOG_MSG
	if ((my_current_power->avg_dc==0) || (my_current_power->avg_dc< MIN_SIG_POWER) || (my_current_power->avg_dc>MAX_SIG_POWER)){
		syslog(LOG_DAEMON|LOG_ERR,"Node %s report %.2lf as avg power in last period\n",nodename,my_current_power->avg_dc);
	}
	#endif
	#if DB_MYSQL
	if (my_cluster_conf.eard.use_mysql)
	{
		if (!my_cluster_conf.eard.use_eardbd) {
			/* current sample reports the value of job_id and step_id active at this moment */
			/* If we want to be strict, we must report intermediate samples at job start and job end */
    		if (!db_insert_periodic_metric(&current_sample))
    			debug( "Periodic power monitoring sample correctly written");
		} else {
			edb_state_t state = eardbd_send_periodic_metric(&current_sample);

			if (edb_state_fail(state)) {
				verbose(0, "Error when sending periodic power metric to eardb");
				state = eardbd_reconnect(&my_cluster_conf, my_node_conf, state);

				if (edb_state_fail(state)) {
					verbose(0, "Error re-connecting with EARDB errnum:%d errmsg:%s\n",
						intern_error_num, intern_error_str);
				}
			}
		}
	}
	#endif

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
		verbose(0," Error creating output file %s\n",strerror(errno));
	}else{
		verbose(2," Created job power monitoring  file %s\n",output_name);
	}	
    sprintf(output_name,"%s/%s.pm_periodic_data.txt",ear_tmp,nodename);
    fd_periodic=open(output_name,O_WRONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd_periodic<0){
        fd_periodic=open(output_name,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    }
    if (fd_periodic<0){
        verbose(0," Error creating output file for periodic monitoring %s\n",strerror(errno));
    }else{
        verbose(2," Created power monitoring file for periodic information %s\n",output_name);
    }
	umask(my_mask);
}

void powermon_mpi_signature(application_t *app)
{
	if (ccontext<0){
		verbose(0,"Panic,powermon_mpi_signature and not current context \n");
	}
    signature_copy(&current_ear_app[ccontext]->app.signature,&app->signature);
    current_ear_app[ccontext]->app.job.def_f=app->job.def_f;
    current_ear_app[ccontext]->app.job.th=app->job.th;
    current_ear_app[ccontext]->app.job.procs=app->job.procs;
    strcpy(current_ear_app[ccontext]->app.job.policy,app->job.policy);
    strcpy(current_ear_app[ccontext]->app.job.app_id,app->job.app_id);
	sig_reported=1;
	save_eard_conf(&eard_dyn_conf);	
}

/*
*	MAIN POWER MONITORING THREAD
*/
	

// frequency_monitoring will be expressed in usecs
void *eard_power_monitoring(void *noinfo)
{
	ulong avg_f;
	energy_data_t e_begin;
	energy_data_t e_end;	
	time_t t_begin,t_end; 
	double t_diff;
	power_data_t my_current_power;

	PM_set_sigusr1();
	form_database_paths();

	memset((void *)&default_app,0,sizeof(powermon_app_t));

	#if REPORT_UNCORE
	state_t st= frequency_uncore_init(2, 24, 85);
	verbose(0,"frequency_uncore_init returns %d",st);
	#endif	

	verbose(2," power monitoring thread created\n");
	if (init_power_ponitoring()!=EAR_SUCCESS) {
		verbose(0," Error in init_power_ponitoring\n");
	}
	// current_sample is the current powermonitoring period
	init_periodic_metric(&current_sample);
		
	create_powermon_out();

	// We will collect and report avg power until eard finishes
	// Get time and Energy
	read_enegy_data(&e_begin);
	aperf_periodic_avg_frequency_init_all_cpus();
	#if REPORT_UNCORE
	frequency_uncore_counters_start();
	#endif
	#if SYSLOG_MSG
	openlog("eard",LOG_PID|LOG_PERROR,LOG_DAEMON);
	#endif

	/*
	*	MAIN LOOP
	*/


	while(!eard_must_exit)
	{
		// Wait for N usecs
		sleep(f_monitoring);
		
		// Get time and Energy
		read_enegy_data(&e_end);
		avg_f=aperf_periodic_avg_frequency_end_all_cpus();
		#if REPORT_UNCORE
		frequency_uncore_counters_stop(uncore_freq);
		frequency_uncore_counters_start();
		#endif
		
		// Compute the power
		compute_power(&e_begin,&e_end,&my_current_power);
		
		// Save current power
		update_historic_info(&my_current_power,avg_f);


		// Set values for next iteration
		copy_energy_data(&e_begin,&e_end);
		
		t_begin=t_end;
	}
	#if REPORT_UNCORE
	st = frequency_uncore_dispose();
	verbose(0,"frequency_uncore_dispose returns %d",st);
	#endif


	pthread_exit(0);
	//exit(0);
}

void powermon_get_status(status_t *my_status)
{
	int i;
	my_status->power=(uint)last_power_reported;
	for (i=0;i<TOTAL_POLICIES;i++){
		my_status->policy_conf[i].pstate=(uint)my_node_conf->policies[i].p_state;
		my_status->policy_conf[i].th=(uint)(my_node_conf->policies[i].th*100.0);
	}
}

int print_powermon_app_fd_binary(int fd,powermon_app_t *app)
{
	print_application_fd_binary(fd,&app->app);
	write(fd,&app->job_created,sizeof(uint));
	print_energy_data_fd_binary(fd,&app->energy_init);
		
}
int read_powermon_app_fd_binary(int fd,powermon_app_t *app)
{
	read_application_fd_binary(fd,&app->app);
	read(fd,&app->job_created,sizeof(uint));
	read_energy_data_fd_binary(fd,&app->energy_init);
}

void print_powermon_app(powermon_app_t *app)
{
	print_application(&app->app);
	printf("job created %d\n",app->job_created);
	print_energy_data(&app->energy_init);
}

