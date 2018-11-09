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
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <common/config.h>
#include <common/types/application.h>
#include <metrics/power_metrics/power_metrics.h>
#include <common/types/configuration/cluster_conf.h>
#include <daemon/eard_checkpoint.h>
char nodename[MAX_PATH_SIZE];
ulong eard_max_pstate=1;

#define NAME_SIZE 128
int EAR_VERBOSE_LEVEL=1;
static const char *__NAME__ = "eard_check_test";
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


void usage(char *app)
{
	printf("Usage: %s nodename\n",app);
	exit(0);
}


cluster_conf_t my_cluster_conf;
my_node_conf_t *my_node_conf;
powermon_app_t my_app;

void main(int argc,char *argv[])
{
    int eards;
    application_t my_job;
    char *id,*sid,*myth,*myuser,*myname,*myacc,*mypolicy;
    char *learning,*p_state;
    char myhost[NAME_SIZE];
    char my_ear_conf_path[NAME_SIZE];

    if (argc==1){
		usage(argv[0]);
    }else{
        strcpy(myhost,argv[1]);
    }   
	strcpy(nodename,myhost);
    if (get_ear_conf_path(my_ear_conf_path)==EAR_ERROR){
        VERBOSE_N(0,"Error opening ear.conf file, not available at regular paths (/etc/ear/ear.conf or $EAR_INSTALL_PATH/etc/sysconf/ear.conf)");
        exit(0);
    }   
    VERBOSE_N(0,"Using %s as EARD configuration file",my_ear_conf_path);
    if (read_cluster_conf(my_ear_conf_path,&my_cluster_conf)!=EAR_SUCCESS){
        VERBOSE_N(0," Error reading cluster configuration\n");
		exit(0);
    }   
	print_cluster_conf(&my_cluster_conf);
	my_node_conf=get_my_node_conf(&my_cluster_conf,myhost);
	if (my_node_conf==NULL){
		VERBOSE_N(0," Error in cluster configuration, node %s not found\n",myhost);
		exit(0);
	}
	print_my_node_conf(my_node_conf);

	my_app.job_created=1;
	my_app.app.is_mpi=1;
	strcpy(my_app.app.node_id,myhost);
	my_app.app.job.id=1;
	my_app.app.job.step_id=10;
	

	eard_dyn_conf_t my_conf;
	my_conf.cconf=&my_cluster_conf;
	my_conf.nconf=my_node_conf;
	my_conf.pm_app=&my_app;

	printf("Saving configuration\n");	
	save_eard_conf(&my_conf);
	printf("Restoring configuration\n");
	restore_eard_conf(&my_conf);

	print_cluster_conf(my_conf.cconf);
	print_my_node_conf(my_conf.nconf);
	print_powermon_app(my_conf.pm_app);
}
