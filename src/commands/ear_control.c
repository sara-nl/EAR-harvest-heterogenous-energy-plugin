/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING
*/

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <common/config.h>
#include <common/states.h>
#include <daemon/eard_rapi.h>
#include <common/ear_verbose.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>
                   
#define NUM_LEVELS  4
#define MAX_PSTATE 16

int EAR_VERBOSE_LEVEL = 1;


cluster_conf_t my_cluster_conf;

static const char *__NAME__ = "econtrol";

void usage(char *app)
{
	printf("Usage: %s [options]"\
            "\n\t--set-freq \tnewfreq\t\t->sets the frequency of all nodes to the requested one"\
            "\n\t--set-def-freq \tnewfreq\t\t->sets the default frequency"\
            "\n\t--set-max-freq \tnewfreq\t\t->sets the maximum frequency"\
            "\n\t--inc-th \tnew_th\t\t->increases the threshold for all nodes"\
            "\n\t--red-def-freq \treduction\t->reduces the default frequency"\
            "\n\t--restore-conf \t\t\t->restores the configuration to all node"\
            "\n\t--ping	\t\t\t->pings all nodes to check wether the nodes are up or not"\
            "\n\nThis app requires privileged access privileged accesss to execute.\n", app);
	exit(1);
}


void main(int argc, char *argv[])
{
    int optidx = 0;
    int c = 0;
    char path_name[128];
   
    if (argc < 2) usage(argv[0]);

    if (get_ear_conf_path(path_name)==EAR_ERROR){
        printf("Error getting ear.conf path\n");
        exit(0);
    }

    if (read_cluster_conf(path_name, &my_cluster_conf) != EAR_SUCCESS) VERBOSE_N(0, "ERROR reading cluster configuration\n");
    
    char *user = getlogin();
    if (user == NULL)
    {
        fprintf(stderr, "ERROR getting username, cannot verify identity of user executing the command. Exiting...\n");
        free_cluster_conf(&my_cluster_conf);
        exit(1);
    }
    else if (!is_privileged(&my_cluster_conf, user, NULL, NULL) && getuid() != 0)
    {
        fprintf(stderr, "This command can only be executed by privileged users, and the current one (%s) is not. Contact your admin for more info.\n", user);
        free_cluster_conf(&my_cluster_conf);
        exit(1);
    }

    while (1)
    {
        int option_optidx = optidx ? optidx : 1;
        int option_idx = 0;
        static struct option long_options[] = {
            {"set-freq",     required_argument, 0, 0},
            {"red-def-freq", required_argument, 0, 1},
            {"red-max-freq", required_argument, 0, 2},
            {"inc-th",       required_argument, 0, 3},
            {"set-def-freq", required_argument, 0, 4},
            {"restore-conf", no_argument, 0, 5},
	        {"ping", 	     optional_argument, 0, 6},
            {"help",         no_argument, 0, 7},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "", long_options, &option_idx);

        if (c == -1)
            break;

        ulong arg;

        switch(c)
        {
            case 0:
                reduce_frequencies_all_nodes(atoi(optarg),my_cluster_conf);
                break;
            case 1:
                arg = atoi(optarg);
                if (arg > MAX_PSTATE)
                {
                    VERBOSE_N(0, "Indicated p_state to reduce def freq above the maximum (%d)", MAX_PSTATE);
                    break;
                }
                red_def_freq_all_nodes(arg,my_cluster_conf);
                break;
            case 2:
                arg = atoi(optarg);
                if (arg > MAX_PSTATE)
                {
                    VERBOSE_N(0, "Indicated p_state to reduce max freq above the maximum (%d)", MAX_PSTATE);
                    break;
                }
                red_max_freq_all_nodes(arg,my_cluster_conf);
                break;
            case 3:
                arg = atoi(optarg);
                if (arg > 100)
                {
                    VERBOSE_N(0, "Indicated threshold increase above theoretical maximum (100%)");
                    break;
                }
                increase_th_all_nodes(arg,my_cluster_conf);
                break;
            case 4:
                arg = atoi(optarg);
                if (arg > MAX_PSTATE)
                {
                    VERBOSE_N(0, "Indicated p_state to set is above the maximum (%d)", MAX_PSTATE);
                    break;
                }
                set_def_freq_all_nodes(arg,my_cluster_conf);
                break;
            case 5:
                restore_conf_all_nodes(my_cluster_conf);
                break;
            case 6:
                if (optarg)
                    old_ping_all_nodes(my_cluster_conf);
                else
                    old_ping_all_nodes(my_cluster_conf);
                break;
            case 7:
                usage(argv[0]);
                break;
        }
    }

    free_cluster_conf(&my_cluster_conf);
}
