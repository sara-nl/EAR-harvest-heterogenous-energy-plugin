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

#ifndef _DB_CONF_H
#define _DB_CONF_H

#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>


typedef struct db_conf
{
    char ip[USER];
    char user[USER];
    char pass[USER];
    char user_commands[USER];
    char pass_commands[USER];
    char database[USER];
    uint port;
	uint max_connections;
	uint report_node_detail;
	uint report_sig_detail;
	uint report_loops;
} db_conf_t;



state_t DB_token(char *token);
state_t DB_parse_token(db_conf_t *conf,char *token);
void print_database_conf(db_conf_t *conf);
void set_default_db_conf(db_conf_t *db_conf);
void copy_eardb_conf(db_conf_t *dest,db_conf_t *src);



#endif
