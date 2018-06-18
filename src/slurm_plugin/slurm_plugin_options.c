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

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <linux/limits.h>
#include <slurm/spank.h>

#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>
#include <slurm_plugin/slurm_plugin_options.h>

extern char buffer1[PATH_MAX];
extern char buffer2[PATH_MAX];
extern int verbosity;

struct spank_option spank_options_manual[9] =
{
	{ "ear", "on|off", "Enables/disables Energy Aware Runtime",
	  1, 0, (spank_opt_cb_f) _opt_ear
	},
	{ "ear-policy", "type", "Selects an energy policy for EAR\n" \
	  "{type=MIN_ENERGY_TO_SOLUTION|MIN_TIME_TO_SOLUTION|MONITORING_ONLY}",
	  1, 0, (spank_opt_cb_f) _opt_ear_policy
	},
	{ "ear-policy-th", "value", "Specifies the threshold to be used by EAR policy" \
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
	{ "ear-verbose", "value", "Specifies the level of the verbosity\n" \
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

int slurm_spank_init(spank_t sp, int ac, char **av)
{
    int i;

    for (i = 0; i < 9; ++i)
    {   
        if (spank_option_register(sp, &spank_options_manual[i]) != ESPANK_SUCCESS)
        {
            slurm_error("unable to register a new option.");
            return -1; 
        }
    }   
}

/*
 * Plugin intern environment variables:
 * - EAR_INSTALL_DIR
 * - EAR_ETC_DIR
 * - EAR
 * - EAR_LEARNING_PHASE
 * - EAR_VERBOSE
 * - EAR_POWER_POLICY
 * - EAR_DB_PSTATE
 * - EAR_USER_DB_PATHNAME
 * - EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN
 * - EAR_PERFORMANCE_PENALTY
 * - EAR_TRACES
 * - EAR_MPI_DIST
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
