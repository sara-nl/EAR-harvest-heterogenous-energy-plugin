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

#define _GNU_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/config.h>
//#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/states.h>
#include <common/string_enhanced.h>
#include <common/types/generic.h>
#include <common/types/configuration/cluster_conf_etag.h>
#include <common/types/configuration/cluster_conf_generic.h>



state_t ETAG_token(char *token)
{
	if (strcasestr(token,"ENERGYTAG")!=NULL) return EAR_SUCCESS;
	return EAR_ERROR;
}
state_t ETAG_parse_token(unsigned int *num_etagsp, energy_tag_t **e_tagsl,char *line)
{
      char *primary_ptr;
      char *secondary_ptr;
			char *token;
	state_t found=EAR_SUCCESS;
			int num_etags=*num_etags;
			energy_tag_t *e_tags=*e_tagsl;
      line[strlen(line)] = '=';
      token = strtok_r(line, " ", &primary_ptr);
			while (token != NULL)
      {
        token = strtok_r(token, "=", &secondary_ptr);
        strtoup(token);

        if (!strcmp(token, "ENERGYTAG"))
        {
          num_etags++;
          if (num_etags==1) e_tags=NULL;
          e_tags = realloc(e_tags, sizeof(energy_tag_t) * (num_etags));
          if (e_tags==NULL){
						*e_tagsl=e_tags;
						*num_etagsp=num_etags;
            error("NULL pointer reading energy tags");
            return EAR_ERROR;
          }
          token = strtok_r(NULL, "=", &secondary_ptr);
          memset(&e_tags[num_etags-1], 0, sizeof(energy_tag_t));
          remove_chars(token, ' ');
          strcpy(e_tags[num_etags-1].tag, token);
          e_tags[num_etags-1].users = NULL;
          e_tags[num_etags-1].groups = NULL;
          e_tags[num_etags-1].accounts = NULL;
        }
        else if (!strcmp(token, "PSTATE"))
        {
          token = strtok_r(NULL, "=", &secondary_ptr);
          e_tags[num_etags-1].p_state = atoi(token);
        }
        else if (!strcmp(token, "USERS"))
        {
					if (LIST_parse_token(token,&e_tags[num_etags-1].num_users,&e_tags[num_etags-1].users) != EAR_SUCCESS) return EAR_ERROR;
        }
        else if (!strcmp(token, "GROUPS"))
        {
					
					if (LIST_parse_token(token,&e_tags[num_etags-1].num_groups,&e_tags[num_etags-1].groups) != EAR_SUCCESS) return EAR_ERROR;
        }
        else if (!strcmp(token, "ACCOUNTS"))
        {
					if (LIST_parse_token(token,&e_tags[num_etags-1].num_accounts,&e_tags[num_etags-1].accounts) != EAR_SUCCESS) return EAR_ERROR;
        }
        token = strtok_r(NULL, " ", &primary_ptr);
      } /* END WHILE */

		return found;

}
void print_energy_tag(energy_tag_t *etag)
{
	verbosen(VCCONF, "--> Tag: %s\t pstate: %u\n", etag->tag, etag->p_state);
	int i;
	for (i = 0; i < etag->num_users; i++)
		verbosen(VCCONF, "---> user: %s\n", etag->users[i]);

	for (i = 0; i < etag->num_accounts; i++)
		verbosen(VCCONF, "---> accounts: %s\n", etag->accounts[i]);
	
	for (i = 0; i < etag->num_groups; i++)
		verbosen(VCCONF, "---> group: %s\n", etag->groups[i]);

}


