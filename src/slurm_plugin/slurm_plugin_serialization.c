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
#include <slurm_plugin/slurm_plugin_env.h>
#include <slurm_plugin/slurm_plugin_helper.h>

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
				if (!isenv_agnostic(sp, comp.library, "0")) {
					plug_comp_setenabled(sp, comp.library, 1);
				} 
			// If disabled by default or de administrator have misswritten
			} else {
				// Library == 1: nothing
				// Library == 0: disable
				// Library == whatever: disable (bug protection)
				if (!isenv_agnostic(sp, comp.library, "1")) {
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

static int frequency_exists(ulong *freqs, int n_freqs, ulong freq)
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
	if (!getenv_agnostic(sp, ENV_LIB_EN, buffer1, SZ_NAME_SHORT)) {
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
	if (!getenv_agnostic(sp, "EAR_USER", app->job.user_id, SZ_NAME_MEDIUM)) {
		strcpy(app->job.user_id, "");
	}
	if (!getenv_agnostic(sp, "EAR_GROUP", app->job.group_id, SZ_NAME_MEDIUM)) {
		strcpy(app->job.group_id, "");
	}
	if (!getenv_agnostic(sp, "SLURM_JOB_NAME", app->job.app_id, SZ_NAME_MEDIUM)) {
		strcpy(app->job.app_id, "");
	}
	if (!getenv_agnostic(sp, "SLURM_JOB_ACCOUNT", app->job.user_acc, SZ_NAME_MEDIUM)) {
		strcpy(app->job.user_acc, "");
	}
	if (!getenv_agnostic(sp, "EAR_POWER_POLICY", app->job.policy, SZ_NAME_MEDIUM)) {
		strcpy(app->job.policy, "");
	}
	if (!getenv_agnostic(sp, "EAR_POWER_POLICY_TH", buffer1, SZ_NAME_SHORT)) {
		app->job.th = -1.0;
	} else {
		app->job.th = atof(buffer1);
	}
	if (!getenv_agnostic(sp, "EAR_FREQUENCY", buffer1, SZ_NAME_MEDIUM)) {
		app->job.def_f = 0;
	} else {
		app->job.def_f = (ulong) atol(buffer1);
		if (!frequency_exists(freqs, n_freqs, app->job.def_f)) {
			app->job.def_f = 0;
		}
	}
	if (!getenv_agnostic(sp, "EAR_LEARNING_PHASE", buffer1, SZ_NAME_MEDIUM)) {
		app->is_learning = 0;
	} else {
		app->is_learning = 1;
		app->job.def_f = freqs[atoi(buffer1)];
	}
	if (!getenv_agnostic(sp, "EAR_ENERGY_TAG", app->job.energy_tag, 32)) {
		strcpy(app->job.energy_tag, "");
	}

	return ESPANK_SUCCESS;
}

int plug_env_readnodes(spank_t sp, plug_pack_t *pack, plug_job_t *job)
{
	if (job->context_local == cntx.sbatch)
	{
		if (plug_comp_isenabled(sp, comp.monitor))
		{
			if (getenv_agnostic(sp, "SLURM_STEP_NODELIST", buffer1, SZ_PATH))  {
				job->hostlist = slurm_hostlist_create(buffer1);
				return ESPANK_SUCCESS;
			}
		}
	}
	job->hostlist = slurm_hostlist_create(job->host);
	return ESPANK_SUCCESS;
}

/*
 *
 *
 * Serialize/deserialize
 *
 *
 */

int plug_deserialize_local(spank_t sp, plug_job_t *job)
{
	plug_verbose(sp, 2, "function plug_env_readuser");

	// Replacing PLUG_ variables to EAR_ with library consumes
	#define replace(name) \
		unsetenv_agnostic(sp, "EAR_" name); \
		repenv_agnostic  (sp, "PLUG_" name, "EAR_" name);

	// EAR options
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

	// User information
	uid_t uid = geteuid();
	gid_t gid = getgid();
	struct passwd *upw = getpwuid(uid);
	struct group *gpw = getgrgid(gid);

	if (upw == NULL || gpw == NULL) {
		plug_verbose(sp, 2, "converting UID/GID in username");
		return (ESPANK_ERROR);
	}

	strncpy(job->user.user, upw->pw_name, SZ_NAME_MEDIUM);
	strncpy(job->user.group, gpw->gr_name, SZ_NAME_MEDIUM);
	getenv_agnostic(sp, "SLURM_JOB_ACCOUNT", job->user.account, SZ_NAME_MEDIUM);

	plug_verbose(sp, 2, "user '%u' ('%s')", uid, job->user.user);
	plug_verbose(sp, 2, "user group '%u' ('%s')", gid, job->user.group);
	plug_verbose(sp, 2, "user account '%s'", job->user.account);

	// Reservation
	getenv_agnostic(sp, "SLURM_NNODES", buffer2, SZ_PATH);
	job->n_nodes = atoi(buffer2);

	// User environment variables
	getenv_agnostic(sp, "LD_PRELOAD", job->var_ld_preload, SZ_PATH);
	getenv_agnostic(sp, "LD_LIBRARY_PATH", job->var_ld_library, SZ_PATH);

	// Context
	job->context_local = spank_context();

	return ESPANK_SUCCESS;
}

int plug_serialize_remote(spank_t sp, plug_pack_t *pack, plug_job_t *job)
{
	// LD_PRELOAD
	apenv_agnostic(job->var_ld_preload, pack->path_inst, SZ_PATH);

	// Appending libraries to LD_PRELOAD
	if (isenv_agnostic(sp, "EAR_MPI_DIST", "openmpi")) {
		snprintf(buffer2, sizeof(buffer2), "%s/%s", job->var_ld_preload, OMPI_C_LIB_PATH);
	} else {
		snprintf(buffer2, sizeof(buffer2), "%s/%s", job->var_ld_preload, IMPI_C_LIB_PATH);
	}

	setenv_agnostic(sp, "LD_PRELOAD", buffer2, 1);
	plug_verbose(sp, 2, "updated LD_PRELOAD envar '%s'", buffer2);

	// Last local context
	if (job->context_local == S_CTX_SRUN) {
		setenv_agnostic(sp, "PLUG_CONTEXT", "SRUN", 1);
	}
	if (job->context_local == S_CTX_SBATCH) {
		setenv_agnostic(sp, "PLUG_CONTEXT", "SBATCH", 1);
	}

	// Paths
	setenv_agnostic(sp, "PLUG_PATH_TEMP", pack->path_temp, 1);
	setenv_agnostic(sp, "PLUG_PATH_INST", pack->path_inst, 1);

	// User things
	setenv_agnostic(sp,  "EAR_USER", job->user.user, 1);
	setenv_agnostic(sp, "EAR_GROUP", job->user.group, 1);

	return ESPANK_SUCCESS;
}

int plug_deserialize_remote(spank_t sp, plug_pack_t *pack, plug_job_t *job)
{
	// Local node
	gethostname(job->host, SZ_NAME_MEDIUM);

	// Local context
	if (isenv_agnostic(sp, "PLUG_CONTEXT", "SRUN")) {
		job->context_local = S_CTX_SRUN;
	}
	if (isenv_agnostic(sp, "PLUG_CONTEXT", "SBATCH")) {
		job->context_local = S_CTX_SBATCH;
	}

	// Paths
	getenv_agnostic(sp, "PLUG_PATH_TEMP", pack->path_temp, SZ_PATH);
	getenv_agnostic(sp, "PLUG_PATH_INST", pack->path_inst, SZ_PATH);

	// User things
	getenv_agnostic(sp,  "EAR_USER", job->user.user, SZ_NAME_MEDIUM);
	getenv_agnostic(sp, "EAR_GROUP", job->user.group, SZ_NAME_MEDIUM);

	return ESPANK_SUCCESS;
}

int plug_serialize_task(spank_t sp, plug_pack_t *pack, plug_job_t *job)
{
	settings_conf_t *setts = &pack->eard.setts;

	// Variable EAR_ENERGY_TAG, unset
	if (setts->user_type != ENERGY_TAG) {
		unsetenv_agnostic(sp, "EAR_ENERGY_TAG");
	}

	// Variable EAR_P_STATE
	snprintf(buffer2, 16, "%u", setts->def_p_state);
	setenv_agnostic(sp, "EAR_P_STATE", buffer2, 1);

	// Variable EAR_FREQUENCY
	snprintf(buffer2, 16, "%lu", setts->def_freq);
	setenv_agnostic(sp, "EAR_FREQUENCY", buffer2, 1);

	// Variable EAR_POWER_POLICY, overwrite
	if(!policy_id_to_name(setts->policy, buffer2) == EAR_ERROR) {
		setenv_agnostic(sp, "EAR_POWER_POLICY", buffer2, 1);
	}

	// Variable EAR_POWER_POLICY_TH, overwrite
	snprintf(buffer2, 16, "%0.2f", setts->th);
	setenv_agnostic(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer2, 1);
	setenv_agnostic(sp, "EAR_PERFORMANCE_PENALTY", buffer2, 1);

	// Variable EAR_LEARNING and EAR_P_STATE
	if(!setts->learning) {
		unsetenv_agnostic(sp, "EAR_P_STATE");
		unsetenv_agnostic(sp, "EAR_LEARNING_PHASE");
	}

	// Final library tweaks
	if (getenv_agnostic(sp, "SLURM_JOB_NAME", buffer2, sizeof(buffer2)) == 1) {
		setenv_agnostic(sp, "EAR_APP_NAME", buffer2, 1);
	}
	if (getenv_agnostic(sp, "PLUG_PATH_TEMP", buffer2, sizeof(buffer2)) == 1) {
		setenv_agnostic(sp, "EAR_TMP", buffer2, 1);
	}

	return ESPANK_SUCCESS;
}
