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
#include <common/types/configuration/cluster_conf.h>


static void print_eard_conf(eard_conf_t *conf)
{
	verbose(VCCONF,"\t eard: verbose %u period %lu max_pstate %lu	\n",conf->verbose,conf->period_powermon,conf->max_pstate);
	verbose(VCCONF,"\t eard: turbo %u port %u use_db %u use_eardbd %u \n",conf->turbo,conf->port,conf->use_mysql,conf->use_eardbd);
	verbose(VCCONF,"\t eard: force_frequencies %u\n",conf->force_frequencies);

}

static void print_eargm_conf(eargm_conf_t *conf)
{
	verbose(VCCONF,"--> EARGM configuration\n");
	verbose(VCCONF,"\t eargm: verbose %u \tuse_aggregation %u \tt1 %lu \tt2 %lu \tenergy limit: %lu \tport: %u \tmode: %u\tmail: %s\thost: %s\n",
			conf->verbose,conf->use_aggregation,conf->t1,conf->t2,conf->energy,conf->port, conf->mode, conf->mail, conf->host);
	verbose(VCCONF,"\t eargm: defcon levels [%u,%u,%u] grace period %u\n",conf->defcon_limits[0],conf->defcon_limits[1],conf->defcon_limits[2],
	conf->grace_periods);
	verbose(VCCONF,"\t policy %u (0=MaxEnergy,1=MaxPower) units=%u (-,K,M)\n",conf->policy,conf->units); 
}

static void print_db_manager(eardb_conf_t *conf)
{
	verbose(VCCONF,"--> EARDBD configuration\n");
	verbose(VCCONF, "---> Aggregation time: %u\tTCP port: %u\tSec. TCP port: %u\tSync Port: %u\tCacheSize: %u\n",
			conf->aggr_time, conf->tcp_port, conf->sec_tcp_port, conf->sync_tcp_port,conf->mem_size);
}

void print_database_conf(db_conf_t *conf)
{
	verbose(VCCONF,"\n--> MARIADB configuration\n");
	verbose(VCCONF, "---> IP: %s\tUser: %s\tPort:%u\tDB:%s\n",
			conf->ip, conf->user, conf->port, conf->database);
}

static void print_islands_conf(node_island_t *conf)
{
	verbose(VCCONF, "Islands configuration\n");
	verbose(VCCONF, "--->id: %u\n", conf->id);
	int i;
	for (i = 0; i < conf->num_ranges; i++)
	{
       
        if (conf->ranges[i].db_ip < 0){
		    verbose(VCCONF, "---->prefix: %s\tstart: %u\tend: %u\n",
                conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end);

        }else if (conf->ranges[i].sec_ip < 0){
		    verbose(VCCONF, "---->prefix: %s\tstart: %u\tend: %u\tip: %s\n",
                conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end, conf->db_ips[conf->ranges[i].db_ip]);
    
        }else{
		    verbose(VCCONF, "---->prefix: %s\tstart: %u\tend: %u\tip: %s\tbackup: %s\n",
                conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end, conf->db_ips[conf->ranges[i].db_ip], conf->backup_ips[conf->ranges[i].sec_ip]);
		}
	}
}

void print_energy_tag(energy_tag_t *etag)
{
	verbose(VCCONF, "--> Tag: %s\t pstate: %u\n", etag->tag, etag->p_state);
	int i;
	for (i = 0; i < etag->num_users; i++)
		verbose(VCCONF, "---> user: %s\n", etag->users[i]);

	for (i = 0; i < etag->num_accounts; i++)
		verbose(VCCONF, "---> accounts: %s\n", etag->accounts[i]);
	
	for (i = 0; i < etag->num_groups; i++)
		verbose(VCCONF, "---> group: %s\n", etag->groups[i]);

}

static void print_earlib_conf(earlib_conf_t *conf)
{
    verbose(VCCONF, "-->Coefficients path: %s\n-->DynAIS levels: %u\n-->DynAIS window size: %u\n",
            conf->coefficients_pathname, conf->dynais_levels, conf->dynais_window);
	verbose(VCCONF, "-->dynais timeout %u ear period %u check every %u\n",
		conf->dynais_timeout,conf->lib_period,conf->check_every);
}

void print_cluster_conf(cluster_conf_t *conf)
{
	char buffer[664];
	policy_id_to_name(conf->default_policy,buffer);	
	verbose(VCCONF, "\nDIRECTORIES\n--->DB_pathname: %s\n--->TMP_dir: %s\n--->ETC_dir: %s\n",
			conf->DB_pathname, conf->tmp_dir, conf->etc_dir);
	verbose(VCCONF, "\nGLOBALS\n--->Verbose: %u\n--->Default_policy: %s\n--->Min_time_perf_acc: %u\n",
			conf->verbose, buffer, conf->min_time_perf_acc);
	int i;
	verbose(VCCONF, "\nAVAILABLE POLICIES\n");
	for (i = 0; i < conf->num_policies; i++)
		print_policy_conf(&conf->power_policies[i]);
	verbose(VCCONF, "\nPRIVILEGED USERS\n");
	for (i = 0; i < conf->num_priv_users; i++)
		verbose(VCCONF, "--->user: %s\n", conf->priv_users[i]);
	verbose(VCCONF, "\nPRIVILEGED GROUPS\n");
	for (i = 0; i < conf->num_priv_groups; i++)
		verbose(VCCONF, "--->groups: %s\n", conf->priv_groups[i]);
	verbose(VCCONF, "\nPRIVILEGED ACCOUNTS\n");
	for (i = 0; i < conf->num_acc; i++)
		verbose(VCCONF, "--->acc: %s\n", conf->priv_acc[i]);
	verbose(VCCONF, "\nNODE CONFIGURATIONS\n");
	for (i = 0; i < conf->num_nodes; i++)
		print_node_conf(&conf->nodes[i]);
	verbose(VCCONF, "\nDB MANAGER\n");
	print_db_manager(&conf->db_manager);
	verbose(VCCONF, "\nEARD CONFIGURATION\n");
	print_eard_conf(&conf->eard);
	verbose(VCCONF, "\nEARGM CONFIGURATION\n");
	print_eargm_conf(&conf->eargm);
	print_database_conf(&conf->database);

	verbose(VCCONF, "\nISLES\n");
	for (i = 0; i < conf->num_islands; i++)
		print_islands_conf(&conf->islands[i]);
	
	verbose(VCCONF, "\nENERGY TAGS\n");
	for (i = 0; i < conf->num_tags; i++)
		print_energy_tag(&conf->e_tags[i]);

    verbose(VCCONF, "\nLIBRARY CONF\n");
    print_earlib_conf(&conf->earlib);

	verbose(VCCONF, "\n");
}
