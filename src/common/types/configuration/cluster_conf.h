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
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <common/sizes.h>
#include <common/config.h>
#include <common/states.h>
#include <common/types/generic.h>
#include <common/types/configuration/policy_conf.h>
#include <common/types/configuration/node_conf.h>
#include <common/string_enhanced.h>

#include <common/types/configuration/cluster_conf_eargm.h>
#include <common/types/configuration/cluster_conf_eard.h>
#include <common/types/configuration/cluster_conf_eardbd.h>
#include <common/types/configuration/cluster_conf_db.h>


#define NORMAL 		0
#define AUTHORIZED	1
#define ENERGY_TAG	2

/*
 *
 * Types
 *
 */


#define POWERCAP_TYPE_NODE  0 
#define POWERCAP_TYPE_APP   1
#define TAG_TYPE_ARCH       0 




typedef struct communication_node
{
    char name[GENERIC_NAME];
    int  distance;
} communication_node_t;

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

typedef struct tags
{
    char id[GENERIC_NAME];
    char type;
    char is_default;
    char powercap_type;
    ulong max_avx512_freq;
    ulong max_avx2_freq;
    ulong max_power;
    ulong min_power;
    ulong max_temp;
    ulong error_power;
    long powercap;
    char energy_model[GENERIC_NAME];
    char energy_plugin[GENERIC_NAME];
    char powercap_plugin[GENERIC_NAME];
    char coeffs[GENERIC_NAME];
} tag_t;


typedef struct earlib_conf
{
	char coefficients_pathname[GENERIC_NAME];
    uint dynais_levels;
    uint dynais_window;
	uint dynais_timeout;
	uint lib_period;
	uint check_every;
} earlib_conf_t;

typedef struct conf_install {
	char dir_temp[SZ_PATH_INCOMPLETE];
	char dir_conf[SZ_PATH_INCOMPLETE];
	char dir_inst[SZ_PATH_INCOMPLETE];
	char dir_plug[SZ_PATH_INCOMPLETE];
	char obj_ener[SZ_PATH_SHORT];
	char obj_power_model[SZ_PATH_SHORT];
} conf_install_t;

typedef struct cluster_conf
{
	// Library & common conf
	char DB_pathname[GENERIC_NAME];
  char net_ext[ID_SIZE];
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
    tag_t *tags;
	uint num_etags;
	energy_tag_t *e_tags;
	uint num_islands;
	node_island_t *islands;
    earlib_conf_t earlib;
    communication_node_t *comm_nodes;
    uint num_comm_nodes;
	conf_install_t install;
} cluster_conf_t;

/*
 *
 * Functions
 *
 */

/** returns the ear.conf path. It checks first at /etc/ear/ear.conf and, it is not available, checks at $EAR_INSTALL_PATH/etc/sysconf/ear.conf */
int get_ear_conf_path(char *ear_conf_path);

/** returns the eardbd.conf path. It checks first at /etc/ear/eardbd.conf and, it is not available, checks at $EAR_INSTALL_PATH/etc/sysconf/eardbd.conf */
int get_eardbd_conf_path(char *ear_conf_path);

/** returns the pointer to the information of nodename */
node_conf_t *get_node_conf(cluster_conf_t *my_conf,char *nodename);


/** returns the configuration for your node with any specific setting specified at ear.conf */
my_node_conf_t *get_my_node_conf(cluster_conf_t *my_conf,char *nodename);

// Cluster configuration read

/** read the cluster configuration from the ear_cluster.conf pointed by conf path */
int read_cluster_conf(char *conf_path,cluster_conf_t *my_conf);
/** Reads data for EAR DB connection for eardbd and eard */
int read_eardbd_conf(char *conf_path,char *username,char *pass);

/** frees a cluster_conf_t */
void free_cluster_conf(cluster_conf_t *conf);

// Cluster configuration verbose



/** Prints in the stdout the whole cluster configuration */
void print_cluster_conf(cluster_conf_t *conf);

/** Prints in the stdout the energy_tag settings */
void print_energy_tag(energy_tag_t *etag);

/** Prints the given library conf */
void print_ear_lib_conf(earlib_conf_t *libc);

// User functions

/** returns  the energy tag entry if the username, group and/or accounts is in the list of the users/groups/acc authorized to use the given energy-tag, NULL otherwise */
energy_tag_t * is_energy_tag_privileged(cluster_conf_t *my_conf, char *user,char *group, char *acc,char *energy_tag);

/** returns true if the username, group and/or accounts is presents in the list of authorized users/groups/accounts */
int is_privileged(cluster_conf_t *my_conf, char *user,char *group, char *acc);

/** returns the user type: NORMAL, AUTHORIZED, ENERGY_TAG */
uint get_user_type(cluster_conf_t *my_conf, char *energy_tag, char *user,char *group, char *acc,energy_tag_t **my_tag);

// Policy functions

/** Check if a given policy if is valid */
int is_valid_policy(unsigned int p_id, cluster_conf_t *conf);


// Copy functions

/** Copy src in dest */
void copy_ear_lib_conf(earlib_conf_t *dest,earlib_conf_t *src);



// Default functions




/** Initializes the default values for earlib conf */
void set_default_earlib_conf(earlib_conf_t *earlibc);

/** Initializes the default values for a given island id conf. This function doesn't allocate memory */
void set_default_island_conf(node_island_t *isl_conf,uint id);

/** Initializes the installation configuration values */
void set_default_conf_install(conf_install_t *inst);

/** Initializes the default values for TAGS */
void set_default_tag_values(tag_t *tag);
// Concrete data functions
int get_node_island(cluster_conf_t *conf, char *hostname);

int get_node_server_mirror(cluster_conf_t *conf, const char *hostname, char *mirror_of);

/** given a node name, get all ips of its range*/
int get_range_ips(cluster_conf_t *my_conf, char *nodename, int **ips);
int get_ip_ranges(cluster_conf_t *my_conf, int **num_ips, int ***ips);

/** returns the ip of the nodename specified */
int get_ip(char *nodename, cluster_conf_t *conf);

/** Returns the short name for a given policy. To be used in eacct etc */
void get_short_policy(char *buf, char *policy, cluster_conf_t *conf);

/** Converts from policy name to policy_id . Returns EAR_ERROR if error*/
int policy_name_to_id(char *my_policy, cluster_conf_t *conf);

/** Converts from policy_id to policy name. Returns error if policy_id is not valid*/
int policy_id_to_name(int policy_id,char *my_policy, cluster_conf_t *conf);



#endif
