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

#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <slurm/spank.h>
#include <cpufreq.h>

#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>
#include <common/types/application.h>
#include <common/types/job.h>
#include <common/config.h>

int EAR_VERBOSE_LEVEL = 0;
SPANK_PLUGIN(EAR_PLUGIN, 1)
pid_t daemon_pid = -1;

unsigned int port_eargmd = 6000;
unsigned int num_nodes;
char host_eargmd[128];
char   host_eard[128];
application_t app;

char buffer1[PATH_MAX];
char buffer2[PATH_MAX];
int auth_mode =  1;
int verbosity = -1;

struct spank_option spank_options_manual[9] =
{
    { "ear", "on|off", "Enables/disables Energy Aware Runtime",
      1, 0, (spank_opt_cb_f) _opt_ear
    },
    { "ear-policy", "type",
      "Selects an energy policy for EAR\n" \
      "{type=MIN_ENERGY_TO_SOLUTION|MIN_TIME_TO_SOLUTION|MONITORING_ONLY}",
      1, 0, (spank_opt_cb_f) _opt_ear_policy
    },
    { "ear-policy-th", "value",
      "Specifies the threshold to be used by EAR policy" \
      " {value=[0..1]}",
      1, 0, (spank_opt_cb_f) _opt_ear_threshold
    },
    { "ear-user-db", "file",
      "Specifies the file to save the user applications metrics summary" \
	  "'file.nodename.csv' file will be created per node. If not defined, these files won't be generated.",
      2, 0, (spank_opt_cb_f) _opt_ear_user_db
    },
	{ "ear-mpi-dist", "dist",
      "Selects the MPI distribution for compatibility of your application" \
	  "{dist=intel|openmpi}",
      2, 0, (spank_opt_cb_f) _opt_ear_mpi_dist
    },
    { "ear-verbose", "value",
      "Specifies the level of the verbosity\n" \
      "{value=[0..5]}; default is 0",
      2, 0, (spank_opt_cb_f) _opt_ear_verbose
    },
    { "ear-learning-phase", "value",
      "Enables the learning phase for a given P_STATE {value=[0..n]}",
      1, 0, (spank_opt_cb_f) _opt_ear_learning
    },
    { "ear-traces", "", "Generates application traces with metrics and internal details",
      0, 0, (spank_opt_cb_f) _opt_ear_traces
    },
	{ "ear-tag", "tag", "Sets an energy tag",
	 2, 0, (spank_opt_cb_f) _opt_ear_tag
    }
};

/*
 *
 *
 *
 *
 *
 */

static int local_update_ld_preload(spank_t sp)
{
    plug_verbose(sp, 2, "function local_update_ld_preload");
	
	char *ear_install_path = NULL;
	char *ld_preload = NULL;

	buffer1[0] = '\0';
	buffer2[0] = '\0';

	/*if (getenv_local("LD_PRELOAD", &ld_preload) == 1) {
		strcpy(buffer1, ld_preload);
	}*/

    if (getenv_local("EAR_INSTALL_PATH", &ear_install_path) == 0)
    {
		plug_error("Error, missing EAR_INSTALL_PATH");
        return ESPANK_ERROR;
    }
    appendenv(buffer1, ear_install_path, PATH_MAX);
    
	// Appending libraries to LD_PRELOAD
    if (isenv_local("EAR_TRACES", "1") == 1)
	{
		if (isenv_local("EAR_MPI_DIST", "openmpi")) {
        	sprintf(buffer2, "%s/%s", buffer1, OMPI_TRACE_LIB_PATH);
		} else {
        	sprintf(buffer2, "%s/%s", buffer1, IMPI_TRACE_LIB_PATH);
		}
    }
	else
	{	
		if (isenv_local("EAR_MPI_DIST", "openmpi")) {
			sprintf(buffer2, "%s/%s", buffer1, OMPI_LIB_PATH);
		} else { 
			sprintf(buffer2, "%s/%s", buffer1, IMPI_LIB_PATH);
		}
    }

    //
    if(setenv_local("LD_PRELOAD", buffer2, 1) != 1) {
		return ESPANK_ERROR;
	}

	plug_verbose(sp, 2, "updated LD_PRELOAD envar '%s'", buffer2);

	return ESPANK_SUCCESS;
}

static void remote_update_slurm_vars(spank_t sp)
{
	plug_verbose(sp, 2, "function remote_update_slurm_vars");

    char p_state[64];
    int p_freq = 1;

    // If policy is monitoring
    if(isenv_remote(sp, "EAR_POWER_POLICY", "MONITORING_ONLY") == 1)
    {
        // If learning phase is not enabled
        if ((existenv_remote(sp, "EAR_LEARNING_PHASE") == 0) ||
            (isenv_remote(sp, "EAR_LEARNING_PHASE", "0") == 1))
        {
			printenv_remote(sp, "SLURM_CPU_FREQ_REQ");
            // If this is passed SLURM's --cpu-freq argument
            if (getenv_remote(sp, "SLURM_CPU_FREQ_REQ", p_state, 64) == 1)
            {
                p_freq = atoi(p_state);
                p_freq = freq_to_p_state(p_freq);

                sprintf(p_state, "%d", p_freq);
                setenv_remote(sp, "EAR_P_STATE", p_state, 0);

				plug_verbose(sp, 2, "Updating to P_STATE '%s' by '--cpu-freq=%d' command",
							 p_state, p_freq);
            }
        }
    }

    // Switching from SLURM_JOB_NAME to EAR_APP_NAME
    if (getenv_remote(sp, "SLURM_JOB_NAME", buffer2, PATH_MAX) == 1) {
        setenv_remote(sp, "EAR_APP_NAME", buffer2, 1);
    }
}

static void print_general_info(spank_t sp)
{
	plug_verbose(sp, 2, "function print_general_info");

	struct rlimit l;
	int r;

	r = getrlimit(RLIMIT_STACK, &l);

	plug_verbose(sp, 2, "plugin compiled in %s", __DATE__);
	plug_verbose(sp, 2, "stack size limit test (res %d, curr: %lld, max: %lld)",
			r, (long long) l.rlim_cur, (long long) l.rlim_max);
	plug_verbose(sp, 2, "buffers size %d", PATH_MAX);
}

	/*
 *
 *
 *
 *
 *
 */
int read_cluster_configuration_file()
{
	return (ESPANK_SUCCESS);
}

int eard_connection(spank_t sp)
{
	char buffer[64];

	gethostname(host_eard, 128);
	init_application(&app);

	if (!getenv_remote(sp, "SLURM_JOB_ID", buffer, 64)) {
		app.job.id = 0;
	} else {
		app.job.id = atoi(buffer);
	}
	if (!getenv_remote(sp, "SLURM_STEP_ID", buffer, 64)) {
		app.job.step_id = 0;
	} else {
		app.job.step_id = atoi(buffer);
	}
	if (!getenv_remote(sp, "SLURM_JOB_USER", app.job.user_id, GENERIC_NAME)) {
		strcpy(app.job.user_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_NAME",  app.job.app_id, GENERIC_NAME)) {
		strcpy(app.job.app_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_ACCOUNT", app.job.user_acc, GENERIC_NAME)) {
		strcpy(app.job.user_acc, "");
	}
	if (!getenv_remote(sp, "EAR_POWER_POLICY", app.job.policy, GENERIC_NAME)) {
		strcpy(app.job.policy, "");
	}
	if (!getenv_remote(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer, 64)) {
		app.job.th = 0;
	} else {
		app.job.th = atof(buffer);
	}

	if (eards_remote_connect(host_eard) < 0) {
		plug_error("ERROR while connecting with EAR daemon");
	}
	if (!eards_new_job(&app)) {
		plug_error("ERROR while connecting with EAR daemon");
	}
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int eard_disconnection()
{
	if (eards_remote_connect(host_eard) < 0) {
		plug_error("ERROR while connecting with EAR daemon");
	}
	eards_end_job(app.job.id, app.job.step_id);
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int eargmd_connection()
{
	char *c_num_nodes;

	gethostname(host_eargmd, 128);
	getenv_local("SLURM_NNODES", &c_num_nodes);
	num_nodes = atoi(c_num_nodes);

	if (eargm_connect(host_eargmd, port_eargmd) < 0) {
		plug_error("ERROR while connecting with EAR global manager daemon");
	}
	if (!eargm_new_job(num_nodes)) {
		plug_error("ERROR while connecting with EAR global manager daemon");
	}
	eargm_disconnect();

	return (ESPANK_SUCCESS);
}

int eargmd_disconnection()
{
	if (eargm_connect(host_eargmd, port_eargmd) < 0) {
		plug_error("ERROR while connecting with EAR global manager daemon");
	}
	if (!eargm_end_job(num_nodes)) {
		plug_error("ERROR while connecting with EAR global manager daemon");
	}
	eargm_disconnect();

	return (ESPANK_SUCCESS);
}

/*
 *
 *
 * SLURM FRAMEWORK
 *
 *
 */
int slurm_spank_init(spank_t sp, int ac, char **av)
{
	int i;

	for (i = 0; i < 9; ++i)
	{
		if (ESPANK_SUCCESS != spank_option_register(sp, &spank_options_manual[i]))
		{
        	slurm_error("singularity: Unable to register a new option.");
        	return -1;
    	}
	}
}

int slurm_spank_local_user_init (spank_t sp, int ac, char **av)
{
    plug_verbose(sp, 2, "function slurm_spank_local_user_init");
    int r;

    if(spank_context () == S_CTX_LOCAL)
    {
    	//
		eargmd_connection();

		//
		find_ear_user_privileges(sp, ac, av);

		//
		//read_cluster_configuration_file();

		// Obsolete
		if ((r = find_ear_conf_file(sp, ac, av)) != ESPANK_SUCCESS) {
            return r;
        }

        if (isenv_local("EAR", "1"))
        {
            if ((r = local_update_ld_preload(sp)) != ESPANK_SUCCESS) {
                return r;
            }
        }
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_user_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_user_init");   

	if(spank_context() == S_CTX_REMOTE && isenv_remote(sp, "EAR", "1"))
	{
		// Printing job remote information
		print_general_info(sp);

		remote_update_slurm_vars(sp);	
	}

	return (ESPANK_SUCCESS);
}

int slurm_spank_task_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_task_init");   

	return eard_connection(sp);
}

int slurm_spank_task_exit (spank_t sp, int ac, char **av)
{
	FUNCTION_INFO_("slurm_spank_task_exit");

	return eard_disconnection();
}

int slurm_spank_exit (spank_t sp, int ac, char **av) {
	FUNCTION_INFO_("slurm_spank_exit");

	return eargmd_disconnection();
}

/*
 *
 *
 *
 *
 *
 */

static int _opt_ear (int val, const char *optarg, int remote)
{
	plug_nude("function _opt_ear");
	char enabled[32];
	int ienabled;

    if (!remote)
	{
		strncpy(enabled, optarg, 32);
        strtoup(enabled);

		if (strcmp(enabled, "ON") == 0)
		{
    		if (setenv_local("EAR", "1", 1) != 1) {
    			return (ESPANK_ERROR);
    		}
		} else {
			if (setenv_local("EAR", "0", 1) != 1) {
                return (ESPANK_ERROR);
            }
		}
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_learning (int val, const char *optarg, int remote)
{
	plug_nude("function _opt_ear_learning");

    char p_state[8];
    int ioptarg;
	int result;

    if (!remote)
    {
        if (optarg == NULL) {
            return (ESPANK_BAD_ARG);
        }
        if ((ioptarg = atoi(optarg)) < 0) {
            return (ESPANK_BAD_ARG);
        }

        sprintf(p_state, "%d", ioptarg);

		result = setenv_local("EAR_LEARNING_PHASE", "1", 1);
		result = result && setenv_local("EAR_P_STATE", p_state, 1);
		result = result && setenv_local("EAR", "1", 1);

		if (result != 1) {
			return (ESPANK_ERROR);
		}
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_policy (int val, const char *optarg, int remote)
{
	plug_nude("function _opt_ear_policy");

    char policy[32];
    int index = 0;
	int result;

    if (!remote)
    {
        if (optarg == NULL) {
            return (ESPANK_BAD_ARG);
        }

        strncpy(policy, optarg, 32);
        strtoup(policy);

        index += (strcmp(policy, "MIN_ENERGY_TO_SOLUTION") == 0);
        index += (strcmp(policy, "MIN_TIME_TO_SOLUTION") == 0) * 2;
        index += (strcmp(policy, "MONITORING_ONLY") == 0) * 3;

        if (index == 0) {
            return (ESPANK_BAD_ARG);
        }

		result = setenv_local("EAR_POWER_POLICY", policy, 1);
		result = result && setenv_local("EAR", "1", 1);

        if (index == 1 || index == 2) {
            result = result && setenv_local("EAR_P_STATE", "1", 1);
        }
        if (index == 3) {
			result = result && setenv_local("EAR_P_STATE", "1", 0);
        }

		if (result != 1) {
			return (ESPANK_ERROR);
		}
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_user_db (int val, const char *optarg, int remote)
{
	plug_nude("function _opt_ear_user_db");

	int result;

    if (!remote) {
        if (optarg == NULL) return (ESPANK_BAD_ARG);

		result = setenv_local("EAR_USER_DB_PATHNAME", optarg, 1);
		result = result && setenv_local("EAR", "1", 1);

		if (result != 1) {
			return (ESPANK_ERROR);
		}
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_threshold (int val, const char *optarg, int remote)
{
	plug_nude("function _opt_ear_threshold");

	char threshold[4];
    double foptarg = -1;
	int result;

    if (!remote)
    {
        if (optarg == NULL) {
            return (ESPANK_BAD_ARG);
        }
        if ((foptarg = atof(optarg)) < 0.0 || foptarg > 1.0) {
            return (ESPANK_BAD_ARG);
        }

        sprintf(threshold, "%0.2f", foptarg);

        result = setenv_local("EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", threshold, 1);
		result = result && setenv_local("EAR_PERFORMANCE_PENALTY", threshold, 1);
		result = result && setenv_local("EAR", "1", 1) != 1;

		if (result != 1) {
			return (ESPANK_ERROR);
		}
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_verbose (int val, const char *optarg, int remote)
{
	plug_nude("function _opt_ear_verbose");

    char c_verbosity[4];
    int ioptarg;
    int result;

    if (!remote)
    {
        if (optarg == NULL) {
            return (ESPANK_BAD_ARG);
        }

        ioptarg = atoi(optarg);
        if (ioptarg < 0) ioptarg = 0;
        if (ioptarg > 4) ioptarg = 4;

        sprintf(c_verbosity, "%i", ioptarg);
        result = setenv_local("EAR_VERBOSE", c_verbosity, 1);
        result = result && setenv_local("EAR", "1", 1);

		if (verbosity == -1) {
			verbosity = ioptarg;
		}

        if (result != 1) {
			return (ESPANK_ERROR);
		}
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_traces (int val, const char *optarg, int remote)
{
	plug_nude("function _opt_ear_traces");
    int result;

    if (!remote) {
        result = setenv_local("EAR_TRACES", "1", 1);
        result = result && setenv_local("EAR", "1", 1);

		if (result != 1) {
			return (ESPANK_ERROR);
		}
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_mpi_dist(int val, const char *optarg, int remote)
{
	plug_nude("function _opt_mpi_dist");

	if (!remote)
	{
        if (optarg == NULL) {
        	return (ESPANK_BAD_ARG);
        }
		if (setenv_local("EAR_MPI_DIST", optarg, 1) != 1) {
			return (ESPANK_ERROR);
        }
	}

	return (ESPANK_SUCCESS);
}

static int _opt_ear_tag(int val, const char *optarg, int remote) 
{
    plug_nude("function _opt_tag");
    return (ESPANK_SUCCESS);
}

#define FUNCTION_INFO_(string) \
	slurm_error(string);

int slurm_spank_job_prolog (spank_t sp, int ac, char **av) { FUNCTION_INFO_("slurm_spank_job_prolog"); return (ESPANK_SUCCESS); }
int slurm_spank_init_post_opt (spank_t sp, int ac, char **av) { FUNCTION_INFO_("slurm_spank_init_post_op"); return (ESPANK_SUCCESS); }
int slurm_spank_task_init_privileged (spank_t sp, int ac, char **av) { FUNCTION_INFO_("slurm_spank_task_init_privileged"); return (ESPANK_SUCCESS); }
int slurm_spank_task_post_fork (spank_t sp, int ac, char **av) { FUNCTION_INFO_("slurm_spank_task_post_fork"); return (ESPANK_SUCCESS); }
int slurm_spank_job_epilog (spank_t sp, int ac, char **av) { FUNCTION_INFO_("slurm_spank_job_epilog"); return (ESPANK_SUCCESS); }
