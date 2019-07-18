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
#include <common/output/verbose.h>
#include <common/states.h>
#include <common/types/configuration/cluster_conf.h>


/*
 *   POLICY FUNCTIONS
 */



void copy_policy_conf(policy_conf_t *dest,policy_conf_t *src)
{
	memcpy((void *)dest,(void *)src,sizeof(policy_conf_t));
}

void init_policy_conf(policy_conf_t *p)
{
    p->policy = -1;
//    p->th = 0;
    p->is_available = 0;
		p->def_freq=(float)0;
    memset(p->settings, 0, sizeof(double)*MAX_POLICY_SETTINGS);
}

void print_policy_conf(policy_conf_t *p) 
{
    char buffer[64];
    int i;
	if (p==NULL) return;

    verbosen(VCCONF,"---> policy %s p_state %u ", p->name ,p->p_state);
    for (i = 0; i < MAX_POLICY_SETTINGS; i++)
        verbosen(VCCONF, " setting%d  %.2lf ", i, p->settings[i]);
    verbosen(VCCONF, "\n");
}

