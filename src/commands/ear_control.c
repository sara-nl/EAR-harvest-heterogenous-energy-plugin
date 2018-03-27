#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/remote_daemon_client.h>

void usage(char *app)
{
	printf("usage:%s nodename freq\n",app);
	exit(1);
}
void main(int argc,char *argv[])
{
	int eards,job_id;
	ulong f;
	if (argc!=3) usage(argv[0]);
	// MAX_FREQ
	f=(ulong)atol(argv[2]);
	printf("-->reducing freq to %lu\n",f);
	eards=eards_remote_connect(argv[1]);
	if(eards<0) printf("Connection error\n");
	eards_set_freq(f);
	eards_remote_disconnect();
	exit(1);
	
}
