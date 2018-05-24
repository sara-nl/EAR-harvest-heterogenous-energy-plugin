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
	printf("usage:%s p_states \n",app);
	exit(1);
}

#define ID_SIZE 64

void main(int argc,char *argv[])
{
	int eards;
	unsigned int p_states;
	
	char myhost[NAME_SIZE];
	if (argc!=2) usage(argv[0]);
	p_states=atol(argv[1]);
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
	fprintf(stdout,"Reducing the frequency in all the nodes by %u p_states\n",p_states);

	if (!eards_red_max_freq(p_states)) fprintf(stderr,"ear_red_freq error sending eards_red_freq command\n");
	eards_remote_disconnect();
}
