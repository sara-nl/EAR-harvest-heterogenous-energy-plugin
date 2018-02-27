/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <common/ear_verbose.h>
#include <common/shared_configuration.h>

extern int eard_must_exit;
extern unsigned long eard_max_freq;

void * eard_dynamic_configuration(void *tmp)
{
	char *my_tmp=(char *)tmp;
	ear_conf_t *dyn_conf;
	int change=0;
	ear_verbose(0,"creating shared memory tmp=%s\n",my_tmp);
	dyn_conf=create_ear_conf_shared_area(my_tmp,eard_max_freq);
	if (dyn_conf==NULL){
		ear_verbose(0,"Error creating shared memory\n");
		pthread_exit(0);
	}
	ear_verbose(0,"shared memory created max_freq %lu th %lf resched %d\n",dyn_conf->max_freq,dyn_conf->th,dyn_conf->force_rescheduling);
	while (!eard_must_exit)
	{
		usleep(5000000);
		change++;
		if (change%2) dyn_conf->max_freq=dyn_conf->max_freq-(3*100000);
		else dyn_conf->max_freq=dyn_conf->max_freq+(3*100000);
		dyn_conf->force_rescheduling=1;
		ear_verbose(0,"dynamic_conf: Max freq. set to %lu\n",dyn_conf->max_freq);
	}
	pthread_exit(0);

}
