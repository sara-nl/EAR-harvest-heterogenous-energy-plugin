/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <common/types/application.h>
#include <common/types/projection.h>
#include <library/common/externs.h>

static const char *__NAME__ = "Monitoring";
void monitoring_init()
{
}
void monitoring_new_loop()
{
}
void monitoring_end_loop()
{
}
ulong monitoring_policy(application_t *sig)
{
	return ear_frequency;
}
ulong monitoring_policy_ok(projection_t *proj, application_t *curr_sig, application_t *last_sig)
{
	return 1;
}


