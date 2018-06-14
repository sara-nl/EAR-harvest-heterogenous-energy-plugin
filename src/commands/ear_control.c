
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <common/config.h>
#include <common/states.h>
#include <daemon/eard_rapi.h>
#include <common/ear_verbose.h>
#include <common/types/application.h>
#include <common/types/cluster_conf.h>
                   
#define NUM_LEVELS  4

int EAR_VERBOSE_LEVEL = 1;


cluster_conf_t my_cluster_conf;

static const char *__NAME__ = "econtrol";

void usage(char *app)
{
	printf("Usage: %s [options]"\
            "\n\t--set-freq sets the frequency of all nodes to the requested one"\
            "\n\t--red-def-freq reduces default frequency"\
            "\n\t--red-max-freq reduces maximum frequency"\
            "\n\t--inc-th increases the threshold for all nodes\n", app);
	exit(1);
}

/*
*	ACTIONS for WARNING and PANIC LEVELS
*/
void increase_th_all_nodes(ulong th)
{
	int i, j, k, rc;
    char node_name[256];

    for (i=0;i < my_cluster_conf.num_islands;i++){
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
    	        rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
                printf("i: %d; j:%d; k:%d\n", i, j, k);
        	    if (rc<0){
	    		    VERBOSE_N(0,"Error connecting with node %s", node_name);
            	}else{
	        		VERBOSE_N(1,"Increasing the PerformanceEfficiencyGain in node %s by %lu\n", node_name,th);
		        	if (!eards_inc_th(th)) VERBOSE_N(0,"Error increasing the th for node %s", node_name);
			        eards_remote_disconnect();
        		}
	        }
        }
    }
}

void red_max_freq(ulong ps)
{
	int i, j, k, rc;
    char node_name[256];
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
    	        rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
        	    if (rc<0){
	    		    VERBOSE_N(0,"Error connecting with node %s", node_name);
            	}else{
    
                	VERBOSE_N(1,"Reducing  the frequency in node %s by %lu\n", node_name,ps);
		        	if (!eards_red_max_freq(ps)) VERBOSE_N(0,"Error reducing the max freq for node %s", node_name);
			        eards_remote_disconnect();
        		}
	        }
        }
    }
}

void red_def_freq(ulong ps)
{
	int i, j, k, rc;
    char node_name[256];
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
    	        rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
        	    if (rc<0){
	    		    VERBOSE_N(0,"Error connecting with node %s", node_name);
            	}else{
                	VERBOSE_N(1,"Reducing  the frequency in node %s by %lu\n", node_name,ps);
		        	if (!eards_red_max_freq(ps)) VERBOSE_N(0,"Error reducing the default freq for node %s", node_name);
			        eards_remote_disconnect();
        		}
	        }
        }
    }
}



void reduce_frequencies_all_nodes(ulong freq)
{
    int i, j, k, rc;
    char node_name[256];

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

                rc=eards_remote_connect(node_name,my_cluster_conf.eard.port);
                if (rc<0){
                    VERBOSE_N(0,"Error connecting with node %s",node_name);
                }else{
                	VERBOSE_N(1,"Reducing  the frequency in node %s by %lu\n", node_name,ps);
                	if (!eards_set_freq(freq)) VERBOSE_N(0,"Error reducing the freq for node %s", node_name);
            	    eards_remote_disconnect();
		        }
            }
        }
    }
}


void main(int argc, char *argv[])
{
    int optidx = 0;
    int c = 0;
    char path_name[128];
    
    if (argc < 3) usage(argv[0]);

    strcpy(path_name, EAR_INSTALL_PATH);
    strcat(path_name, "/etc/sysconf/ear.conf");
    if (read_cluster_conf(path_name, &my_cluster_conf) != EAR_SUCCESS) VERBOSE_N(0, "ERROR reading cluster configuration\n");
    
    while (1)
    {
        int option_optidx = optidx ? optidx : 1;
        int option_idx = 0;
        static struct option long_options[] = {
            {"set-freq", required_argument, 0, 0},
            {"red-def-freq", required_argument, 0, 1},
            {"red-max-freq", required_argument, 0, 2},
            {"inc-th", required_argument, 0, 3}
        };

        c = getopt_long(argc, argv, "", long_options, &option_idx);

        if (c == -1)
            break;

        switch(c)
        {
            case 0:
                reduce_frequencies_all_nodes(atoi(optarg));
                break;
            case 1:
                red_def_freq(atoi(optarg));
                break;
            case 2:
                red_max_freq(atoi(optarg));
                break;
            case 3:
                increase_th_all_nodes(atoi(optarg));
                break;
        }
    }

    free_cluster_conf(&my_cluster_conf);
}
