/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

/**
*    \file remote_daemon_client.h
*    \brief This file defines the client side of the remote EAR API
*
* 	 Note:Specific functions could be substituted by a generic function passing a local_config_t
*/


#ifndef _REMOTE_CLIENT_API_H
#define _REMOTE_CLIENT_API_H


/** Connects with the EARD running in the given nodename. The current implementation supports a single command per connection
*	The sequence must be : connect +  command + disconnect
* 	@param the nodename to connect with
*/
int eards_remote_connect(char *nodename);

/** Notifies the EARD the job with job_id starts the execution. It is supposed to be used by the EAR slurm plugin
*/
int eards_new_job(int job_id);

/** Notifies the EARD the job with job_id ends the execution. It is supposed to be used by the EAR slurm plugin
*/
int eards_end_job(int job_id);

/**  Sets freq as the frequency to be used in the node where the API is connected with
*/
int eards_set_freq(ulong freq);

/** Sets th as the new threashold to be used by the policy. New th must be passed as % th=0.75 --> 75. It is designed to be used by the min_time_to_solution policy
*/
int eards_set_th(unsigned long th);

/** Disconnect from the previously connected EARD
*/
int eards_remote_disconnect();


#else
#endif
