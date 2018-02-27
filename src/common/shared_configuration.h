/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _SHARED_CONF_H
#define _SHARED_CONF_H
#include <common/types/generic.h>

typedef struct ear_conf{
	ulong 	max_freq;
	double 	th;
	int 	force_rescheduling;
} ear_conf_t;

ear_conf_t * create_ear_conf_shared_area(char * ear_conf_path, ulong max_freq);
ear_conf_t * attach_ear_conf_shared_area(char * ear_conf_path);
void dettach_ear_conf_shared_area();
void ear_conf_shared_area_dispose(char * ear_conf_path);



#else
#endif
