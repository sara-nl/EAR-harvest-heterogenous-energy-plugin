/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <limits.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <common/config.h>
#include <common/states.h>
#include <common/system/user.h>
#include <common/output/verbose.h>
#include <common/types/version.h>
#include <common/types/application.h>
#include <common/types/configuration/policy_conf.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/messaging/msg_internals.h>
#include <daemon/remote_api/eard_rapi.h>
                   
#define NUM_LEVELS  4
#define MAX_PSTATE  16
#define IP_LENGTH   24

#define EAR_TYPE_POLICY             25 //it should be 2005, but we don't want to interfere with the messaging api
#define EAR_TYPE_FULL_STATUS        26 //it should be 2006, but we don't want to interfere with the messaging api
#define EAR_TYPE_APP_STATUS_MASTER  27 //EAR_TYPES refer to data specifically, but since both APP_STATUS rc codes use the type
                                       //APP_STATUS, we need subspecific types for this case
#define EAR_TYPE_APP_STATUS_NODE    28 //same as above

/* Status modes */
#define ERR_ONLY    1
#define FULL_STATUS 2
#define POLICY_ONLY 3
#define NODE_ONLY   4

typedef struct ip_table
{
    int ip_int;
    char ip[IP_LENGTH];
    char name[IP_LENGTH];
    int counter;
    uint power;
    uint max_power;
    int job_id;
    int step_id;
    uint current_freq;
    uint temp;
    eard_policy_info_t policies[TOTAL_POLICIES];
} ip_table_t;

cluster_conf_t my_cluster_conf;
int no_error = 0;

void fill_ip(char *buff, ip_table_t *table)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

   	s = getaddrinfo(buff, NULL, &hints, &result);
    if (s != 0) {
		printf("getaddrinfo fails for port %s (%s)", buff, strerror(errno));
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
    int k;
    unsigned int i, j;
    char node_name[256];
    int num_ips = 0;
    my_node_conf_t *aux_node_conf;
    ip_table_t *new_ips = NULL;
    for (i = 0; i < my_cluster_conf.num_islands; i++){
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
                aux_node_conf = get_my_node_conf(&my_cluster_conf, node_name);
                if (aux_node_conf == NULL) 
                    fprintf(stderr, "Error reading node %s configuration\n", node_name);
                else
                    new_ips[num_ips].max_power = (uint) aux_node_conf->max_error_power;

				/*#if USE_EXT
				strcat(node_name,NW_EXT);
				#endif*/
                if (strlen(my_cluster_conf.net_ext) > 0)
                    strcat(node_name, my_cluster_conf.net_ext);

                fill_ip(node_name, &new_ips[num_ips]);
                free(aux_node_conf->policies);
                free(aux_node_conf);
                num_ips++;
            }
        }
    }
    *ips = new_ips;
    return num_ips;
}

void print_ips(ip_table_t *ips, int num_ips, char mode)
{
    int i, j, counter = 0;
    if (mode != ERR_ONLY)
    {
        printf("%10s\t", "hostname");
        if (mode == NODE_ONLY || mode == FULL_STATUS)
            printf("%5s\t%4s\t%4s\t%6s\t%6s", "power", "temp", "freq", "job_id", "stepid");
        if (mode == FULL_STATUS || mode == POLICY_ONLY)
            printf("  %6s  %5s  %2s", "policy", "pfreq", "th");
        printf("\n");
    }
	char temp[GENERIC_NAME];
    char final[GENERIC_NAME];
    for (i=0; i<num_ips; i++)
	{
        if (ips[i].counter && ips[i].power != 0 )
        {
            if (mode != ERR_ONLY) 
            {
                printf("%10s\t", ips[i].name);
                if (mode == NODE_ONLY || mode == FULL_STATUS)
                {
                    printf("%5d\t%3dC\t%.2lf\t%6d\t%6d", ips[i].power, ips[i].temp, 
                            (double)ips[i].current_freq/1000000.0, ips[i].job_id, ips[i].step_id);
                }

                if (mode == FULL_STATUS || mode == POLICY_ONLY)
                {
	    	        for (j = 0; j < TOTAL_POLICIES; j++)
        		    {
	    		        policy_id_to_name(j, temp, &my_cluster_conf);
                        get_short_policy(final, temp, &my_cluster_conf);
		        	    printf("  %6s  %.2lf  %3u", final, (double)ips[i].policies[j].freq/1000000.0, ips[i].policies[j].th);
	    	        }
                }
                printf("\n");
            }
            if (ips[i].power < ips[i].max_power)
                counter++;
            else if (ips[i].max_power == 0)
                counter++;
        }
	}
    if (counter < num_ips && !no_error)
    {
        if (mode != ERR_ONLY) printf("\n\nINACTIVE NODES\n");
        char first_node = 1;
        for (i = 0; i <num_ips; i++)
        {
            if (mode == ERR_ONLY)
            {
                if (!ips[i].counter || !ips[i].power || ips[i].power > ips[i].max_power)
                {
                    if (!first_node)
                        printf(",");
                    else first_node = 0;
                    printf("%8s", ips[i].name);
                }
            }
            else {
                if (!ips[i].counter) {
                    printf("%10s\t%10s\n", ips[i].name, ips[i].ip);
                } else if (!ips[i].power || ips[i].power > ips[i].max_power) {
                    printf("%10s\t%10s\t->power error (reported %dW)\n", ips[i].name, ips[i].ip, ips[i].power);
                }
	        }
        }
        printf("\n");
    }
}

void usage(char *app)
{
	printf("Usage: %s [options]"\
            "\n\t--set-freq \t[newfreq]\t\t->sets the frequency of all nodes to the requested one"\
            "\n\t--set-def-freq \t[newfreq]  [pol_name]\t->sets the default frequency for the selected policy "\
            "\n\t--set-max-freq \t[newfreq]\t\t->sets the maximum frequency"\
            "\n\t--inc-th \t[new_th]   [pol_name]\t->increases the threshold for all nodes"\
            "\n\t--set-th \t[new_th]   [pol_name]\t->sets the threshold for all nodes"\
            "\n\t--red-def-freq \t[n_pstates]\t\t->reduces the default and max frequency by n pstates"\
            "\n\t--restore-conf \t\t\t\t->restores the configuration to all node"\
            "\n\t--status \t\t\t\t->requests the current status for all nodes. The ones responding show the current "\
            "\n\t\t\t\t\t\t\tpower, IP address and policy configuration. A list with the ones not"\
            "\n\t\t\t\t\t\t\tresponding is provided with their hostnames and IP address."\
            "\n\t\t\t\t\t\t\t--status=node_name retrieves the status of that node individually."\
            "\n\t--type \t\t[status_type]\t\t->Specifies what type of status will be requested: hardware,"\
            "\n\t\t\t\t\t\t\tpolicy, full (hardware+policy), app_node, app_master or power. [default:hardware]"\
            "\n\t--ping	\t\t\t\t->pings all nodes to check wether the nodes are up or not. Additionally,"\
            "\n\t\t\t\t\t\t\t--ping=node_name pings that node individually."\
            "\n\t--version \t\t\t\t->displays current EAR version."\
            "\n\t--help \t\t\t\t\t->displays this message.", app);
    printf("\n\nThis app requires privileged access privileged accesss to execute.\n");
	exit(0);
}

void check_ip(status_t status, ip_table_t *ips, int num_ips)
{
    int i, j;
    //printf("checking ip %d with status %d and power %lu\n", status.ip, status.ok, status.node.power);
    for (i = 0; i < num_ips; i++)
        if (htonl(status.ip) == htonl(ips[i].ip_int))
		{
            ips[i].counter |= status.ok;
			ips[i].power = status.node.power;
            ips[i].current_freq = status.node.avg_freq;
            ips[i].temp = status.node.temp;
            ips[i].job_id = status.app.job_id;
            ips[i].step_id = status.app.step_id;
			//refactor
			for (j = 0; j < TOTAL_POLICIES; j++)
			{
				ips[i].policies[j].freq = status.policy_conf[j].freq;
				ips[i].policies[j].th = status.policy_conf[j].th;
			}	
		}
}

void check_app_status(app_status_t status, ip_table_t *ips, int num_ips, char is_master)
{
    int i;
    int gpusi;
    double GPU_power=0;
    ulong GPU_freq=0;
    for (i = 0; i < num_ips; i++)
        if (htonl(status.ip) == htonl(ips[i].ip_int))
        {
            if (is_master)
                printf("%7lu-%-4lu %6d %10.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf", 
                        status.job_id, status.step_id, status.nodes,
                        status.signature.DC_power, status.signature.CPI, status.signature.GBS, 
                        status.signature.Gflops, status.signature.time, (double)status.signature.avg_f/1000000);
            else
                printf("%15s %7lu-%-4lu %6d %10.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf", 
                        ips[i].name, status.job_id, status.step_id, status.master_rank,
                        status.signature.DC_power, status.signature.CPI, status.signature.GBS, 
                        status.signature.Gflops, status.signature.time, (double)status.signature.avg_f/1000000);
#if USE_GPU_LIB
            GPU_power=0;GPU_freq=0;
            if (status.signature.gpu_sig.num_gpus>0){
                for (gpusi=0;gpusi<status.signature.gpu_sig.num_gpus;gpusi++){
                    GPU_power += status.signature.gpu_sig.gpu_data[gpusi].GPU_power;
                    GPU_freq += status.signature.gpu_sig.gpu_data[gpusi].GPU_freq;
                }
                GPU_freq = GPU_freq/status.signature.gpu_sig.num_gpus;
            }
            printf(" %9.2lf %9.2lf", GPU_power, (double)GPU_freq/1000000);
#endif
            printf("\n");
        }
}

void clean_ips(ip_table_t *ips, int num_ips)
{
    int i = 0;
    for (i = 0; i < num_ips; i++)
        ips[i].counter=0;
}

void process_status(int num_status, status_t *status, char error_only)
{
    if (num_status > 0)
    {
        int i, num_ips;
        ip_table_t *ips = NULL;
        num_ips = generate_node_names(my_cluster_conf, &ips);
        clean_ips(ips, num_ips);
        for (i = 0; i < num_status; i++)
            check_ip(status[i], ips, num_ips);
        print_ips(ips, num_ips, error_only);
        free(ips);
        free(status);
    }
    else printf("An error retrieving status has occurred.\n");
}

void process_app_status(int num_status, app_status_t *status, char is_master)
{
    if (num_status > 0)
    {
        int i, num_ips;

        if (is_master)
            printf("%7s-%-4s %6s %10s %8s %8s %8s %8s %8s", 
                    "Job", "Step", "Nodes", "DC power", "CPI", "GBS", "Gflops", "Time", "Avg Freq");
        else
            printf("%15s %7s-%-4s %6s %10s %8s %8s %8s %8s %8s", 
                    "Node id", "Job", "Step", "M-Rank", "DC power", "CPI", "GBS", "Gflops", "Time", "Avg Freq");
#if USE_GPU_LIB
        printf(" %9s %9s", "GPU power", "GPU freq");
#endif
        printf("\n");
        ip_table_t *ips = NULL;
        num_ips = generate_node_names(my_cluster_conf, &ips);
        clean_ips(ips, num_ips);
        for (i = 0; i < num_status; i++)
            check_app_status(status[i], ips, num_ips, is_master);
        free(ips);
        free(status);
    }
    else if (num_status < 0) printf("An error retrieving status has occurred.\n");
    else printf("No apps are currently running on any node\n");
}

void generate_ip(ip_table_t *ips, char *node_name)
{
    my_node_conf_t *aux_node_conf;
    strcpy(ips[0].name, node_name);
    aux_node_conf = get_my_node_conf(&my_cluster_conf, node_name);
    if (aux_node_conf == NULL) 
        fprintf(stderr, "Error reading node %s configuration\n", node_name);
    else
        ips[0].max_power = (uint) aux_node_conf->max_error_power;
    if (strlen(my_cluster_conf.net_ext) > 0)
        strcat(node_name, my_cluster_conf.net_ext);

    fill_ip(node_name, &ips[0]);

}

void process_single_status(int num_status, status_t *status, char *node_name, char error_only)
{
    if (num_status > 0)
    {
        ip_table_t ips;
        generate_ip(&ips, node_name);
        check_ip(*status, &ips, 1);
        print_ips(&ips, 1, error_only);
    }
    else printf("An error retrieving status has occurred.\n");
}


int main(int argc, char *argv[])
{
    int c = 0;
    int num_status = 0;
    int status_type = 0;

    char path_name[128];
    char node_name[256];

    strcpy(node_name,"");

    status_t *status;

    verb_level = 0;
    //verb_enabled = 0;
    if (argc < 2) usage(argv[0]);

    if (get_ear_conf_path(path_name)==EAR_ERROR){
        printf("Error getting ear.conf path\n"); //error
        exit(1);
    }

    if (read_cluster_conf(path_name, &my_cluster_conf) != EAR_SUCCESS) printf("ERROR reading cluster configuration\n");
    
    if (getuid() != 0 && !is_privileged_command(&my_cluster_conf))
    {
        printf("This command can only be executed by privileged users. Contact your admin for more info.\n");
        free_cluster_conf(&my_cluster_conf);
        exit(1); //error
    }

    while (1)
    {
        int option_idx = 0;
        static struct option long_options[] = {
            {"set-freq",     	required_argument, 0, 0},
            {"red-def-freq", 	required_argument, 0, 1},
            {"set-max-freq", 	required_argument, 0, 2},
            {"inc-th",       	required_argument, 0, 3},
            {"set-def-freq", 	required_argument, 0, 4},
            {"set-th",          required_argument, 0, 5},
            {"restore-conf", 	optional_argument, 0, 6},
	        {"ping", 	     	optional_argument, 0, 'p'},
            {"status",       	optional_argument, 0, 's'},
            {"powerstatus",     optional_argument, 0, 'w'},
            {"release",         optional_argument, 0, 'l'},
            {"set-risk",        required_argument, 0, 'r'},
            {"setopt",          required_argument, 0, 'o'},
            {"type",            required_argument, 0, 't'},
            {"verbose",         optional_argument, 0, 'b'},
            {"error",           no_argument, 0, 'e'},
            {"active-only",     no_argument, 0, 'i'},
            {"help",         	no_argument, 0, 'h'},
            {"version",         no_argument, 0, 'v'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "p::shvb::", long_options, &option_idx);

        if (c == -1)
            break;

        int arg;
		int arg2;

        switch(c)
        {
            case 0:
                set_freq_all_nodes(atoi(optarg),&my_cluster_conf);
                break;
            case 1:
                arg = atoi(optarg);
                red_def_max_pstate_all_nodes(arg,&my_cluster_conf);
                break;
            case 2:
                arg = atoi(optarg);
                set_max_freq_all_nodes(arg,&my_cluster_conf);
                break;
            case 3:
                arg = atoi(optarg);
				if (optind+1 > argc)
				{
					printf("Missing policy argument for set-def-freq\n");
                    break;
				}
				arg2 = policy_name_to_id(argv[optind], &my_cluster_conf);
				if (!is_valid_policy(arg2, &my_cluster_conf) || arg2 == EAR_ERROR)
				{
					printf("Invalid policy (%s).\n", argv[optind]);
                    break;
				}
                if (arg > 100)
                {
                    printf("Indicated threshold increase above theoretical maximum (100%%)\n");
                    break;
                }
                increase_th_all_nodes(arg, arg2, &my_cluster_conf);
                break;
            case 4:
                arg = atoi(optarg);
				if (optind+1 > argc)
				{
					printf("Missing policy argument for set-def-freq\n");
                    break;
				}
				arg2 = policy_name_to_id(argv[optind], &my_cluster_conf);
				if (!is_valid_policy(arg2, &my_cluster_conf) || arg2 == EAR_ERROR)
				{
					printf("Invalid policy (%s).\n", argv[optind]);
                    break;
				}
                set_def_freq_all_nodes(arg, arg2, &my_cluster_conf);
                break;
            case 5:
                arg = atoi(optarg);
                if (arg > 100)
                {
                    printf("Indicated threshold increase above theoretical maximum (100%%)\n");
                    break;
                }
                if (optind+1 > argc)
				{
					printf("Missing policy argument for set-def-freq\n");
                    break;
				}
				arg2 = policy_name_to_id(argv[optind], &my_cluster_conf);
				if (!is_valid_policy(arg2, &my_cluster_conf) || arg2 == EAR_ERROR)
				{
					printf("Invalid policy (%s).\n", argv[optind]);
                    break;
				}
                set_th_all_nodes(arg, arg2, &my_cluster_conf);
                break;
            case 6:
                if (optarg)
                {
                    strcpy(node_name, optarg);
                    strtoup(node_name);
                    if (strcmp(node_name, optarg))
                    {
                        strcpy(node_name, optarg);
                        strcat(node_name, my_cluster_conf.net_ext);
                    }
                    else strcpy(node_name, optarg);
                    int rc=eards_remote_connect(node_name, my_cluster_conf.eard.port);
                    if (rc<0){
                        printf("Error connecting with node %s\n", node_name);
                    }else{
                        if (!eards_restore_conf()) printf("Error restoring configuration for node: %s\n", node_name);
                        eards_remote_disconnect();
                    }
                }
                else
                {
                    restore_conf_all_nodes(&my_cluster_conf);
                }
                break;
            case 'i':
                no_error = 1;
                break;
            case 'p':
                if (optarg)
                {
                    strcpy(node_name, optarg);
                    strtoup(node_name);
                    if (strcmp(node_name, optarg))
                    {
                        strcpy(node_name, optarg);
                        strcat(node_name, my_cluster_conf.net_ext);
                    }
                    else strcpy(node_name, optarg);
                    int rc=eards_remote_connect(node_name, my_cluster_conf.eard.port);
                    if (rc<0){
                        printf("Error connecting with node %s\n", node_name);
                    }else{
                        printf("Node %s ping!\n", node_name);
                        if (!eards_ping()) printf("Error doing ping for node %s\n", node_name);
                        eards_remote_disconnect();
                    }
                }
                else
                    ping_all_nodes(&my_cluster_conf);
                break;
            case 's':
                status_type = EAR_TYPE_STATUS;
                if (optarg)
                {
                    strcpy(node_name, optarg);
                    strtoup(node_name);
                    if (strcmp(node_name, optarg))
                    {
                        strcpy(node_name, optarg);
                        strcat(node_name, my_cluster_conf.net_ext);
                    }
                    else strcpy(node_name, optarg);
                }
                break;
            case 't':
                if (!strcasecmp(optarg, "HARDWARE"))
                    status_type = EAR_TYPE_STATUS;
                else if (!strcasecmp(optarg, "APP_NODE"))
                    status_type = EAR_TYPE_APP_STATUS_NODE;
                else if (!strcasecmp(optarg, "APP_MASTER"))
                    status_type = EAR_TYPE_APP_STATUS_MASTER;
                else if (!strcasecmp(optarg, "POWER"))
                    status_type = EAR_TYPE_POWER_STATUS;
                else if (!strcasecmp(optarg, "FULL"))
                    status_type = EAR_TYPE_FULL_STATUS;
                else if (!strcasecmp(optarg, "POLICY"))
                    status_type = EAR_TYPE_POLICY;
                else
                    printf("Warning: specified type is invalid (%s)\n", optarg);
                break;
            case 'r':
                if (optarg)
                {
                    arg = atoi(optarg);
                    if (optind+1 > argc)
                    {
                        printf("Sending risk level %d to all nodes\n", arg);
                        set_risk_all_nodes(arg, arg2, &my_cluster_conf);
                        break;
                    }
                    int rc = eards_remote_connect(argv[optind], my_cluster_conf.eard.port);
                    if (rc < 0){
                        printf("Error connecting with node %s\n", argv[optind]);
                    }else{
                        printf("Sending risk level %d to %s\n", arg, argv[optind]);
                        eards_set_risk(arg, 0);
                        eards_remote_disconnect();
                    }
                }
                break;
            case 'w':
                status_type = EAR_TYPE_POWER_STATUS;
                if (optarg)
                {
                    strcpy(node_name, optarg);
                    strtoup(node_name);
                    if (strcmp(node_name, optarg))
                    {
                        strcpy(node_name, optarg);
                        strcat(node_name, my_cluster_conf.net_ext);
                    }
                    else strcpy(node_name, optarg);
                }
                break;
            case 'o':
                if (optarg)
                {
                    powercap_opt_t msg;
                    memset(&msg, 0, sizeof(powercap_opt_t));
                    msg.num_greedy = 1;
                    msg.greedy_nodes = calloc(1, sizeof(int));
                    msg.extra_power = calloc(1, sizeof(int));
                    msg.greedy_nodes[0] = 772087468;
                    msg.extra_power[0] = 25;
                    request_t command;
                    command.req = EAR_RC_SET_POWERCAP_OPT;
                    command.time_code = time(NULL);
                    command.my_req.pc_opt = msg;
                    command.node_dist = INT_MAX;
                    int rc = eards_remote_connect(optarg, my_cluster_conf.eard.port);
                    if (rc < 0) {
                        printf("Error connecting with node %s\n", optarg);
                    }else{
                        printf("Reading power_status from node %s\n", optarg);
                        send_command(&command);
                        eards_remote_disconnect();
                    }
                    free(msg.greedy_nodes);
                    free(msg.extra_power);
                   
                }
                break;
            case 'l':
                if (optarg) {}
                else
                {
                    pc_release_data_t pc;
                    cluster_release_idle_power(&my_cluster_conf, &pc);
                    printf("released %u watts\n", pc.released);
                }
            case 'e':
                num_status = status_all_nodes(&my_cluster_conf, &status);
                process_status(num_status, status, 1);
                break;
            case 'h':
                usage(argv[0]);
                break;
            case 'v':
                print_version();
                break;
            case 'b':
                verb_enabled = 1;
                if (optarg) verb_level = atoi(optarg);
                else verb_level = 1;
                break;


        }
    }

    if (strlen(node_name) > 0 && status_type > 0)
    {
        int num_status = 0;
        int rc=eards_remote_connect(node_name, my_cluster_conf.eard.port);
        powercap_status_t *powerstatus;
        app_status_t *appstatus;
        if (rc<0)
            printf("Error connecting with node %s\n", node_name);
        else
        {
            switch(status_type)
            {
                case EAR_TYPE_STATUS:
                    if ((num_status = eards_get_status(&my_cluster_conf, &status)) != 1) 
                        printf("Error doing status for node %s, returned (%d)\n", node_name, num_status);
                    process_single_status(num_status, status, node_name, NODE_ONLY);
                    break;
                case EAR_TYPE_POLICY:
                    if ((num_status = eards_get_status(&my_cluster_conf, &status)) != 1) 
                        printf("Error doing status for node %s, returned (%d)\n", node_name, num_status);
                    process_single_status(num_status, status, node_name, POLICY_ONLY);
                    break;
                case EAR_TYPE_FULL_STATUS:
                    if ((num_status = eards_get_status(&my_cluster_conf, &status)) != 1) 
                        printf("Error doing status for node %s, returned (%d)\n", node_name, num_status);
                    process_single_status(num_status, status, node_name, FULL_STATUS);
                    break;
                case EAR_TYPE_POWER_STATUS:
                    printf("Reading power_status from node %s\n", node_name);
                    num_status = eards_get_powercap_status(&my_cluster_conf, &powerstatus);
                    if (num_status > 0)
                    {
                        int i;
                        for (i = 0; i < num_status; i++)
                        {
                            printf("powercap_status %d: idle_nodes: %d\t released_pow: %d\t int num_greedy: %d \trequested: %d\t current_pow: %u total_powcap: %u\n", i, powerstatus[i].idle_nodes,
                                        powerstatus[i].released, powerstatus[i].num_greedy, powerstatus[i].requested, powerstatus[i].current_power, powerstatus[i].total_powercap);
                        }
                        free(powerstatus);
                    }
                    break;
                case EAR_TYPE_APP_STATUS_MASTER:
                    num_status = eards_get_app_master_status(&my_cluster_conf, &appstatus);
                    process_app_status(num_status, appstatus, 1);
                    break;
                case EAR_TYPE_APP_STATUS_NODE:
                    num_status = eards_get_app_node_status(&my_cluster_conf, &appstatus);
                    process_app_status(num_status, appstatus, 0);
                    break;
            }

            eards_remote_disconnect();
        }
    }
    else if (status_type > 0)
    {
        switch(status_type)
        {
            powercap_status_t *powerstatus;
            app_status_t *appstatus;
            case EAR_TYPE_STATUS:
                num_status = status_all_nodes(&my_cluster_conf, &status);
                process_status(num_status, status, NODE_ONLY);
                break;
            case EAR_TYPE_POLICY:
                num_status = status_all_nodes(&my_cluster_conf, &status);
                process_status(num_status, status, POLICY_ONLY);
                break;
            case EAR_TYPE_FULL_STATUS:
                num_status = status_all_nodes(&my_cluster_conf, &status);
                process_status(num_status, status, FULL_STATUS);
                break;
            case EAR_TYPE_POWER_STATUS:
                num_status = cluster_get_powercap_status(&my_cluster_conf, &powerstatus);
                if (num_status > 0)
                {
                    int i;
                    for (i = 0; i < num_status; i++)
                    {
                        printf("powercap_status %d: idle_nodes: %d\t released_pow: %d\t int num_greedy: %d \trequested: %d\t current_pow: %u total_powcap: %u\t total_nodes: %d\n", 
                                        i, powerstatus[i].idle_nodes, powerstatus[i].released, powerstatus[i].num_greedy, powerstatus[i].requested, 
                                        powerstatus[i].current_power, powerstatus[i].total_powercap, powerstatus[i].total_nodes);
                    }
                    free(powerstatus);
                }
                else printf("powercap_status returned with invalid (%d) num_powerstatus\n", num_status);
                break;
            case EAR_TYPE_APP_STATUS_MASTER:
                num_status = get_app_master_status_all_nodes(&my_cluster_conf, &appstatus);
                process_app_status(num_status, appstatus, 1);
                break;
            case EAR_TYPE_APP_STATUS_NODE:
                num_status = get_app_node_status_all_nodes(&my_cluster_conf, &appstatus);
                process_app_status(num_status, appstatus, 0);
                break;
        }
    }
    free_cluster_conf(&my_cluster_conf);
}
