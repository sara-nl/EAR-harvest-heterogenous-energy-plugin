/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <common/ear_verbose.h>

extern int eard_must_exit;
unsigned int f_monitoring;
// frequency_monitoring will be expressed in usecs
void *eard_power_monitoring(void *frequency_monitoring)
{
	unsigned int *f_monitoringp=(unsigned int *)frequency_monitoring;
	unsigned long e_begin,e_end;	
	unsigned long t_ms;
	double avg_dc_power;
	time_t t; 
    struct tm *tm; 
    char s[64];

	ear_verbose(0,"power monitoring thread created\n");
	f_monitoring=*f_monitoringp;
	t_ms=f_monitoring/1000;
	// We will collect and report avg power until eard finishes
	read_dc_energy(&e_begin);
	while(!eard_must_exit)
	{
		usleep(f_monitoring);
		read_dc_energy(&e_end);
		avg_dc_power=(e_end-e_begin)/t_ms;
		t = time(NULL);
   		tm = localtime(&t);
    	strftime(s, sizeof(s), "%c", tm);
		e_begin=e_end;
		printf("time %s: avg power: %lf\n",s,avg_dc_power);
	}
	pthread_exit(0);
}
