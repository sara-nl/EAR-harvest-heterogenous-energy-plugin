/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <metrics/ipmi/energy_node.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int eard_must_exit;
// frequency_monitoring will be expressed in usecs
void *eard_power_monitoring(void *frequency_monitoring)
{
	unsigned int *f_monitoringp=(unsigned int *)frequency_monitoring;
	unsigned int f_monitoring=*f_monitoringp;
	unsigned long begin,end;	
	unsigned long t_ms;
	double avg_dc_power;
	t_ms=f_monitoring/1000;
	// We will collect and report avg power until eard finishes
	read_dc_energy(&begin);
	while(!eard_must_exit)
	{
		usleep(f_monitoring);
		read_dc_energy(&end);
		avg_dc_power=(end-begin)/t_ms;
		printf("avg power: %lf\n",avg_dc_power);
		begin=end;
	}
	pthread_exit(0);
}
