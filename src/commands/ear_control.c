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

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <common/config.h>
#include <common/states.h>
#include <daemon/eard_rapi.h>
#include <common/ear_verbose.h>
#include <common/types/application.h>
#include <common/types/configuration/policy_conf.h>
#include <common/types/configuration/cluster_conf.h>
                   
#define NUM_LEVELS  4
#define MAX_PSTATE  16
#define IP_LENGTH   24


typedef struct ip_table
{
    int ip_int;
    char ip[IP_LENGTH];
    char name[IP_LENGTH];
    int counter;
	uint power;
	eard_policy_info_t policies[TOTAL_POLICIES];
} ip_table_t;

int EAR_VERBOSE_LEVEL = 0;

cluster_conf_t my_cluster_conf;

static const char *__NAME__ = "econtrol";

void fill_ip(char *buff, ip_table_t *table)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    int ip1, ip2;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

   	s = getaddrinfo(buff, NULL, &hints, &result);
    if (s != 0) {
		fprintf(stderr,"getaddrinfo fails for port %s (%s)\n",buff,strerror(errno));
		return;
    }


   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *saddr = (struct sockaddr_in*) (rp->ai_addr);
            //ina = saddr->sin_addr;    
            table->ip_int = saddr->sin_addr.s_addr;
            strcpy(table->ip, inet_ntoa(saddr->sin_addr));
        }
    }
    freeaddrinfo(result);
}

int generate_node_names(cluster_conf_t my_cluster_conf, ip_table_t **ips)
{
    int i, j, k, rc; 
    char node_name[256];
    int num_ips = 0;
    ip_table_t *new_ips = NULL;
    for (i=0;i< my_cluster_conf.num_islands;i++){
        for (j = 0; j < my_cluster_conf.islands[i].num_ranges; j++)
        {   
            for (k = my_cluster_conf.islands[i].ranges[j].start; k <= my_cluster_conf.islands[i].ranges[j].end; k++)
            {   
                if (k == -1) 
                    sprintf(node_name, "%s", my_cluster_conf.islands[i].ranges[j].prefix);
                else if (my_cluster_conf.islands[i].ranges[j].end == my_cluster_conf.islands[i].ranges[j].start)
                    sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                else {
                    if (k < 10 && my_cluster_conf.islands[i].ranges[j].end > 10) 
                        sprintf(node_name, "%s0%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                    else 
                        sprintf(node_name, "%s%u", my_cluster_conf.islands[i].ranges[j].prefix, k); 
                }   
                new_ips = realloc(new_ips, sizeof(ip_table_t)*(num_ips+1));
                strcpy(new_ips[num_ips].name, node_name);
                fill_ip(node_name, &new_ips[num_ips]);
                num_ips++;
            }
        }
    }
    *ips = new_ips;
    return num_ips;
}

void print_ips(ip_table_t *ips, int num_ips)
{
    int i, j, counter = 0;
    printf("%10s\t%10s\t%5s\t", "hostname", "ip", "power");
    for (i = 0; i < TOTAL_POLICIES; i++)
        printf("  %6s  %6s%5s\t   ", "policy", "pstate", "th");
    printf("\n");
	char temp[GENERIC_NAME];
    char final[GENERIC_NAME];
    for (i=0; i<num_ips; i++)
	{
        if (ips[i].counter && ips[i].power != 0 )
        {
            printf("%10s\t%10s\t%5d", ips[i].name, ips[i].ip, ips[i].power); 
		    for (j = 0; j < TOTAL_POLICIES; j++)
		    {
			    policy_id_to_name(j, temp);
                get_short_policy(final, temp);
			    printf("  %5s  %5u  %8u\t", final, ips[i].policies[j].pstate, ips[i].policies[j].th); 
		    }
            printf("\n");
            if (ips[i].power < MAX_SIG_POWER)
                counter++;
        }
	}
    if (counter < num_ips)
    {
        printf("\n\nINACTIVE NODES\n");
        for (i = 0; i <num_ips; i++)
        {
            if (!ips[i].counter)
                printf("%10s\t%10s\n", ips[i].name, ips[i].ip);
            else if (!ips[i].power || ips[i].power > MAX_SIG_POWER)
                printf("%10s\t%10s\t->power error (reported %dW)\n", ips[i].name, ips[i].ip, ips[i].power);
        }
    }
}

void usage(char *app)
{
	printf("Usage: %s [options]"\
            "\n\t--set-freq \tnewfreq\t\t\t->sets the frequency of all nodes to the requested one"\
            "\n\t--set-def-freq \tnewfreq\tpolicy_id\t->sets the default frequency for the selected policy id"\
            "\n\t--set-max-freq \tnewfreq\t\t\t->sets the maximum frequency"\
            "\n\t--inc-th \tnew_th\t\t\t->increases the threshold for all nodes"\
            "\n\t--set-th \tnew_th\t\t\t->sets the threshold for all nodes"\
            "\n\t--red-def-freq \tn_pstates\t\t->reduces the default and max frequency by n pstates"\
            "\n\t--restore-conf \t\t\t\t->restores the configuration to all node"\
            "\n\t--status \t\t\t\t->requests the current status for all nodes. The ones responding show the current "\
            "\n\t\t\t\t\t\t\tpower, IP address and policy configuration. A list with the ones not"\
            "\n\t\t\t\t\t\t\tresponding is provided with their hostnames and IP address."\
            "\n\t--ping	\t\t\t\t->pings all nodes to check wether the nodes are up or not. Additionally,"\
            "\n\t\t\t\t\t\t\t--ping=node_name pings that node individually."\
            "\n\t--help \t\t\t\t\t->displays this message."\
            "\n\nThis app requires privileged access privileged accesss to execute.\n", app);
	exit(1);
}

void check_ip(status_t status, ip_table_t *ips, int num_ips)
{
    int i, j;
    for (i = 0; i < num_ips; i++)
        if (htonl(status.ip) == htonl(ips[i].ip_int))
		{
            ips[i].counter |= status.ok;
			ips[i].power = status.power;
			//refactor
			for (j = 0; j < TOTAL_POLICIES; j++)
			{
				ips[i].policies[j].pstate = status.policy_conf[j].pstate;
				ips[i].policies[j].th = status.policy_conf[j].th;
			}	
		}
}

void clean_ips(ip_table_t *ips, int num_ips)
{
    int i = 0;
    for (i = 0; i < num_ips; i++)
        ips[i].counter=0;
}

void process_status(int num_status, status_t *status)
{
    if (num_status > 0)
    {
        int i, num_ips;
        ip_table_t *ips = NULL;
        num_ips = generate_node_names(my_cluster_conf, &ips);
        clean_ips(ips, num_ips);
        for (i = 0; i < num_status; i++)
            check_ip(status[i], ips, num_ips);
        print_ips(ips, num_ips);
        free(status);
    }
    else
        printf("An error retrieving status has occurred.\n");
}


void main(int argc, char *argv[])
{
    int optidx = 0;
    int c = 0;
    char path_name[128];
    status_t *status;
    int num_status = 0;
   
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
            {"set-freq",     	required_argument, 0, 0},
            {"red-def-freq", 	required_argument, 0, 1},
            {"set-max-freq", 	required_argument, 0, 2},
            {"inc-th",       	required_argument, 0, 3},
            {"set-def-freq", 	required_argument, 0, 4},
            {"set-th",          required_argument, 0, 5},
            {"restore-conf", 	no_argument, 0, 6},
	        {"ping", 	     	optional_argument, 0, 'p'},
            {"status",       	no_argument, 0, 's'},
            {"help",         	no_argument, 0, 'h'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "p::shv::", long_options, &option_idx);

        if (c == -1)
            break;

        int arg;
		int arg2;

        switch(c)
        {
            case 0:
                set_freq_all_nodes(atoi(optarg),my_cluster_conf);
                break;
            case 1:
                arg = atoi(optarg);
                red_def_max_pstate_all_nodes(arg,my_cluster_conf);
                break;
            case 2:
                arg = atoi(optarg);
                set_max_freq_all_nodes(arg,my_cluster_conf);
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
				if (optind+1 > argc)
				{
					VERBOSE_N(0, "Missing policy argument for set-def-freq");
                    break;
				}
				arg2 = atoi(argv[optind+1]);
				if (!is_valid_policy(arg2))
				{
					VERBOSE_N(0, "Invalid policy index.");
                    break;
				}
                set_def_freq_all_nodes(arg, arg2, my_cluster_conf);
                break;
            case 5:
                arg = atoi(optarg);
                if (arg > 100)
                {
                    VERBOSE_N(0, "Indicated threshold increase above theoretical maximum (100%)");
                    break;
                }
                set_th_all_nodes(arg, my_cluster_conf);
                break;
            case 6:
                restore_conf_all_nodes(my_cluster_conf);
                break;
            case 'p':
                if (optarg)
                {
                    int rc=eards_remote_connect(optarg ,my_cluster_conf.eard.port);
                    if (rc<0){
                        VERBOSE_N(0,"Error connecting with node %s", optarg);
                    }else{
                        VERBOSE_N(1,"Node %s ping!\n", optarg);
                        if (!eards_ping()) VERBOSE_N(0,"Error doing ping for node %s", optarg);
                        eards_remote_disconnect();
                    }
                }
                else
                    old_ping_all_nodes(my_cluster_conf);
                break;
            case 's':
                num_status = status_all_nodes(my_cluster_conf, &status);
                process_status(num_status, status);
                break;
            case 'h':
                usage(argv[0]);
                break;
            case 'v':
                if (optarg)
                    EAR_VERBOSE_LEVEL = atoi(optarg);
                else EAR_VERBOSE_LEVEL = 1;


        }
    }

    free_cluster_conf(&my_cluster_conf);
}
