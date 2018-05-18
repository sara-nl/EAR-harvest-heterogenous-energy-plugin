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

void main(int argc,char *argv[])
{
	int eards,job_id;
	application_t my_job;
	char myhost[NAME_SIZE];
	if (argc!=2) usage(argv[0]);
	job_id=atoi(argv[1]);
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
	my_job.job.id=job_id;
	my_job.job.step_id=0;
	eards_new_job(&my_job);
	eards_remote_disconnect();
}
