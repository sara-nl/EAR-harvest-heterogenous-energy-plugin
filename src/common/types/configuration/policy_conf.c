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
#include <limits.h>
#include <common/config.h>
#include <common/output/verbose.h>
#include <common/states.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/hardware/frequency.h>


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
		p->p_state=UINT_MAX;
    memset(p->settings, 0, sizeof(double)*MAX_POLICY_SETTINGS);
}

void print_policy_conf(policy_conf_t *p) 
{
    char buffer[64];
    int i;
	if (p==NULL) return;

    verbosen(VCCONF,"---> policy %s p_state %u def_freq %.1f NormalUsersAuth %u", p->name ,p->p_state,p->def_freq,p->is_available);
    for (i = 0; i < MAX_POLICY_SETTINGS; i++)
        verbosen(VCCONF, " setting%d  %.2lf ", i, p->settings[i]);
    verbosen(VCCONF, " tag: %s", p->tag);
    verbosen(VCCONF, "\n");
}

void check_policy_values(policy_conf_t *p,int nump)
{
	int i=0;
	ulong f;
	for (i=0;i<nump;i++){
		check_policy(&p[i]);
	}
}

void check_policy(policy_conf_t *p)
{
	unsigned long f;
	uint ppstate;
	/* We are using pstates */
	if (p->def_freq==(float)0){
		if (p->p_state>=frequency_get_num_pstates()) p->p_state=frequency_get_nominal_pstate();
	}else{
	/* We are using frequencies */
		f=(unsigned long)(p->def_freq*1000000);
  	if (!frequency_is_valid_frequency(f)){
      	error("Default frequency %lu for policy %s is not valid",f,p->name);
      	p->def_freq=(float)frequency_closest_frequency(f)/(float)1000000;
      	error("New def_freq %f",p->def_freq);
  	}
	}
}

void compute_policy_def_freq(policy_conf_t *p)
{
	if (p->def_freq==(float)0){
		p->def_freq=(float)frequency_pstate_to_freq(p->p_state)/1000000.0;
	}else{
		p->p_state=frequency_closest_pstate((unsigned long)(p->def_freq*1000000));
	}

}

state_t POLICY_token(unsigned int *num_policiesp, policy_conf_t **power_policiesl,char *line)
{
	    char *primary_ptr;
      char *secondary_ptr;
      char *key, *value,*token;
			int num_policies=*num_policiesp;
			policy_conf_t *power_policies=(policy_conf_t *)*power_policiesl;
      policy_conf_t *curr_policy;
      line[strlen(line)] = '=';
      token = strtok_r(line, " ", &primary_ptr);
      if (num_policies>=TOTAL_POLICIES){
        power_policies = realloc(power_policies, sizeof(policy_conf_t)*(num_policies + 1));
        if (power_policies==NULL){
          error("NULL pointer in get_cluster_config");
          return EAR_ERROR;
        }
				*power_policiesl=power_policies;
      }
      curr_policy = &power_policies[num_policies];
      init_policy_conf(curr_policy);
      /* POLICY DEFINITION */
      while (token != NULL)
			{
        key = strtok_r(token, "=", &secondary_ptr);
        strtoup(key);
        value = strtok_r(NULL, "=", &secondary_ptr);
        if (!strcmp(key, "POLICY"))
        {
            strcpy(curr_policy->name, value);
        }
        else if (!strcmp(key, "SETTINGS"))
        {
            value = strtok(value, ",");
            int i;
            for (i = 0; (i < MAX_POLICY_SETTINGS) && (value != NULL); i++)
            {
                curr_policy->settings[i] = atof(value);
                value = strtok(NULL, ",");
            }
        }
        else if (!strcmp(key, "DEFAULTFREQ"))
        {
            curr_policy->def_freq= atof(value);
        }
        else if (!strcmp(key, "DEFAULTPSTATE"))
        {
            curr_policy->p_state=atoi(value);
        }
        else if (!strcmp(key, "PRIVILEGED"))
        {
            curr_policy->is_available=(atoi(value)==0);
        }
        else if (!strcmp(key, "TAG"))
        {
            strcpy(curr_policy->tag, value);
        }

        token = strtok_r(NULL, " ", &primary_ptr);
    }
    curr_policy->policy = num_policies;
    num_policies++;
		*num_policiesp = num_policies;
		return EAR_SUCCESS;
}
