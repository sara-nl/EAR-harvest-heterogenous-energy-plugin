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

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <common/config.h>
#include <common/states.h>
#include <common/types/configuration/cluster_conf.h>


char _print_tree = 0;
char _print_summ = 0;

char **_node_names  = NULL;
int *_ips           = NULL;
int *_recv_msgs     = NULL;
int *_prop_msgs     = NULL;
int *_ret_status    = NULL;
int _num_ips        = 0;
int _read_ips       = 0;

int _num_props = NUM_PROPS;

int _get_id(int ip)
{
    int i;
    for (i = 0; i < _num_ips; i++)
    {
        if (_ips[i] == ip) return i;
    }
    return 0; //we should return EAR_ERROR and check for errors, but we also should never get to this situation 
}

void _get_node_name(int ip, char *buff)
{
    struct sockaddr_in temp;
    char next_ip[64];
    int i, found = 0;

    for (i = 0; i < _num_ips; i++)
    {
        if (_ips[i] == ip)
        {
            found = 1;
            break;
        }
    }
    if (found) 
    {
        strcpy(buff, _node_names[i]);
        return;
    }
    temp.sin_addr.s_addr = ip;
    strcpy(next_ip, inet_ntoa(temp.sin_addr));

    strcpy(buff, next_ip);
}

void read_node_info(char *read_path)
{
    char line[512];
    int i = 0;

    FILE *node_file = fopen(read_path, "r");
    if (node_file == NULL)
    {
        printf("warning: could not open node file (%s)\n", strerror(errno));
        exit(1);
    }
    
    //count number of lines to do allocation in one go
    while(fgets(line, 512, node_file) != NULL)
        _num_ips++;

    rewind(node_file);

    _ips = calloc(_num_ips, sizeof(int));
    _node_names = calloc(_num_ips, sizeof(char *));

    char *token;
    while (fgets(line, 512, node_file) != NULL)
    {
        //parse file, first nodenames then ips
        token = strtok(line, ";");
        _node_names[i] = calloc(strlen(token)+1, sizeof(char));
        strcpy(_node_names[i], token);

        token = strtok(NULL, ";");
        _ips[i] = atoi(token);
        i++;
    }
            
    _read_ips = 1;
    
    fclose(node_file);
}

void save_node_info(char *save_path)
{
    int i;

    FILE *node_file = fopen(save_path, "w");
    if (node_file == NULL)
    {
        printf("warning: could not save node file (%s)\n", strerror(errno));
        return;
    }

    for (i = 0; i < _num_ips; i++)
    {
        fprintf(node_file, "%s;%d\n", _node_names[i], _ips[i]);
    }

    fclose(node_file);

}

int _get_ip(char *node_name, cluster_conf_t *my_conf)
{
    int i;

    /* If we have read the ips, we use that list. If not, we store the ips we get with the 
     * usual method in the list to use it later as translation back from ip -> nodename */
    if (_read_ips)
    {
        for (i = 0; i < _num_ips; i++)
        {
            if (!strcmp(node_name, _node_names[i]))
                return _ips[i];
        }
        printf("warning: no ip found for node %s\n", node_name);
        return EAR_ERROR;
    }
    else
    {
        int temp_ip = get_ip(node_name, my_conf);
        if (temp_ip != EAR_ERROR)
        {
            _num_ips++;

            //copy the nodename and its ip to the table
            _node_names = realloc(_node_names, sizeof(char *)*_num_ips);
            _node_names[_num_ips - 1] = calloc(strlen(node_name) + 1, sizeof(char));
            strcpy(_node_names[_num_ips -1], node_name);

            _ips = realloc(_ips, sizeof(int)*_num_ips);
            _ips[_num_ips - 1] = temp_ip;

        }
        return temp_ip;
    }
    return EAR_ERROR;
}

int _get_ip_ranges(cluster_conf_t *my_conf, int **num_ips, int ***ips)
{
    int i, j, k;
    int **aux_ips;
    int *sec_aux_ips;
    int *ip_counter;
    char aux_name[256];
    int total_ranges = 0;
    int current_range = 0;
    //get total number of ranges
    for (i = 0; i < my_conf->num_islands; i++)
        total_ranges += my_conf->islands[i].num_ranges;
    
    if (total_ranges < 1)
    {
        error("No IP ranges found.");
        return EAR_ERROR;
    }
    //allocate memory for each range of IPs as well as each range's IP counter
    ip_counter = calloc(total_ranges, sizeof(int));
    aux_ips = calloc(total_ranges, sizeof(int*));

    for (i = 0; i < my_conf->num_islands; i++)
    {
        for (j = 0; j < my_conf->islands[i].num_ranges; j++)
        {
            node_range_t range = my_conf->islands[i].ranges[j];
            if (range.end == -1)
            {
                sprintf(aux_name, "%s", range.prefix);
                sec_aux_ips = calloc(1, sizeof(int));
                sec_aux_ips[0] = _get_ip(aux_name, my_conf);
                aux_ips[current_range] = sec_aux_ips;
                ip_counter[current_range] = 1;
                current_range++;
                continue;
            }
            if (range.end == range.start)
            {
                sprintf(aux_name, "%s%u", range.prefix, range.start);
                sec_aux_ips = calloc(1, sizeof(int));
                sec_aux_ips[0] = _get_ip(aux_name, my_conf);
                aux_ips[current_range] = sec_aux_ips;
                ip_counter[current_range] = 1;
                current_range++;
                continue;
            }

            int total_ips = range.end-range.start + 1;
            int it = 0;
            sec_aux_ips = calloc(total_ips, sizeof(int));
            for (k = range.start; k <= range.end ; k++)
            {
                if (k < 10 && range.end > 10)
                    sprintf(aux_name, "%s0%u", range.prefix, k);
                else
                    sprintf(aux_name, "%s%u", range.prefix, k);

                sec_aux_ips[it] = _get_ip(aux_name, my_conf);
                it ++;
            }
            aux_ips[current_range] = sec_aux_ips;
            ip_counter[current_range] = it;
            current_range++;
        }
    }

    if (ips == NULL || num_ips == NULL)
    {
        for (i = 0; i < total_ranges; i++)
            free(aux_ips[i]);
        free(aux_ips);
        free(ip_counter);
        total_ranges = 0;
    }
    else
    {
        *ips = aux_ips;
        *num_ips = ip_counter;
    }
    
    return total_ranges;
        
}

int propagate_sim(int target_idx, int *ips, int ip_count, int depth)
{
    int current_dist, off_ip, i, j, current_status, global_ip_id;
    char next_ip[64];

    current_status = 1;
    
    global_ip_id = _get_id(ips[target_idx]);
    _recv_msgs[global_ip_id]++;
    

    // to calculate who to propagate to
    current_dist = target_idx - target_idx%_num_props;
    off_ip = target_idx%_num_props;

    for (i = 1; i <= _num_props; i++)
    {
        if ((current_dist*_num_props + i*_num_props + off_ip) >= ip_count) break;

        //if we reach this point, we are propagating
        _prop_msgs[global_ip_id]++;

        _get_node_name(ips[current_dist*_num_props + i*_num_props + off_ip], next_ip);

        if (_print_tree)
        {
            for (j = 0; j < depth; j++)
                printf("\t");

            printf("[%d][%d][%s] -> depth %d\n", i, j, next_ip, depth);
        }

        current_status += propagate_sim(current_dist*_num_props + i*_num_props + off_ip, ips, ip_count, depth + 1);

    }

    _ret_status[global_ip_id] += current_status;

    return current_status;

}

void initial_prop_sim(cluster_conf_t *conf)
{
    int i, j, total_ranges;
    int **ips, *ip_counts;

    int depth = 0;
    char next_ip[64];

    total_ranges = _get_ip_ranges(conf, &ip_counts, &ips);
    _recv_msgs = calloc(_num_ips, sizeof(int));
    _prop_msgs = calloc(_num_ips, sizeof(int));
    _ret_status = calloc(_num_ips, sizeof(int));

    if (_print_tree)
        printf("\n[%s][%s][IP]\n", "R_ID", "IP_ID");
    for (i = 0; i < total_ranges; i++)
    {
        for (j = 0; j < ip_counts[i] && j < _num_props; j++)
        {
            _get_node_name(ips[i][j], next_ip);
    
            if (_print_tree)
                printf("[%d][%d][%s] -> Initial propagation (depth %d)\n", i, j, next_ip, depth);

            propagate_sim(j, ips[i], ip_counts[i], depth + 1);

            if (_print_tree)
                printf("\n");
        }
    }
    
    //print recount data
    if (_print_summ)
    {
        for (i = 0; i < _num_ips; i++)
        {
            printf("node %s\t recv_msgs: %d\t sent_msgs: %d\t ret_status: %d\n", 
                    _node_names[i], _recv_msgs[i], _prop_msgs[i], _ret_status[i]);
        }
    }
    
    //free allocated data
    for (i = 0; i < total_ranges; i++)
        free(ips[i]);

    free(ips);
    free(ip_counts);

    //global recount data
    free(_recv_msgs);
    free(_prop_msgs);
    free(_ret_status);

}

void usage(char *arg)
{
    printf("Usage: %s [options]\n", arg);
    printf("\t-r \t\tRuns the simulation and prints the propagation tree data.\n");
    printf("\t-x \t\tRuns the simulation and prints the summary of each node propagation status.\n");
    printf("\t-f [file_path]\tSpecifies file_path to read the nodes' ips from.\n");
    printf("\t-s [file_path]\tSpecifies file_path to save the nodes' ips to.\n");
    printf("\t-p [file_path]\tSpecifies ear.conf path.\n");
    printf("\t-n [num_props]\tSpecifies the number of propagations each node will do.\n");
    printf("\t-h \t\tShows this message.\n");

    exit(0);
}

int main(int argc,char *argv[])
{
	cluster_conf_t my_cluster;
    int c;
	char file_path[256], ear_path[256], save_path[256];
    char to_run = 0, read_file = 0, read_path = 0, save_file = 0;

    while ((c = getopt(argc, argv, "rxhs:f:p:n:")) != -1)
    {
        switch(c)
        {
            case 'x':
                to_run = 1;
                _print_summ = 1;
                break;
            case 'r':
                to_run = 1;
                _print_tree = 1;
                break;
            case 'p':
                read_path = 1;
		        strcpy(ear_path, optarg);
                break;
            case 'f':
                read_file = 1;
                strcpy(file_path, optarg);
                break;
            case 's':
                save_file = 1;
                strcpy(save_path, optarg);
                break;
            case 'n':
                _num_props = atoi(optarg);
                if (_num_props < 1)
                {
                    printf("error: The number of propagations per node must be greater than 0\n");
                }
                break;
            case 'h':
                usage(argv[0]);
                break;

        }
    }


	if (!read_path)
    {
		if (get_ear_conf_path(ear_path)==EAR_ERROR){
			printf("Error getting ear.conf path\n");
			exit(0);
		}
	}

    if (read_file)
    {
        read_node_info(file_path);
    }

	read_cluster_conf(ear_path,&my_cluster);

    //Simulate propagation
    if (to_run)
    {
        initial_prop_sim(&my_cluster);
        printf("\nFinished simulation\n");
    }

    if (save_file)
    {
        if (_num_ips < 1)
        {
            _get_ip_ranges(&my_cluster, NULL, NULL);
        }
        save_node_info(save_path);
    }

    //Free everything and exit
    free_cluster_conf(&my_cluster);

    if (_num_ips > 0)
    {

        int i;
//        for (i = 0; i < _num_ips; i++)
//            printf("node %s\t ip %d\n", _node_names[i], _ips[i]);

        for (i = 0; i < _num_ips; i++)
            free(_node_names[i]);
        free(_node_names);
        free(_ips);

    //    printf("Free'd all nodes stored internally\n");
    }

	return 0;
}
