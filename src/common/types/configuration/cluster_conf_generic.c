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
#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>
#include <common/string_enhanced.h>
//#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/types/configuration/cluster_conf_generic.h>



state_t GENERIC_parse_token(cluster_conf_t *conf,char *token,char *def_policy)
{
		debug("GENERIC_parse_token %s",token);
	  if (!strcmp(token, "DEFAULTPOWERPOLICY"))
    {
      token = strtok(NULL, "=");
      token = strtok(token, "\n");
      remove_chars(token, ' ');
      strcpy(def_policy, token);
    }
    else if (!strcmp(token, "DATABASEPATHNAME"))
    {
      token = strtok(NULL, "=");
      token = strtok(token, "\n");
      remove_chars(token, ' ');
      strcpy(conf->DB_pathname, token);
    }
    else if (!strcmp(token, "VERBOSE"))
    {
      token = strtok(NULL, "=");
      conf->verbose = atoi(token);
    }
    else if (!strcmp(token, "MINTIMEPERFORMANCEACCURACY"))
    {
      token = strtok(NULL, "=");
      conf->min_time_perf_acc = atoi(token);
    }
    else if (!strcmp(token, "NETWORKEXTENSION"))
    {
            token = strtok(NULL, "=");
            strcpy(conf->net_ext, token);
    }

    else if (!strcmp(token, "TMPDIR"))
    {
      token = strtok(NULL, "=");
      token = strtok(token, "\n");
      remove_chars(token, ' ');
      strcpy(conf->install.dir_temp, token);
    }
    else if (!strcmp(token, "ETCDIR"))
    {
      token = strtok(NULL, "=");
      token = strtok(token, "\n");
      remove_chars(token, ' ');
      strcpy(conf->install.dir_conf, token);
    }
    else if (!strcmp(token, "INSTDIR"))
    {
      token = strtok(NULL, "=");
      token = strtok(token, "\n");
      remove_chars(token, ' ');
      strcpy(conf->install.dir_inst, token);

      sprintf(conf->install.dir_plug, "%s/lib/plugins", token);
    }
    else if (!strcmp(token, "PLUGINENERGY"))
    {
      token = strtok(NULL, "=");
      token = strtok(token, "\n");
      remove_chars(token, ' ');
      strcpy(conf->install.obj_ener, token);
    }
    else if (!strcmp(token, "PLUGINPOWERMODEL"))
    {
      token = strtok(NULL, "=");
      token = strtok(token, "\n");
      remove_chars(token, ' ');
      strcpy(conf->install.obj_power_model, token);
    }
		debug("End GENERIC_token");
		return EAR_SUCCESS;
}


state_t AUTH_token(char *token)
{
  if (strcasestr(token,"AUTHORIZED")!=NULL) return EAR_SUCCESS;
	return EAR_ERROR;
}
state_t AUTH_parse_token(char *token,unsigned int *num_elemsp,char ***list_elemsp)
{
	return LIST_parse_token(token,num_elemsp,list_elemsp);
}
state_t LIST_parse_token(char *token,unsigned int *num_elemsp,char ***list_elemsp)
{
	int num_elems=*num_elemsp;
	char **list_elems=*list_elemsp;
  token = strtok(NULL, "=");
  token = strtok(token, ",");
  while (token != NULL)
  {
  	num_elems++;
  	list_elems = (char **)realloc(list_elems, sizeof(char *)*num_elems);
  	if (list_elems == NULL){
			num_elems=0;
			*num_elemsp=num_elems;
			*list_elemsp=list_elems;
  		error("NULL pointer reading authorized users list");
  		return EAR_ERROR;
  	}
  	strclean(token, '\n');
  	list_elems[num_elems-1] = (char *)malloc(strlen(token)+1);
  	remove_chars(token, ' ');
  	strcpy(list_elems[num_elems-1], token);
  	token = strtok(NULL, ",");
  }
	*num_elemsp=num_elems;
	*list_elemsp=list_elems;
	return EAR_SUCCESS;
}
