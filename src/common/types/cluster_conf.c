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



#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <common/config.h>
#include <common/states.h>
#include <common/environment.h>
#include <common/types/cluster_conf.h>
#include <common/ear_verbose.h>

/*
    char DB_pathname[GENERIC_NAME];
    char Coefficients_pathname[GENERIC_NAME];
    char tmp_dir[GENERIC_NAME];
    uint verbose;
    // List of policies
    uint num_policies;
    policy_conf_t *power_policies;
    uint default_policy;            // selecs one of the power_policies
    // Lis of autorized users
    uint num_priv_users;
    char **priv_users;
    uint num_acc;
    char *priv_acc[ACC];
    uint num_special;
    special_app_t   *special;
    // List of nodes
    uint num_nodes;
    node_conf_t *nodes;
*/


/*
#define MIN_ENERGY_TO_SOLUTION          0
#define MIN_TIME_TO_SOLUTION            1
#define MONITORING_ONLY                 2
*/

/*
	char name[GENERIC_NAME]
	uint cpus;
	uint island;	
	uint num_special_node_conf;
	policy_conf_t *special_node_conf;	
*/

static const char *__NAME__ = "cluster_conf:";

/** read the cluster configuration from the ear_cluster.conf pointed by conf path */
int read_cluster_conf(char *conf_path,cluster_conf_t *my_conf)
{
	int i;
	char *db,*coeff,*tmp,*verbose;
	db=getenv("EAR_DB_PATHNAME");
	if (db==NULL){
		VERBOSE_N(0,"EAR_DB_PATHNAME not defined\n");
	}
	coeff=getenv("EAR_COEFF_DB_PATHNAME");
	if (coeff==NULL){
		VERBOSE_N(0,"EAR_COEFF_DB_PATHNAME not defined\n");
	}
	tmp=getenv("EAR_TMP");
	if (tmp==NULL){
		VERBOSE_N(0,"EAR_TMP not defined\n");
	}
	verbose=getenv("EAR_VERBOSE");
	if (verbose==NULL){
		VERBOSE_N(0,"EAR_VERBOSE not defined\n");
	}
	if ((db==NULL) || (coeff==NULL) || (tmp==NULL) || (verbose==NULL)) return EAR_ERROR;

	// PATHS

	strcpy(my_conf->DB_pathname,db);
	strcpy(my_conf->Coefficients_pathname,coeff);
	strcpy(my_conf->tmp_dir,tmp);
	my_conf->verbose=atoi(verbose);

	// POLICIES

	my_conf->num_policies=3;
	my_conf->power_policies=malloc(sizeof(policy_conf_t)*my_conf->num_policies);
	// MIN_ENERGY_TO_SOLUTION
	my_conf->power_policies[0].policy=0;
	my_conf->power_policies[0].th=0.1;
	my_conf->power_policies[0].p_state=1;
	// MIN_TIME_TO_SOLUTION
	my_conf->power_policies[1].policy=1;
	my_conf->power_policies[1].th=PERFORMANCE_GAIN;
	my_conf->power_policies[1].p_state=EAR_MIN_P_STATE;
	// MONITORING_ONLY
	my_conf->power_policies[2].policy=2;
	my_conf->power_policies[2].th=0;
	my_conf->power_policies[2].p_state=EAR_MIN_P_STATE;
	my_conf->default_policy=1;

	// PRIVILEGED USERS
	my_conf->num_priv_users=0;
	my_conf->priv_users=NULL;
	my_conf->num_special=0;
	my_conf->special=NULL;

	// NODES
	my_conf->num_nodes=1;
	my_conf->nodes=malloc(sizeof(node_conf_t)*my_conf->num_nodes);
	
	gethostname(my_conf->nodes[0].name,sizeof(my_conf->nodes[0].name));
	my_conf->nodes[0].cpus=16;
	my_conf->nodes[0].island=0;
	my_conf->nodes[0].num_special_node_conf=1;
	my_conf->nodes[0].special_node_conf=malloc(sizeof(policy_conf_t)*my_conf->nodes[0].num_special_node_conf);
	my_conf->nodes[0].special_node_conf[0].policy=1;
	my_conf->nodes[0].special_node_conf[0].th=PERFORMANCE_GAIN;
	my_conf->nodes[0].special_node_conf[0].p_state=EAR_MIN_P_STATE+1;
	
	return EAR_SUCCESS;
}


/*
* NODE FUNCTIONS
*/
node_conf_t *get_my_node_conf(cluster_conf_t *my_conf,char *nodename)
{
	int i=0;
	node_conf_t *n=NULL;
	do{ // At least one node is assumed
		if (strcmp(my_conf->nodes[i].name,nodename)==0){	
			n=&my_conf->nodes[i];
		}
		i++;
	}while((n==NULL) && (i<my_conf->num_nodes));
	return n;
}

void print_node_conf(node_conf_t *my_node_conf)
{
	int i;
	fprintf(stdout,"--> nodename %s cpus %u island %u \n",my_node_conf->name,my_node_conf->cpus,my_node_conf->island);
	if (my_node_conf->num_special_node_conf>0){
		for (i=0;i<my_node_conf->num_special_node_conf;i++){
			print_policy_conf(&my_node_conf->special_node_conf[i]);
		}
	}
}


/*
* POLICY FUNCTIONS
*/
void print_policy_conf(policy_conf_t *p)
{
	fprintf(stdout,"--> policy %u th %.2lf p_state %u\n",p->policy,p->th,p->p_state);
}
