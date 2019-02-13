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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <common/config.h>
#include <daemon/eard_rapi.h>
#include <common/types/configuration/cluster_conf.h>

void usage(char *app)
{
	printf("usage:%s job_id \n",app);
	exit(1);
}

cluster_conf_t my_cluster_conf;

void main(int argc,char *argv[])
{
	int eards;
	application_t my_job;
	char *id,*sid,*myth,*myuser,*myname,*myacc,*mypolicy;
	char *learning,*p_state;
	char myhost[NAME_SIZE];
	char my_ear_conf_path[NAME_SIZE];

	if (argc==1){
	/*NEW_JOB*/
		if (gethostname(myhost,NAME_SIZE)<0){
			verbose(0, "Error getting hostname %s", strerror(errno)); //error
			exit(1);
		}
	}else{
		strcpy(myhost,argv[1]);
	}
    if (get_ear_conf_path(my_ear_conf_path)==EAR_ERROR){
        verbose(0,"Error opening ear.conf file, not available at regular paths (/etc/ear/ear.conf or $EAR_INSTALL_PATH/etc/sysconf/ear.conf)");
        exit(0);
    }
    verbose(2,"Using %s as EARD configuration file",my_ear_conf_path);
    if (read_cluster_conf(my_ear_conf_path,&my_cluster_conf)!=EAR_SUCCESS){
        verbose(0," Error reading cluster configuration\n");
    }

	eards=eards_remote_connect(myhost,my_cluster_conf.eard.port);
	if(eards<0){ 
		verbose(0, "Connection error"); //error
		exit(1);
	}
	verbose(2,"Connected to host %s",myhost);
	id=getenv("SLURM_JOB_ID");
	sid=getenv("SLURM_STEP_ID");
	if ((id==NULL) || (sid==NULL)){
		verbose(0,"ear_new_job error: job id or step id have null values"); //error
		exit(1);
	}
	my_job.job.id=atoi(id);
	my_job.job.step_id=atoi(sid);

	myuser=getenv("LOGNAME");
	myname=getenv("SLURM_JOB_NAME");
	myacc=getenv("SLURM_JOB_ACCOUNT");
	mypolicy=getenv("EAR_POWER_POLICY");
	myth=getenv("EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN");
	learning=getenv("EAR_LEARNING_PHASE");
	p_state=getenv("EAR_P_STATE");	

	if (myuser==NULL) 	strcpy(my_job.job.user_id,"");
	else				strcpy(my_job.job.user_id,myuser);

	if (myname==NULL) 	strcpy(my_job.job.app_id,"");
	else 				strcpy(my_job.job.app_id,myname);

	if (myacc==NULL) 	strcpy(my_job.job.user_acc,"");
	else 				strcpy(my_job.job.user_acc,myacc);

	if (mypolicy==NULL)	strcpy(my_job.job.policy,"");
	else 				strcpy(my_job.job.policy,mypolicy);

	if (myth==NULL) my_job.job.th=0;
	else my_job.job.th=my_job.job.th=strtod(myth,NULL);
	#if API_DEBUG
	verbose(0, "ear_new_job: id %d step_id %d appname %s user %s policy %s th %lf",
	my_job.job.id,my_job.job.step_id,my_job.job.app_id,my_job.job.user_id,my_job.job.policy,my_job.job.th);
	#endif
	if (learning!=NULL){
		my_job.is_learning=(uint8_t)atoi(learning);
	}else{
		my_job.is_learning=0;
	}
	if (p_state!=NULL) {
		my_job.job.def_f=(ulong)atoi(p_state);
	}

	if (!eards_new_job(&my_job)) {
		verbose(0,"ear_new_job error sending new_job command"); //error
	}
	eards_remote_disconnect();
}
