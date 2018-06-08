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
#define __CLUSTER_CONF_H
#include <stdint.h>
#include <common/config.h>
#include <common/environment.h>
#include <common/types/generic.h>

#define GENERIC_NAME 256
#define USER		64
#define ACC			64
/*
* EARD configuration
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
* Global manager configuration
*/

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
} eargm_conf_t;

typedef struct node_range
{
	char prefix[USER];
	uint start;
	uint end;
} node_range_t;

typedef struct eardb_conf 
{
	uint aggr_time;
	uint tcp_port;
	uint udp_port;
} eardb_conf_t;

typedef struct policy_conf
{
    uint policy; // from environment.h
    double th;
    uint p_state;
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

typedef struct special_app
{
	char user[USER];
	uint p_state;
} special_app_t;

typedef struct energy_tag
{
	char tag[USER];
	uint p_state;
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
} node_island_t;

typedef struct cluster_conf
{
	// Library & common conf
	char Coefficients_pathname[GENERIC_NAME];
	char tmp_dir[GENERIC_NAME];
	char etc_dir[GENERIC_NAME];
	char DB_pathname[GENERIC_NAME];
	uint verbose;
	eard_conf_t		eard;
	eargm_conf_t 	eargm;
	// List of policies	
	uint num_policies;
	policy_conf_t *power_policies;
	uint default_policy;			// selecs one of the power_policies
	// Lis of autorized users
	uint num_priv_users;
	char **priv_users;
	uint num_acc;
	char **priv_acc;
	// Special cases
	uint num_special;
	uint min_time_perf_acc;
	special_app_t	*special;
	// List of nodes
	uint num_nodes;
	node_conf_t *nodes;
	db_conf_t database;
	eardb_conf_t db_manager;
	uint num_tags;
	energy_tag_t *e_tags;
	uint num_islands;
	node_island_t *islands;
} cluster_conf_t;

// Function declarations

// CLUSTER level functions
/** read the cluster configuration from the ear_cluster.conf pointed by conf path */
int read_cluster_conf(char *conf_path,cluster_conf_t *my_conf);

void get_cluster_config(FILE *conf_file, cluster_conf_t *conf);

// NODE level functions

/** returns the pointer to the information of nodename */
node_conf_t *get_my_node_conf(cluster_conf_t *my_conf,char *nodename);
/** prints in the stdout the node configuration */
void print_node_conf(node_conf_t *my_node_conf);

// POLICY level functions
/** prints in the stdout policy configuration */
void print_policy_conf(policy_conf_t *p);

/** Converts from policy name to policy_id */
int policy_name_to_id(char *my_policy);

/** Given a cluster, node and  policy, returns the policy configuration for that cluser,node,policy */
policy_conf_t *get_my_policy_conf(cluster_conf_t *my_cluster,node_conf_t *my_node,uint p_id);

/** Prints in the stdout the whole cluster configuration */
void print_cluster_conf(cluster_conf_t *conf);

void free_cluster_conf(cluster_conf_t *conf);

#else
#endif
