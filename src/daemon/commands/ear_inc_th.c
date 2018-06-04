#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <common/config.h>
#include <daemon/eard_rapi.h>

#define NAME_SIZE 128
int EAR_VERBOSE_LEVEL=1;
void usage(char *app)
{
	printf("usage:%s perc to be added to current PerformanceEfficiencyGain (5 means 5%)\n",app);
	exit(1);
}

#define ID_SIZE 64

void main(int argc,char *argv[])
{
	int eards;
	ulong p;
	
	char myhost[NAME_SIZE];
	if (argc!=2) usage(argv[0]);
	p=atol(argv[1]);
	// SET FREQ
	if (gethostname(myhost,NAME_SIZE)<0){
		fprintf(stderr,"Error getting hostname %s\n",strerror(errno));
		exit(1);
	}
	eards=eards_remote_connect(myhost);
	if(eards<0){ 
		fprintf(stderr,"Connection error\n");
		exit(1);
	}
	fprintf(stdout,"Increasing the PerformanceEfficiencyGain in all the nodes by %lu\n",p);

	if (!eards_inc_th(p)) fprintf(stderr,"eards_inc_th error sending eards_inc_th command\n");
	eards_remote_disconnect();
}
