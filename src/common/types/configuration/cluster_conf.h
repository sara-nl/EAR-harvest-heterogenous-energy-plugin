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

#ifndef _CLUSTER_CONF_H
#define _CLUSTER_CONF_H

#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <common/types/configuration/policy_conf.h>
#include <common/string_enhanced.h>
#include <common/types/generic.h>
#include <common/ear_verbose.h>
#include <common/states.h>
#include <common/config.h>

#define GENERIC_NAME	256
#define USER			64
#define ACC				64

/*
 *
 * Types
 *
 */

/*
* EARDVerbose=
* EARDPerdiocPowermon=
* EARDMaxPstate=
* EARDTurbo=
* EARDPort=
*/

typedef struct eard_conf
{
	uint verbose;			/* default 1 */
	ulong period_powermon;	/* default 30000000 (30secs) */
	ulong max_pstate; 		/* default 1 */
	uint turbo;				/* Fixed to 0 by the moment */
	uint port;				/* mandatory */
} eard_conf_t;

/*
* EARGMVerbose=
* EARGMT1=
* EARGMT2=
* EARGMEnergy=
* EARGMPort=
*/
typedef struct eargm_conf
{
	uint 	verbose;		/* default 1 */
	ulong	t1;				/* default 60 seconds */
	ulong	t2;				/* default 600 seconds */
	ulong 	energy;			/* mandatory */
	uint 	port;			/* mandatory */
	uint 	mode;
	char 	mail[GENERIC_NAME];
    char    host[GENERIC_NAME];
} eargm_conf_t;

typedef struct node_range
{
	char prefix[USER];
	int start;
	int end;
} node_range_t;

typedef struct eardb_conf 
{
	uint aggr_time;
	uint tcp_port;
	uint udp_port;
    uint mem_size;
} eardb_conf_t;

typedef struct policy_conf
{
    uint policy; // from environment.h
    double th;
    uint p_state;
    char is_available; //default at 0, not available
} policy_conf_t;

typedef struct node_conf
{
	char name[GENERIC_NAME];
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
	char db_ip[USER];
	char *coef_file;
	uint num_policies;
	policy_conf_t policies[TOTAL_POLICIES];
}my_node_conf_t;

typedef struct energy_tag
{
	char tag[USER];
	uint p_state;
	char **users;
	uint num_users;
	char **groups;
	uint num_groups;
	char **accounts;
	uint num_accounts;
} energy_tag_t;

typedef struct db_conf
{
    char ip[USER];
    char user[USER];
    char pass[USER];
    char database[USER];
    uint port;
} db_conf_t;

typedef struct node_island
{
	uint id;
	uint num_ranges;
	node_range_t *ranges;
	char db_ip[GENERIC_NAME];
	char backup_ip[GENERIC_NAME];
} node_island_t;

typedef struct earlib_conf
{
	char coefficients_pathname[GENERIC_NAME];
    uint dynais_levels;
    uint dynais_window;
} earlib_conf_t;

typedef struct cluster_conf
{
	// Library & common conf
	char tmp_dir[GENERIC_NAME];
	char etc_dir[GENERIC_NAME];
	char DB_pathname[GENERIC_NAME];
	uint verbose;
	eard_conf_t		eard;
	eargm_conf_t 	eargm;
	// List of policies	
	uint num_policies;
	policy_conf_t power_policies[TOTAL_POLICIES];
	uint default_policy;			// selecs one of the power_policies
	// Lis of autorized users
	uint num_priv_users;
	char **priv_users;
	uint num_priv_groups;
	char **priv_groups;
	uint num_acc;
	char **priv_acc;
	// Special cases
	uint min_time_perf_acc;
	// List of nodes
	uint num_nodes;
	node_conf_t *nodes;
	db_conf_t database;
	eardb_conf_t db_manager;
	uint num_tags;
	energy_tag_t *e_tags;
	uint num_islands;
	node_island_t *islands;
    earlib_conf_t earlib;
} cluster_conf_t;

/*
 *
 * Functions
 *
 */

/** returns the ear.conf path. It checks first at /etc/ear/ear.conf and, it is not available, checks at $EAR_INSTALL_PATH/etc/sysconf/ear.conf */
int get_ear_conf_path(char *ear_conf_path);

/** returns the pointer to the information of nodename */
node_conf_t *get_node_conf(cluster_conf_t *my_conf,char *nodename);

my_node_conf_t *get_my_node_conf(cluster_conf_t *my_conf,char *nodename);

// Cluster configuration read

/** read the cluster configuration from the ear_cluster.conf pointed by conf path */
int read_cluster_conf(char *conf_path,cluster_conf_t *my_conf);

/** frees a cluster_conf_t */
void free_cluster_conf(cluster_conf_t *conf);

// Cluster configuration verbose

/** prints in the stderr the node configuration */
void print_node_conf(node_conf_t *node_conf);

/** prints in the stderr the specific node configuration */
void print_my_node_conf(my_node_conf_t *my_node_conf);

/** prints in the stdout policy configuration */
void print_policy_conf(policy_conf_t *p);

/** Prints in the stdout the whole cluster configuration */
void print_cluster_conf(cluster_conf_t *conf);

/** Given a cluster, node and policy, returns the policy configuration for that cluser,node,policy */
policy_conf_t *get_my_policy_conf(cluster_conf_t *my_cluster,my_node_conf_t *my_node,uint p_id);

#endif
