/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/



/**
*    \file remote_daemon_client.h
*    \brief This file defines the client side of the remote EARD API
*
* 	 Note:All these funcions applies to a single node . Global commands must be applying by sending commands to all nodes. 
*/

#ifndef _REMOTE_CLIENT_API_H
#define _REMOTE_CLIENT_API_H

#include <common/config.h>
#include <common/types/application.h>
#include <common/types/configuraton/cluster_conf.h>

/** Connects with the EARD running in the given nodename. The current implementation supports a single command per connection
*	The sequence must be : connect +  command + disconnect
* 	@param the nodename to connect with
*/
int eards_remote_connect(char *nodename,uint port);

/** Notifies the EARD the job with job_id starts the execution. It is supposed to be used by the EAR slurm plugin
*/
int eards_new_job(application_t *new_job);

/** Notifies the EARD the job with job_id ends the execution. It is supposed to be used by the EAR slurm plugin
*/
int eards_end_job(job_id jid,job_id sid);

/**  Sets freq as the maximim frequency to be used in the node where the API is connected with
*/
int eards_set_max_freq(ulong freq);

/** Sets the frequency of all the cores to freq */
int eards_set_freq(unsigned long freq);

/** Sets temporally the default frequency of all the policies to freq */
int eards_set_def_freq(unsigned long freq);


/**  Reduce the maximum and default freq by the given number of p_states
*/
int eards_red_max_and_def_freq(uint p_states);

/** Restores the configuration originally set in ear.conf (it doesn´t read the file again)*/
int eards_restore_conf();


/** Sets th as the new threashold to be used by the policy. New th must be passed as % th=0.75 --> 75. It is designed to be used by the min_time_to_solution policy
*/
int eards_set_th(ulong th);

/** Increases the current threshold by th units.New th must be passed as % th=0.05 --> 5. It is designed to be used by the min_time_to_solution policy
*/
int eards_inc_th(ulong th);


/** Disconnect from the previously connected EARD
*/
int eards_remote_disconnect();

/** Increases the current threshold to all nodes in my_cluster_conf. */
void increase_th_all_nodes(ulong th, cluster_conf_t my_cluster_conf);

/** Reduces the maximum and default freq to all nodes in my_cluster_conf. */
void red_max_freq(ulong ps, cluster_conf_t my_cluster_conf);

/** Reduces the default freq to all nodes in my_cluster_conf. */
void red_def_freq(ulong ps, cluster_conf_t my_cluster_conf);

/** Reduces the frequency of all nodes in my_cluster_conf.*/
void reduce_frequencies_all_nodes(ulong freq, cluster_conf_t my_cluster_conf);

/** Sets the default frequency for all nodes in my_custer_conf. */
void set_def_freq(ulong freq, cluster_conf_t my_cluster_conf);

/** Restores the default configuration for all nodes in my_cluster_conf. */
void restore_conf(cluster_conf_t my_cluster_conf);

#endif
