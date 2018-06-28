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

#include <common/types/configuration/cluster_conf_read.h>

static void cae_conf(cluster_conf_t *my_conf)
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

static void get_cluster_config(FILE *conf_file, cluster_conf_t *conf)
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
			token = strtok(NULL, "=");
			token = strtok(token, ",");
			conf->e_tags = realloc(conf->e_tags, sizeof(energy_tag_t)*(conf->num_tags+1));
			strcpy(conf->e_tags[conf->num_tags].tag, token);
			token = strtok(NULL, ",");
			conf->e_tags[conf->num_tags].p_state = atoi(token);
			conf->num_tags++;
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

	free(conf->e_tags);

	for (i = 0; i < conf->num_islands; i++)
		free(conf->islands[i].ranges);

	free(conf->islands);

	memset(conf, 0, sizeof(cluster_conf_t));
}
