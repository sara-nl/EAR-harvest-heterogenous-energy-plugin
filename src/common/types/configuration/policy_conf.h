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
 * @file policy_conf.h
 * @brief This file includes types, constants,and headers to specify the number of supported policies and the policies id's
 *
 * When adding a new policy, we must provide a set of functions to identify policies by name (targeted to users) and by policy id, for internal use.
 * Moreover, basic functions to copy, print etc are provided
 */


#ifndef _POLICY_CONF_H
#define _POLICY_CONF_H

#include <common/config.h>
#include <common/types/generic.h>

#define TOTAL_POLICIES          3
#define MIN_ENERGY_TO_SOLUTION  0
#define MIN_TIME_TO_SOLUTION    1
#define MONITORING_ONLY         2


typedef struct policy_conf
{
    uint policy; 
    double th; 
    uint p_state;
    char is_available; //default at 0, not available
} policy_conf_t;


#include <common/types/configuration/cluster_conf.h>

/** Check if a given policy if is valid */
int is_valid_policy(unsigned int p_id);

/** Returns the short name for a given policy. To be used in eacct etc */
void get_short_policy(char *buf, char *policy);

/** copy dest=src */
void copy_policy_conf(policy_conf_t *dest,policy_conf_t *src);

/** prints in the stdout policy configuration */
void print_policy_conf(policy_conf_t *p);

/** Converts from policy name to policy_id . Returns EAR_ERROR if error*/
int policy_name_to_id(char *my_policy);

/** Converts from policy_id to policy name. Returns error if policy_id is not valid*/
int policy_id_to_name(int policy_id,char *my_policy);


#endif
