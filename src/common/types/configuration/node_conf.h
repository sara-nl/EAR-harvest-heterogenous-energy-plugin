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

#ifndef _NODE_CONF_H
#define _NODE_CONF_H

#include <common/config.h>
#include <common/types/generic.h>
#include <common/types/configuration/policy_conf.h>

/**
* @file node_conf.h
* @brief This file includes basic types and functions headers to manipulate nodes and nodes settings
*
*/


/*
 *
 * Types
 *
 */

#define NODE_PREFIX 64
#define GENERIC_NODE_NAME 256
typedef struct node_range
{
	char prefix[NODE_PREFIX];
	int start;
	int end;
    int db_ip;
    int sec_ip;
} node_range_t;


typedef struct node_conf
{
	char name[GENERIC_NODE_NAME];
	node_range_t *range;
	uint range_count;
	uint cpus;
	uint island;	
	uint num_special_node_conf;
	policy_conf_t *special_node_conf;
	char *coef_file;
	ulong db_ip;
} node_conf_t;

typedef struct my_node_conf
{
	uint cpus;
	uint island;
	ulong max_pstate;
	char db_ip[NODE_PREFIX];
	char db_sec_ip[NODE_PREFIX];
	char *coef_file;
	uint num_policies;
	policy_conf_t policies[TOTAL_POLICIES];
}my_node_conf_t;


typedef struct node_island
{
	uint id;
	uint num_ranges;
	node_range_t *ranges;
	char **db_ips;
    uint num_ips;
	char **backup_ips;
    uint num_backups;
} node_island_t;


/*
 *
 * Functions
 *
 */

char range_conf_contains_node(node_conf_t *node, char *nodename);
char island_range_conf_contains_node(node_island_t *node, char *nodename);


/** Copies dest=src */
void copy_my_node_conf(my_node_conf_t *dest,my_node_conf_t *src);


/** prints in the stderr the node configuration */
void print_node_conf(node_conf_t *node_conf);

/** prints in the stderr the specific node configuration */
void print_my_node_conf(my_node_conf_t *my_node_conf);

int print_my_node_conf_fd_binary(int fd,my_node_conf_t *myconf);
int read_my_node_conf_fd_binary(int fd,my_node_conf_t *myconf);


/** Given a  node and policy, returns the policy configuration for that cluser,node,policy */
policy_conf_t *get_my_policy_conf(my_node_conf_t *my_node,uint p_id);


#else
#endif
