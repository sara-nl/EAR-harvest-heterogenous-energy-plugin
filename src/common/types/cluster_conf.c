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
//#define __OLD__CONF__

/** read the cluster configuration from the ear_cluster.conf pointed by conf path */
int read_cluster_conf(char *conf_path,cluster_conf_t *my_conf)
{
	#ifdef __OLD__CONF__
	int i;
	char *db,*coeff,*tmp,*verbose;
	db=getenv("EAR_DB_PATHNAME");
	if (db==NULL){
		VERBOSE_N(0,"EAR_DB_PATHNAME not defined\n");
        db="/home/xlalonso/";
	}
	coeff=getenv("EAR_COEFF_DB_PATHNAME");
	if (coeff==NULL){
		VERBOSE_N(0,"EAR_COEFF_DB_PATHNAME not defined\n");
        coeff="/home/xlalonso/";
	}
	tmp=getenv("EAR_TMP");
	if (tmp==NULL){
		VERBOSE_N(0,"EAR_TMP not defined\n");
        tmp="/var/ear";
	}
	verbose=getenv("EAR_VERBOSE");
	if (verbose==NULL){
		VERBOSE_N(0,"EAR_VERBOSE not defined\n");
        verbose="2";
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
	#else
	FILE *conf_file = fopen(conf_path, "r");
	if (conf_file == NULL)
	{
		fprintf(stderr, "ERROR opening file: %s\n", conf_path);
		return EAR_ERROR;
	}
	get_cluster_config(conf_file, my_conf);
    //print_cluster_conf(my_conf);
	#endif
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
	fprintf(stderr,"--> nodename %s cpus %u island %u \n",my_node_conf->name,my_node_conf->cpus,my_node_conf->island);
	if (my_node_conf->num_special_node_conf>0){
		for (i=0;i<my_node_conf->num_special_node_conf;i++){
			print_policy_conf(&my_node_conf->special_node_conf[i]);
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

policy_conf_t *get_my_policy_conf(cluster_conf_t *my_cluster,node_conf_t *my_node,uint p_id)
{
	policy_conf_t *my_policy=NULL;
	uint i;
    if (my_node != NULL)
    {
	    // We look first for special configurations
    	for(i=0;i<my_node->num_special_node_conf;i++){
		    if (my_node->special_node_conf[i].policy==p_id) return(&my_node->special_node_conf[i]);
	    }
    }
	// and now for default values
	for(i=0;i<my_cluster->num_policies;i++){
		if (my_cluster->power_policies[i].policy==p_id) return (&my_cluster->power_policies[i]);
	}
	return NULL;
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
    int range_start, range_end;
    char nodename[GENERIC_NAME];

    start = strtok_r(namelist, "[", &buffer_ptr);
    token = strtok_r(NULL, ",", &buffer_ptr);
    //in this case, only one node is specified in the line
    if (token == NULL)
    {
        conf->nodes = realloc(conf->nodes, sizeof(node_conf_t)*(conf->num_nodes+1));
        sprintf(conf->nodes[conf->num_nodes].name, "%s", start);

        return 1;
    }
    //at least one node if we reach this point
    int node_count = 0;
    while (token != NULL)
    {
        strclean(token, ']');
        if (strchr(token, '-'))
        {
            char has_zero = strchr(token, '0') != NULL;
            range_start = atoi(strtok_r(token, "-", &second_ptr));
            range_end = atoi(strtok_r(NULL, "-", &second_ptr));
            int i;
            for (i = range_start; i <= range_end; i++, node_count++)
            {
                conf->nodes = realloc(conf->nodes, sizeof(node_conf_t)*(conf->num_nodes+node_count+1));
                //CASE FOR <10 numbers
                if (i < 10 && has_zero)
                    sprintf(conf->nodes[conf->num_nodes+node_count].name, "%s0%u\0", start, i);

                else
                    sprintf(conf->nodes[conf->num_nodes+node_count].name, "%s%u\0", start, i);

            }
        }
        else
        {
            node_count++;
            conf->nodes = realloc(conf->nodes, sizeof(node_conf_t)*(conf->num_nodes+node_count));
            sprintf(conf->nodes[conf->num_nodes+node_count-1].name, "%s%s\0", start, token);
        }
        token = strtok_r(NULL, ",", &buffer_ptr);
    }
    return node_count;
}


/** Fills the cluster_conf_t from the info from the FILE */
void get_cluster_config(FILE *conf_file, cluster_conf_t *conf)
{
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
        else if (!strcmp(token, "COEFFICIENTSDB"))
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
        else if (!strcmp(token, "USERNAME"))
        {
            //special parsing on this one
            token = strtok(NULL, "=");
            char *token2 = strtok(NULL, "=");
            char *token3 = strtok(NULL, "=");
        
            token = strtok(token, " ");
            token2 = strtok(token2, " ");
            token3 = strtok(token3, " ");
            
            conf->num_special++;
            conf->special = realloc(conf->special, sizeof(special_app_t)*conf->num_special);
            int i = conf->num_special - 1;
            strcpy(conf->special[i].user, token);
            strcpy(conf->special[i].appname, token2);
            conf->special[i].p_state = atoi(token3);
        }
        else if (!strcmp(token, "ISLAND"))
        {
            int i = 0;
            int island = 0;
            int num_nodes = 0;
            int num_cpus = 0;
            //fully restore the line
            line[strlen(line)] = '=';
            char *primary_ptr;
            char *secondary_ptr;
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
                                conf->nodes[conf->num_nodes+i].special_node_conf = 
                                            realloc(conf->nodes[conf->num_nodes+i].special_node_conf, 
                                                sizeof(policy_conf_t)*(k+1));
                                conf->nodes[conf->num_nodes+i].special_node_conf[k].policy = 
                                    conf->power_policies[j].policy;
                                conf->nodes[conf->num_nodes+i].special_node_conf[k].p_state = atoi(token);
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
                            conf->nodes[conf->num_nodes+i].special_node_conf = 
                                        realloc(conf->nodes[conf->num_nodes+i].special_node_conf, 
                                                sizeof(policy_conf_t)*(k+1));
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].policy = MIN_TIME_TO_SOLUTION;
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].th = atof(token);
                            conf->nodes[conf->num_nodes+i].num_special_node_conf++;

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
                            conf->nodes[conf->num_nodes+i].special_node_conf = 
                                        realloc(conf->nodes[conf->num_nodes+i].special_node_conf, 
                                                sizeof(policy_conf_t)*(k+1));
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].policy = MIN_ENERGY_TO_SOLUTION;
                            conf->nodes[conf->num_nodes+i].special_node_conf[k].th = atof(token);
                            conf->nodes[conf->num_nodes+i].num_special_node_conf++;
                        }
                    }
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
            conf->num_nodes += num_nodes;
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
    
   // for (i = 0; i < conf->num_nodes; i++)
     //   free(conf->nodes[i].special_node_conf);
    free(conf->nodes);

    free(conf->power_policies);

}

void print_special_app(special_app_t *app)
{
    fprintf(stderr, "--->appname: %s\tuser: %s\t p_state: %u\n",
            app->appname, app->user, app->p_state);
}

void print_cluster_conf(cluster_conf_t *conf)
{
    fprintf(stderr, "DIRECTORIES\n--->DB_pathname: %s\n--->Coefficients_pathname: %s\n--->TMP_dir: %s\n", 
            conf->DB_pathname, conf->Coefficients_pathname, conf->tmp_dir);
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
    fprintf(stderr, "\nNODE CONFIGURATIONS\n");
    for (i = 0; i < conf->num_nodes; i++)
        print_node_conf(&conf->nodes[i]);

}
