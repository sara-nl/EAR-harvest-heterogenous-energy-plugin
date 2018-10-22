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

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <common/config.h>
#include <common/states.h>
#include <common/types/configuration/cluster_conf.h>


/*
 *  * POLICY FUNCTIONS
 *   */

/* Given a  node and policy_id, returns the policy configuration (or NULL) */
policy_conf_t *get_my_policy_conf(my_node_conf_t *my_node,uint p_id)
{
    policy_conf_t *my_policy=NULL;
    uint i;
    uint nump=0;
    while((nump<my_node->num_policies) && (my_node->policies[nump].policy!=p_id)) nump++;
    if (nump<my_node->num_policies){
        my_policy=&my_node->policies[nump];
    }
    return my_policy;
}


/** Converts from policy name to policy_id . Returns EAR_ERROR if error*/
int policy_name_to_id(char *my_policy)
{
    if (my_policy!=NULL){
        if ((strcmp(my_policy,"MIN_ENERGY_TO_SOLUTION")==0) || (strcmp(my_policy,"min_energy_to_solution")==0)) return MIN_ENERGY_TO_SOLUTION;
        else if ((strcmp(my_policy,"MIN_TIME_TO_SOLUTION")==0) || (strcmp(my_policy,"min_time_to_solution")==0)) return MIN_TIME_TO_SOLUTION;
        else if ((strcmp(my_policy,"MONITORING_ONLY")==0) || (strcmp(my_policy,"monitoring_only")==0)) return MONITORING_ONLY;
    }
	return EAR_ERROR;
}


/** Converts from policy_id to policy name. Returns error if policy_id is not valid*/
int policy_id_to_name(int policy_id,char *my_policy)
{
	int ret=EAR_SUCCESS;
	switch(policy_id)
    {
        case MIN_ENERGY_TO_SOLUTION:
            strcpy(my_policy,"MIN_ENERGY_TO_SOLUTION");
        	break;
        case MIN_TIME_TO_SOLUTION:
            strcpy(my_policy,"MIN_TIME_TO_SOLUTION");
        	break;
        case MONITORING_ONLY:
            strcpy(my_policy,"MONITORING_ONLY");
        	break;
		default: ret=EAR_ERROR;
    }
	return ret;

}

void copy_policy_conf(policy_conf_t *dest,policy_conf_t *src)
{
	memcpy((void *)dest,(void *)src,sizeof(policy_conf_t));
}
int is_valid_policy(unsigned int p_id)
{
	return (p_id<TOTAL_POLICIES);
}
