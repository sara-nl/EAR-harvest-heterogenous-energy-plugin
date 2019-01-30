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

#include <stdio.h>
#include <stdlib.h>
#include <common/config.h>
#include <common/output/verbose.h>
#include <common/output/error.h>
#include <common/output/debug.h>
#include <common/sizes.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/environment.h>


//#define __OLD__CONF__


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
			if ((my_conf->islands[i].ranges[range_id].sec_ip>=0) && (my_conf->islands[i].num_backups)){
            	strcpy(n->db_sec_ip, my_conf->islands[i].backup_ips[my_conf->islands[i].ranges[range_id].sec_ip]);
			}
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



/** returns the ear.conf path. It checks at $EAR_ETC/ear/ear.conf  */
int get_ear_conf_path(char *ear_conf_path)
{
	char my_path[GENERIC_NAME];
	char *my_etc;
	int fd;
	my_etc=getenv("EAR_ETC");
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
		i=0;
		while((i<my_conf->num_priv_users) && (!found)){
			if (strcmp(user,my_conf->priv_users[i])==0) found=1;
			else if (strcmp(my_conf->priv_users[i],"all")==0) found=1;
			else i++;
		}
	}
	if (found)	return found;
	if (group!=NULL){
		i=0;
		while((i<my_conf->num_priv_groups) && (!found)){
			if (strcmp(group,my_conf->priv_groups[i])==0) found=1;
			else i++;
		}
	}
	if (found)	return found;
	if (acc!=NULL){
		i=0;
		while((i<my_conf->num_acc) && (!found)){
        	if (strcmp(acc,my_conf->priv_acc[i])==0) found=1;
			else i++;
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
		i=0;
        while((i<my_tag->num_users) && (!found)){
			debug("Can_use_energy_tag user %s vs %s\n",user,my_tag->users[i]);
            if (strcmp(user,my_tag->users[i])==0) found=1;
			else if (strcmp(my_tag->users[i],"all")==0) found=1;
			else i++;
        }
    }
    if (found)  return my_tag;
    if (group!=NULL){
		i=0;
        while((i<my_tag->num_groups) && (!found))
		{
			debug("Can_use_energy_tag group %s (%lu) vs %s (%lu)\n",
					group, strlen(group), my_tag->groups[i], strlen(my_tag->groups[i]));

            if (strcmp(group,my_tag->groups[i])==0) found=1;
			else i++;
        }
    }
    if (found)  return my_tag;
    if (acc!=NULL){
		i=0;
        while((i<my_tag->num_accounts) && (!found)){
			debug("Can_use_energy_tag acc %s vs %s\n",acc,my_tag->accounts[i]);
            if (strcmp(acc,my_tag->accounts[i])==0) found=1;
			else i++;
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
	i=0;
	while((i<my_conf->num_tags) && (!found)){
		if (strcmp(energy_tag,my_conf->e_tags[i].tag)==0){
			found=1;
			my_tag=&my_conf->e_tags[i];
		}else i++;
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
	i=0;
	while ((i<my_conf->num_tags) && (!found)){
		if (strcmp(etag,my_conf->e_tags[i].tag)==0)	found=1;
		else i++;
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
	verbose(VPRIV,"Checking user %s group %s acc %s etag %s\n",user,group,acc,energy_tag);
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

/* Copy src in dest */
void copy_ear_lib_conf(earlib_conf_t *dest,earlib_conf_t *src)
{
	if ((dest!=NULL) && (src!=NULL)){
		strcpy(dest->coefficients_pathname,src->coefficients_pathname);
		dest->dynais_levels=src->dynais_levels;
		dest->dynais_window=src->dynais_window;
		dest->dynais_timeout=src->dynais_timeout;
		dest->lib_period=src->lib_period;
		dest->check_every=src->check_every;
	}
}

/* Prints the given library conf */
void print_ear_lib_conf(earlib_conf_t *libc)
{
	if (libc!=NULL){
		verbose(VCCONF,"coeffs %s dynais level %u dynais window_size %u\n",
		libc->coefficients_pathname,libc->dynais_levels,libc->dynais_window);
		verbose(VCCONF,"dynais timeout %u lib_period %u check_every %u\n",
		libc->dynais_timeout,libc->lib_period,libc->check_every);
	}
}

void copy_eard_conf(eard_conf_t *dest,eard_conf_t *src)
{
	memcpy(dest,src,sizeof(eard_conf_t));
}
void copy_eargmd_conf(eargm_conf_t *dest,eargm_conf_t *src)
{
	memcpy(dest,src,sizeof(eargm_conf_t));
}
void copy_eardb_conf(db_conf_t *dest,db_conf_t *src)
{
	memcpy(dest,src,sizeof(db_conf_t));
}
void copy_eardbd_conf(eardb_conf_t *dest,eardb_conf_t *src)
{
	memcpy(dest,src,sizeof(eardb_conf_t));
}

/*** DEFAULT VALUES ****/
void set_default_eardbd_conf(eardb_conf_t *eardbdc)
{
	eardbdc->aggr_time     = 60;
	eardbdc->insr_time     = 30;
	eardbdc->tcp_port      = 4711;
	eardbdc->sec_tcp_port  = 4712;
	eardbdc->sync_tcp_port = 4713;
	eardbdc->mem_size      = 120;

	eardbdc->mem_size_types[0] = 60;
	eardbdc->mem_size_types[1] = 22;
	eardbdc->mem_size_types[2] = 5;
	eardbdc->mem_size_types[3] = 0;
	eardbdc->mem_size_types[4] = 7;
	eardbdc->mem_size_types[5] = 5;
	eardbdc->mem_size_types[6] = 1;
}

void set_default_eard_conf(eard_conf_t *eardc)
{
	eardc->verbose=1;           /* default 1 */
    eardc->period_powermon=POWERMON_FREQ;  /* default 30000000 (30secs) */
    eardc->max_pstate=1;       /* default 1 */
    eardc->turbo=USE_TURBO;             /* Fixed to 0 by the moment */
    eardc->port=DAEMON_PORT_NUMBER;              /* mandatory */
    eardc->use_mysql=1;         /* Must EARD report to DB */
    eardc->use_eardbd=1;        /* Must EARD report to DB using EARDBD */
	eardc->force_frequencies=1; /* EARD will force frequencies */
}

void set_default_earlib_conf(earlib_conf_t *earlibc)
{
	strcpy(earlibc->coefficients_pathname,DEFAULT_COEFF_PATHNAME);
	earlibc->dynais_levels=DEFAULT_DYNAIS_LEVELS;
	earlibc->dynais_window=DEFAULT_DYNAIS_WINDOW_SIZE;
	earlibc->dynais_timeout=MAX_TIME_DYNAIS_WITHOUT_SIGNATURE;
	earlibc->lib_period=PERIOD;
	earlibc->check_every=MPI_CALLS_TO_CHECK_PERIODIC;
}
void set_default_eargm_conf(eargm_conf_t *eargmc)
{
	eargmc->verbose=1;
	eargmc->use_aggregation=1;
	eargmc->t1=DEFAULT_T1;
	eargmc->t2=DEFAULT_T2;
	eargmc->energy=DEFAULT_T2*DEFAULT_POWER;
	eargmc->units=1;
	eargmc->policy=MAXPOWER;
	eargmc->port=EARGM_PORT_NUMBER;
	eargmc->mode=0;
	eargmc->defcon_limits[0]=85;
	eargmc->defcon_limits[1]=90;
	eargmc->defcon_limits[2]=95;
	eargmc->grace_periods=GRACE_T1;
	strcpy(eargmc->mail,"nomail");
}

void set_default_db_conf(db_conf_t *db_conf)
{
    strcpy(db_conf->user, "ear_daemon");
    strcpy(db_conf->ip, "127.0.0.1");
    db_conf->port = 0;
}

/*
 *
 *
 *
 */

int get_node_island(cluster_conf_t *conf, char *hostname)
{
	my_node_conf_t *node;
	int island;

	node = get_my_node_conf(conf, hostname);

	if (node == NULL) {
		return EAR_ERROR;
	}

	island = node->island;
	free(node);

	return island;
}

int get_node_server_mirror(cluster_conf_t *conf, const char *hostname, char *mirror_of)
{
	char hostalias[SZ_NAME_MEDIUM];
	node_island_t *is;
	const char *a, *n;
	int found_server;
	int found_mirror;
	int found_both;
	int i, k;
	char *p;

	//
	strncpy(hostalias, hostname, SZ_NAME_MEDIUM);
	found_both   = 0;
	found_server = 0;
	found_mirror = 0;
	a = hostalias;
	n = hostname;
	
	// Finding a possible short form
	if ((p = strchr(a, '.')) != NULL) {
		p[0] = '\0';
	}

	for (i = 0; i < conf->num_islands && !found_both; ++i)
	{
		is = &conf->islands[i];

		for (k = 0; k < is->num_ranges && !found_both; k++)
		{
			p = is->db_ips[is->ranges[k].db_ip];

			if (!found_server && p != NULL &&
				((strncmp(p, n, strlen(n)) == 0) || (strncmp(p, a, strlen(a) == 0))))
			{
				found_server = 1;
			}

			if (!found_mirror && is->ranges[k].sec_ip >= 0)
			{
				p = is->backup_ips[is->ranges[k].sec_ip];

				if (p != NULL &&
					((strncmp(p, n, strlen(n)) == 0) || (strncmp(p, a, strlen(a) == 0))))
				{
					strcpy(mirror_of, is->db_ips[is->ranges[k].db_ip]);
					found_mirror = 1;
				}
			}

			found_both = found_server && found_mirror;
		}
	}

	//Codes:
	//	- 0000 (0x00): nothing
	//	- 0001 (0x01): server
	//	- 0010 (0x02): mirror
	//	- 0011 (0x03): both
	found_mirror = (found_mirror << 1);
	return (found_server | found_mirror);
}
