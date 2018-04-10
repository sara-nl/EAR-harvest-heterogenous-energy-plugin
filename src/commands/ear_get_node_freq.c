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
#include <control/frequency.h>
#include <unistd.h>
#include <sys/types.h>
int EAR_VERBOSE_LEVEL=1;


void usage(char *app)
{
	printf("usage:%s num_cpus\n",app);
	exit(1);
}
void main(int argc,char *argv[])
{
	int i;
	ulong cpus;
	if (argc!=2) usage(argv[0]);
	cpus=atoi(argv[1]);
	frequency_init();
	for (i=0;i<cpus;i++){
		printf("CPU freq for cpu %d is %lu\n",i,frequency_get_cpu_freq(i));
	}	
	frequency_dispose();
	
}
