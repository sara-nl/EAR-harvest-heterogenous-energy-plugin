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

void print_my_node_conf(my_node_conf_t *my_node_conf)
{
	int i;
	if (my_node_conf!=NULL){
		fprintf(stderr,"My node: cpus %u max_pstate %lu island %u ip %s ",
			my_node_conf->cpus,my_node_conf->max_pstate,my_node_conf->island,my_node_conf->db_ip);
		if (my_node_conf->db_sec_ip!=NULL){
			fprintf(stderr,"sec_ip %s ",my_node_conf->db_sec_ip);
		}
		if (my_node_conf->coef_file!=NULL){
			fprintf(stderr,"coeffs %s ",my_node_conf->coef_file);
		}
		fprintf(stderr,"\n");
		for (i=0;i<my_node_conf->num_policies;i++){
			print_policy_conf(&my_node_conf->policies[i]);
		}
	}
}


int print_my_node_conf_fd_binary(int fd,my_node_conf_t *myconf)
{
	int coef_len;
	int part1,part2;
	part1=sizeof(uint)*2+sizeof(ulong)+USER*2;
	part2=sizeof(uint)+sizeof(policy_conf_t)*TOTAL_POLICIES;
	write(fd,(char *)myconf,part1);
	if (myconf->coef_file!=NULL){
		coef_len=strlen(myconf->coef_file);
		write(fd,(char *)&coef_len,sizeof(int));
		write(fd,myconf->coef_file,coef_len);
	}else{ 
		coef_len=0;	
		write(fd,(char *)&coef_len,sizeof(int));
	}
	write(fd,(char *)myconf+part1+sizeof(char *),part2);
	return 0;
}
int read_my_node_conf_fd_binary(int fd,my_node_conf_t *myconf)
{
	int coef_len;
	int part1,part2;
	part1=sizeof(uint)*2+sizeof(ulong)+USER*2;
	part2=sizeof(uint)+sizeof(policy_conf_t)*TOTAL_POLICIES;
	read(fd,(char *)myconf,part1);
	read(fd,(char *)&coef_len,sizeof(int));
	if (coef_len>0){
		myconf->coef_file=(char *)malloc(coef_len+1);
		myconf->coef_file[coef_len]='\0';
		read(fd,myconf->coef_file,coef_len);
	}else{
		myconf->coef_file=NULL;
	}
	read(fd,(char *)myconf+part1+sizeof(char *),part2);
	
}

void print_policy_conf(policy_conf_t *p)
{
	char buffer[64];
	policy_id_to_name(p->policy,buffer);
	fprintf(stderr,"---> policy %s th %.2lf p_state %u\n",buffer,p->th,p->p_state);
}

static void print_eard_conf(eard_conf_t *conf)
{
	fprintf(stderr,"\t eard: verbose %u period %lu max_pstate %lu	\n",conf->verbose,conf->period_powermon,conf->max_pstate);
	fprintf(stderr,"\t eard: turbo %u port %u use_db %u use_eardbd %u \n",conf->turbo,conf->port,conf->use_mysql,conf->use_eardbd);
	fprintf(stderr,"\t eard: force_frequencies %u\n",conf->force_frequencies);

}

static void print_eargm_conf(eargm_conf_t *conf)
{
	fprintf(stderr,"--> EARGM configuration\n");
	fprintf(stderr,"\t eargm: verbose %u \tuse_aggregation %u \tt1 %lu \tt2 %lu \tenergy limit: %lu \tport: %u \tmode: %u\tmail: %s\thost: %s\n",
			conf->verbose,conf->use_aggregation,conf->t1,conf->t2,conf->energy,conf->port, conf->mode, conf->mail, conf->host);
	fprintf(stderr,"\t eargm: defcon levels [%u,%u,%u] grace period %u\n",conf->defcon_limits[0],conf->defcon_limits[1],conf->defcon_limits[2],
	conf->grace_periods);
	fprintf(stderr,"\t policy %u (0=MaxEnergy,1=MaxPower) units=%u (-,K,M)\n",conf->policy,conf->units); 
}

static void print_db_manager(eardb_conf_t *conf)
{
	fprintf(stderr, "---> Aggregation time: %u\tTCP port: %u\tSec. TCP port: %u\tSync Port: %u\tCacheSize: %u\n",
			conf->aggr_time, conf->tcp_port, conf->sec_tcp_port, conf->sync_tcp_port,conf->mem_size);
}

void print_database_conf(db_conf_t *conf)
{
	fprintf(stderr, "---> IP: %s\tUser: %s\tPort:%u\tDB:%s\n",
			conf->ip, conf->user, conf->port, conf->database);
}

static void print_islands_conf(node_island_t *conf)
{
	fprintf(stderr, "--->id: %u\n", conf->id);
	int i;
	for (i = 0; i < conf->num_ranges; i++)
	{
       
        if (conf->ranges[i].db_ip < 0)
		    fprintf(stderr, "---->prefix: %s\tstart: %u\tend: %u\n",
                conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end);

        else if (conf->ranges[i].sec_ip < 0)
		    fprintf(stderr, "---->prefix: %s\tstart: %u\tend: %u\tip: %s\n",
                conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end, conf->db_ips[conf->ranges[i].db_ip]);
    
        else
		    fprintf(stderr, "---->prefix: %s\tstart: %u\tend: %u\tip: %s\tbackup: %s\n",
                conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end, conf->db_ips[conf->ranges[i].db_ip], conf->backup_ips[conf->ranges[i].sec_ip]);
	}
}

void print_energy_tag(energy_tag_t *etag)
{
	fprintf(stderr, "--> Tag: %s\t pstate: %u\n", etag->tag, etag->p_state);
	int i;
	for (i = 0; i < etag->num_users; i++)
		fprintf(stderr, "---> user: %s\n", etag->users[i]);

	for (i = 0; i < etag->num_accounts; i++)
		fprintf(stderr, "---> accounts: %s\n", etag->accounts[i]);
	
	for (i = 0; i < etag->num_groups; i++)
		fprintf(stderr, "---> group: %s\n", etag->groups[i]);

}

static void print_earlib_conf(earlib_conf_t *conf)
{
    fprintf(stderr, "-->Coefficients path: %s\n-->DynAIS levels: %u\n-->DynAIS window size: %u\n",
            conf->coefficients_pathname, conf->dynais_levels, conf->dynais_window);
	fprintf(stderr, "-->dynais timeout %u ear period %u check every %u\n",
		conf->dynais_timeout,conf->lib_period,conf->check_every);
}

void print_cluster_conf(cluster_conf_t *conf)
{
	char buffer[664];
	policy_id_to_name(conf->default_policy,buffer);	
	fprintf(stderr, "\nDIRECTORIES\n--->DB_pathname: %s\n--->TMP_dir: %s\n--->ETC_dir: %s\n",
			conf->DB_pathname, conf->tmp_dir, conf->etc_dir);
	fprintf(stderr, "\nGLOBALS\n--->Verbose: %u\n--->Default_policy: %s\n--->Min_time_perf_acc: %u\n",
			conf->verbose, buffer, conf->min_time_perf_acc);
	int i;
	fprintf(stderr, "\nAVAILABLE POLICIES\n");
	for (i = 0; i < conf->num_policies; i++)
		print_policy_conf(&conf->power_policies[i]);
	fprintf(stderr, "\nPRIVILEGED USERS\n");
	for (i = 0; i < conf->num_priv_users; i++)
		fprintf(stderr, "--->user: %s\n", conf->priv_users[i]);
	fprintf(stderr, "\nPRIVILEGED GROUPS\n");
	for (i = 0; i < conf->num_priv_groups; i++)
		fprintf(stderr, "--->groups: %s\n", conf->priv_groups[i]);
	fprintf(stderr, "\nPRIVILEGED ACCOUNTS\n");
	for (i = 0; i < conf->num_acc; i++)
		fprintf(stderr, "--->acc: %s\n", conf->priv_acc[i]);
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
	
	fprintf(stderr, "\nENERGY TAGS\n");
	for (i = 0; i < conf->num_tags; i++)
		print_energy_tag(&conf->e_tags[i]);

    fprintf(stderr, "\nLIBRARY CONF\n");
    print_earlib_conf(&conf->earlib);

	fprintf(stderr, "\n");
}
