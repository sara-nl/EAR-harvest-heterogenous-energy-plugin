#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <common/config.h>
#include <daemon/eard_rapi.h>
#include <common/types/configuration/cluster_conf.h>

static const char *__NAME__ = "set_freq->eard";
cluster_conf_t my_cluster_conf;

#define NAME_SIZE 128
int EAR_VERBOSE_LEVEL=1;

void usage(char *app)
{
	printf("usage:%s new_freq (in KHz)\n",app);
	exit(1);
}

#define ID_SIZE 64

void main(int argc,char *argv[])
{
	int eards;
	unsigned long f;
	
	char myhost[NAME_SIZE],my_ear_conf_path[NAME_SIZE];
	if (argc!=2) usage(argv[0]);
	f=atol(argv[1]);
	// SET FREQ
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
	fprintf(stdout,"Setting the frequency in all the nodes to %lu\n",f);

	if (!eards_set_freq(f)) fprintf(stderr,"ear_set_freq error sending eards_set_freq command\n");
	eards_remote_disconnect();
}
