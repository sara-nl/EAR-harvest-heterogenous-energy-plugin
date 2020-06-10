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
#include <common/states.h>
#include <common/types/generic.h>
#include <common/types/configuration/cluster_conf_db.h>





state_t DB_token(char *token)
{
  if (strcasestr(token,"DB")!=NULL) return EAR_SUCCESS;
  if (strcasestr(token,"MariaDB")!=NULL) return EAR_SUCCESS;
	return EAR_ERROR;
}
state_t DB_parse_token(db_conf_t *conf,char *token)
{
		state_t found=EAR_ERROR;
		debug("DB_parse_token %s",token);
    if (!strcmp(token, "MARIADBIP"))
    {
      token = strtok(NULL, "=");
      strclean(token, '\n');
      remove_chars(token, ' ');
      strcpy(conf->ip, token);
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "MARIADBUSER"))
    {
      token = strtok(NULL, "=");
      strclean(token, '\n');
      remove_chars(token, ' ');
      strcpy(conf->user, token);
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "MARIADBPASSW"))
    {
      token = strtok(NULL, "=");
      strclean(token, '\n');
      remove_chars(token, ' ');
      strcpy(conf->pass, token);
			found = EAR_SUCCESS;
    }
    else if (!strcmp(token, "MARIADBCOMMANDSUSER"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      strclean(token, '\n');
      remove_chars(token, ' ');
      strcpy(conf->user_commands, token);
    }
    else if (!strcmp(token, "MARIADBCOMMANDSPASSW"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      strclean(token, '\n');
      remove_chars(token, ' ');
      strcpy(conf->pass_commands, token);
    }
    else if (!strcmp(token, "MARIADBDATABASE"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      strclean(token, '\n');
      remove_chars(token, ' ');
      strcpy(conf->database, token);
    }
    else if (!strcmp(token, "MARIADBPORT"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->port = atoi(token);
    }
    else if (!strcmp(token, "DBMAXCONNECTIONS"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->max_connections = atoi(token);
    }
    else if (!strcmp(token, "DBREPORTNODEDETAIL"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->report_node_detail = atoi(token);
    }
    else if (!strcmp(token, "DBREPORTSIGDETAIL"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->report_sig_detail = atoi(token);
    }
    else if (!strcmp(token, "DBREPORTLOOPS"))
    {
			found = EAR_SUCCESS;
      token = strtok(NULL, "=");
      conf->report_loops = atoi(token);
    }
		debug("End DB token");
		return found;

}
void print_database_conf(db_conf_t *conf)
{
  verbosen(VCCONF,"\n--> DB configuration\n");
  verbosen(VCCONF, "---> IP: %s\tUser: %s\tUser commands %s\tPort:%u\tDB:%s\n",
      conf->ip, conf->user, conf->user_commands,conf->port, conf->database);
  verbosen(VCCONF,"-->max_connections %u report_node_details %u report_sig_details %u report_loops %u\n",conf->max_connections,conf->report_node_detail,conf->report_sig_detail,conf->report_loops);


}
void set_default_db_conf(db_conf_t *db_conf)
{
  strcpy(db_conf->user, "ear_daemon");
  strcpy(db_conf->database, "EARDB");
  strcpy(db_conf->user_commands, "ear_daemon");
  strcpy(db_conf->ip, "127.0.0.1");
  db_conf->port = 3306;
  db_conf->max_connections=MAX_DB_CONNECTIONS;
  db_conf->report_node_detail=DEMO;
  db_conf->report_sig_detail=!DB_SIMPLE;
  db_conf->report_loops=!LARGE_CLUSTER;
}
void copy_eardb_conf(db_conf_t *dest,db_conf_t *src)
{
	  memcpy(dest,src,sizeof(db_conf_t));

}



