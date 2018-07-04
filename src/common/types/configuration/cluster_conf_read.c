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

static char *__NAME__ = "CONFIGURATION";

static void insert_th_policy(cluster_conf_t *conf, char *token, int policy)
{
	int i;
	for (i = 0; i < conf->num_policies; i++)
	{
		if (conf->power_policies[i].policy == policy)
			conf->power_policies[i].th = atof(token);
	}

}

static int set_nodes_conf(cluster_conf_t *conf, char *namelist)
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

static int get_default_pstate(policy_conf_t *pow_pol, int num_pol, int policy)
{
	int i;
	for (i = 0; i < num_pol; i++)
	{
		if (pow_pol[i].policy == policy)
			return pow_pol[i].p_state;
	}
	return 0;
}

static void fill_policies(cluster_conf_t *conf)
{
	int i;
	for (i = 0; i < TOTAL_POLICIES; i++)
		conf->power_policies[i].policy = i;
}

static void generate_node_ranges(node_island_t *island, char *nodelist)
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

void get_cluster_config(FILE *conf_file, cluster_conf_t *conf)
{
	memset(conf, 0, sizeof(cluster_conf_t));
	char line[256];
	char *token;

	//filling the default policies before starting
	fill_policies(conf);
	
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

            //EARLIB CONF
		else if (!strcmp(token, "COEFFICIENTSDIR"))
		{
			token = strtok(NULL, "=");
			token = strtok(token, "\n");
			strcpy(conf->earlib.coefficients_pathname, token);
		}
        else if (!strcmp(token, "DYNAISLEVELS"))
        {
            token = strtok(NULL, "=");
            conf->earlib.dynais_levels = atoi(token);
        }
        else if (!strcmp(token, "DYNAISWINDOWSIZE"))
        {
            token = strtok(NULL, "=");
            conf->earlib.dynais_window = atoi(token);
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
				/*if (conf->num_policies == 1)
					conf->power_policies = NULL;
				//conf->power_policies = malloc(sizeof(policy_conf_t));
				conf->power_policies = realloc(conf->power_policies, sizeof(policy_conf_t)*conf->num_policies);

				conf->power_policies[conf->num_policies-1].policy = policy_name_to_id(token);*/
                int i;
                for (i = 0; i < TOTAL_POLICIES; i++)
                    if (conf->power_policies[i].policy == policy_name_to_id(token)) conf->power_policies[i].is_available = 1;
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
		else if (!strcmp(token, "ENERGYTAG"))
		{
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
                        token = strtok_r(NULL, ",", &secondary_ptr);
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
                        token = strtok_r(NULL, ",", &secondary_ptr);
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
                        token = strtok_r(NULL, ",", &secondary_ptr);
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
        else if (!strcmp(token, "DATABASECACHEMEMORYSIZE"))
        {
            token = strtok(NULL, "=");
            conf->db_manager.mem_size = atoi(token);
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
            int idx = -1;
			int island = 0;
			int num_nodes = 0;
			int num_cpus = 0;
			//fully restore the line
			line[strlen(line)] = '=';

			if (conf->num_islands == 0)
				conf->islands = NULL;

			//token = strtok_r(line, " ", &primary_ptr);
			token = strtok(line, "=");
			while (token != NULL)
			{
				strtoup(token);
				if (!strcmp(token, "ISLAND"))
				{
					token = strtok(NULL, " ");
                    int aux = atoi(token);
                    idx = -1;
                    for (i = 0; i < conf->num_islands; i++)
                        if (conf->islands[i].id == aux) idx = i;

                    if (idx < 0)
                    {
			            conf->islands = realloc(conf->islands, sizeof(node_island_t)*(conf->num_islands+1));
            			memset(&conf->islands[conf->num_islands], 0, sizeof(node_island_t));
	    				conf->islands[conf->num_islands].id = atoi(token);
                    }
				}
				else if (!strcmp(token, "DATABASECACHEIP"))
				{
					token = strtok(NULL, " ");
					strclean(token, '\n');
                    if (idx < 0)
    					strcpy(conf->islands[conf->num_islands].db_ip, token);
                    else
                        strcpy(conf->islands[idx].db_ip, token);
				}
                else if (!strcmp(token, "DATABASECACHEBACKUPIP"))
                {
					token = strtok(NULL, " ");
					strclean(token, '\n');
                    if (idx < 0)
    					strcpy(conf->islands[conf->num_islands].backup_ip, token);
                    else
                        strcpy(conf->islands[idx].backup_ip, token);

                }
				else if (!strcmp(token, "NODES"))
				{
					token = strtok(NULL, " ");
                    if (idx < 0)
					    generate_node_ranges(&conf->islands[conf->num_islands], token);
                    else
                        generate_node_ranges(&conf->islands[idx], token);
				}
				token = strtok(NULL, "=");
			}
            if (idx < 0)
    			conf->num_islands++;
		}

	}

}

int read_cluster_conf(char *conf_path,cluster_conf_t *my_conf)
{
	FILE *conf_file = fopen(conf_path, "r");
	if (conf_file == NULL)
	{
		fprintf(stderr, "ERROR opening file: %s\n", conf_path);
		return EAR_ERROR;
	}
	get_cluster_config(conf_file, my_conf);
	fclose(conf_file);
	//print_cluster_conf(my_conf);
	return EAR_SUCCESS;
}

void free_cluster_conf(cluster_conf_t *conf)
{
	int i;
	for (i = 0; i < conf->num_priv_users; i++)
		free(conf->priv_users[i]);
	free(conf->priv_users);

	for (i = 0; i < conf->num_acc; i++)
		free(conf->priv_acc[i]);
	free(conf->priv_acc);

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
