
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
            "\n\t--restore-conf \t\t\t->restores the configuration to all nodes"\
            "\n\nThis app requires root permission to execute.\n", app);
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
            {"help",         no_argument, 0, 6},
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
                usage(argv[0]);
                break;
        }
    }

    free_cluster_conf(&my_cluster_conf);
}
