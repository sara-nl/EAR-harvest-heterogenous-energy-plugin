#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <global_manager/eargm_rapi.h>
#include <common/types/configuration/cluster_conf.h>

void usage(char *app)
{
	printf("usage:%s num_nodes \n",app);
	exit(1);
}

cluster_conf_t my_cluster;

void main(int argc,char *argv[])
{
	int eargms;
	unsigned int num_nodes;
	char myhost[NAME_SIZE],my_ear_conf_path[NAME_SIZE];
	if (argc!=2) usage(argv[0]);
	num_nodes=(unsigned int)atoi(argv[1]);
	// NEW_JOB
    if (get_ear_conf_path(my_ear_conf_path)==EAR_ERROR){
        verbose(0,"Error opening ear.conf file, not available at regular paths (/etc/ear/ear.conf or $EAR_INSTALL_PATH/etc/sysconf/ear.conf)");
        exit(0);
    }

	read_cluster_conf(my_ear_conf_path,&my_cluster);
	#if API_DEBUG
	fprintf(stderr,"Using port %u\n",my_cluster.eargm.port);
	#endif
	eargms=eargm_connect(my_cluster.eargm.host,my_cluster.eargm.port);
	if(eargms<0){ 
		fprintf(stderr,"Connection error\n");
		exit(1);
	}
	if (!eargm_new_job(num_nodes)) fprintf(stderr,"eargm_new_job error sending new_job command\n");
	eargm_disconnect();
}
