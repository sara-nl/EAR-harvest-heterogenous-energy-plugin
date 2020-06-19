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
#include <common/types/risk.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>

#include <daemon/eard_conf_rapi.h>
#include <daemon/eard_rapi_internals.h>

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

/** Sends a local ping */
int eards_ping();

/** Sends a new job request */
int eards_new_job(application_t *new_job);

/** Sends a end job request */
int eards_end_job(job_id jid,job_id sid);

/** gets the policy_configuration for a given policy 
 * */
int eards_get_policy_info(new_policy_cont_t *p);
/** set the policy_configuration for a given policy 
 * */
int eards_set_policy_info(new_policy_cont_t *p);


/** Increases the current threshold to all nodes in my_cluster_conf. */
void increase_th_all_nodes(ulong th, ulong p_id, cluster_conf_t my_cluster_conf);

/** Sets the maximum and default freq to all nodes in my_cluster_conf. */
void set_max_freq_all_nodes(ulong ps, cluster_conf_t my_cluster_conf);

/** Reduces the default freq to all nodes in my_cluster_conf. */
void red_def_max_pstate_all_nodes(ulong ps, cluster_conf_t my_cluster_conf);

/** Reduces the frequecy of all nodes, using set frequenchy. */
void reduce_frequencies_all_nodes(ulong freq, cluster_conf_t my_cluster_conf);

/** Sets the default frequency for all nodes in my_custer_conf. */
void set_def_freq_all_nodes(ulong freq, ulong policy, cluster_conf_t my_cluster_conf);

/** Restores the default configuration for all nodes in my_cluster_conf. */
void restore_conf_all_nodes(cluster_conf_t my_cluster_conf);

/** Executes a simple ping to all nodes */
void ping_all_nodes_propagated(cluster_conf_t my_cluster_conf);

/** Executes a simple ping to all nodes in a sequential manner */
void ping_all_nodes(cluster_conf_t my_cluster_conf);

/** Asks all the nodes for their current status */
int status_all_nodes(cluster_conf_t my_cluster_conf, status_t **status);

/** Asks for powercap_status for all nodes */
int cluster_get_powercap_status(cluster_conf_t *my_cluster_conf, powercap_status_t **pc_status);

int eards_get_powercap_status(cluster_conf_t my_cluster_conf, powercap_status_t **pc_status);

/** Send powercap_options to all nodes */
int cluster_set_powercap_opt(cluster_conf_t my_cluster_conf, powercap_opt_t *pc_opt);

/** Sets frequency for all nodes. */
void set_freq_all_nodes(ulong freq, cluster_conf_t my_cluster_conf);

/** Sets the default pstate for a given policy in all nodes */
void set_def_pstate_all_nodes(uint pstate,ulong policy,cluster_conf_t my_cluster_conf);

/** Sets the maximum pstate in all the nodes */
void set_max_pstate_all_nodes(uint pstate,cluster_conf_t my_cluster_conf);

/** Sets the th to all nodes. */
void set_th_all_nodes(ulong th, ulong p_id, cluster_conf_t my_cluster_conf);

/** Sends the status command through the currently open fd, reads the returning value and places it
*   in **status. Returns the amount of status_t placed in **status. */
int send_status(request_t *command, status_t **status);

/* Power management extensions */
int eards_set_powerlimit(unsigned long limit);
int eards_red_powerlimit(unsigned int type, unsigned long limit);
int eards_inc_powerlimit(unsigned int type, unsigned long limit);
int eards_set_risk(risk_t risk,unsigned long target);
void set_risk_all_nodes(risk_t risk, unsigned long target, cluster_conf_t my_cluster_conf);

int cluster_release_idle_power(cluster_conf_t *my_cluster_conf, pc_release_data_t *released);

#endif
