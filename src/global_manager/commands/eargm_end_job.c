#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <global_manager/eargm_rapi.h>
#include <common/types/cluster_conf.h>

#define NAME_SIZE 128
int EAR_VERBOSE_LEVEL=0;
static const char *__NAME__ = "end_job->eargmd";

void usage(char *app)
{
	printf("usage:%s num_nodes \n",app);
	exit(1);
}

#define ID_SIZE 64
cluster_conf_t my_cluster;

void main(int argc,char *argv[])
{
	int eargms;
	unsigned int num_nodes;
	char myhost[NAME_SIZE],my_ear_conf_path[NAME_SIZE];
	if (argc!=2) usage(argv[0]);
	num_nodes=(unsigned int)atoi(argv[1]);
	// NEW_JOB
	if (gethostname(myhost,NAME_SIZE)<0){
		fprintf(stderr,"Error getting hostname %s\n",strerror(errno));
		exit(1);
	}
	read_cluster_conf(my_ear_conf_path,&my_cluster);
	fprintf(stderr,"Using port %u\n",my_cluster.eargm.port);
	eargms=eargm_connect(myhost,my_cluster.eargm.port);
	if(eargms<0){ 
		fprintf(stderr,"Connection error\n");
		exit(1);
	}
	if (!eargm_end_job(num_nodes)) fprintf(stderr,"eargm_end_job error sending new_job command\n");
	eargm_disconnect();
}
