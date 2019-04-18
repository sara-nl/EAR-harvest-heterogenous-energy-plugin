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

#include <pwd.h>
#include <grp.h>
#include <slurm_plugin/slurm_plugin.h>

// Buffers
static char buffer1[SZ_PATH];
static char buffer2[SZ_PATH];
static char buffer3[SZ_PATH];

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
	if (plug_env_islocal(sp)) {
		return unsetenv_local(var);
	} else {
		return unsetenv_remote(sp, var);
	}
}

int plug_env_setenv(spank_t sp, char *var, char *val, int ow)
{
	if (plug_env_islocal(sp)) {
		return setenv_local(var, val, ow);
	} else {
		return setenv_remote(sp, var, val, ow);
	}
}

int plug_env_getenv(spank_t sp, char *var, char *buf, int len)
{
	if (plug_env_islocal(sp)) {
		return getenv_local(var, buf, len);
	} else {
		return getenv_remote(sp, var, buf, len);
	}
}

int plug_env_isenv(spank_t sp, char *var, char *val)
{
	if (plug_env_islocal(sp)) {
		return isenv_local(var, val);
	} else {
		return isenv_remote(sp, var, val);
	}
}

int plug_env_repenv(spank_t sp, char *var_old, char *var_new)
{
	if (plug_env_islocal(sp)) {
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

int plug_comp_setenabled(spank_t sp, plug_comp_t comp, int enabled)
{
	if (enabled) return plug_env_setenv(sp, comp, "1", 1);
	             return plug_env_setenv(sp, comp, "0", 1);
}

int plug_comp_isenabled(spank_t sp, plug_comp_t comp)
{
	return plug_env_isenv(sp, comp, "1");
}

/*
 *
 *
 *
 *
 *
 */

int plug_verb_test(spank_t sp, int level)
{
	static int verbosity = -1;
	char env_remote[8];
	char *env_local;

	if (verbosity == -1)
	{
		if (plug_env_getenv(sp, "EAR_PLUGIN_VERBOSE", env_remote, 8) == 1) {
			verbosity = atoi(env_remote);
		} else {
			verbosity = 0;
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

int plug_env_readvars(spank_t sp)
{
	#define replace(name) \
		plug_env_unsetenv(sp, "EAR_" name); \
		plug_env_repenv  (sp, "PLG_" name, "EAR_" name);

	// Replacing options
	replace("LEARNING_PHASE");
	replace("POWER_POLICY");
	replace("POWER_POLICY_TH");
	replace("P_STATE");
	replace("FREQUENCY");
	replace("MIN_PERFORMANCE_EFFICIENCY_GAIN");
	replace("PERFORMANCE_PENALTY");
	replace("TRACE_PATH");
	//replace("MPI_DIST");
	replace("USER_DB_PATHNAME");
	replace("ENERGY_TAG");
	replace("APP_NAME");
	replace("TMP");

	return EAR_SUCCESS;
}

int plug_env_readjob(spank_t sp, plug_job_t *job)
{
	plug_verbose(sp, 2, "function plug_env_readuser");

	// User information
	uid_t uid = geteuid();
	gid_t gid = getgid();
	struct passwd *upw = getpwuid(uid);
	struct group *gpw = getgrgid(gid);

	if (upw == NULL) {
		plug_verbose(sp, 2, "converting UID in username");
		return (ESPANK_ERROR);
	}

	if (gpw == NULL) {
		plug_verbose(sp, 2, "converting GID in groupname");
		return (ESPANK_ERROR);
	}

	strcpy(job->user.name_upw, upw->pw_name, SZ_NAME_MEDIUM);
	strcpy(job->user.name_gpw, gpw->gr_name, SZ_NAME_MEDIUM);
	plug_env_getenv(sp, "SLURM_JOB_ACCOUNT", job->user.name_acc, SZ_NAME_MEDIUM);

	plug_verbose(sp, 2, "user '%u' ('%s')", job->user.uid, job->user.name_upw);
	plug_verbose(sp, 2, "user group '%u' ('%s')", job->user.gid, job->user.name_gpw);
	plug_verbose(sp, 2, "user account '%s'", job->user.name_acc);

	// Reservation
	plug_env_getenv(sp, "SLURM_NNODES", buffer, SZ_PATH);
	job->n_nodes = atoi(buffer);

	// User environment variables
	plug_env_getenv(sp, "LD_PRELOAD", job->var_ld_preload, SZ_PATH);
	plug_env_getenv(sp, "LD_LIBRARY_PATH", job->var_ld_library, SZ_PATH);

	// Context
	job->local_context = spank_context();

	return ESPANK_SUCCESS;
}

int plug_env_readstack(spank_t sp, int ac, char **av, plug_pack_t *pack)
{
	plug_verbose(sp, 2, "function plug_env_readstack");

	int found_earmgd_port = 0;
	int found_eargmd_host = 0;
	int found_path_inst = 0;
	int found_path_temp = 0;
	int i;

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 8) && (strncmp ("default=", av[i], 8) == 0))
		{
			plug_verbose(sp, 2, "plugstack found library by default '%s'", &av[i][8]);
			
			// If enabled by default
			if (strncmp ("default=on", av[i], 10) == 0) {
				// Library == 1: enable
				// Library == 0: nothing
				// Library == whatever: enable (bug protection)
				if (!plug_env_isenv(sp, comp.library, "0")) {
					plug_comp_setenabled(sp, comp.library, 1);
				} 
			// If disabled by default or de administrator have misswritten
			} else {
				// Library == 1: nothing
				// Library == 0: disable
				// Library == whatever: disable (bug protection)
				if (!plug_env_isenv(sp, comp.library, "1")) {
					plug_comp_setenabled(sp, comp.library, 0);
				}
			}
		}
		if ((strlen(av[i]) > 14) && (strncmp ("localstatedir=", av[i], 14) == 0))
		{
			strncpy(pack->path_temp, &av[i][14], SZ_NAME_MEDIUM);
			found_path_temp = 1;

			plug_verbose(sp, 2, "plugstack found temporal files in path '%s'", pack->path_temp);
		}
		if ((strlen(av[i]) > 7) && (strncmp ("prefix=", av[i], 7) == 0))
		{
			strncpy(pack->path_inst, &av[i][7], SZ_NAME_MEDIUM);
			found_path_inst = 1;

			plug_verbose(sp, 2, "plugstack found prefix in path '%s'", pack->path_inst);
		}
		if ((strlen(av[i]) > 12) && (strncmp ("eargmd_host=", av[i], 12) == 0))
		{
			strncpy(pack->eargmd.host, &av[i][12], SZ_NAME_MEDIUM);
			found_eargmd_host = 1;
		}
		if ((strlen(av[i]) > 12) && (strncmp ("eargmd_port=", av[i], 12) == 0))
		{
			pack->eargmd.port = atoi(&av[i][12]);
			found_earmgd_port = 1;
		}
	}

	// EARGMD enabled?
	pack->eargmd.enabled = found_eargmd_host && found_earmgd_port;

	// TMP folder missing?
	if (!found_path_temp) {
		plug_verbose(sp, 2, "missing plugstack localstatedir directory");
		return (ESPANK_STOP);
	}

	// Prefix folder missing?
	if (!found_path_inst) {
		plug_verbose(sp, 2, "missing plugstack prefix directory");
		return (ESPANK_ERROR);
	}

	return ESPANK_SUCCESS;
}

static int frequency_exists(plug_freqs_t *freqs, ulong freq)
{
	int i;
	for (i = 0; i < n_freqs; ++i) {
		if (freqs[i] == freq) {
			return 1;
		}
	}
	return 0;
}

int plug_env_readapp(spank_t sp, plug_pack_t *pack, plug_job_t *job)
{
	application_t *app = &job->app;
	ulong *freqs = pack->eard.freqs.freqs;
	int n_freqs = pack->eard.freqs.n_freqs;
	uint32_t item;

	init_application(app);

	// Gathering variables
	if (!plug_env_getenv(sp, ENV_LIB_EN, buffer1, SZ_NAME_SHORT)) {
		app->is_mpi = 0;
	} else {
		if (strcmp(buffer1,"0") == 0) {
			app->is_mpi = 0;
		} else {
			app->is_mpi = 1;
		}
	}
	if (spank_get_item (sp, S_JOB_ID, &item) == ESPANK_SUCCESS) {
		app->job.id = item;
	} else {
		app->job.id = NO_VAL;
	}
	if (spank_get_item (sp, S_JOB_STEPID, &item) == ESPANK_SUCCESS) {
		app->job.step_id = item;
	} else {
		app->job.step_id = NO_VAL;
	}
	if (!plug_env_getenv(sp, "SLURM_JOB_ACCOUNT", app->job.user_acc, SZ_NAME_SHORT)) {
		strcpy(app->job.user_acc, "");
	}
	if (!plug_env_getenv(sp, "EAR_USER", app->job.user_id, SZ_NAME_MEDIUM)) {
		strcpy(app->job.user_id, "");
	}
	if (!plug_env_getenv(sp, "EAR_GROUP", app->job.group_id, SZ_NAME_MEDIUM)) {
		strcpy(app->job.group_id, "");
	}
	if (!plug_env_getenv(sp, "SLURM_JOB_NAME", app->job.app_id, SZ_NAME_MEDIUM)) {
		strcpy(app->job.app_id, "");
	}
	if (!plug_env_getenv(sp, "SLURM_JOB_ACCOUNT", app->job.user_acc, SZ_NAME_MEDIUM)) {
		strcpy(app->job.user_acc, "");
	}
	if (!plug_env_getenv(sp, "EAR_POWER_POLICY", app->job.policy, SZ_NAME_MEDIUM)) {
		strcpy(app->job.policy, "");
	}
	if (!plug_env_getenv(sp, "EAR_POWER_POLICY_TH", buffer1, SZ_NAME_SHORT)) {
		app->job.th = -1.0;
	} else {
		app->job.th = atof(buffer1);
	}
	if (!plug_env_getenv(sp, "EAR_FREQUENCY", buffer1, SZ_NAME_MEDIUM)) {
		app->job.def_f = 0;
	} else {
		app->job.def_f = (ulong) atol(buffer1);
		if (!frequency_exists(freqs, n_freqs, app->job.def_f)) {
			app->job.def_f = 0;
		}
	}
	if (!plug_env_getenv(sp, "EAR_LEARNING_PHASE", buffer1, SZ_NAME_MEDIUM)) {
		app->is_learning = 0;
	} else {
		app->is_learning = 1;
		app->job.def_f = freqs[atoi(buffer1)];
	}
	if (!plug_env_getenv(sp, "EAR_ENERGY_TAG", app->job.energy_tag, 32)) {
		strcpy(app->job.energy_tag, "");
	}

	return ESPANK_SUCCESS;
}

int plug_env_readnodes(spank_t sp, plug_pack_t *pack)
{
	if (job->local_context = S_CTX_SBATCH)
	{
		if (plug_comp_isenabled(sp, comp.monitor))
		{
			if (plug_env_getenv(sp, "SLURM_STEP_NODELIST", buffer1, SZ_PATH))  {
				nodes = slurm_hostlist_create(buffer1);
				return ESPANK_SUCCESS;
			}
		}
	}
	nodes = slurm_hostlist_create(pack->eard.host);
	return ESPANK_SUCCESS;
}

/*
 *
 *
 * Serialize/deserialize
 *
 *
 */

int plug_env_serialize_remote(spank_t sp, plug_job_t *job)
{
	// LD_PRELOAD
	apenv(job->var_ld_preload, pack->path_inst, SZ_PATH);

	// Appending libraries to LD_PRELOAD
	if (plug_env_isenv(sp, "EAR_MPI_DIST", "openmpi")) {
		snprintf(buffer2, sizeof(buffer2), "%s/%s", job->var_ld_preload, OMPI_C_LIB_PATH);
	} else {
		snprintf(buffer2, sizeof(buffer2), "%s/%s", job->var_ld_preload, IMPI_C_LIB_PATH);
	}

	plug_env_setenv(sp, "LD_PRELOAD", buffer2, 1);
	plug_verbose(sp, 2, "updated LD_PRELOAD envar '%s'", buffer2);

	// Last local context
	if (job->local_context == S_CTX_SRUN) {
		plug_env_setenv(sp, "PLUG_CONTEXT", "SRUN", 1);
	}
	if (job->local_context == S_CTX_SBATCH) {
		plug_env_setenv(sp, "PLUG_CONTEXT", "SBATCH", 1);
	}

	// Paths
	plug_env_setenv(sp, "PLUG_PATH_TEMP", pack->path_temp, 1);
	plug_env_setenv(sp, "PLUG_PATH_INST", pack->path_inst, 1);

	// User things
	plug_env_setenv(sp,  "EAR_USER", job.user->name_upw, 1);
	plug_env_setenv(sp, "EAR_GROUP", job.user->name_gpw, 1);

	return ESPANK_SUCCESS;
}

int plug_env_deserialize_remote(spank_t sp, plug_job_t *job)
{
	// Local node
	gethostname(pack->eard.host, SZ_NAME_MEDIUM);

	// Local context
	if (plug_env_isenv(sp, "PLUG_CONTEXT", "SRUN")) {
		job->local_context = S_CTX_SRUN;
	}
	if (plug_env_isenv(sp, "PLUG_CONTEXT", "SBATCH")) {
		job->local_context = S_CTX_SBATCH;
	}

	// Paths
	plug_env_getenv(sp, "PLUG_PATH_TEMP", pack->path_temp);
	plug_env_getenv(sp, "PLUG_PATH_INST", pack->path_inst);

	// User things
	plug_env_getenv(sp,  "EAR_USER", job.user->name_upw);
	plug_env_getenv(sp, "EAR_GROUP", job.user->name_gpw);

	return ESPANK_SUCCESS;
}

int plug_env_serialize_task()
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
	if(!policy_id_to_name(setts->policy, buffer2) == EAR_ERROR) {
		plug_env_setenv(sp, "EAR_POWER_POLICY", buffer2, 1);
	}

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
	if (plug_env_getenv(sp, "PLUG_PATH_TEMP", buffer2, sizeof(buffer2)) == 1) {
		plug_env_setenv(sp, "EAR_TMP", buffer2, 1);
	}

	return ESPANK_SUCCESS;
}