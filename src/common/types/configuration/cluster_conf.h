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

#define NORMAL 		0
#define AUTHORIZED	1
#define ENERGY_TAG	2

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
	uint use_mysql;			/* Must EARD report to DB */
	uint use_eardbd;		/* Must EARD report to DB using EARDBD */
	uint force_frequencies; /* 1=EARD will force pstates specified in policies , 0=will not */
} eard_conf_t;

/*
* EARGMVerbose=
* EARGMT1=
* EARGMT2=
* EARGMEnergy=
* EARGMPort=
*/
#define DECON_LIMITS	3
typedef struct eargm_conf
{
	uint 	verbose;		/* default 1 */
	uint	use_aggregation; /* Use aggregated metrics.Default 1 */
	ulong	t1;				/* default 60 seconds */
	ulong	t2;				/* default 600 seconds */
	ulong 	energy;			/* mandatory */
	uint 	port;			/* mandatory */
	uint 	mode;
	uint	defcon_limits[3];
	uint	grace_periods;
	char 	mail[GENERIC_NAME];
    char    host[GENERIC_NAME];
} eargm_conf_t;

typedef struct node_range
{
	char prefix[USER];
	int start;
	int end;
    int db_ip;
    int sec_ip;
} node_range_t;

typedef struct eardb_conf 
{
	uint aggr_time;
	uint tcp_port;
	uint sec_tcp_port;
	uint sync_tcp_port;
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

typedef struct communication_node
{
    char name[GENERIC_NAME];
    int  distance;
} communication_node_t;

typedef struct my_node_conf
{
	uint cpus;
	uint island;
	ulong max_pstate;
	char db_ip[USER];
	char db_sec_ip[USER];
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
	char **db_ips;
    uint num_ips;
	char **backup_ips;
    uint num_backups;
} node_island_t;

typedef struct earlib_conf
{
	char coefficients_pathname[GENERIC_NAME];
    uint dynais_levels;
    uint dynais_window;
	uint dynais_timeout;
	uint lib_period;
	uint check_every;
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
    communication_node_t *comm_nodes;
    uint num_comm_nodes;
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

/** Prints the DB configuration */
void print_database_conf(db_conf_t *conf);

/** prints in the stderr the specific node configuration */
void print_my_node_conf(my_node_conf_t *my_node_conf);

/** prints in the stdout policy configuration */
void print_policy_conf(policy_conf_t *p);

/** Prints in the stdout the whole cluster configuration */
void print_cluster_conf(cluster_conf_t *conf);

/** Prints in the stdout the energy_tag settings */
void print_energy_tag(energy_tag_t *etag);

/** Given a cluster, node and policy, returns the policy configuration for that cluser,node,policy */
policy_conf_t *get_my_policy_conf(cluster_conf_t *my_cluster,my_node_conf_t *my_node,uint p_id);


/** returns  the energy tag entry if the username, group and/or accounts is in the list of the users/groups/acc authorized to use the given energy-tag, NULL otherwise */
energy_tag_t * is_energy_tag_privileged(cluster_conf_t *my_conf, char *user,char *group, char *acc,char *energy_tag);

/** returns true if the username, group and/or accounts is presents in the list of authorized users/groups/accounts */
int is_privileged(cluster_conf_t *my_conf, char *user,char *group, char *acc);



/** returns the user type: NORMAL, AUTHORIZED, ENERGY_TAG */
uint get_user_type(cluster_conf_t *my_conf, char *energy_tag, char *user,char *group, char *acc,energy_tag_t **my_tag);


/* Given a cluser, node and policy_id, returns the policy configuration (or NULL) */
policy_conf_t *get_my_policy_conf(cluster_conf_t *my_cluster,my_node_conf_t *my_node,uint p_id);

/** Converts from policy name to policy_id . Returns EAR_ERROR if error*/
int policy_name_to_id(char *my_policy);

/** Converts from policy_id to policy name. Returns error if policy_id is not valid*/
int policy_id_to_name(int policy_id,char *my_policy);

/* Copy src in dest */
void copy_ear_lib_conf(earlib_conf_t *dest,earlib_conf_t *src);
/* Prints the given library conf */
void print_ear_lib_conf(earlib_conf_t *libc);


void copy_eard_conf(eard_conf_t *dest,eard_conf_t *src);
void copy_eargmd_conf(eargm_conf_t *dest,eargm_conf_t *src);
void copy_eardb_conf(db_conf_t *dest,db_conf_t *src);
void copy_eardbd_conf(eardb_conf_t *dest,eardb_conf_t *src);


/* Default functions */
void set_default_eard_conf(eard_conf_t *eardc);
void set_default_eargm_conf(eargm_conf_t *eardc);




#endif
