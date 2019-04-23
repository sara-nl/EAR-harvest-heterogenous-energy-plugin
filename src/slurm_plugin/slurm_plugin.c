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

#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>
#include <slurm_plugin/slurm_plugin_options.h>
#include <slurm_plugin/slurm_plugin_reports.h>

// Spank
SPANK_PLUGIN(EAR_PLUGIN, 1)

// Verbosity
int verbosity = -1;

// Context
static struct passwd *upw;
static struct group *gpw;
static uid_t uid;
static gid_t gid;

// Buffers
char buffer1[SZ_PATH];
char buffer2[SZ_PATH];
char buffer3[SZ_PATH]; // helper buffer

// Externs
extern char eargmd_host[SZ_NAME_MEDIUM];
extern unsigned int eargmd_port;
extern unsigned int eargmd_enbl;
extern int eard_exst;

/*
 *
 * Disabling
 *
 */

void _local_library_disable()
{
	setenv_local("SLURM_EAR_LIBRARY", "0", 1);
}

void _remote_library_disable(spank_t sp)
{
	if(isenv_remote(sp, "SLURM_EAR_LIBRARY", "1")) {
		setenv_remote(sp, "LD_PRELOAD", "", 1);
		setenv_remote(sp, "SLURM_EAR_LIBRARY", "0", 1);
	}
}

void _local_plugin_enable()
{
	setenv_local("SLURM_EAR_PLUGIN", "1", 1);
}

void _local_plugin_disable()
{
	setenv_local("SLURM_EAR_PLUGIN", "0", 1);
}

void _remote_plugin_disable(spank_t sp)
{
	setenv_remote(sp, "SLURM_EAR_PLUGIN", "0", 1);
}

int _is_plugin_enabled(spank_t sp)
{
	if (spank_context() == S_CTX_SRUN || spank_context() == S_CTX_SBATCH) {
		return isenv_local("SLURM_EAR_PLUGIN", "1");
	}

	if (spank_context() == S_CTX_REMOTE) {
		return isenv_remote(sp, "SLURM_EAR_PLUGIN", "1");
	}

	return 0;
}

/*
 *
 *
 * Information update
 *
 *
 */

int _read_plugstack(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function _read_plugstack");

	int found_earmgd_port = 0;
	int found_eargmd_host = 0;
	int found_predir = 0;
	int found_tmpdir = 0;
	char *pre_dir = NULL;
	char *tmp_dir = NULL;
	int i;

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 8) && (strncmp ("default=", av[i], 8) == 0))
		{
			plug_verbose(sp, 2, "plugstack found library by default '%s'", &av[i][8]);
			
			// If enabled by default
			if (strncmp ("default=on", av[i], 10) == 0) {
				// EAR == 1: enable
				// EAR == 0: nothing
				// EAR == whatever: enable (bug protection)
				if (!isenv_local("SLURM_EAR_LIBRARY", "0")) {
					setenv_local("SLURM_EAR_LIBRARY", "1", 1);
				} 
			// If disabled by default or de administrator have misswritten
			} else {
				// EAR == 1: nothing
				// EAR == 0: disable
				// EAR == whatever: disable (bug protection)
				if (!isenv_local("SLURM_EAR_LIBRARY", "1")) {
					setenv_local("SLURM_EAR_LIBRARY", "0", 1);
				}
			}
		}
		if ((strlen(av[i]) > 14) && (strncmp ("localstatedir=", av[i], 14) == 0))
		{
			tmp_dir = &av[i][14];
			found_tmpdir = 1;

			plug_verbose(sp, 2, "plugstack found temporal files in path '%s'", tmp_dir);
			setenv_local("SLURM_EAR_PATH_TEMP", tmp_dir, 1);
		}
		if ((strlen(av[i]) > 7) && (strncmp ("prefix=", av[i], 7) == 0))
		{
			pre_dir = &av[i][7];
			found_predir = 1;

			plug_verbose(sp, 2, "plugstack found prefix in path '%s'", pre_dir);
			setenv_local("SLURM_EAR_PATH_INST", pre_dir, 1);
		}
		if ((strlen(av[i]) > 12) && (strncmp ("eargmd_host=", av[i], 12) == 0))
		{
			found_eargmd_host = 1;
			strncpy(eargmd_host, &av[i][12], SZ_NAME_MEDIUM);
		}
		if ((strlen(av[i]) > 12) && (strncmp ("eargmd_port=", av[i], 12) == 0))
		{
			found_earmgd_port = 1;
			eargmd_port = atoi(&av[i][12]);
		}
	}

	// EARGMD enabled?
	eargmd_enbl = found_eargmd_host && found_earmgd_port;

	// TMP folder missing?
	if (!found_tmpdir) {
		plug_verbose(sp, 2, "missing plugstack localstatedir directory");
		return (ESPANK_STOP);
	}

	// Prefix folder missing?
	if (!found_predir) {
		plug_verbose(sp, 2, "missing plugstack prefix directory");
		return (ESPANK_ERROR);
	}

	return (EAR_SUCCESS);
}

int _read_user_info(spank_t sp)
{
	plug_verbose(sp, 2, "function _read_user_info");

	#define replace(name) \
		unsetenv_local("SLURM_EAR_" name); \
		replenv_local("OPT_" name, "SLURM_EAR_" name); \
		unsetenv_local("OPT_"); \

	// Replacing options
	replace("LEARNING_PHASE");
	replace("POWER_POLICY");
	replace("POWER_POLICY_TH");
	replace("P_STATE");
	replace("FREQUENCY");
	replace("MIN_PERFORMANCE_EFFICIENCY_GAIN");
	replace("PERFORMANCE_PENALTY");
	replace("TRACE_PATH");
	replace("MPI_DIST");
	replace("USER_DB_PATHNAME");
	replace("ENERGY_TAG");
	replace("APP_NAME");
	replace("TMP");

	// Getting user ids
	uid = geteuid();
	gid = getgid();

	// Getting user names
	upw = getpwuid(uid);
	gpw = getgrgid(gid);

	if (upw == NULL) {
		plug_verbose(sp, 2, "converting UID in username");
		return (ESPANK_ERROR);
	}

	if (gpw == NULL) {
		plug_verbose(sp, 2, "converting GID in groupname");
		return (ESPANK_ERROR);
	}

	// To environment variables
	setenv_local("SLURM_SLURM_EAR_USER", upw->pw_name, 1);
	setenv_local("SLURM_SLURM_EAR_GROUP", gpw->gr_name, 1);

	plug_verbose(sp, 2, "user detected '%u -> %s'", uid, upw->pw_name);
	plug_verbose(sp, 2, "user group detected '%u -> %s'", gid, gpw->gr_name);
	plug_verbose(sp, 2, "user account detected '%s'", getenv("SLURM_JOB_ACCOUNT"));

	return (ESPANK_SUCCESS);
}

int _set_ld_preload(spank_t sp)
{
	plug_verbose(sp, 2, "function _set_ld_preload");
	
	char *ear_root_dir = NULL;

	buffer1[0] = '\0';
	buffer2[0] = '\0';

	if (getenv_local("SLURM_EAR_PATH_INST", &ear_root_dir) == 0)
	{
		plug_verbose(sp, 2, "Error, wrong environment for setting LD_PRELOAD");
		return ESPANK_ERROR;
	}
	appendenv(buffer1, ear_root_dir, sizeof(buffer1));

	// Appending libraries to LD_PRELOAD
	if (isenv_local("SLURM_EAR_MPI_DIST", "openmpi")) {
		snprintf(buffer2, sizeof(buffer2), "%s/%s", buffer1, OMPI_C_LIB_PATH);
	} else {
		snprintf(buffer2, sizeof(buffer2), "%s/%s", buffer1, IMPI_C_LIB_PATH);
	}

	//
	setenv_local("LD_PRELOAD", buffer2, 1);

	plug_verbose(sp, 2, "updated LD_PRELOAD envar '%s'", buffer2);

	return (ESPANK_SUCCESS);
}

/*
 *
 *
 * Framework
 *
 *
 */

int slurm_spank_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_init");

	_opt_register(sp);

	if (spank_context() == S_CTX_SRUN) {
		setenv_local("SLURM_LAST_CONTEXT", "SRUN", 1);
	}
	if (spank_context() == S_CTX_SBATCH) {
		setenv_local("SLURM_LAST_CONTEXT", "SBATCH", 1);
	}
	if (spank_context() == S_CTX_SRUN || spank_context() == S_CTX_SBATCH) {
		_local_plugin_enable();	
	}

	return ESPANK_SUCCESS;
}

int slurm_spank_init_post_opt(spank_t sp, int ac, char **av)
{
    plug_verbose(sp, 2, "function slurm_spank_init_post_opt");

	// No need of testing the context
	if(!_is_plugin_enabled(sp)) {
		return ESPANK_SUCCESS;
	}

	// Filling user data
	if (_read_user_info(sp) != ESPANK_SUCCESS) {
		_local_library_disable();
		return ESPANK_SUCCESS;
	}

	// Reading plugstack.conf
	if (_read_plugstack(sp, ac, av) != ESPANK_SUCCESS) {
		_local_plugin_disable();
		return ESPANK_SUCCESS;
	}

	//
	if (spank_context() == S_CTX_SRUN && isenv_local("SLURM_EAR_LIBRARY", "1")) {
		_set_ld_preload(sp);
	}

    return (ESPANK_SUCCESS);
}

int slurm_spank_user_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_user_init");

	if(!_is_plugin_enabled(sp)) {
		return (ESPANK_SUCCESS);
	}

	//
	if (spank_context() == S_CTX_REMOTE)
  	{
		_remote_init_environment(sp, ac, av);

		if (remote_eard_report_start(sp) == ESPANK_SUCCESS)
		{
			if (isenv_remote(sp, "SLURM_EAR_LIBRARY", "1") && isenv_remote(sp, "SLURM_LAST_CONTEXT", "SRUN")) {
				remote_read_shared_data_set_environment(sp);
			}
		}

		remote_print_environment(sp);
	}
	
	return (ESPANK_SUCCESS);
}

int slurm_spank_task_exit (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_task_exit");

	spank_err_t err;
	int status = 0;

	// Not needed to test if the plugin is enabled
	if (eard_exst == 0)
	{
		err = spank_get_item (sp, S_TASK_EXIT_STATUS, &status);

		if (err == ESPANK_SUCCESS) {
			eard_exst = WEXITSTATUS(status);
		}
	}

	return (ESPANK_SUCCESS);
}

int slurm_spank_exit (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "slurm_spank_exit");

	// Not needed to test if the plugin is enabled
	if (spank_context() == S_CTX_REMOTE) {
		remote_eard_report_finish(sp);
	}

	return (ESPANK_SUCCESS);
}
