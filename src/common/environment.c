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



#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <common/environment.h>

char *conf_ear_tmp=NULL;
char *conf_ear_db_pathname=NULL;
char *conf_ear_user_db_pathname=NULL;
char *conf_ear_gui_pathname=NULL;
char *conf_ear_coeff_db_pathname=NULL;
char *conf_ear_app_name=NULL;
int conf_ear_learning_phase=DEFAULT_LEARNING_PHASE;
int conf_ear_power_policy=-1;
double conf_ear_power_policy_th=0.0;
int conf_ear_reset_freq=DEFAULT_RESET_FREQ;
unsigned long conf_ear_p_state=DEFAULT_MAX_P_STATE;
unsigned long conf_ear_def_freq=0;
double conf_ear_performance_accuracy=DEFAULT_PERFORMANCE_ACURACY;
int conf_ear_verbose=DEFAULT_VERBOSE;
int conf_ear_use_turbo=USE_TURBO;
int conf_ear_local_id=-1;
int conf_ear_num_nodes=0;
int conf_ear_total_processes=1;
int conf_ear_dynais_levels=DEFAULT_DYNAIS_LEVELS;
int conf_ear_dynais_window_size=DEFAULT_DYNAIS_WINDOW_SIZE;


# define __USE_GNU
# include <dlfcn.h>
# undef  __USE_GNU
static int (*my_omp_get_max_threads)(void) = NULL;


void set_ear_total_processes(int procs)
{
	conf_ear_total_processes=procs;
}
int get_ear_total_processes()
{
	return conf_ear_total_processes;
}

char * getenv_ear_tmp()
{
	char *my_ear_tmp;
	my_ear_tmp=getenv("EAR_TMP");
	if (my_ear_tmp==NULL){
		my_ear_tmp=getenv("TMP");
		//??
		if (my_ear_tmp==NULL) my_ear_tmp=getenv("HOME");
	}
	conf_ear_tmp=malloc(strlen(my_ear_tmp)+1);
	strcpy(conf_ear_tmp,my_ear_tmp);
	return conf_ear_tmp;	
}
char *getenv_ear_db_pathname()
{
	char *my_ear_db_pathname = getenv("EAR_DB_PATHNAME");

	if (my_ear_db_pathname != NULL && strcmp(my_ear_db_pathname,"") != 0)
	{
		conf_ear_db_pathname=malloc(strlen(my_ear_db_pathname)+1);
		strcpy(conf_ear_db_pathname,my_ear_db_pathname);
	}

	return conf_ear_db_pathname;
}

char *getenv_ear_user_db_pathname()
{
	char *my_user_db_pathname = getenv("EAR_USER_DB_PATHNAME");

	if (my_user_db_pathname != NULL && strcmp(my_user_db_pathname, "") != 0)
	{
		conf_ear_user_db_pathname=malloc(strlen(my_user_db_pathname) + 1);
		strcpy(conf_ear_user_db_pathname,my_user_db_pathname);
	}

	return conf_ear_user_db_pathname;
}

char *getenv_ear_gui_pathname()
{
	char *my_gui_path;
	my_gui_path=getenv("EAR_GUI_PATH");
	if (my_gui_path==NULL){
		my_gui_path=getenv("HOME");	
	}
	conf_ear_gui_pathname=malloc(strlen(my_gui_path)+1);
	strcpy(conf_ear_gui_pathname,my_gui_path);
	return conf_ear_gui_pathname;
}
char *getenv_ear_coeff_db_pathname()
{
	char *my_coeff_db;
	char *install_path;
	my_coeff_db=getenv("EAR_COEFF_DB_PATHNAME");
	if (my_coeff_db==NULL){
		install_path=getenv("EAR_INSTALL_PATH");
		if (install_path==NULL) return NULL;
		my_coeff_db=malloc(strlen(install_path)+strlen(DEFAULT_COEFF_PATHNAME)+2);
		sprintf(my_coeff_db,"%s/%s",install_path,DEFAULT_COEFF_PATHNAME);	
	}
	conf_ear_coeff_db_pathname=malloc(strlen(my_coeff_db)+1);
	strcpy(conf_ear_coeff_db_pathname,my_coeff_db);
	return conf_ear_coeff_db_pathname;
}
char *getenv_ear_app_name()
{
	char *my_name;
	my_name=getenv("EAR_APP_NAME");
	if (my_name!=NULL){
		if (strcmp(my_name,"")==0) return NULL;
		conf_ear_app_name=malloc(strlen(my_name)+1);
		strcpy(conf_ear_app_name,my_name);	
	}
	return conf_ear_app_name;
}
void set_ear_app_name(char *name)
{
	if (conf_ear_app_name!=NULL) free(conf_ear_app_name);
	conf_ear_app_name=malloc(strlen(name)+1);
	strcpy(conf_ear_app_name,name);
}
int getenv_ear_learning_phase()
{
	char *learning=getenv("EAR_LEARNING_PHASE");
	if (learning!=NULL){
		if (strcmp(learning,"1")==0) conf_ear_learning_phase=1;
	}
	return conf_ear_learning_phase;
}
int getenv_ear_power_policy()
{
	char *my_policy;
	conf_ear_power_policy=DEFAULT_POWER_POLICY;
	my_policy=getenv("EAR_POWER_POLICY");
	if (my_policy!=NULL){
		if ((strcmp(my_policy,"MIN_ENERGY_TO_SOLUTION")==0) || (strcmp(my_policy,"min_energy_to_solution")==0)) conf_ear_power_policy=MIN_ENERGY_TO_SOLUTION;
		else if ((strcmp(my_policy,"MIN_TIME_TO_SOLUTION")==0) || (strcmp(my_policy,"min_time_to_solution")==0)) conf_ear_power_policy=MIN_TIME_TO_SOLUTION;
		else if ((strcmp(my_policy,"MONITORING_ONLY")==0) || (strcmp(my_policy,"monitoring_only")==0)) conf_ear_power_policy=MONITORING_ONLY;
	}	
	return conf_ear_power_policy;
}
// power_policy must be read before, but we check it
double getenv_ear_power_policy_th()
{
	char *my_th;
	if (conf_ear_power_policy<0) getenv_ear_power_policy();
	switch (conf_ear_power_policy){	
		case MIN_ENERGY_TO_SOLUTION:
			my_th=getenv("EAR_PERFORMANCE_PENALTY");
			if (my_th!=NULL){ 
				conf_ear_power_policy_th=strtod(my_th,NULL);
				if ((conf_ear_power_policy_th>1.0) || (conf_ear_power_policy_th<0)) conf_ear_power_policy_th=DEFAULT_MIN_ENERGY_TH;
			}else conf_ear_power_policy_th=DEFAULT_MIN_ENERGY_TH;
			break;
		case MIN_TIME_TO_SOLUTION:
			my_th=getenv("EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN");
			if (my_th!=NULL){
				conf_ear_power_policy_th=strtod(my_th,NULL);
				if ((conf_ear_power_policy_th<DEFAULT_MIN_TIME_TH) || (conf_ear_power_policy_th>1.0)) conf_ear_power_policy_th=DEFAULT_MIN_TIME_TH;
			}else conf_ear_power_policy_th=DEFAULT_MIN_TIME_TH;
			break;
		case MONITORING_ONLY:
			conf_ear_power_policy_th=0.0;
			break;
	}
	return conf_ear_power_policy_th;
}
int getenv_ear_reset_freq()
{
	char *my_reset=getenv("EAR_RESET_FREQ");	
	if (my_reset!=NULL){
		if (strcmp(my_reset,"1")==0) conf_ear_reset_freq=1;
	}
	return conf_ear_reset_freq;
}

unsigned long getenv_ear_p_state()
{
	char *my_pstate = getenv("EAR_P_STATE");
	int max_p_state;

	// p_state depends on policy it must be called before getenv_ear_p_state, but we check it
	if (conf_ear_power_policy < 0) getenv_ear_power_policy();

	switch(conf_ear_power_policy)
	{
		case MIN_ENERGY_TO_SOLUTION:
			conf_ear_p_state = DEFAULT_MAX_P_STATE;
			break;
		case MIN_TIME_TO_SOLUTION:
			my_pstate = getenv("EAR_MIN_P_STATE");

			if (my_pstate == NULL) {
				conf_ear_p_state = DEFAULT_MIN_P_STATE;
			} else {
				conf_ear_p_state = atoi(my_pstate);
			}
			break;
		case MONITORING_ONLY:
			my_pstate = getenv("EAR_P_STATE");
			
			if (my_pstate != NULL)
			{
				conf_ear_p_state=atoi(my_pstate);

				// p_state[0] is turbo, p_state[1] is nominal, p_state[max_p_states-1] is the lower freq
				if (!conf_ear_use_turbo) {
					max_p_state=DEFAULT_MAX_P_STATE;
				}

				// Invalid value
				if (conf_ear_p_state < max_p_state) {
					conf_ear_p_state = DEFAULT_MAX_P_STATE;
				}
			} else {
				conf_ear_p_state=DEFAULT_MAX_P_STATE;
			}
			break;
	}
	return conf_ear_p_state;
}

double getenv_ear_performance_accuracy()
{
	char *my_perf_acu=getenv("EAR_PERFORMANCE_ACCURACY");
	if (my_perf_acu!=NULL){
		conf_ear_performance_accuracy=atof(my_perf_acu);
		if (conf_ear_performance_accuracy<0) conf_ear_performance_accuracy=DEFAULT_PERFORMANCE_ACURACY;
	}
	return conf_ear_performance_accuracy;
}
int getenv_ear_verbose()
{
	char *my_verbose;
	my_verbose=getenv("EAR_VERBOSE");
	if (my_verbose!=NULL){
		conf_ear_verbose=atoi(my_verbose);
		if ((conf_ear_verbose<0) || (conf_ear_verbose>4)) conf_ear_verbose=DEFAULT_VERBOSE;
	}	
	return conf_ear_verbose;
}

int getenv_ear_local_id()
{
	char *my_local_id;
	//my_local_id=getenv("SLURM_LOCALID");
	//if (my_local_id!=NULL){ 
	//	conf_ear_local_id=atoi(my_local_id);
	//}
	// if not defined, it is computed later based on the number of total processes and EAR_NUM_NODES
	return conf_ear_local_id;
}

int getenv_ear_num_nodes()
{
	char *my_num_nodes = getenv("EAR_NUM_NODES");

	if (my_num_nodes == NULL)
	{ 
		my_num_nodes = getenv("SLURM_STEP_NUM_NODES");

		if (my_num_nodes == NULL) conf_ear_num_nodes=1;
		else conf_ear_num_nodes = atoi(my_num_nodes);

	} else conf_ear_num_nodes = atoi(my_num_nodes);

	return conf_ear_num_nodes;
}



int getenv_ear_dynais_levels()
{
	char *dynais_levels=getenv("EAR_DYNAIS_LEVELS");
	int dyn_level;
	if (dynais_levels!=NULL){
		dyn_level=atoi(dynais_levels);
		if (dyn_level>0) conf_ear_dynais_levels=dyn_level;
	}
	return conf_ear_dynais_levels;
}
int getenv_ear_dynais_window_size()
{
        char *dynais_size=getenv("EAR_DYNAIS_WINDOW_SIZE");
        int dyn_size;
        if (dynais_size!=NULL){
                dyn_size=atoi(dynais_size);
                if (dyn_size>0) conf_ear_dynais_window_size=dyn_size;
        }
        return conf_ear_dynais_window_size;

}
int get_ear_dynais_levels()
{
	return conf_ear_dynais_levels;
}
int get_ear_dynais_window_size()
{
	return conf_ear_dynais_window_size;
}


// get_ functions must be used after getenv_
char * get_ear_tmp()
{
	return conf_ear_tmp;
}
void set_ear_tmp(char *new_tmp)
{
	if (conf_ear_tmp!=NULL) free(conf_ear_tmp);
	conf_ear_tmp=malloc(strlen(new_tmp)+1);
	strcpy(conf_ear_tmp,new_tmp);
}

char *get_ear_db_pathname()
{
	return conf_ear_db_pathname;
}

char * get_ear_user_db_pathname()
{
	return conf_ear_user_db_pathname;
}
char * get_ear_gui_pathname()
{
	return conf_ear_gui_pathname;
}
char * get_ear_coeff_db_pathname()
{
	return conf_ear_coeff_db_pathname;
}
char * get_ear_app_name()
{
	return conf_ear_app_name;
}
int get_ear_learning_phase()
{
	return conf_ear_learning_phase;
}
int get_ear_power_policy()
{
	return conf_ear_power_policy;
}
double get_ear_power_policy_th()
{
	return conf_ear_power_policy_th;
}
int get_ear_reset_freq()
{
	return conf_ear_reset_freq;
}
unsigned long get_ear_p_state()
{
	return conf_ear_p_state;
}
double get_ear_performance_accuracy()
{
	return conf_ear_performance_accuracy;
}
int get_ear_verbose()
{
	return conf_ear_verbose;
}
void set_ear_verbose(int verb)
{
	conf_ear_verbose=verb;
}
int get_ear_local_id()
{
	if (conf_ear_local_id<0) getenv_ear_local_id();
	return conf_ear_local_id;
}
int get_ear_num_nodes()
{
	if (conf_ear_num_nodes==0) getenv_ear_num_nodes();
	return conf_ear_num_nodes;
}

// This function reads and process environment variable It must be called before using get_ functions
void ear_lib_environment()
{
	getenv_ear_verbose();
	getenv_ear_tmp();
	getenv_ear_db_pathname();
	getenv_ear_user_db_pathname();
	getenv_ear_gui_pathname();
	getenv_ear_coeff_db_pathname();
	getenv_ear_app_name();
	getenv_ear_learning_phase();
	getenv_ear_power_policy();
	getenv_ear_power_policy_th();
	getenv_ear_reset_freq();
	getenv_ear_p_state();
	getenv_ear_performance_accuracy();
	getenv_ear_num_nodes();
	getenv_ear_local_id();
	getenv_ear_dynais_levels();
	getenv_ear_dynais_window_size();
}
// This function writes ear variables in $EAR_TMP/environment.txt file
void ear_print_lib_environment()
{
#ifdef DEBUG
	char *tmp;
	char environ[256];
	char var[256];
	int fd;
	tmp=get_ear_tmp();
	sprintf(environ,"%s/ear_lib_environment.txt",tmp);
	fd=open(environ,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd<0){
		fprintf(stderr,"EAR error reporting environment variables %s\n",strerror(errno));
		return;
	}
	sprintf(var,"EAR_TMP=%s\n",get_ear_tmp());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_VERBOSE=%d\n",get_ear_verbose());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_DB_PATHNAME=%s\n",get_ear_db_pathname());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_USER_DB_PATHNAME=%s\n",get_ear_user_db_pathname());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_COEFF_DB_PATHNAME=%s\n",get_ear_coeff_db_pathname());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_GUI_PATH=%s\n",get_ear_gui_pathname());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_APP_NAME=%s\n",get_ear_app_name());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_LEARNING_PHASE=%d\n",get_ear_learning_phase());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_POWER_POLICY=%d\n",get_ear_power_policy());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_POLICY_TH=%lf\n",get_ear_power_policy_th());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_RESET_FREQ=%d\n",get_ear_reset_freq());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_P_STATE=%lu\n",get_ear_p_state());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_NUM_NODES=%d\n",get_ear_num_nodes());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_PERFORMANCE_ACURACY=%lf\n",get_ear_performance_accuracy());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_DYNAIS_LEVELS=%d\n",get_ear_dynais_levels());
	write(fd,var,strlen(var));
	sprintf(var,"EAR_DYNAIS_WINDOW_SIZE=%d\n",get_ear_dynais_window_size());
	write(fd,var,strlen(var));

	close(fd);	
#endif
}
void ear_daemon_environment()
{
    getenv_ear_verbose();
    getenv_ear_tmp();
    getenv_ear_db_pathname();
}
void ear_print_daemon_environment()
{
#ifdef DEBUG
    char *tmp;
    char environ[256];
    char var[256];
    int fd;
    tmp=get_ear_tmp();
    sprintf(environ,"%s/ear_daemon_environment.txt",tmp);
    fd=open(environ,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd<0){
        fprintf(stderr,"EAR error reporting environment variables %s\n",strerror(errno));
        return;
    }
    sprintf(var,"EAR_TMP=%s\n",get_ear_tmp());
    write(fd,var,strlen(var));
    sprintf(var,"EAR_VERBOSE=%d\n",get_ear_verbose());
    write(fd,var,strlen(var));
    sprintf(var,"EAR_DB_PATHNAME=%s\n",get_ear_db_pathname());
    write(fd,var,strlen(var));
	close(fd);
#endif
}


int check_threads()
{
        my_omp_get_max_threads = (int(*)(void)) dlsym (RTLD_DEFAULT, "mkl_get_num_threads");
        if (my_omp_get_max_threads==NULL){
                my_omp_get_max_threads = (int(*)(void)) dlsym (RTLD_DEFAULT, "omp_get_num_threads");
                if (my_omp_get_max_threads==NULL) return 0;
                else return 1;
        }
        else return 1;
}



int get_num_threads()
{
    int num_th;
    if (my_omp_get_max_threads!=NULL){
        char *omp_numth=getenv("OMP_NUM_THREADS");
        // we check first for openmp
        if (omp_numth!=NULL){
            return atoi(omp_numth);
        }   
        omp_numth=getenv("MKL_NUM_THREADS");
        // we check for MKL
        if (omp_numth!=NULL){
            return atoi(omp_numth);
        }   
    }   
    return 1;
} 

int get_total_resources()
{
	int procs_per_node;

	check_threads();

	procs_per_node = get_ear_total_processes()/get_ear_num_nodes();

	// TODO: When an executable has the OpenMP symbols, the OMP_NUM_THREADS
	// variable is taking into account. But maybe the executable is not using
	// any OMP thread.
	return procs_per_node*get_num_threads();
}

