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

#include <dirent.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_environment.h>
#include <slurm_plugin/slurm_plugin_options.h>

#define SRUN_OPTIONS	10

static char buffer[SZ_PATH];
static char mpi_opt[SZ_PATH];
static char mpi_dst[SZ_PATH];

struct spank_option spank_options_manual[SRUN_OPTIONS] =
{
	{ "ear", "on|off", "Enables/disables Energy Aware Runtime",
	  1, 0, (spank_opt_cb_f) _opt_ear
	},
	{ 
	  "ear-policy", "type", "Selects an energy policy for EAR\n" \
	  "{type=MIN_ENERGY_TO_SOLUTION|MIN_TIME_TO_SOLUTION|MONITORING_ONLY}",
	  1, 0, (spank_opt_cb_f) _opt_ear_policy
	},
	{ "ear-cpufreq", "frequency", "Specifies the start frequency to be used by EAR policy (in KHz)",
	  1, 0, (spank_opt_cb_f) _opt_ear_frequency
	},
	{ "ear-policy-th", "value", "Specifies the threshold to be used by EAR policy" \
	  " {value=[0..1]}",
	  1, 0, (spank_opt_cb_f) _opt_ear_threshold
	},
	{ "ear-user-db", "file",
	  "Specifies the file to save the user applications metrics summary" \
	  "'file.nodename.csv' file will be created per node. If not defined, these files won't be generated.",
	  1, 0, (spank_opt_cb_f) _opt_ear_user_db
	},
	{ "ear-mpi-dist", "dist", mpi_opt,
	  1, 0, (spank_opt_cb_f) _opt_ear_mpi_dist
	},
	{ "ear-verbose", "value", "Specifies the level of the verbosity\n" \
	  "{value=[0..5]}; default is 0",
	  1, 0, (spank_opt_cb_f) _opt_ear_verbose
	},
	{ "ear-learning", "value",
	  "Enables the learning phase for a given P_STATE {value=[1..n]}",
	  1, 0, (spank_opt_cb_f) _opt_ear_learning
	},
	{ "ear-tag", "tag", "Sets an energy tag (max 32 chars)",
	  1, 0, (spank_opt_cb_f) _opt_ear_tag
	},
	{ "ear-traces", "path", "Saves application traces with metrics and internal details",
	  1, 0, (spank_opt_cb_f) _opt_ear_traces
	}
};

static int _opt_register_mpi(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function _opt_dir");

	struct dirent *file;
	int i, pc, qc;
	char *p, *q;
	DIR *dir;

	// Filing a default option string
	pc = sprintf(mpi_dst, "default,");
	p = &mpi_dst[pc];

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 7) && (strncmp("prefix=", av[i], 7) == 0))
		{
			sprintf(buffer, "%s/lib/", &av[i][7]);
			dir = opendir(buffer);

			if (dir)
			{
			while ((file = readdir(dir)) != NULL)
			{
				if (strstr(file->d_name, "libear.") != NULL)
				{
					q = &file->d_name[7];
					qc = strlen(q);

					if (qc > 2 && strcmp(&q[qc - 3], ".so") == 0) {
						q[qc - 3] = '\0';
						pc = sprintf(p, "%s,", q);
						p = &p[pc];
					}
				}
			}
			closedir(dir);
			}
		}
	}

	// Cleaning the last comma	
	p = &p[-1];
	*p = '\0';

	// Filling the option string with distribution options
	sprintf(mpi_opt, "Selects the MPI distribution for compatibility of your application {dist=%s}", mpi_dst);

	return ESPANK_SUCCESS;
}

int _opt_register(spank_t sp, int ac, char **av)
{
	spank_err_t s;
	int length;
	int i;

	//
	_opt_register_mpi(sp, ac, av);

	//
	length = SRUN_OPTIONS - !exenv_agnostic(sp, "EAR_GUI");

	for (i = 0; i < length; ++i)
	{
		if ((s = spank_option_register(sp, &spank_options_manual[i])) != ESPANK_SUCCESS)
		{
			plug_verbose(NULL, 2, "unable to register SPANK option %s", spank_options_manual[i].name);
			return s;
		}
	}

	return ESPANK_SUCCESS;
}

/*
 *
 *
 *
 */

int _opt_ear (int val, const char *optarg, int remote)
{
	plug_verbose(NULL, 2, "function _opt_ear");

	if (!remote)
	{
		if (optarg == NULL) {
			return ESPANK_BAD_ARG;
		}

		strncpy(buffer, optarg, 8);
		strtoup(buffer);

		if (strcmp(buffer, "ON") == 0) {
			setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
		} else if (strcmp(buffer, "OFF") == 0) {
			setenv_agnostic(NULL, Var.comp_libr.cmp, "0", 1);
		} else {
			plug_verbose(NULL, 2, "Invalid enabling value '%s'", buffer);
			return ESPANK_BAD_ARG;
		}
	}

	return ESPANK_SUCCESS;
}

int _opt_ear_learning (int val, const char *optarg, int remote)
{
	plug_verbose(NULL, 2, "function _opt_ear_learning");

	int ioptarg;

	if (!remote)
	{
		if (optarg == NULL) {
			return ESPANK_BAD_ARG;
		}
		if ((ioptarg = atoi(optarg)) < 0) {
			return ESPANK_BAD_ARG;
		}

		snprintf(buffer, 4, "%d", ioptarg);
		setenv_agnostic(NULL, Var.learning.loc, buffer, 1);
		setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
	}

	return ESPANK_SUCCESS;
}

int _opt_ear_policy (int val, const char *optarg, int remote)
{
	plug_verbose(NULL, 2, "function _opt_ear_policy");

	if (!remote)
	{
		if (optarg == NULL) {
			return ESPANK_BAD_ARG;
		}

		strncpy(buffer, optarg, 32);
		strtoup(buffer);

		if (policy_name_to_id(buffer) < 0) {
			plug_verbose(NULL, 2, "Invalid policy '%s'", buffer);
			return ESPANK_STOP;
		}

		setenv_agnostic(NULL, Var.policy.loc, buffer, 1);
		setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
	}

	return ESPANK_SUCCESS;
}

int _opt_ear_frequency (int val, const char *optarg, int remote)
{
    plug_verbose(NULL, 2, "function _opt_ear_threshold");

    ulong loptarg;

    if (!remote)
    {
        if (optarg == NULL) {
            return ESPANK_BAD_ARG;
        }
		
		loptarg = (ulong) atol(optarg);
		snprintf(buffer, 16, "%lu", loptarg);
		setenv_agnostic(NULL, Var.frequency.loc, buffer, 1);
		setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
    }

    return ESPANK_SUCCESS;
}

int _opt_ear_threshold (int val, const char *optarg, int remote)
{
	plug_verbose(NULL, 2, "function _opt_ear_threshold");

	double foptarg = -1;

	if (!remote)
	{
		if (optarg == NULL) {
			return ESPANK_BAD_ARG;
		}
		if ((foptarg = atof(optarg)) < 0.0 || foptarg > 1.0) {
			return ESPANK_BAD_ARG;
		}

		snprintf(buffer, 8, "%0.2f", foptarg);
		setenv_agnostic(NULL, Var.policy_th.loc, buffer, 1);
		setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
	}

	return ESPANK_SUCCESS;
}

int _opt_ear_user_db (int val, const char *optarg, int remote)
{
	plug_verbose(NULL, 2, "function _opt_ear_user_db");

	if (!remote)
	{
		if (optarg == NULL) {
			return ESPANK_BAD_ARG;
		}

		setenv_agnostic(NULL, Var.path_usdb.loc, optarg, 1);
		setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
	}

	return ESPANK_SUCCESS;
}

int _opt_ear_verbose (int val, const char *optarg, int remote)
{
	plug_verbose(NULL, 2, "function _opt_ear_verbose");

	int ioptarg;

	if (!remote)
	{
		if (optarg == NULL) {
			return ESPANK_BAD_ARG;
		}

		ioptarg = atoi(optarg);
		if (ioptarg < 0) ioptarg = 0;
		if (ioptarg > 4) ioptarg = 4;
		snprintf(buffer, 4, "%d", ioptarg);

		setenv_agnostic(NULL, Var.verbose.loc, buffer, 1);
		setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
	}

	return ESPANK_SUCCESS;
}

int _opt_ear_traces (int val, const char *optarg, int remote)
{
	plug_verbose(NULL, 2, "function _opt_ear_traces");

	if (!remote)
	{
		if (optarg == NULL) {
			return ESPANK_BAD_ARG;
		}

		setenv_agnostic(NULL, Var.path_trac.loc, optarg, 1);
		setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
	}

	return ESPANK_SUCCESS;
}

int _opt_ear_mpi_dist(int val, const char *optarg, int remote)
{
	plug_verbose(NULL, 2, "function _opt_mpi_dist");

	if (!remote)
	{
		if (optarg == NULL) {
			return (ESPANK_BAD_ARG);
		}

		if (strcmp(optarg, "default") != 0) {
			setenv_agnostic(NULL, Var.version.loc, optarg, 1);
		}
		setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
	}

	return (ESPANK_SUCCESS);
}

int _opt_ear_tag(int val, const char *optarg, int remote)
{
	plug_verbose(NULL, 2, "function _opt_tag");

	if (!remote)
	{
		if (optarg == NULL) {
			return ESPANK_BAD_ARG;
		}

		setenv_agnostic(NULL, Var.tag.loc, optarg, 1);
		setenv_agnostic(NULL, Var.comp_libr.cmp, "1", 1);
	}
	return ESPANK_SUCCESS;
}
