#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <common/remote_daemon_client.h>

void usage(char *app)
{
	printf("usage:%s job_id \n",app);
	exit(1);
}
#define NAME_SIZE 128
void main(int argc,char *argv[])
{
	int eards,job_id;
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
	eards_new_job(job_id);
	eards_remote_disconnect();

}
