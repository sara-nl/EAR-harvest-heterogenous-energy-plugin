#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <common/config.h>
#include <daemon/eard_command_api.h>

#define NAME_SIZE 128
int EAR_VERBOSE_LEVEL=1;


void main(int argc,char *argv[])
{
	int eards;
	char *id,*sid;
	job_id jid,step_id;
	char myhost[NAME_SIZE];

    id=getenv("SLURM_JOB_ID");
    sid=getenv("SLURM_STEP_ID");
    if ((id==NULL) || (sid==NULL)){
        fprintf(stderr,"ear_new_job error: job id or step id have null values\n");
        exit(1);
    }
    jid=atoi(id);
    step_id=atoi(sid);

    if (gethostname(myhost,NAME_SIZE)<0){
        fprintf(stderr,"Error getting hostname %s\n",strerror(errno));
        exit(1);
    }

	// END_JOB
	eards=eards_remote_connect(myhost);
	if(eards<0){ 
		fprintf(stderr,"Connection error\n");
		exit(1);
	}

	fprintf(stdout,"ear_end_job id %d step_id %d\n",jid,step_id);
	eards_end_job(jid,step_id);
	eards_remote_disconnect();
	exit(0);
}
