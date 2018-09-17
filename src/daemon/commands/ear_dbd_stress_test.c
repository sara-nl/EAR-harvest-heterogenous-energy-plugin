/**************************************************************
 * *   Energy Aware Runtime (EAR)
 * *   This program is part of the Energy Aware Runtime (EAR).
 * *
 * *   EAR provides a dynamic, transparent and ligth-weigth solution for
 * *   Energy management.
 * *
 * *       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017
 * *   BSC Contact     mailto:ear-support@bsc.es
 * *   Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * *   EAR is free software; you can redistribute it and/or
 * *   modify it under the terms of the GNU Lesser General Public
 * *   License as published by the Free Software Foundation; either
 * *   version 2.1 of the License, or (at your option) any later version.
 * *
 * *   EAR is distributed in the hope that it will be useful,
 * *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 * *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * *   Lesser General Public License for more details.
 * *
 * *   You should have received a copy of the GNU Lesser General Public
 * *   License along with EAR; if not, write to the Free Software
 * *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * *   The GNU LEsser General Public License is contained in the file COPYING
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <database_cache/eardbd_api.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/types/periodic_metric.h>
cluster_conf_t my_cluster_conf;
char *eardbd,*eardbd_sec;
int num_events;

void stress_eardbd()
{
	my_node_conf_t  my_node_conf;
	periodic_metric_t current_sample;
	int my_events=0;
	int ret1;
	strcpy(my_node_conf.db_ip,eardbd);
	strcpy(my_node_conf.db_sec_ip,eardbd_sec);
	if (eardbd_connect(&my_cluster_conf, &my_node_conf)!=EAR_SUCCESS){
		printf("Error connecting with %s\n",eardbd);
		exit(0);
	}
	current_sample.job_id=0;
	current_sample.step_id=0;
	current_sample.start_time=0;
	current_sample.end_time=0;
	current_sample.DC_energy=1000;
	strcpy(current_sample.node_id,"test_node");
	for (my_events=0;my_events<num_events;my_events++){
		printf("Process %d sending event %d to %s\n",getpid(),my_events,eardbd);
		if ((ret1=eardbd_send_periodic_metric(&current_sample))!=EAR_SUCCESS){	
			printf("Error process %d sending synthetic periodic metric to EARDBD %s\n",getpid(),eardbd);
		}
		sleep(1);
	}
	eardbd_disconnect();
	exit(0);
}
void Usage(char *app)
{
	printf("Usage: %s num_procs eardbd eardbdsec num_events\n",app);
	exit(0);
}
void main(int argc,char *argv[])
{
	int num_procs=1;
	int i,pid;
    char ear_path[256];
    if (get_ear_conf_path(ear_path)==EAR_ERROR){
            printf("Error getting ear.conf path\n");
            exit(0);
    }
    read_cluster_conf(ear_path,&my_cluster_conf);

	if (argc!=5) Usage(argv[0]);
	num_procs=atoi(argv[1]);
	eardbd=argv[2];
	eardbd_sec=argv[3];
	num_events=atoi(argv[4]);
	printf("Creating %d processes to stress eardbd at node %s,%s\n",num_procs,eardbd,eardbd_sec);
	for (i=0;i<num_procs;i++){
		pid=fork();
		if (pid==0)	stress_eardbd();
		else if (pid<0) exit(0);
	}
	while(waitpid(-1,NULL,0)>0);
}
	
