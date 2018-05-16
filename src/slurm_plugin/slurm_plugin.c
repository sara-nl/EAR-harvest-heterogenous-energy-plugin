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

#include <cpufreq.h>
#include <slurm/spank.h>

#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>
#include <common/string_enhanced.h>
#include <common/config.h>
#include <sys/time.h>
#include <sys/resource.h>

SPANK_PLUGIN(EAR_PLUGIN, 1)
pid_t daemon_pid = -1;
extern int verbosity;

struct spank_option spank_options[] = {
    { "ear", "", "Enables Energy Aware Runtime",
      0, 0, (spank_opt_cb_f) _opt_ear
    },
    { "ear-policy", "type",
      "Selects an energy policy for EAR.\n" \
      "{type=MIN_ENERGY_TO_SOLUTION|MIN_TIME_TO_SOLUTION|MONITORING_ONLY}",
      1, 0, (spank_opt_cb_f) _opt_ear_policy
    },
    { "ear-policy-th", "value",
      "Specifies the threshold to be used by EAR policy" \
      " {value=[0..1]}",
      1, 0, (spank_opt_cb_f) _opt_ear_threshold
    },
    { "ear-user-db", "file",
      "Specifies the file to save the user applications metrics summary\n" \
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
    SPANK_OPTIONS_TABLE_END
};

/*
 *
 *
 * Daemon execution
 *
 *
 */

static void exec_ear_daemon(spank_t sp)
{
    verbose(sp, 2, "function exec_ear_daemon");

    static char buffer[PATH_MAX];
    char *ear_install_path;
    char *ear_verbose;
    char *ear_tmp;

    // Getting environment variables configuration
    ear_install_path = getenv("EAR_INSTALL_PATH");
	ear_verbose      = getenv("EAR_VERBOSE");
	ear_tmp          = getenv("EAR_TMP");

    sprintf(buffer, "%s/%s", ear_install_path, EAR_DAEMON_PATH);

    // Executing EAR daemon
    if (execl(buffer, buffer, "1", ear_tmp, ear_verbose, (char *) 0) == -1) {
        slurm_error("Error while executing %s (%s)", buffer, strerror(errno));
        exit(errno);
    }
}

#define DAEMON_INTERMEDIATE 1

#if DAEMON_INTERMEDIATE
static void my_chld_f(int s)
{
	slurm_error("Intermediate: killing daemon %d", daemon_pid);
	kill(daemon_pid, SIGTERM);
}

static void fork_intermediate_daemon(spank_t sp)
{
	verbose(sp, 2, "function fork_intermediate_daemon");
	int ret, exit_code;

	daemon_pid = fork();

	if (daemon_pid == 0) {
		exec_ear_daemon(sp);
	} else if (daemon_pid < 0) {
		slurm_error("Intermediate fork returned %i (%s)", daemon_pid, strerror(errno));
		exit(errno);
	}

	/*
	 * SIGNAL
	 */
	struct sigaction my_sigchld;
	sigset_t my_mask;

	sigemptyset(&my_mask);
	sigaddset(&my_mask, SIGTERM);

	sigprocmask(SIG_UNBLOCK, &my_mask, NULL);
	my_sigchld.sa_handler = my_chld_f;
	sigemptyset(&my_sigchld.sa_mask);
	my_sigchld.sa_flags = SA_RESTART;
	
	sigaction(SIGTERM, &my_sigchld, NULL);

	/*
 	 * WAIT
 	 */
	slurm_error("Intermediate: waiting for daemon %d", daemon_pid);
	ret = waitpid(-1, &exit_code, 0);

	if (ret != daemon_pid) {
        	slurm_error("Intermediate: unexpected pid");
    	} else {
		slurm_error("Intermediate: wait pid success");
	}

	if (ret > 0)
    	{
        	if (WIFEXITED(exit_code)) {
			slurm_error("Intermediate: process %d exits with exit code %d", ret, WEXITSTATUS(exit_code));
		} else {
			slurm_error("Intermediate: process %d finsh because of signal %d", ret, WTERMSIG(exit_code));
		}
	}

	exit(0);
}
#endif

static int fork_ear_daemon(spank_t sp)
{
    verbose(sp, 2, "function fork_ear_daemon");

    if ((existenv_local("EAR_INSTALL_PATH") == 1) &&
        (existenv_local("EAR_DB_PATHNAME") == 1) &&
        (existenv_local("EAR_VERBOSE") == 1) &&
        (existenv_local("EAR_TMP") == 1))
    {
        daemon_pid = fork();

        if (daemon_pid == 0) {
            #if DAEMON_INTERMEDIATE
			fork_intermediate_daemon(sp);
			#else
            exec_ear_daemon(sp);
            #endif
        } else if (daemon_pid < 0) {
            slurm_error("Fork returned %i (%s)", daemon_pid, strerror(errno));
            return ESPANK_ERROR;
        }
    } else {
        slurm_error("Missing crucial environment variable");
        return ESPANK_ENV_NOEXIST;
    }
    return ESPANK_SUCCESS;
}

/*
 *
 *
 *
 *
 *
 */

static int local_update_ld_preload(spank_t sp)
{
    verbose(sp, 2, "function local_update_ld_preload");
	
	static char buffer[PATH_MAX];
	char *ear_install_path = NULL;
	char *ld_preload = NULL;

	buffer[0] = '\0';

    if (getenv_local("EAR_INSTALL_PATH", &ear_install_path) == 0)
    {
		slurm_error("Error, missing EAR_INSTALL_PATH");
        return ESPANK_ERROR;
    }
    appendenv(buffer, ear_install_path, PATH_MAX);
    
	// Appending libraries to LD_PRELOAD
    if (isenv_local("EAR_TRACES", "1") == 1)
	{
		if (isenv_local("EAR_MPI_DIST", "openmpi")) {
        	sprintf(buffer, "%s/%s", buffer, OMPI_TRACE_LIB_PATH);
		} else {
        	sprintf(buffer, "%s/%s", buffer, IMPI_TRACE_LIB_PATH);
		}
    }
	else
	{	
		if (isenv_local("EAR_MPI_DIST", "openmpi")) {
			sprintf(buffer, "%s/%s", buffer, OMPI_LIB_PATH);
		} else { 
			sprintf(buffer, "%s/%s", buffer, IMPI_LIB_PATH);
		}
    }

    //
    if(setenv_local("LD_PRELOAD", buffer, 1) != 1) {
		return ESPANK_ERROR;
	}

	verbose(sp, 1, "updated LD_PRELOAD envar '%s'", buffer);

	return ESPANK_SUCCESS;
}

static void remote_update_slurm_vars(spank_t sp)
{
	verbose(sp, 2, "function remote_update_slurm_vars");

    static char buffer[PATH_MAX];
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

				verbose(sp, 1, "Updating to P_STATE '%s' by '--cpu-freq=%d' command", p_state, p_freq);
            }
        }
    }

    // Switching from SLURM_JOB_NAME to EAR_APP_NAME
    if (getenv_remote(sp, "SLURM_JOB_NAME", buffer, PATH_MAX) == 1) {
        setenv_remote(sp, "EAR_APP_NAME", buffer, 1);
    }
}

static void print_general_info(spank_t sp)
{
	verbose(sp, 2, "function print_general_info");

	struct rlimit l;
	int r;

	r = getrlimit(RLIMIT_STACK, &l);

	verbose(sp, 2, "plugin compiled in %s", __DATE__);
	verbose(sp, 2, "stack size limit test (res %d, curr: %lld, max: %lld)",
			r, (long long) l.rlim_cur, (long long) l.rlim_max);
}

/*
 *
 *
 * SLURM FRAMEWORK
 *
 *
 */
int slurm_spank_local_user_init (spank_t sp, int ac, char **av)
{
    verbose(sp, 2, "function slurm_spank_local_user_init");
    int r;

    if(spank_context () == S_CTX_LOCAL)
    {
    	// Printing job local information
		print_general_info(sp);

		find_ear_user_privileges(sp, ac, av);

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
	verbose(sp, 2, "function slurm_spank_task_init");   

	if(spank_context() == S_CTX_REMOTE && isenv_remote(sp, "EAR", "1"))
	{
		// Printing job remote information
		print_general_info(sp);

		remote_update_slurm_vars(sp);	
	}

	return (ESPANK_SUCCESS);
}

int slurm_spank_init (spank_t sp, int ac, char **av)
{
    verbose(sp, 2, "function slurm_spank_init");

    if(spank_context() == S_CTX_SLURMD) {
        return slurm_spank_slurmd_init(sp, ac, av);
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_slurmd_init (spank_t sp, int ac, char **av)
{
    verbose(sp, 2, "function slurm_spank_slurmd_init");
    int r;

    if(spank_context() == S_CTX_SLURMD && daemon_pid < 0)
    {
        if((r = find_ear_conf_file(sp, ac, av)) != ESPANK_SUCCESS) {
			verbose(sp, 0, "Error while reading configuration file %d", r);
            return r;
        }

        return fork_ear_daemon(sp);
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_slurmd_exit (spank_t sp, int ac, char **av)
{
	verbose(sp, 2, "function slurm_spank_slurmd_exit");

	if(spank_context() == S_CTX_SLURMD && daemon_pid > 0)
	{
		kill(daemon_pid, SIGTERM);
	}

	return (ESPANK_SUCCESS);
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

    if (!remote) {
    	if (setenv_local("EAR", "1", 1) != 1) {
    		return (ESPANK_ERROR);
    	}
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_learning (int val, const char *optarg, int remote)
{
	plug_nude("function _opt_ear_learning");

    char p_state[2];
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
