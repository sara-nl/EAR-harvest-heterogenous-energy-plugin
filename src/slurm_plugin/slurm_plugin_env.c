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

#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>

// Context
static struct passwd *upw;
static struct group *gpw;
static uid_t uid;
static gid_t gid;

// Buffers
extern char buffer1[SZ_PATH];
extern char buffer2[SZ_PATH];
extern char buffer3[SZ_PATH];

/*
 *
 *
 *
 *
 *
 */

int plug_env_islocal(spank_t sp)
{
	int context = spank_context();
	return context == S_CTX_SRUN || context == S_CTX_SBATCH;
}

int plug_env_isremote(spank_t sp)
{
	int context = spank_context();
	return context == S_CTX_REMOTE;
}

int plug_env_unsetenv(spank_t sp, char *var)
{
	if (plug_env_islocal()) {
		return unsetenv_local(var);
	} else {
		return setenv_remote(sp, var);
	}
}

int plug_env_setenv(spank_t sp, char *var, char *val, int ow)
{
	if (plug_env_islocal()) {
		return setenv_local(var, val, ow);
	} else {
		return setenv_remote(sp, var, val, ow);
	}
}

int plug_env_getenv(spank_t sp, char *var, char *buf, int len)
{
	if (plug_env_islocal()) {
		return getenv_local(var, buf, len);
	} else {
		return getenv_remote(sp, var, buf, len);
	}
}

int plug_env_isenv(spank_t sp, char *var, char *val)
{
	if (plug_env_islocal()) {
		return isenv_local(component, val);
	} else {
		return isenv_remote(sp, component, val);
	}
}

int plug_env_repenv(spank_t sp, char *var_old, char *var_new)
{
	if (plug_env_islocal()) {
		return repenv_local(var_old, var_new);
	} else {
		return repenv_remote(sp, var_old, var_new);
	}
}

/*
 *
 *
 *
 *
 *
 */

void plug_env_clean(spank_t sp, int ac, char **av)
{
	// Unsetting loader
	plug_env_unsetenv(sp, "LD_PRELOAD");

	#define unset_and_replace(name) \
		plug_env_unsetenv(sp, "EAR_" name); \
		plug_env_repenv(sp, "PLG_" name, "EAR_" name);

	// Replacing options
	unset_and_replace("LEARNING_PHASE");
	unset_and_replace("POWER_POLICY");
	unset_and_replace("POWER_POLICY_TH");
	unset_and_replace("P_STATE");
	unset_and_replace("FREQUENCY");
	unset_and_replace("MIN_PERFORMANCE_EFFICIENCY_GAIN");
	unset_and_replace("PERFORMANCE_PENALTY");
	unset_and_replace("TRACE_PATH");
	unset_and_replace("MPI_DIST");
	unset_and_replace("USER_DB_PATHNAME");
	unset_and_replace("ENERGY_TAG");
	unset_and_replace("APP_NAME");
	unset_and_replace("TMP");
}


int plug_env_verbtest(spank_t sp, int level)
{
	static int verbosity = -1;
	char env_remote[8];
	char *env_local;

	if (verbosity == -1)
	{
		if (spank_remote(sp)) {
			if (getenv_remote(sp, "EAR_PLUGIN_VERBOSE", env_remote, 8) == 1) {
				verbosity = atoi(env_remote);
			} else {
				verbosity = 0;
			}
		} else {
			if (getenv_local("EAR_PLUGIN_VERBOSE", &env_local) == 1) {
				verbosity = atoi(env_local);
			} else {
				verbosity = 0;
			}
		}
	}

	return verbosity >= level;
}

/*
 *
 *
 *
 *
 *
 */

int plug_env_readstack(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function plug_env_readstack");

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
				if (!plug_env_isenv(sp, ENV_LIB_EN, "0")) {
					plug_env_setenv(sp, ENV_LIB_EN, "1", 1);
				} 
			// If disabled by default or de administrator have misswritten
			} else {
				// EAR == 1: nothing
				// EAR == 0: disable
				// EAR == whatever: disable (bug protection)
				if (!plug_env_isenv(sp, ENV_LIB_EN, "1")) {
					plug_env_setenv(sp, ENV_LIB_EN, "0", 1);
				}
			}
		}
		if ((strlen(av[i]) > 14) && (strncmp ("localstatedir=", av[i], 14) == 0))
		{
			tmp_dir = &av[i][14];
			found_tmpdir = 1;

			plug_verbose(sp, 2, "plugstack found temporal files in path '%s'", tmp_dir);
			plug_env_setenv(sp, "PLG_PATH_TMP", tmp_dir, 1);
		}
		if ((strlen(av[i]) > 7) && (strncmp ("prefix=", av[i], 7) == 0))
		{
			pre_dir = &av[i][7];
			found_predir = 1;

			plug_verbose(sp, 2, "plugstack found prefix in path '%s'", pre_dir);
			plug_env_setenv(sp, "PLG_PATH_PFX", pre_dir, 1);
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

int plug_env_readuser(spank_t sp)
{
	plug_verbose(sp, 2, "function plug_env_readuser");
	
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
	plug_env_setenv(sp, "EAR_USER", upw->pw_name, 1);
	plug_env_setenv(sp, "EAR_GROUP", gpw->gr_name, 1);

	plug_verbose(sp, 2, "user detected '%u -> %s'", uid, upw->pw_name);
	plug_verbose(sp, 2, "user group detected '%u -> %s'", gid, gpw->gr_name);
	plug_verbose(sp, 2, "user account detected '%s'", getenv("SLURM_JOB_ACCOUNT"));

	return (ESPANK_SUCCESS);
}

static int frequency_exists(ulong *freqs, int n_freqs, ulong freq)
{
	int i;

	for (i = 0; i < n_frequencies; ++i) {
		if (frequencies[i] == frequency) {
			return 1;
		}
	}

	return 0;
}

int plug_env_readapp(spank_t sp, application_t *app, ulong *freqs, int n_freqs)
{
	uint32_t aux_val;

	init_application(&eard_appl);

	// Gathering variables
	if (!plug_env_getenv(sp, ENV_LIB_EN, buffer1, SZ_NAME_SHORT)) {
		app.is_mpi = 0;
	} else {
		if (strcmp(buffer1,"0") == 0) {
			app.is_mpi = 0;
		} else {
			app.is_mpi = 1;
		}
	}
	if (spank_get_item (sp, S_JOB_ID, &aux_val) == ESPANK_SUCCESS) {
		app.job.id = aux_val;
	} else {
		app.job.id = NO_VAL;
	}
	if (spank_get_item (sp, S_JOB_STEPID, &aux_val) == ESPANK_SUCCESS) {
		app.job.step_id = aux_val;
	} else {
		app.job.step_id = NO_VAL;
	}
	if (!plug_env_getenv(sp, "SLURM_JOB_ACCOUNT", app.job.user_acc, SZ_NAME_SHORT)) {
		strcpy(app.job.user_acc, "");
	}
	if (!plug_env_getenv(sp, "EAR_USER", app.job.user_id, SZ_NAME_MEDIUM)) {
		strcpy(app.job.user_id, "");
	}
	if (!plug_env_getenv(sp, "EAR_GROUP", app.job.group_id, SZ_NAME_MEDIUM)) {
		strcpy(app.job.group_id, "");
	}
	if (!plug_env_getenv(sp, "SLURM_JOB_NAME",  app.job.app_id, SZ_NAME_MEDIUM)) {
		strcpy(app.job.app_id, "");
	}
	if (!plug_env_getenv(sp, "SLURM_JOB_ACCOUNT", app.job.user_acc, SZ_NAME_MEDIUM)) {
		strcpy(app.job.user_acc, "");
	}
	if (!plug_env_getenv(sp, "EAR_POWER_POLICY", app.job.policy, SZ_NAME_MEDIUM)) {
		strcpy(app.job.policy, "");
	}
	if (!plug_env_getenv(sp, "EAR_POWER_POLICY_TH", buffer1, SZ_NAME_SHORT)) {
		app.job.th = -1.0;
	} else {
		app.job.th = atof(buffer1);
	}
	if (!plug_env_getenv(sp, "EAR_FREQUENCY", buffer1, SZ_NAME_MEDIUM)) {
		app.job.def_f = 0;
	} else {
		app.job.def_f = (ulong) atol(buffer1);
		if (!frequency_exists(frqs, n_freqs, app.job.def_f)) {
			app.job.def_f = 0;
		}
	}
	if (!plug_env_getenv(sp, "EAR_LEARNING_PHASE", buffer1, SZ_NAME_MEDIUM)) {
		app.is_learning = 0;
	} else {
		app.is_learning = 1;
		app.job.def_f = frequencies[atoi(buffer1)];
	}
	if (!plug_env_getenv(sp, "EAR_ENERGY_TAG", app.job.energy_tag, 32)) {
		strcpy(app.job.energy_tag, "");
	}

	return 1;
}

int plug_env_readnodes(spank_t sp, hostlist_t nodes)
{
	if (plug_env_isenv(sp, ENV_PLG_CTX, "SBATCH"))
	{
		if (plug_env_isenv(sp, "EAR_MONITOR", "1"))
		{
			if (plug_env_getenv(sp, "SLURM_STEP_NODELIST", buffer1, SZ_PATH))  {
				nodes = slurm_hostlist_create(buffer1);
				return 1;
			}
		}
		if (gethostname(buffer1, SZ_NAME_MEDIUM) == 0) {
			nodes = slurm_hostlist_create(buffer1);
			return 1;
		}
	}
	return 0;
}

/*
 *
 *
 *
 *
 *
 */

int plug_env_setpreload(spank_t sp)
{
	plug_verbose(sp, 2, "function plug_env_setpreload");
	
	char *ear_root_dir = NULL;

	buffer1[0] = '\0';
	buffer2[0] = '\0';

	if (plug_env_getenv(sp, "PLG_PATH_PFX", &ear_root_dir) == 0)
	{
		plug_verbose(sp, 2, "Error, wrong environment for setting LD_PRELOAD");
		return ESPANK_ERROR;
	}
	apenv(buffer1, ear_root_dir, sizeof(buffer1));

	// Appending libraries to LD_PRELOAD
	if (plug_env_isenv(sp, "EAR_MPI_DIST", "openmpi")) {
		snprintf(buffer2, sizeof(buffer2), "%s/%s", buffer1, OMPI_C_LIB_PATH);
	} else {
		snprintf(buffer2, sizeof(buffer2), "%s/%s", buffer1, IMPI_C_LIB_PATH);
	}

	//
	plug_env_setenv(sp, "LD_PRELOAD", buffer2, 1);

	plug_verbose(sp, 2, "updated LD_PRELOAD envar '%s'", buffer2);

	return (ESPANK_SUCCESS);
}

int plug_env_setenviron(spank_t sp, settings_conf_t *setts)
{
	// Variable EAR_ENERGY_TAG, unset
	if (setts->user_type != ENERGY_TAG) {
		plug_env_unsetenv(sp, "EAR_ENERGY_TAG");
	}

	// Variable EAR_P_STATE
	snprintf(buffer2, 16, "%u", setts->def_p_state);
	plug_env_setenv(sp, "EAR_P_STATE", buffer2, 1);

	// Variable EAR_FREQUENCY
	snprintf(buffer2, 16, "%lu", setts->def_freq);
	plug_env_setenv(sp, "EAR_FREQUENCY", buffer2, 1);

	// Variable EAR_POWER_POLICY, overwrite
	if(policy_id_to_name(setts->policy, buffer2) == EAR_ERROR)
	{
		// Closing shared memory
		dettach_settings_conf_shared_area();
		plug_error(sp, "invalid policy returned");
		return (ESPANK_ERROR);
	}
	plug_env_setenv(sp, "EAR_POWER_POLICY", buffer2, 1);

	// Variable EAR_POWER_POLICY_TH, overwrite
	snprintf(buffer2, 16, "%0.2f", setts->th);
	plug_env_setenv(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer2, 1);
	plug_env_setenv(sp, "EAR_PERFORMANCE_PENALTY", buffer2, 1);

	// Variable EAR_LEARNING and EAR_P_STATE
	if(!setts->learning) {
		plug_env_unsetenv(sp, "EAR_P_STATE");
		plug_env_unsetenv(sp, "EAR_LEARNING_PHASE");
	}

	// Final library tweaks
	if (plug_env_getenv(sp, "SLURM_JOB_NAME", buffer2, sizeof(buffer2)) == 1) {
		plug_env_setenv(sp, "EAR_APP_NAME", buffer2, 1);
	}
	if (plug_env_getenv(sp, "PLG_PATH_TMP", buffer2, sizeof(buffer2)) == 1) {
		plug_env_setenv(sp, "EAR_TMP", buffer2, 1);
	}
}
