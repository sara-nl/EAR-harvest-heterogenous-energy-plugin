#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <global_manager/eargm_rapi.h>
#include <common/types/configuration/cluster_conf.h>

void usage(char *app)
{
	verbose(0, "usage:%s num_nodes", app);
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
    if (get_ear_conf_path(my_ear_conf_path)==EAR_ERROR){
        verbose(0,"Error opening ear.conf file, not available at regular paths (/etc/ear/ear.conf or $EAR_INSTALL_PATH/etc/sysconf/ear.conf)");
        exit(0);
    }
	read_cluster_conf(my_ear_conf_path,&my_cluster);
	#if API_DEBUG
	verbose(0, "Using port %u", my_cluster.eargm.port);
	#endif
	eargms=eargm_connect(my_cluster.eargm.host,my_cluster.eargm.port);
	if(eargms<0){ 
		verbose(0, "Connection error"); //error
		exit(1);
	}
	if (!eargm_end_job(num_nodes)) {
		verbose(0, "eargm_end_job error sending new_job command"); //error
	}
	eargm_disconnect();
}
