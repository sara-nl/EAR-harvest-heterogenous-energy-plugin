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

#include <common/types/configuration/cluster_conf.h>


//#define __OLD__CONF__

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

/*
 * POLICY FUNCTIONS
 */
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

