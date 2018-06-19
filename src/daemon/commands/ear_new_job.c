#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <common/config.h>
#include <daemon/eard_rapi.h>
#include <common/types/cluster_conf.h>
#include <stdint.h>

#define NAME_SIZE 128
int EAR_VERBOSE_LEVEL=1;
static const char *__NAME__ = "new_job->eard";

void usage(char *app)
{
	printf("usage:%s job_id \n",app);
	exit(1);
}

#define ID_SIZE 64
cluster_conf_t my_cluster_conf;

void main(int argc,char *argv[])
{
	int eards;
	application_t my_job;
	char *id,*sid,*myth,*myuser,*myname,*myacc,*mypolicy;
	char *learning,*p_state;
	char myhost[NAME_SIZE];
	char my_ear_conf_path[NAME_SIZE];
	// NEW_JOB
	if (gethostname(myhost,NAME_SIZE)<0){
		fprintf(stderr,"Error getting hostname %s\n",strerror(errno));
		exit(1);
	}
    if (get_ear_conf_path(my_ear_conf_path)==EAR_ERROR){
        VERBOSE_N(0,"Error opening ear.conf file, not available at regular paths (/etc/ear/ear.conf or $EAR_INSTALL_PATH/etc/sysconf/ear.conf)");
        exit(0);
    }
    VERBOSE_N(0,"Using %s as EARGM configuration file",my_ear_conf_path);
    if (read_cluster_conf(my_ear_conf_path,&my_cluster_conf)!=EAR_SUCCESS){
        VERBOSE_N(0," Error reading cluster configuration\n");
    }

	eards=eards_remote_connect(myhost,my_cluster_conf.eard.port);
	if(eards<0){ 
		fprintf(stderr,"Connection error\n");
		exit(1);
	}
	id=getenv("SLURM_JOB_ID");
	sid=getenv("SLURM_STEP_ID");
	if ((id==NULL) || (sid==NULL)){
		fprintf(stderr,"ear_new_job error: job id or step id have null values\n");
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
	fprintf(stdout,"ear_new_job: id %d step_id %d appname %s user %s policy %s th %lf\n",
	my_job.job.id,my_job.job.step_id,my_job.job.app_id,my_job.job.user_id,my_job.job.policy,my_job.job.th);
	if (learning!=NULL){
		my_job.is_learning=(uint8_t)atoi(learning);
	}
	if (p_state!=NULL) {
		my_job.job.def_f=(ulong)atoi(p_state);
	}

	if (!eards_new_job(&my_job)) fprintf(stderr,"ear_new_job error sending new_job command\n");
	eards_remote_disconnect();
}
