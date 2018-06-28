#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <common/config.h>
#include <daemon/eard_rapi.h>
#include <common/types/configuration/cluster_conf.h>

#define NAME_SIZE 128
int EAR_VERBOSE_LEVEL=1;
static const char *__NAME__ = "end_job->eard";
cluster_conf_t my_cluster_conf;



void main(int argc,char *argv[])
{
	int eards;
	char *id,*sid;
	job_id jid,step_id;
	char myhost[NAME_SIZE],my_ear_conf_path[NAME_SIZE];

    id=getenv("SLURM_JOB_ID");
    sid=getenv("SLURM_STEP_ID");
    if ((id==NULL) || (sid==NULL)){
        fprintf(stderr,"ear_new_job error: job id or step id have null values\n");
        exit(1);
    }
    jid=atoi(id);
    step_id=atoi(sid);
	if (argc==1){
    	if (gethostname(myhost,NAME_SIZE)<0){
    	    fprintf(stderr,"Error getting hostname %s\n",strerror(errno));
    	    exit(1);
    	}
	}else{
		strcpy(myhost,argv[1]);
	}

    if (get_ear_conf_path(my_ear_conf_path)==EAR_ERROR){
        VERBOSE_N(0,"Error opening ear.conf file, not available at regular paths (/etc/ear/ear.conf or $EAR_INSTALL_PATH/etc/sysconf/ear.conf)");
        exit(0);
    }
    VERBOSE_N(2,"Using %s as eard configuration file",my_ear_conf_path);
    if (read_cluster_conf(my_ear_conf_path,&my_cluster_conf)!=EAR_SUCCESS){
        VERBOSE_N(0," Error reading cluster configuration\n");
    }


	// END_JOB
	eards=eards_remote_connect(myhost,my_cluster_conf.eard.port);
	if(eards<0){ 
		fprintf(stderr,"Connection error\n");
		exit(1);
	}
	#if API_DEBUG
	fprintf(stdout,"ear_end_job id %d step_id %d\n",jid,step_id);
	#endif
	eards_end_job(jid,step_id);
	eards_remote_disconnect();
	exit(0);
}
