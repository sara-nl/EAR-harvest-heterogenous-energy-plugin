/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

/**
*    \file shared_configuration.h
*    \brief This file defines the API to create/attach/dettach/release the shared memory area between the EARD and the EARLib. It is used to communicate the EARLib updates 
*	remotelly requested
*/

#ifndef _SHARED_CONF_H
#define _SHARED_CONF_H
#include <common/types/generic.h>

typedef struct ear_conf{
	ulong 	max_freq;
	double 	th;
	int 	force_rescheduling;
} ear_conf_t;

/** Creates the shared mmemory. It is used by EARD (server)
*	@param ear_conf_path specifies the path (folder) to create the file used by mmap
* 	@param max_freq is used to initialize the default freq. th is set by default to PERFORMANCE_GAIN (compilation option)
*/

ear_conf_t * create_ear_conf_shared_area(char * ear_conf_path, ulong max_freq);

/** Connects with a previously created shared memory region. It is used by EARLib (client)
*	@param ear_conf_path specifies the path (folder) where the mapped file were created
*/
ear_conf_t * attach_ear_conf_shared_area(char * ear_conf_path);

/** Disconnect from a previously connected shared memory region. It is used by EARLib (client)
*/
void dettach_ear_conf_shared_area();

/** Releases a shared memory area previously created. It is used by EARD (server)
*/
void ear_conf_shared_area_dispose(char * ear_conf_path);



#else
#endif
