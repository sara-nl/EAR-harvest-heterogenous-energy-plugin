/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

/**
*    \file monitoring.h
*    \brief This file implements the policy API for the monitoring_only policy
*
*/

#ifndef _MONITORING_H
#define _MONITORING_H

void monitoring_init();
void monitoring_new_loop();
void monitoring_end_loop();
ulong monitoring_policy(application_t *sig);
ulong monitoring_policy_ok(projection_t *proj, application_t *curr_sig, application_t *last_sig);


#else
#endif
