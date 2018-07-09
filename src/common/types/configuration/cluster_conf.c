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
            if (!strcmp(aux_name, nodename)) return i;
            else continue;
        }
        if (node->ranges[i].end == node->ranges[i].start)
        {
            sprintf(aux_name, "%s%u", node->ranges[i].prefix, node->ranges[i].start);
            if (!strcmp(aux_name, nodename)) return i;
            else continue;
        }
        for (j = node->ranges[i].end; j >= node->ranges[i].start && j > 0; j--)
        {
            if (j < 10 && node->ranges[i].end > 10)
                sprintf(aux_name, "%s0%u", node->ranges[i].prefix, j);
            else
                sprintf(aux_name, "%s%u", node->ranges[i].prefix, j);
            if (!strcmp(aux_name, nodename)) return i;
        }
    }

    return -1;
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
    int range_id = -1;
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
		if ((range_id = island_range_conf_contains_node(&my_conf->islands[i], nodename)) >= 0) {
            n->island = my_conf->islands[i].id;
            strcpy(n->db_ip, my_conf->islands[i].db_ips[my_conf->islands[i].ranges[range_id].db_ip]);
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
	n->max_pstate=my_conf->eard.max_pstate;

	return n;
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

/** CHECKING USER TYPE */
/* returns true if the username, group and/or accounts is presents in the list of authorized users/groups/accounts */
int is_privileged(cluster_conf_t *my_conf, char *user,char *group, char *acc)
{
	int i;
	int found=0;
	if (user!=NULL){
		for (i=0;(i<my_conf->num_priv_users) && (!found);i++){
			if (strcmp(user,my_conf->priv_users[i])==0) found=1;
		}
	}
	if (found)	return found;
	if (group!=NULL){
		for (i=0;(i<my_conf->num_priv_groups) && (!found);i++){
			if (strcmp(group,my_conf->priv_groups[i])==0) found=1;
		}
	}
	if (found)	return found;
	if (acc!=NULL){
		for (i=0;(i<my_conf->num_acc) && (!found);i++){
        	if (strcmp(acc,my_conf->priv_acc[i])==0) found=1;
    	}
	}
	return found;
}

/* returns true if the username, group and/or accounts can use the given energy_tag_t */
energy_tag_t * can_use_energy_tag(char *user,char *group, char *acc,energy_tag_t *my_tag)
{
	int i;
    int found=0;
    if (user!=NULL){
        for (i=0;(i<my_tag->num_users) && (!found);i++){
            if (strcmp(user,my_tag->users[i])==0) found=1;
        }
    }
    if (found)  return my_tag;
    if (group!=NULL){
        for (i=0;(i<my_tag->num_groups) && (!found);i++){
            if (strcmp(group,my_tag->groups[i])==0) found=1;
        }
    }
    if (found)  return my_tag;
    if (acc!=NULL){
        for (i=0;(i<my_tag->num_accounts) && (!found);i++){
            if (strcmp(acc,my_tag->accounts[i])==0) found=1;
        }
    }
	if (found) return my_tag;
	return NULL;
}

/* returns  the energy tag entry if the username, group and/or accounts is in the list of the users/groups/acc authorized to use the given energy-tag, NULL otherwise */
energy_tag_t * is_energy_tag_privileged(cluster_conf_t *my_conf, char *user,char *group, char *acc,char *energy_tag)
{
	int i;
	int found=0;
	energy_tag_t *my_tag;
	if (energy_tag==NULL) return NULL;
	for (i=0;(i<my_conf->num_tags) && (!found);i++){
		if (strcmp(energy_tag,my_conf->e_tags[i].tag)==0){
			found=1;
			my_tag=&my_conf->e_tags[i];	
		}
	}	
	if (!found)	return NULL;
	return can_use_energy_tag(user,group,acc,my_tag);
}

/** Returns true if the energy tag exists */
energy_tag_t * energy_tag_exists(cluster_conf_t *my_conf,char *etag)
{
	int i;
	int found=0;
	if (etag==NULL)	return NULL;
	for (i=0;(i<my_conf->num_tags) && (!found);i++){
		if (strcmp(etag,my_conf->e_tags[i].tag)==0)	found=1;
	}
	if (found) return &my_conf->e_tags[i];
	return NULL;
}

/** returns the user type: NORMAL, AUTHORIZED, ENERGY_TAG */
uint get_user_type(cluster_conf_t *my_conf, char *energy_tag, char *user,char *group, char *acc,energy_tag_t **my_tag)
{
	uint type=NORMAL;
	energy_tag_t *is_tag;
	int flag;
	*my_tag=NULL;
	fprintf(stderr,"Checking user %s group %s acc %s etag %s\n",user,group,acc,energy_tag);	
	/* We first check if it is authorized user */
	flag=is_privileged(my_conf,user,group,acc);
	if (flag){
		if (energy_tag!=NULL){
			is_tag=energy_tag_exists(my_conf,energy_tag); 
			if (is_tag!=NULL){ *my_tag=is_tag;return ENERGY_TAG;}
			else return AUTHORIZED;
		}else return AUTHORIZED;
	}
	/* It is an energy tag user ? */
	is_tag=is_energy_tag_privileged(my_conf, user,group,acc,energy_tag);
	if (is_tag!=NULL){ 
		*my_tag=is_tag;
		return ENERGY_TAG;
	} else return NORMAL;
}

