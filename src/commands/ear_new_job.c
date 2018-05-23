#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <common/config.h>
#include <common/remote_daemon_client.h>

#define NAME_SIZE 128

void usage(char *app)
{
	printf("usage:%s job_id \n",app);
	exit(1);
}

#define ID_SIZE 64

void main(int argc,char *argv[])
{
	int eards;
	application_t my_job;
	char *id,*sid,*myth,*myuser,*myname,*myacc,*mypolicy;
	char myhost[NAME_SIZE];
	// NEW_JOB
	if (gethostname(myhost,NAME_SIZE)<0){
		fprintf(stderr,"Error getting hostname %s\n",strerror(errno));
		exit(1);
	}
	eards=eards_remote_connect(myhost);
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

	if (!eards_new_job(&my_job)) fprintf(stderr,"ear_new_job error sending new_job command\n");
	eards_remote_disconnect();
}
