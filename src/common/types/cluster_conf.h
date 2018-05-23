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
#include <commion/environment.h>

#define GENERIC_NAME 256
#define USER		64
#define ACC			64

typedef struct policy_conf
{
    uint policy; // from environment.h
    double th;
    uint p_state;
}policy_conf_t;

typedef struct node_conf
{
	char name[GENERIC_NAME]
	uint cpus;
	uint island;	
	uint num_special_node_conf;
	policy_conf_t *special_node_conf;	
}node_conf_t;
typedef struct special_app
{
	char user[USER];
	char appname[GENERIC_NAME];
	uint p_state;
}special_app_t;
typedef struct cluster_conf
{
	char DB_pathname[GENERIC_NAME];
	char Coefficients_pathname[GENERIC_NAME];
	char tmp_dir[GENERIC_NAME];
	uint verbose;
	// List of policies	
	uint num_policies;
	policy_conf_t *power_policies;
	uint default_policy;			// selecs one of the power_policies
	// Lis of autorized users
	uint num_priv_users;
	char *priv_users[USER];
	uint num_acc;
	char *priv_acc[ACC];
	uint num_special;
	special_app_t	*special;
	// List of nodes
	uint num_nodes;
	node_conf_t *nodes;
} cluster_conf_t;


// Function declarations


/** read the cluster configuration from the ear_cluster.conf pointed by conf path */
uint read_cluster_conf(char *conf_path,cluster_conf_t *my_conf);
#else
#endif
