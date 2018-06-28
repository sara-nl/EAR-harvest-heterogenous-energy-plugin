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
#include <string.h>
#include <fcntl.h>
#include <common/config.h>
#include <common/states.h>
#include <common/environment.h>
#include <common/string_enhanced.h>
#include <common/types/cluster_conf.h>
#include <common/ear_verbose.h>



/*
#define MIN_ENERGY_TO_SOLUTION          0
#define MIN_TIME_TO_SOLUTION            1
#define MONITORING_ONLY                 2
*/


static const char *__NAME__ = "cluster_conf:";
//#define __OLD__CONF__

void cae_conf(cluster_conf_t *my_conf)
{
    int i;
    char *db=NULL,*coeff=NULL,*tmp=NULL,*verbose=NULL,*etc=NULL;
    db=getenv("EAR_DB_PATHNAME");
    if (db==NULL){
        VERBOSE_N(0,"EAR_DB_PATHNAME not defined\n");
        //db="/home/xlalonso/";
        db="/home/xjcorbalan/my_ear/dbs/db.";
    }
    coeff=getenv("EAR_COEFF_DB_PATHNAME");
    if (coeff==NULL){
        VERBOSE_N(0,"EAR_COEFF_DB_PATHNAME not defined\n");
        //coeff="/home/xlalonso/";
        coeff="/home/xjcorbalan/my_ear/coeffs/coeff.";
    }
    tmp=getenv("EAR_TMP");
    if (tmp==NULL){
        VERBOSE_N(0,"EAR_TMP not defined\n");
        tmp="/var/ear";
    }
    etc=getenv("ETC");
    if (etc==NULL){
        VERBOSE_N(0,"ETC not defined\n");
        etc="/etc";
    }
    verbose=getenv("EAR_VERBOSE");
    if (verbose==NULL){
        VERBOSE_N(0,"EAR_VERBOSE not defined\n");
        verbose="1";
    }

    if ((db==NULL) || (coeff==NULL) || (tmp==NULL) || (verbose==NULL) || (etc==NULL)) return;

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
    my_conf->power_policies[0].p_state=DEFAULT_MAX_P_STATE;
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

	// ISLANDS
	

	//EARD
	my_conf->eard.verbose=1;
	my_conf->eard.period_powermon=POWERMON_FREQ;
	my_conf->eard.max_pstate=1;
	my_conf->eard.turbo=0;
	my_conf->eard.port=DAEMON_PORT_NUMBER;
	
	//EARGM
	
	//EARDB


}


/** read the cluster configuration from the ear_cluster.conf pointed by conf path */
int read_cluster_conf(char *conf_path,cluster_conf_t *my_conf)
{


	#ifdef __OLD__CONF__
	cae_conf(my_conf);	
	#else
	FILE *conf_file = fopen(conf_path, "r");
	if (conf_file == NULL)
	{
		fprintf(stderr, "ERROR opening file: %s\n", conf_path);
		return EAR_ERROR;
	}
	get_cluster_config(conf_file, my_conf);
    fclose(conf_file);
    //print_cluster_conf(my_conf);
	#endif
	return EAR_SUCCESS;
}

char range_conf_contains_node(node_conf_t *node, char *nodename)
{
    char aux_name[64];
    int i, j;
    for (i = 0; i < node->range_count; i++)
    {
        if (node->range[i].end == -1)
        {
            sprintf(aux_name, "%s", node->range[i].prefix);
            if (!strcmp(aux_name, nodename)) return 1;
            else continue;
        }
        if (node->range[i].end == node->range[i].start)
        {
            sprintf(aux_name, "%s%u", node->range[i].prefix, node->range[i].start);
            if (!strcmp(aux_name, nodename)) return 1;
            else continue;
        }
        for (j = node->range[i].end; j >= node->range[i].start && j > 0; j--)
        {
            if (j < 10 && node->range[i].end > 10)
                sprintf(aux_name, "%s0%u", node->range[i].prefix, j);
            else
                sprintf(aux_name, "%s%u", node->range[i].prefix, j);
            if (!strcmp(aux_name, nodename)) return 1;
        }
    }

    return 0;
}

char island_range_conf_contains_node(node_island_t *node, char *nodename)
{
    char aux_name[64];
    int i, j;
    for (i = 0; i < node->num_ranges; i++)
    {
        if (node->ranges[i].end == -1)
        {
            sprintf(aux_name, "%s", node->ranges[i].prefix);
            if (!strcmp(aux_name, nodename)) return 1;
            else continue;
        }
        if (node->ranges[i].end == node->ranges[i].start)
        {
            sprintf(aux_name, "%s%u", node->ranges[i].prefix, node->ranges[i].start);
            if (!strcmp(aux_name, nodename)) return 1;
            else continue;
        }
        for (j = node->ranges[i].end; j >= node->ranges[i].start && j > 0; j--)
        {
            if (j < 10 && node->ranges[i].end > 10)
                sprintf(aux_name, "%s0%u", node->ranges[i].prefix, j);
            else
                sprintf(aux_name, "%s%u", node->ranges[i].prefix, j);
            if (!strcmp(aux_name, nodename)) return 1;
        }
    }

    return 0;
}



/*
* NODE FUNCTIONS
*/
node_conf_t *get_node_conf(cluster_conf_t *my_conf,char *nodename)
{
	int i=0;
	node_conf_t *n=NULL;
	do{ // At least one node is assumed
		//if (strcmp(my_conf->nodes[i].name,nodename)==0){	
		if (range_conf_contains_node(&my_conf->nodes[i], nodename)) {
        	n=&my_conf->nodes[i];
		}
		i++;
	}while((n==NULL) && (i<my_conf->num_nodes));
	return n;
}


my_node_conf_t *get_my_node_conf(cluster_conf_t *my_conf,char *nodename)
{
	int i=0, j=0;
	my_node_conf_t *n=calloc(1, sizeof(my_node_conf_t));
    n->num_policies = my_conf->num_policies;
    n->policies = calloc(n->num_policies, sizeof(policy_conf_t));
    int num_spec_nodes = 0;
    
    while(i<my_conf->num_nodes)
    {
		if (range_conf_contains_node(&my_conf->nodes[i], nodename)) {
            n->cpus = my_conf->nodes[i].cpus;
            n->coef_file = my_conf->nodes[i].coef_file;
            num_spec_nodes = my_conf->nodes[i].num_special_node_conf;
            //n->policies = my_conf->nodes[i].special_node_conf;
            memcpy(n->policies, my_conf->nodes[i].special_node_conf, num_spec_nodes * sizeof(policy_conf_t));
		}
		i++;
    }
    
    i = 0;
    do{ // At least one node is assumed
		if (island_range_conf_contains_node(&my_conf->islands[i], nodename)) {
            n->island = my_conf->islands[i].id;
            strcpy(n->db_ip, my_conf->islands[i].db_ip);
		}
		i++;
	}while(i<my_conf->num_islands);


    //pending checks for policies
    for (i = 0; i < my_conf->num_policies; i++)
    {
        char found = 0;
        for (j = 0; j < num_spec_nodes; j++)
            if (my_conf->power_policies[i].policy == n->policies[j].policy) found = 1;
        
        if (!found)
        {
            memcpy(&n->policies[num_spec_nodes], &my_conf->power_policies[i], sizeof(policy_conf_t));
            num_spec_nodes++;
        }
    }

	return n;
}

void print_node_conf(node_conf_t *my_node_conf)
{
	int i;
	fprintf(stderr,"-->cpus %u island %u def_file: %s\n", my_node_conf->cpus, my_node_conf->island, my_node_conf->coef_file);
    for (i = 0; i < my_node_conf->range_count; i++)
        printf("---->prefix: %s\tstart: %u\tend: %u\n", my_node_conf->range[i].prefix, my_node_conf->range[i].start, my_node_conf->range[i].end);
    for (i=0;i<my_node_conf->num_special_node_conf;i++){
        print_policy_conf(&my_node_conf->special_node_conf[i]);
    }
}

/*
 * uint cpus;
 * uint island;
 * char db_ip[USER];
 * char *coef_file;
 * uint num_policies;
 * policy_conf_t *policies;
 */

void print_my_node_conf(my_node_conf_t *my_node_conf)
{
	int i;
	if (my_node_conf!=NULL){
		fprintf(stderr,"My node: cpus %u island %u ip %s ",my_node_conf->cpus,my_node_conf->island,my_node_conf->db_ip);
		if (my_node_conf->coef_file!=NULL){
			fprintf(stderr,"coeffs %s \n",my_node_conf->coef_file);
		}
		for (i=0;i<my_node_conf->num_policies;i++){
			print_policy_conf(&my_node_conf->policies[i]);
		}
	}
}


/*
* POLICY FUNCTIONS
*/

/** Prints in the stderr policy configuration */
void print_policy_conf(policy_conf_t *p)
{
	fprintf(stderr,"---> policy %u th %.2lf p_state %u\n",p->policy,p->th,p->p_state);
}


/** Converts from policy name to policy_id */
int policy_name_to_id(char *my_policy)
{
    if (my_policy!=NULL){
        if ((strcmp(my_policy,"MIN_ENERGY_TO_SOLUTION")==0) || (strcmp(my_policy,"min_energy_to_solution")==0)) return MIN_ENERGY_TO_SOLUTION;
        else if ((strcmp(my_policy,"MIN_TIME_TO_SOLUTION")==0) || (strcmp(my_policy,"min_time_to_solution")==0)) return MIN_TIME_TO_SOLUTION;
        else if ((strcmp(my_policy,"MONITORING_ONLY")==0) || (strcmp(my_policy,"monitoring_only")==0)) return MONITORING_ONLY;
    }
	return EAR_ERROR;
}

policy_conf_t *get_my_policy_conf(cluster_conf_t *my_cluster,my_node_conf_t *my_node,uint p_id)
{
	policy_conf_t *my_policy=NULL;
	uint i;
	uint nump=0;
    while((nump<my_node->num_policies) && (my_node->policies[nump].policy!=p_id)) nump++;
    if (nump<my_node->num_policies){
        my_policy=&my_node->policies[nump];
    }
	return my_policy;
}

/** Converts from policy_id to policy name. Returns error if policy_id is not valid*/
int policy_id_to_name(int policy_id,char *my_policy)
{
	int ret=EAR_SUCCESS;
	switch(policy_id)
    {
        case MIN_ENERGY_TO_SOLUTION:
            strcpy(my_policy,"MIN_ENERGY_TO_SOLUTION");
        	break;
        case MIN_TIME_TO_SOLUTION:
            strcpy(my_policy,"MIN_TIME_TO_SOLUTION");
        	break;
        case MONITORING_ONLY:
            strcpy(my_policy,"MONITORING_ONLY");
        	break;
		default: ret=EAR_ERROR;
    }
	return ret;

}



void insert_th_policy(cluster_conf_t *conf, char *token, int policy)
{
    int i;
    for (i = 0; i < conf->num_policies; i++)
    {
        if (conf->power_policies[i].policy == policy)
        conf->power_policies[i].th = atof(token);
    }

}

int set_nodes_conf(cluster_conf_t *conf, char *namelist)
{
    char *buffer_ptr;
    char *second_ptr;
    char *token;
    char *start;
    char *next_token;
    int range_start, range_end;
    char nodename[GENERIC_NAME];

    start = strtok_r(namelist, "[", &buffer_ptr);
    token = strtok_r(NULL, ",", &buffer_ptr);
    
    conf->nodes = realloc(conf->nodes, sizeof(node_conf_t)*(conf->num_nodes+1));
    memset(&conf->nodes[conf->num_nodes], 0, sizeof(node_conf_t));

    //in this case, only one node is specified in the line
    if (token == NULL)
    {

        conf->nodes[conf->num_nodes].range = realloc(conf->nodes[conf->num_nodes].range, sizeof(node_range_t)*(conf->nodes[conf->num_nodes].range_count+1));
        memset(&conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count], 0, sizeof(node_range_t));
        sprintf(conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count].prefix, "%s", start);
        conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count].start =
            conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count].end = -1;
        conf->nodes[conf->num_nodes].range_count++;

        return 1;
    }
    //at least one node if we reach this point
    int node_count = 0;
    while (token != NULL)
    {

        conf->nodes[conf->num_nodes].range = realloc(conf->nodes[conf->num_nodes].range, sizeof(node_range_t)*(conf->nodes[conf->num_nodes].range_count+1));
        memset(&conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count], 0, sizeof(node_range_t));
        sprintf(conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count].prefix, "%s", start);

        if (strchr(token, ']'))
        {
            next_token = strtok_r(NULL, "[", &buffer_ptr);
            strclean(token, ']');
        }
        else next_token = NULL; 
        
        if (strchr(token, '-'))
        {
            conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count].start = atoi(strtok_r(token, "-", &second_ptr));
            conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count].end = atoi(strtok_r(NULL, "-", &second_ptr));
            
        }
        else
        {
            conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count].end =
                conf->nodes[conf->num_nodes].range[conf->nodes[conf->num_nodes].range_count].start =
                    atoi(token);
        }
        token = strtok_r(NULL, ",", &buffer_ptr);
        if (next_token != NULL) start = next_token;
        conf->nodes[conf->num_nodes].range_count++;
    }
    return 1;
}

void generate_node_ranges(node_island_t *island, char *nodelist)
{
    char *buffer_ptr;
    char *second_ptr;
    char *token;
    char *start;
    char *next_token;
    int range_start, range_end;
    char nodename[GENERIC_NAME];

    start = strtok_r(nodelist, "[", &buffer_ptr);
    token = strtok_r(NULL, ",", &buffer_ptr);
    //in this case, only one node is specified in the line
    if (token == NULL)
    {
        island->ranges = realloc(island->ranges, sizeof(node_range_t)*(island->num_ranges+1));
        memset(&island->ranges[island->num_ranges], 0, sizeof(node_range_t));
        sprintf(island->ranges[island->num_ranges].prefix, "%s", start);
        island->ranges[island->num_ranges].start = island->ranges[island->num_ranges].end = -1;
        island->num_ranges++;
    }
    //at least one range if we reach this point
    int range_count = 0;
    while (token != NULL)
    {
        island->ranges = realloc(island->ranges, sizeof(node_range_t)*(island->num_ranges+range_count+1));
        memset(&island->ranges[island->num_ranges+range_count], 0, sizeof(node_range_t));
        strcpy(island->ranges[island->num_ranges+range_count].prefix, start);
        if (strchr(token, ']'))
        {
            next_token = strtok_r(NULL, "[", &buffer_ptr);
            strclean(token, ']');
        }
        else next_token = NULL; 
        
        if (strchr(token, '-'))
        {
            island->ranges[island->num_ranges+range_count].start = atoi(strtok_r(token, "-", &second_ptr));
            island->ranges[island->num_ranges+range_count].end = atoi(strtok_r(NULL, "-", &second_ptr));
            range_count++;
        }
        else
        {
            island->ranges[island->num_ranges+range_count].start = island->ranges[island->num_ranges+range_count].end = 
                atoi(token);
            range_count++;
        }
        token = strtok_r(NULL, ",", &buffer_ptr);
        if (next_token != NULL) start = next_token;
    }
    island->num_ranges += range_count;
}

int get_default_pstate(policy_conf_t *pow_pol, int num_pol, int policy)
{
    int i;
    for (i = 0; i < num_pol; i++)
    {
        if (pow_pol[i].policy == policy)
            return pow_pol[i].p_state;
    }
    return 0;
}

/** Fills the cluster_conf_t from the info from the FILE */
void get_cluster_config(FILE *conf_file, cluster_conf_t *conf)
{
    memset(conf, 0, sizeof(cluster_conf_t));
    char line[256];
    char *token;
    while (fgets(line, 256, conf_file) != NULL)
    {
        if (line[0] == '#') continue;
        token = strtok(line, "=");
        strtoup(token);
        
        if (!strcmp(token, "DEFAULTPOWERPOLICY"))
        {
            token = strtok(NULL, "=");
            token = strtok(token, "\n");
            conf->default_policy = policy_name_to_id(token);
        }
        else if (!strcmp(token, "DATABASEPATHNAME"))
        {
            token = strtok(NULL, "=");
            token = strtok(token, "\n");
            strcpy(conf->DB_pathname, token);
        }
        else if (!strcmp(token, "COEFFICIENTSDIR"))
        {
            token = strtok(NULL, "=");
            token = strtok(token, "\n");
            strcpy(conf->Coefficients_pathname, token);
        }
        else if (!strcmp(token, "TMPDIR"))
        {
            token = strtok(NULL, "=");
            token = strtok(token, "\n");
            strcpy(conf->tmp_dir, token);
        }
        else if (!strcmp(token, "ETCDIR"))
        {
            token = strtok(NULL, "=");
            token = strtok(token, "\n");
            strcpy(conf->etc_dir, token);
        }
        else if (!strcmp(token, "VERBOSE"))
        {
            token = strtok(NULL, "=");
            conf->verbose = atoi(token);
        }
        else if (!strcmp(token, "SUPPORTEDPOLICIES"))
        {
            token = strtok(NULL, "=");
            token = strtok(token, ",");
            while (token != NULL)
            {
                conf->num_policies++;
                strclean(token, '\n');
                if (conf->num_policies == 1)
                    conf->power_policies = NULL;
                    //conf->power_policies = malloc(sizeof(policy_conf_t));
                conf->power_policies = realloc(conf->power_policies, sizeof(policy_conf_t)*conf->num_policies);

                conf->power_policies[conf->num_policies-1].policy = policy_name_to_id(token);
                token = strtok(NULL, ",");
            }
        }
        else if (!strcmp(token, "DEFAULTPSTATES"))
        {
            token = strtok(NULL, "=");
            token = strtok(token, ",");
            int i;
            for (i = 0; i < conf->num_policies && token != NULL; token =strtok(NULL, ","), i++) 
            {
                conf->power_policies[i].p_state = atoi(token);
            }
        }
        else if (!strcmp(token, "MINEFFICIENCYGAIN"))
        {
            token = strtok(NULL, "=");
            insert_th_policy(conf, token, MIN_TIME_TO_SOLUTION);
        }
        else if (!strcmp(token, "MAXPERFORMANCEPENALTY"))
        {
            token = strtok(NULL, "=");
            insert_th_policy(conf, token, MIN_ENERGY_TO_SOLUTION);
        }
        else if (!strcmp(token, "MINTIMEPERFORMANCEACCURACY"))
        {
            token = strtok(NULL, "=");
            conf->min_time_perf_acc = atoi(token);
        }
        else if (!strcmp(token, "PRIVILEGEDUSERS"))
        {
            token = strtok(NULL, "=");
            token = strtok(token, ",");
            while (token != NULL)
            {
                conf->num_priv_users++;
                conf->priv_users = realloc(conf->priv_users, sizeof(char *)*conf->num_priv_users);
                strclean(token, '\n');           
                conf->priv_users[conf->num_priv_users-1] = malloc(strlen(token)+1);
                strcpy(conf->priv_users[conf->num_priv_users-1], token);
                token = strtok(NULL, ",");
            }
        }
        else if (!strcmp(token, "PRIVILEGEDACCOUNTS"))
        {
            token = strtok(NULL, "=");
            token = strtok(token, ",");
            while (token != NULL)
            {
                conf->num_acc++;
                conf->priv_acc = realloc(conf->priv_acc, sizeof(char *)*conf->num_acc);
                strclean(token, '\n');
                conf->priv_acc[conf->num_acc-1] = malloc(strlen(token)+1);
                strcpy(conf->priv_acc[conf->num_acc-1], token);
                token = strtok(NULL, ",");
            }
        }
        else if (!strcmp(token, "USERSWITHENERGYTAG"))
        {
            //special parsing on this one
            token = strtok(NULL, "=");
            token = strtok(token, ",");
            while (token != NULL)
            {
                strclean(token, '\n');
                conf->num_special++;
                conf->special = realloc(conf->special, sizeof(special_app_t)*conf->num_special);
                strcpy(conf->special[conf->num_special - 1].user, token);
                token = strtok(NULL, ",");            
            }
        }
        else if (!strcmp(token, "ENERGYTAG"))
        {
            // token = strtok(NULL, "=");
            // token = strtok(token, ",");
            // conf->e_tags = realloc(conf->e_tags, sizeof(energy_tag_t)*(conf->num_tags+1));
            // strcpy(conf->e_tags[conf->num_tags].tag, token);
            // token = strtok(NULL, ",");
            // conf->e_tags[conf->num_tags].p_state = atoi(token);
            // conf->num_tags++;

            //fully restore the line as we need 2 buffer pointers for this task
            line[strlen(line)] = '=';
            char *primary_ptr;
            char *secondary_ptr;
            token = strtok_r(line, " ", &primary_ptr);
            while (token != NULL)
            {
                token = strtok_r(token, "=", &secondary_ptr);
                strtoup(token);

                //this must always be the first one
                if (!strcmp(token, "ENERGYTAG"))
                {
                    conf->e_tags = realloc(conf->e_tags, sizeof(energy_tag_t) * (conf->num_tags+1));
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    memset(&conf->e_tags[conf->num_tags], 0, sizeof(energy_tag_t));
                    strcpy(conf->e_tags[conf->num_tags].tag, token);
                    conf->e_tags[conf->num_tags].users = NULL;
                    conf->e_tags[conf->num_tags].groups = NULL;
                    conf->e_tags[conf->num_tags].accounts = NULL;
                    conf->num_tags++;
                }
                else if (!strcmp(token, "FREQ"))
                {
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    conf->e_tags[conf->num_tags-1].p_state = atoi(token);
                }
                else if (!strcmp(token, "USERS"))
                {
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    token = strtok_r(token, ",", &secondary_ptr);
                    while (token != NULL)
                    {
                        conf->e_tags[conf->num_tags-1].users = realloc(conf->e_tags[conf->num_tags-1].users, 
                                                                       sizeof(char *)*(conf->e_tags[conf->num_tags-1].num_users+1));
                        conf->e_tags[conf->num_tags-1].users[conf->e_tags[conf->num_tags-1].num_users] = malloc(strlen(token)+1);
                        strcpy(conf->e_tags[conf->num_tags-1].users[conf->e_tags[conf->num_tags-1].num_users], token);
                        conf->e_tags[conf->num_tags-1].num_users++;
                    }
                }
                else if (!strcmp(token, "GROUPS"))
                {
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    token = strtok_r(token, ",", &secondary_ptr);
                    while (token != NULL)
                    {
                        conf->e_tags[conf->num_tags-1].groups = realloc(conf->e_tags[conf->num_tags-1].groups, 
                                                                       sizeof(char *)*(conf->e_tags[conf->num_tags-1].num_groups+1));
                        conf->e_tags[conf->num_tags-1].groups[conf->e_tags[conf->num_tags-1].num_groups] = malloc(strlen(token)+1);
                        strcpy(conf->e_tags[conf->num_tags-1].groups[conf->e_tags[conf->num_tags-1].num_groups], token);
                        conf->e_tags[conf->num_tags-1].num_groups++;
                    }
                }
                else if (!strcmp(token, "ACCOUNTS"))
                {
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    token = strtok_r(token, ",", &secondary_ptr);
                    while (token != NULL)
                    {
                        conf->e_tags[conf->num_tags-1].accounts = realloc(conf->e_tags[conf->num_tags-1].accounts, 
                                                                       sizeof(char *)*(conf->e_tags[conf->num_tags-1].num_accounts+1));
                        conf->e_tags[conf->num_tags-1].accounts[conf->e_tags[conf->num_tags-1].num_accounts] = malloc(strlen(token)+1);
                        strcpy(conf->e_tags[conf->num_tags-1].accounts[conf->e_tags[conf->num_tags-1].num_accounts], token);
                        conf->e_tags[conf->num_tags-1].num_accounts++;
                    }
                }
                token = strtok_r(NULL, " ", &primary_ptr);
            }
        }

        //HARDWARE NODE CONFIG
        else if (!strcmp(token, "NODENAME"))
        {
            int i = 0;
            int island = 0;
            int num_nodes = 0;
            int num_cpus = 0;
            //fully restore the line
            line[strlen(line)] = '=';
            char *primary_ptr;
            char *secondary_ptr;
            char *coef_file = NULL;
            token = strtok_r(line, " ", &primary_ptr);
            while (token != NULL)
            {

                //fetches the first half of the pair =
                token = strtok_r(token, "=", &secondary_ptr);
                strtoup(token);
                if (!strcmp(token, "ISLAND"))
                {
                    //fetches the second half of the pair = 
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    island = atoi(token);
                }
                else if (!strcmp(token, "NODENAME"))
                {
                    //fetches the second half of the pair = 
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    num_nodes = set_nodes_conf(conf, token);
                }
                else if (!strcmp(token, "CPUS"))
                {
                    int i;
                    num_cpus = atoi(strtok_r(NULL, "=", &secondary_ptr));
                }
                else if (!strcmp(token, "DEFAULTPSTATES"))
                {
                    //fetches the second half of the pair = 
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    char *tertiary_ptr;
                    int i, j, k;
                    char found = 0;
                    token = strtok_r(token, ",", &tertiary_ptr);
                    for (j = 0; j < conf->num_policies && token != NULL; j++)
                    {
                        for (i = 0; i < num_nodes; i++)
                        {
                            found = 0;
                            for (k = 0; k < conf->nodes[conf->num_nodes+i].num_special_node_conf; k++)
                            {
                                if (conf->nodes[conf->num_nodes+i].special_node_conf[k].policy ==
                                        conf->power_policies[j].policy)
                                {
                                    found = 1;
                                    conf->nodes[conf->num_nodes+i].special_node_conf[k].p_state = atoi(token);
                                    break;
                                }
                            }
                            if (!found)
                            {
                                //if there is no such p_state and it's the same, we don't create a new one
                                if (conf->power_policies[j].p_state == atoi(token))
                                    break;
                                
                                k = conf->nodes[conf->num_nodes+i].num_special_node_conf;
                                if (k == 0)
                                    conf->nodes[conf->num_nodes+i].special_node_conf = NULL;
                                conf->nodes[conf->num_nodes+i].special_node_conf = 
                                            realloc(conf->nodes[conf->num_nodes+i].special_node_conf, 
                                                sizeof(policy_conf_t)*(k+1));
                                conf->nodes[conf->num_nodes+i].special_node_conf[k].policy = 
                                    conf->power_policies[j].policy;
                                conf->nodes[conf->num_nodes+i].special_node_conf[k].p_state = atoi(token);
                                conf->nodes[conf->num_nodes+i].special_node_conf[k].th = 
                                    conf->power_policies[j].th;
                                conf->nodes[conf->num_nodes+i].num_special_node_conf++;
                            }
                        }
                        token = strtok_r(NULL, ",", &tertiary_ptr);
                    }
                    

                }
                else if (!strcmp(token, "MINEFFICIENCYGAIN"))
                {
                    //fetches the second half of the pair = 
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    int i, k;
                    char found = 0;
                    for (i = 0; i < num_nodes; i++)
                    {
                        for (k = 0; k < conf->nodes[conf->num_nodes+i].num_special_node_conf; k++)
                        {
                            if (conf->nodes[conf->num_nodes+i].special_node_conf[k].policy ==
                                    MIN_TIME_TO_SOLUTION)
                            {
                                found = 1;
                                conf->nodes[conf->num_nodes+i].special_node_conf[k].th = atof(token);
                                break;
                            }
                        }
                        if (!found)
                        {
                            k = conf->nodes[conf->num_nodes+i].num_special_node_conf;
                            if (k == 0)
                                conf->nodes[conf->num_nodes+i].special_node_conf = NULL;
                            conf->nodes[conf->num_nodes+i].special_node_conf = 
                                        realloc(conf->nodes[conf->num_nodes+i].special_node_conf, 
                                                sizeof(policy_conf_t)*(k+1));
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].policy = MIN_TIME_TO_SOLUTION;
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].th = atof(token);
                            conf->nodes[conf->num_nodes+i].num_special_node_conf++;
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].p_state = 
                                get_default_pstate(conf->power_policies, conf->num_policies, MIN_TIME_TO_SOLUTION);
                        }
                    }
                }
                else if (!strcmp(token, "MAXPERFORMANCEPENALTY"))
                {
                    //fetches the second half of the pair = 
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    int i, k;
                    char found = 0;
                    for (i = 0; i < num_nodes; i++)
                    {
                        for (k = 0; k < conf->nodes[conf->num_nodes+i].num_special_node_conf; k++)
                        {
                            if (conf->nodes[conf->num_nodes+i].special_node_conf[k].policy ==
                                    MIN_ENERGY_TO_SOLUTION)
                            {
                                found = 1;
                                conf->nodes[conf->num_nodes+i].special_node_conf[k].th = atof(token);
                                break;
                            }
                        }
                        if (!found)
                        {
                            k = conf->nodes[conf->num_nodes+i].num_special_node_conf;
                            if (k == 0)
                                conf->nodes[conf->num_nodes+i].special_node_conf = NULL;
                            conf->nodes[conf->num_nodes+i].special_node_conf = 
                                        realloc(conf->nodes[conf->num_nodes+i].special_node_conf, 
                                                sizeof(policy_conf_t)*(k+1));
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].policy = MIN_ENERGY_TO_SOLUTION;
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].th = atof(token);
                            conf->nodes[conf->num_nodes+i].num_special_node_conf++;
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].p_state = 
                                get_default_pstate(conf->power_policies, conf->num_policies, MIN_TIME_TO_SOLUTION);
                        }
                    }
                }

                else if (!strcmp(token, "DEFCOEFFICIENTSFILE"))
                {
                    token = strtok_r(NULL, "=", &secondary_ptr);
                    strclean(token, '\n');
                    coef_file = malloc(sizeof(char)*strlen(token)+1);
                    strcpy(coef_file, token);
                }

                //fetches the second half of the pair = 
                token = strtok_r(NULL, "=", &secondary_ptr);
                token = strtok_r(NULL, " ", &primary_ptr);
            }

            //global data for all nodes in that line
            for (i = 0; i < num_nodes; i++)
            {
                conf->nodes[conf->num_nodes+i].cpus = num_cpus;
                conf->nodes[conf->num_nodes+i].island = island;
            }
            if (coef_file != NULL)
                for (i = 0; i < num_nodes; i++)
                    conf->nodes[conf->num_nodes+i].coef_file = coef_file;
            conf->num_nodes += num_nodes;
        }

        //EARD conf
        else if (!strcmp(token, "NODEDAEMONVERBOSE"))
        {
            token = strtok(NULL, "=");
            conf->eard.verbose = atoi(token);
        }
        else if (!strcmp(token, "NODEDAEMONPOWERMONFREQ"))
        {
            token = strtok(NULL, "=");
            conf->eard.period_powermon = atoi(token);
        }
        else if (!strcmp(token, "NODEDAEMONMAXPSTATE"))
        {
            token = strtok(NULL, "=");
            conf->eard.max_pstate = atoi(token);
        }
        else if (!strcmp(token, "NODEDAEMONTURBO"))
        {
            token = strtok(NULL, "=");
            conf->eard.turbo = atoi(token);
        }
        else if (!strcmp(token, "NODEDAEMONPORT"))
        {
            token = strtok(NULL, "=");
            conf->eard.port = atoi(token);
        }

        //DB MANAGER
        else if (!strcmp(token, "DATABASECACHEAGGREGATIONTIME"))
        {
            token = strtok(NULL, "=");
            conf->db_manager.aggr_time = atoi(token);
        }
        else if (!strcmp(token, "DATABASECACHEPORTTCP"))
        {
            token = strtok(NULL, "=");
            conf->db_manager.tcp_port = atoi(token);
        }
        else if (!strcmp(token, "DATABASECACHEPORTUDP"))
        {
            token = strtok(NULL, "=");
            conf->db_manager.udp_port = atoi(token);
        }

        //GLOBAL MANAGER
        else if (!strcmp(token, "GLOBALMANAGERVERBOSE"))
        {
            token = strtok(NULL, "=");
            conf->eargm.verbose = atoi(token);
        }
        else if (!strcmp(token, "GLOBALMANAGERPERIODT1"))
        {
            token = strtok(NULL, "=");
            conf->eargm.t1 = atoi(token);
        }
        else if (!strcmp(token, "GLOBALMANAGERPERIODT2"))
        {
            token = strtok(NULL, "=");
            conf->eargm.t2 = atoi(token);
        }
        else if (!strcmp(token, "GLOBALMANAGERENERGYLIMIT"))
        {
            token = strtok(NULL, "=");
            conf->eargm.energy = atoi(token);
        }
        else if (!strcmp(token, "GLOBALMANAGERPORT"))
        {
            token = strtok(NULL, "=");
            conf->eargm.port = atoi(token);
        }
        else if (!strcmp(token, "GLOBALMANAGERMODE"))
        {
            token = strtok(NULL, "=");
            conf->eargm.mode = atoi(token);
        }
        else if (!strcmp(token, "GLOBALMANAGERMAIL"))
        {
            token = strtok(NULL, "=");
            strclean(token, '\n');
            strcpy(conf->eargm.mail, token);
        }
        else if (!strcmp(token, "GLOBALMANAGERHOST"))
        {
            token = strtok(NULL, "=");
            strclean(token, '\n');
            strcpy(conf->eargm.host, token);
        }

        //MARIADB/MYSQL config
        else if (!strcmp(token, "MARIADBIP"))
        {
            token = strtok(NULL, "=");
            strclean(token, '\n');
            strcpy(conf->database.ip, token);
        }
        else if (!strcmp(token, "MARIADBUSER"))
        {
            token = strtok(NULL, "=");
            strclean(token, '\n');
            strcpy(conf->database.user, token);
        }
        else if (!strcmp(token, "MARIADBPASSW"))
        {
            token = strtok(NULL, "=");
            strclean(token, '\n');
            strcpy(conf->database.pass, token);
        }
        else if (!strcmp(token, "MARIADBDATABASE"))
        {
            token = strtok(NULL, "=");
            strclean(token, '\n');
            strcpy(conf->database.database, token);
        }
        else if (!strcmp(token, "MARIADBPORT"))
        {
            token = strtok(NULL, "=");
            conf->database.port = atoi(token);
        }

        //ISLES config
        else if (!strcmp(token, "ISLAND"))
        {

            int i = 0;
            int island = 0;
            int num_nodes = 0;
            int num_cpus = 0;
            //fully restore the line
            line[strlen(line)] = '=';

            if (conf->num_islands == 0)
                conf->islands = NULL;
            conf->islands = realloc(conf->islands, sizeof(node_island_t)*(conf->num_islands+1));
            memset(&conf->islands[conf->num_islands], 0, sizeof(node_island_t));

            //token = strtok_r(line, " ", &primary_ptr); 
            token = strtok(line, "=");
            while (token != NULL)
            {
                strtoup(token);
                if (!strcmp(token, "ISLAND"))
                {
                    token = strtok(NULL, " ");
                    conf->islands[conf->num_islands].id = atoi(token);
                }
                else if (!strcmp(token, "DATABASECACHEIP"))
                {
                    token = strtok(NULL, " ");
                    strclean(token, '\n');
                    strcpy(conf->islands[conf->num_islands].db_ip, token);
                }
                else if (!strcmp(token, "NODES"))
                {
                    token = strtok(NULL, " ");
                    generate_node_ranges(&conf->islands[conf->num_islands], token);
                }
                token = strtok(NULL, "=");
            }
            conf->num_islands++;
        }

    }

}

/** frees a cluster_conf_t */
void free_cluster_conf(cluster_conf_t *conf)
{
    int i;
    for (i = 0; i < conf->num_priv_users; i++)
        free(conf->priv_users[i]);
    free(conf->priv_users);
    
    for (i = 0; i < conf->num_acc; i++)
        free(conf->priv_acc[i]);
    free(conf->priv_acc);

    free(conf->special);
    
    for (i = 0; i < conf->num_nodes; i++)
        free(conf->nodes[i].special_node_conf);

    char *prev_file = NULL;
    for (i = 0; i < conf->num_nodes; i++) {
        if (conf->nodes[i].coef_file != NULL && conf->nodes[i].coef_file != prev_file)  {
            prev_file = conf->nodes[i].coef_file;
            free(conf->nodes[i].coef_file);
        }
            free(conf->nodes[i].range);
    }
    
    free(conf->nodes);

    free(conf->power_policies);

    for (i = 0; i < conf->num_islands; i++)
        free(conf->islands[i].ranges);

    free(conf->islands);

    int j;
    for (i = 0; i < conf->num_tags; i++)
    {
        for (j = 0; j < conf->e_tags[i].num_users; j++)
            free(conf->e_tags[i].users[j]);

        for (j = 0; j < conf->e_tags[i].num_groups; j++)
            free(conf->e_tags[i].groups[j]);

        for (j = 0; j < conf->e_tags[i].num_accounts; j++)
            free(conf->e_tags[i].accounts[j]);

        free(conf->e_tags[i].users);
        free(conf->e_tags[i].groups);
        free(conf->e_tags[i].accounts);
    }
    free(conf->e_tags);

    memset(conf, 0, sizeof(cluster_conf_t));

}

void print_special_app(special_app_t *app)
{
    fprintf(stderr, "--->user: %s\tpstate: %u\n",
            app->user, app->p_state);
}

void print_energy_tag(energy_tag_t *tag)
{
    fprintf(stderr, "---->tag: %s\tpstate: %u\n",
            tag->tag, tag->p_state);
}

void print_eard_conf(eard_conf_t *conf)
{
	fprintf(stderr,"\t eard: verbose %u period %lu max_pstate %lu	\n",conf->verbose,conf->period_powermon,conf->max_pstate);
	fprintf(stderr,"\t eard: turbo %u port %u\n",conf->turbo,conf->port);

}

void print_eargm_conf(eargm_conf_t *conf)
{
	fprintf(stderr,"--> EARGM configuration\n");
	fprintf(stderr,"\t eargm: verbose %u \tt1 %lu \tt2 %lu \tenergy limit: %lu \tport: %u \tmode: %u\tmail: %s\thost: %s\n",
                    conf->verbose,conf->t1,conf->t2,conf->energy,conf->port, conf->mode, conf->mail, conf->host);
}

void print_db_manager(eardb_conf_t *conf)
{
    fprintf(stderr, "---> Aggregation time: %u\tTCP port: %u\tUDP port: %u\n",
            conf->aggr_time, conf->tcp_port, conf->udp_port);
}

void print_database_conf(db_conf_t *conf)
{
    fprintf(stderr, "---> IP: %s\tUser: %s\tPort:%u\tHost:%s\n",
        conf->ip, conf->user, conf->port, conf->database);
}

void print_islands_conf(node_island_t *conf)
{
    fprintf(stderr, "--->id: %u\tip: %s\n", conf->id, conf->db_ip);
    int i;
    for (i = 0; i < conf->num_ranges; i++)
    {
        fprintf(stderr, "---->prefix: %s\tstart: %u\tend: %u\n", conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end);
    }
}

void print_cluster_conf(cluster_conf_t *conf)
{
    fprintf(stderr, "\nDIRECTORIES\n--->DB_pathname: %s\n--->Coefficients_pathname: %s\n--->TMP_dir: %s\n--->ETC_dir: %s\n", 
            conf->DB_pathname, conf->Coefficients_pathname, conf->tmp_dir, conf->etc_dir);
    fprintf(stderr, "\nGLOBALS\n--->Verbose: %u\n--->Default_policy: %u\n--->Min_time_perf_acc: %u\n",
            conf->verbose, conf->default_policy, conf->min_time_perf_acc);
    int i;
    fprintf(stderr, "\nAVAILABLE POLICIES\n");
    for (i = 0; i < conf->num_policies; i++)
        print_policy_conf(&conf->power_policies[i]);
    fprintf(stderr, "\nPRIVILEGED USERS\n");
    for (i = 0; i < conf->num_priv_users; i++)
        fprintf(stderr, "--->user: %s\n", conf->priv_users[i]);
    fprintf(stderr, "\nPRIVILEGED ACCOUNTS\n");
    for (i = 0; i < conf->num_acc; i++)
        fprintf(stderr, "--->acc: %s\n", conf->priv_acc[i]);
    fprintf(stderr, "\nSPECIAL APPLICATIONS\n");
    for (i = 0; i < conf->num_special; i++)
        print_special_app(&conf->special[i]);
    for (i = 0; i < conf->num_tags; i++)
        print_energy_tag(&conf->e_tags[i]);
    fprintf(stderr, "\nNODE CONFIGURATIONS\n");
    for (i = 0; i < conf->num_nodes; i++)
        print_node_conf(&conf->nodes[i]);
    fprintf(stderr, "\nDB MANAGER\n");
    print_db_manager(&conf->db_manager);
    fprintf(stderr, "\nEARD CONFIGURATION\n");
	print_eard_conf(&conf->eard);
    fprintf(stderr, "\nEARGM CONFIGURATION\n");
	print_eargm_conf(&conf->eargm);	
    print_database_conf(&conf->database);

    fprintf(stderr, "\nISLES\n");
    for (i = 0; i < conf->num_islands; i++)
        print_islands_conf(&conf->islands[i]);
    fprintf(stderr, "\n");

}
/** returns the ear.conf path. It checks first at /etc/ear/ear.conf and, it is not available, checks at $EAR_INSTALL_PATH/etc/s
ysconf/ear.conf */
int get_ear_conf_path(char *ear_conf_path)
{
	char my_path[GENERIC_NAME];
	char *my_etc;
	int fd;
	my_etc=getenv("ETC");
	if (my_etc==NULL) return EAR_ERROR;	
	sprintf(my_path,"%s/ear/ear.conf",my_etc);
	fd=open(my_path,O_RDONLY);
    if (fd>0){
        strcpy(ear_conf_path,my_path);
        return EAR_SUCCESS;
    }
	return EAR_ERROR;
}

