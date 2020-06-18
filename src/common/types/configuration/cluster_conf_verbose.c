/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <common/config.h>
#include <common/output/verbose.h>
#include <common/types/configuration/cluster_conf.h>


void print_eard_conf(eard_conf_t *conf)
{
	verbosen(VCCONF,"\t eard: verbosen %u period %lu max_pstate %lu	\n",conf->verbose,conf->period_powermon,conf->max_pstate);
	verbosen(VCCONF,"\t eard: turbo %u port %u use_db %u use_eardbd %u \n",conf->turbo,conf->port,conf->use_mysql,conf->use_eardbd);
	verbosen(VCCONF,"\t eard: force_frequencies %u\n",conf->force_frequencies);
	verbosen(VCCONF,"\t eard: use_log %u\n",conf->use_log);

}

void print_eargm_conf(eargm_conf_t *conf)
{
	verbosen(VCCONF,"--> EARGM configuration\n");
	verbosen(VCCONF,"\t eargm: verbosen %u \tuse_aggregation %u \tt1 %lu \tt2 %lu \tenergy limit: %lu \tport: %u \tmode: %u\tmail: %s\thost: %s\n",
			conf->verbose,conf->use_aggregation,conf->t1,conf->t2,conf->energy,conf->port, conf->mode, conf->mail, conf->host);
	verbosen(VCCONF,"\t eargm: defcon levels [%u,%u,%u] grace period %u\n",conf->defcon_limits[0],conf->defcon_limits[1],conf->defcon_limits[2],
	conf->grace_periods);
	verbosen(VCCONF,"\t policy %u (0=MaxEnergy,other=error) units=%u (-,K,M)\n",conf->policy,conf->units); 
	verbosen(VCCONF,"\t use_log %u\n",conf->use_log);
	#if POWERCAP
	verbosen(VCCONF,"\t cluster_power_limit %lu powercap_check_period %lu\n",conf->power,conf->t1_power);
	verbosen(VCCONF,"\t powercap_mode %lu (0=monitoring, 1=auto [def])\n",conf->powercap_mode);
	verbosen(VCCONF,"\t power limit for action %lu\n",conf->defcon_power_limit);
	verbosen(VCCONF,"\t powercap_action %s\n",conf->powercap_action);
	verbosen(VCCONF,"\t energycap_action %s\n",conf->energycap_action);
	#endif
}

void print_db_manager(eardb_conf_t *conf)
{
	verbosen(VCCONF,"--> EARDBD configuration\n");
	verbosen(VCCONF, "---> Insertion time %u\tAggregation time: %u\tTCP port: %u\tSec. TCP port: %u\tSync Port: %u\tCacheSize: %u\n",
			conf->insr_time,conf->aggr_time, conf->tcp_port, conf->sec_tcp_port, conf->sync_tcp_port,conf->mem_size);
	verbosen(VCCONF,"--> use_log %u\n",conf->use_log);
	
}

void print_database_conf(db_conf_t *conf)
{
	verbosen(VCCONF,"\n--> MARIADB configuration\n");
	verbosen(VCCONF, "---> IP: %s\tUser: %s\tUser commands %s\tPort:%u\tDB:%s\n",
			conf->ip, conf->user, conf->user_commands,conf->port, conf->database);
	verbosen(VCCONF,"-->max_connections %u report_node_details %u report_sig_details %u report_loops %u\n",conf->max_connections,conf->report_node_detail,conf->report_sig_detail,conf->report_loops);
}

void print_islands_conf(node_island_t *conf)
{
	int i, j;
	verbosen(VCCONF, "Islands configuration\n");
	verbosen(VCCONF, "--->id: %u (min_power %.0lf, max_power %.0lf,power_cap %.1lf power_cap_type=%s)\n", conf->id,conf->min_sig_power,conf->max_sig_power,conf->max_power_cap,conf->power_cap_type);
	verbosen(VCCONF, "--->       (power>%.0lf or temp>%lu are errors)\n", conf->max_error_power,conf->max_temp);
    if (conf->num_tags > 0)
    {
        verbosen(VCCONF, "--->       tags: ");
        for (i = 0; i < conf->num_tags; i++)
        {
            verbosen(VCCONF, "tag%d:%s  ", i, conf->tags[i]);
        }
        verbosen(VCCONF, "\n");
    }
	for (i = 0; i < conf->num_ranges; i++)
	{
       
        if (conf->ranges[i].db_ip < 0){
		    verbosen(VCCONF, "---->prefix: %s\tstart: %u\tend: %u\n",
                conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end);

        }else if (conf->ranges[i].sec_ip < 0){
		    verbosen(VCCONF, "---->prefix: %s\tstart: %u\tend: %u\tip: %s\n",
                conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end, conf->db_ips[conf->ranges[i].db_ip]);
    
        }else{
		    verbosen(VCCONF, "---->prefix: %s\tstart: %u\tend: %u\tip: %s\tbackup: %s\n",
                conf->ranges[i].prefix, conf->ranges[i].start, conf->ranges[i].end, conf->db_ips[conf->ranges[i].db_ip], conf->backup_ips[conf->ranges[i].sec_ip]);
		}
        if (conf->ranges[i].num_tags > 0) {
            verbosen(VCCONF, "\t----tags: ");
            for (j = 0; j < conf->ranges[i].num_tags; j++)
            {
                verbosen(VCCONF, "tag%d:%s  ", j, conf->specific_tags[conf->ranges[i].specific_tags[j]]);
            }
            verbosen(VCCONF, "\n");
        }

	}
}

void print_tags_conf(tag_t *tag)
{
    verbosen(VCCONF, "--> Tag: %s\ttype: %d\tdefault: %d\tpowercap_type: %d\n", tag->id, tag->type, tag->is_default, tag->powercap_type);
    verbosen(VCCONF, "\t\tavx512_freq: %lu\tavx2_freq: %lu\tmax_power: %lu\tmin_power: %lu\terror_power: %lu\tpowercap: %lu\n", 
                     tag->max_avx512_freq, tag->max_avx2_freq, tag->max_power, tag->min_power, tag->error_power, tag->powercap);
    verbosen(VCCONF, "\t\tenergy_model: %s\tenergy_plugin: %s\tpowercap_plugin: %s\n", tag->energy_model, tag->energy_plugin, tag->powercap_plugin);
}

void print_energy_tag(energy_tag_t *etag)
{
	verbosen(VCCONF, "--> Tag: %s\t pstate: %u\n", etag->tag, etag->p_state);
	int i;
	for (i = 0; i < etag->num_users; i++)
		verbosen(VCCONF, "---> user: %s\n", etag->users[i]);

	for (i = 0; i < etag->num_accounts; i++)
		verbosen(VCCONF, "---> accounts: %s\n", etag->accounts[i]);
	
	for (i = 0; i < etag->num_groups; i++)
		verbosen(VCCONF, "---> group: %s\n", etag->groups[i]);

}

void print_earlib_conf(earlib_conf_t *conf)
{
    verbosen(VCCONF, "-->Coefficients path: %s\n-->DynAIS levels: %u\n-->DynAIS window size: %u\n",
            conf->coefficients_pathname, conf->dynais_levels, conf->dynais_window);
	verbosen(VCCONF, "-->dynais timeout %u ear period %u check every %u\n",
		conf->dynais_timeout,conf->lib_period,conf->check_every);
}



void print_cluster_conf(cluster_conf_t *conf)
{
	char buffer[664];
	verbosen(VCCONF, "\nDIRECTORIES\n--->DB_pathname: %s\n--->TMP_dir: %s\n--->ETC_dir: %s\n",
			conf->DB_pathname, conf->install.dir_temp, conf->install.dir_conf);
	verbosen(VCCONF, "\nPlugins_path: %s\n",conf->install.dir_plug);
	verbosen(VCCONF, "PLUGINS: Energy %s power_models %s\n",conf->install.obj_ener,conf->install.obj_power_model);
    if (strlen(conf->net_ext) > 1)
    {
    	verbosen(VCCONF, "\nGLOBALS\n--->Verbose: %u\n--->Default_policy: %s\n--->Min_time_perf_acc: %u\n--->Network_extension: %s\n",
	    		conf->verbose, conf->power_policies[conf->default_policy].name, conf->min_time_perf_acc, conf->net_ext);
    }
    else
    {
    	verbosen(VCCONF, "\nGLOBALS\n--->Verbose: %u\n--->Default_policy: %s\n--->Min_time_perf_acc: %u\n",
	    		conf->verbose, conf->power_policies[conf->default_policy].name, conf->min_time_perf_acc);
    }

	int i;
	verbosen(VCCONF, "\nAVAILABLE POLICIES\n");
	for (i = 0; i < conf->num_policies; i++)
		print_policy_conf(&conf->power_policies[i]);
	verbosen(VCCONF, "\nPRIVILEGED USERS\n");
	for (i = 0; i < conf->num_priv_users; i++)
		verbosen(VCCONF, "--->user: %s\n", conf->priv_users[i]);
	verbosen(VCCONF, "\nPRIVILEGED GROUPS\n");
	for (i = 0; i < conf->num_priv_groups; i++)
		verbosen(VCCONF, "--->groups: %s\n", conf->priv_groups[i]);
	verbosen(VCCONF, "\nPRIVILEGED ACCOUNTS\n");
	for (i = 0; i < conf->num_acc; i++)
		verbosen(VCCONF, "--->acc: %s\n", conf->priv_acc[i]);
	verbosen(VCCONF, "\nNODE CONFIGURATIONS\n");
	for (i = 0; i < conf->num_nodes; i++)
		print_node_conf(&conf->nodes[i]);
	verbosen(VCCONF, "\nDB MANAGER\n");
	print_db_manager(&conf->db_manager);
	verbosen(VCCONF, "\nEARD CONFIGURATION\n");
	print_eard_conf(&conf->eard);
	verbosen(VCCONF, "\nEARGM CONFIGURATION\n");
	print_eargm_conf(&conf->eargm);
	print_database_conf(&conf->database);

	verbosen(VCCONF, "\nISLES\n");
	for (i = 0; i < conf->num_islands; i++)
		print_islands_conf(&conf->islands[i]);
	
    verbosen(VCCONF, "\nGENERAL TAGS\n");
    for (i = 0; i < conf->num_tags; i++)
        print_tags_conf(&conf->tags[i]);

	verbosen(VCCONF, "\nENERGY TAGS\n");
	for (i = 0; i < conf->num_etags; i++)
		print_energy_tag(&conf->e_tags[i]);

    verbosen(VCCONF, "\nLIBRARY CONF\n");
    print_earlib_conf(&conf->earlib);

	verbosen(VCCONF, "\n");
}
