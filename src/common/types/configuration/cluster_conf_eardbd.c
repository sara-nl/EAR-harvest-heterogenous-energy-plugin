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
#include <common/string_enhanced.h>
#include <common/types/configuration/cluster_conf_eardbd.h>

state_t EARDBD_token(char *token)
{
	if (strcasestr(token,"EARDBD")!=NULL) return EAR_SUCCESS;
	if (strcasestr(token,"DBDAEMON")!=NULL) return EAR_SUCCESS;
	return EAR_ERROR;
}

state_t EARDBD_parse_token(eardb_conf_t *conf,char *token)
{
		state_t found=EAR_ERROR;

    if (!strcmp(token, "DBDAEMONAGGREGATIONTIME"))
    {
      token = strtok(NULL, "=");
      conf->aggr_time = atoi(token);
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "DBDAEMONINSERTIONTIME"))
    {
      token = strtok(NULL, "=");
      conf->insr_time = atoi(token);
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "DBDAEMONPORTTCP"))
    {
      token = strtok(NULL, "=");
      conf->tcp_port = atoi(token);
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "DBDAEMONPORTSECTCP"))
    {
      token = strtok(NULL, "=");
      conf->sec_tcp_port = atoi(token);
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "DBDAEMONSYNCPORT"))
    {
      token = strtok(NULL, "=");
      conf->sync_tcp_port = atoi(token);
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "DBDAEMONMEMORYSIZE"))
    {
      token = strtok(NULL, "=");
      conf->mem_size = atoi(token);
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "DBDAEMONMEMORYSIZEPERTYPE"))
    {
      int i = 0;

      token = strtok(NULL, "=");
      token = strtok(token, ",");

    	while (token != NULL && i < EARDBD_TYPES)
    	{
        conf->mem_size_types[i] = atoi(token);
        token = strtok(NULL, ",");
        i++;
    	}
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "DBDAEMONUSELOG"))
    {
      token = strtok(NULL, "=");
      conf->use_log = atoi(token);
			found = EAR_SUCCESS;
    }

		return found;
}



void copy_eardbd_conf(eardb_conf_t *dest,eardb_conf_t *src)
{
	  memcpy(dest,src,sizeof(eardb_conf_t));
}

void set_default_eardbd_conf(eardb_conf_t *eardbdc)
{
  eardbdc->aggr_time    = DEF_DBD_AGGREGATION_TIME;
  eardbdc->insr_time    = DEF_DBD_INSERTION_TIME;
  eardbdc->tcp_port   = DEF_DBD_SERVER_PORT;
  eardbdc->sec_tcp_port = DEF_DBD_MIRROR_PORT;
  eardbdc->sync_tcp_port  = DEF_DBD_SYNCHR_PORT;
  eardbdc->mem_size   = DEF_DBD_ALLOC_MBS;
  eardbdc->use_log    = DEF_DBD_FILE_LOG;
  eardbdc->mem_size_types[0] = 60;
  eardbdc->mem_size_types[1] = 22;
  eardbdc->mem_size_types[2] = 5;
  eardbdc->mem_size_types[3] = 0;
  eardbdc->mem_size_types[4] = 7;
  eardbdc->mem_size_types[5] = 5;
  eardbdc->mem_size_types[6] = 1;


}
void print_db_manager(eardb_conf_t *conf)
{
	verbosen(VCCONF,"--> EARDBD configuration\n");
  verbosen(VCCONF, "---> Insertion time %u\tAggregation time: %u\tTCP port: %u\tSec. TCP port: %u\tSync Port: %u\tCacheSize: %u\n",
      conf->insr_time,conf->aggr_time, conf->tcp_port, conf->sec_tcp_port, conf->sync_tcp_port,conf->mem_size);
  verbosen(VCCONF,"--> use_log %u\n",conf->use_log);
}
