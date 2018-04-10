/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
/** This program sets the node frequency . It must be executed with privileges
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <control/frequency.h>

int EAR_VERBOSE_LEVEL=1;
void usage(char *app)
{
	printf("usage:%s node_freq (in MHz)\n",app);
	printf("	requires root privileges\n");
	printf("	execute it with sudo\n");
	exit(1);
}
void main(int argc,char *argv[])
{
	int uid;
	ulong f;
	if (argc!=2) usage(argv[0]);
	if (getuid()!=0) usage(argv[0]);
	f=(ulong)atoi(argv[1]);	
	printf("This program will change the governor to userspace and set the node f to %lu\n",f);
	frequency_init();
	frequency_set_userspace_governor_all_cpus();
	if (frequency_set_all_cpus(f)!=f){
		printf("warning, node freq was not changed correctly\n");
	}
	frequency_dispose();
	
}
