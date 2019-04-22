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
#include <slurm_plugin/slurm_plugin_environment.h>
#include <slurm_plugin/slurm_plugin_serialization.h>

// Buffers
static char buffer[SZ_PATH];

/*
 *
 *
 *
 *
 *
 */

int plug_read_plugstack(spank_t sp, int ac, char **av, plug_serialization_t *sd)
{
	plug_verbose(sp, 2, "function plug_read_plugstack");

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
				if (!isenv_agnostic(sp, Component.library, "0")) {
					plug_component_setenabled(sp, Component.library, 1);
				}
			// If disabled by default or de administrator have misswritten
			} else {
				// Library == 1: nothing
				// Library == 0: disable
				// Library == whatever: disable (bug protection)
				if (!isenv_agnostic(sp, Component.library, "1")) {
					plug_component_setenabled(sp, Component.library, 0);
				}
			}
		}
		if ((strlen(av[i]) > 14) && (strncmp ("localstatedir=", av[i], 14) == 0))
		{
			strncpy(sd->pack.path_temp, &av[i][14], SZ_NAME_MEDIUM);
			found_path_temp = 1;

			plug_verbose(sp, 2, "plugstack found temporal files in path '%s'", sd->pack.path_temp);
		}
		if ((strlen(av[i]) > 7) && (strncmp ("prefix=", av[i], 7) == 0))
		{
			strncpy(sd->pack.path_inst, &av[i][7], SZ_NAME_MEDIUM);
			found_path_inst = 1;

			plug_verbose(sp, 2, "plugstack found prefix in path '%s'", sd->pack.path_inst);
		}
		if ((strlen(av[i]) > 12) && (strncmp ("eargmd_host=", av[i], 12) == 0))
		{
			strncpy(sd->pack.eargmd.host, &av[i][12], SZ_NAME_MEDIUM);
			found_eargmd_host = 1;
		}
		if ((strlen(av[i]) > 12) && (strncmp ("eargmd_port=", av[i], 12) == 0))
		{
			sd->pack.eargmd.port = atoi(&av[i][12]);
			found_earmgd_port = 1;
		}
	}

	// EARGMD enabled?
	sd->pack.eargmd.enabled = found_eargmd_host && found_earmgd_port;

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

int plug_read_application(spank_t sp, plug_serialization_t *sd)
{
	application_t *app = &sd->job.app;
	ulong *freqs = sd->pack.eard.freqs.freqs;
	int n_freqs = sd->pack.eard.freqs.n_freqs;
	uint32_t item;

	init_application(app);

	// Gathering variables
	app->is_mpi = plug_component_isenabled(sp, Component.library);

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
	if (!getenv_agnostic(sp, "EAR_POWER_POLICY_TH", buffer, SZ_NAME_SHORT)) {
		app->job.th = -1.0;
	} else {
		app->job.th = atof(buffer);
	}
	if (!getenv_agnostic(sp, "EAR_FREQUENCY", buffer, SZ_NAME_MEDIUM)) {
		app->job.def_f = 0;
	} else {
		app->job.def_f = (ulong) atol(buffer);
		if (!frequency_exists(freqs, n_freqs, app->job.def_f)) {
			app->job.def_f = 0;
		}
	}
	if (!getenv_agnostic(sp, "EAR_LEARNING_PHASE", buffer, SZ_NAME_MEDIUM)) {
		app->is_learning = 0;
	} else {
		app->is_learning = 1;
		app->job.def_f = freqs[atoi(buffer)];
	}
	if (!getenv_agnostic(sp, "EAR_ENERGY_TAG", app->job.energy_tag, 32)) {
		strcpy(app->job.energy_tag, "");
	}

	return ESPANK_SUCCESS;
}

int plug_read_hostlist(spank_t sp, plug_serialization_t *sd)
{
	if (sd->subject.context_local == Context.sbatch)
	{
		if (plug_component_isenabled(sp, Component.monitor))
		{
			if (getenv_agnostic(sp, "SLURM_STEP_NODELIST", buffer, SZ_PATH))  {
				sd->pack.eard.hostlist = slurm_hostlist_create(buffer);
				return ESPANK_SUCCESS;
			}
		}
	}
	sd->pack.eard.hostlist = slurm_hostlist_create(sd->subject.host);
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

	/*
 	 * EAR variables
 	 */
	#define replace(name) \
		unsetenv_agnostic(sp, "EAR_" name); \
		repenv_agnostic  (sp, "PLUG_" name, "EAR_" name);

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

	/*
	 * User information
	 */
	uid_t uid = geteuid();
	gid_t gid = getgid();
	struct passwd *upw = getpwuid(uid);
	struct group  *gpw = getgrgid(gid);

	if (upw == NULL || gpw == NULL) {
		plug_verbose(sp, 2, "converting UID/GID in username");
		return (ESPANK_ERROR);
	}

	strncpy(sd->job.user.user, upw->pw_name, SZ_NAME_MEDIUM);
	strncpy(sd->job.user.group, gpw->gr_name, SZ_NAME_MEDIUM);

	getenv_agnostic(sp, "SLURM_JOB_ACCOUNT", sd->job.user.account, SZ_NAME_MEDIUM);
	getenv_agnostic(sp, "LD_LIBRARY_PATH", sd->job.ld_library, SZ_PATH);
	getenv_agnostic(sp, "LD_PRELOAD", sd->job.ld_preload, SZ_PATH);

	plug_verbose(sp, 2, "user '%u' ('%s')", uid, sd->job.user.user);
	plug_verbose(sp, 2, "user group '%u' ('%s')", gid, sd->job.user.group);
	plug_verbose(sp, 2, "user account '%s'", sd->job.user.account);

	/*
	 * Job
	 */
	getenv_agnostic(sp, "SLURM_NNODES", buffer, SZ_PATH);
	sd->job.n_nodes = atoi(buffer);

	return ESPANK_SUCCESS;
}

int plug_serialize_remote(spank_t sp, plug_serialization_t *sd)
{
	/*
	 * User
	 */
	setenv_agnostic(sp, "EAR_USER",  sd->job.user.user,  1);
	setenv_agnostic(sp, "EAR_GROUP", sd->job.user.group, 1);
	setenv_agnostic(sp, "PLUG_PATH_TEMP", sd->pack.path_temp, 1);
	setenv_agnostic(sp, "PLUG_PATH_INST", sd->pack.path_inst, 1);

	/*
	 * Subject
	 */
	if (plug_context_is(sp, Context.srun)) {
		setenv_agnostic(sp, "PLUG_CONTEXT", "SRUN", 1);
	} else if (plug_context_is(sp, Context.sbatch)) {
		setenv_agnostic(sp, "PLUG_CONTEXT", "SBATCH", 1);
	}

	return ESPANK_SUCCESS;
}

int plug_deserialize_remote(spank_t sp, plug_serialization_t *sd)
{
	/*
	 * User
	 */
	getenv_agnostic(sp, "EAR_USER",  sd->job.user.user,  SZ_NAME_MEDIUM);
	getenv_agnostic(sp, "EAR_GROUP", sd->job.user.group, SZ_NAME_MEDIUM);
	getenv_agnostic(sp, "PLUG_PATH_TEMP", sd->pack.path_temp, SZ_PATH);
	getenv_agnostic(sp, "PLUG_PATH_INST", sd->pack.path_inst, SZ_PATH);

	/*
 	 * Subject
 	 */
	gethostname(sd->subject.host, SZ_NAME_MEDIUM);

	if (isenv_agnostic(sp, "PLUG_CONTEXT", "SRUN")) {
		sd->subject.context_local = Context.srun;
	} else if (isenv_agnostic(sp, "PLUG_CONTEXT", "SBATCH")) {
		sd->subject.context_local = Context.sbatch;
	}

	return ESPANK_SUCCESS;
}

int plug_serialize_task(spank_t sp, plug_serialization_t *sd)
{
	/*
  	 * EAR variables
  	 */
	settings_conf_t *setts = &sd->pack.eard.setts;

	// Variable EAR_ENERGY_TAG, unset
	if (setts->user_type != ENERGY_TAG) {
		unsetenv_agnostic(sp, "EAR_ENERGY_TAG");
	}

	snprintf(buffer, 16, "%u", setts->def_p_state);
	setenv_agnostic(sp, "EAR_P_STATE", buffer, 1);

	snprintf(buffer, 16, "%lu", setts->def_freq);
	setenv_agnostic(sp, "EAR_FREQUENCY", buffer, 1);

	if(!policy_id_to_name(setts->policy, buffer) == EAR_ERROR) {
		setenv_agnostic(sp, "EAR_POWER_POLICY", buffer, 1);
	}

	snprintf(buffer, 16, "%0.2f", setts->th);
	setenv_agnostic(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer, 1);
	setenv_agnostic(sp, "EAR_PERFORMANCE_PENALTY", buffer, 1);

	if(!setts->learning) {
		unsetenv_agnostic(sp, "EAR_P_STATE");
		unsetenv_agnostic(sp, "EAR_LEARNING_PHASE");
	}

	if (getenv_agnostic(sp, "SLURM_JOB_NAME", buffer, sizeof(buffer)) == 1) {
		setenv_agnostic(sp, "EAR_APP_NAME", buffer, 1);
	}
	if (getenv_agnostic(sp, "PLUG_PATH_TEMP", buffer, sizeof(buffer)) == 1) {
		setenv_agnostic(sp, "EAR_TMP", buffer, 1);
	}

	/*
	 * LD_PRELOAD
	 */
	getenv_agnostic(sp, "LD_PRELOAD", sd->job.user.env.ld_preload, SZ_PATH);
	apenv_agnostic(sd->job.ld_preload, sd->pack.path_inst, SZ_PATH);

	// Appending libraries to LD_PRELOAD
	if (isenv_agnostic(sp, "EAR_MPI_DIST", "openmpi")) {
		snprintf(buffer, SZ_PATH, "%s/%s", sd->job.ld_preload, OMPI_C_LIB_PATH);
	} else {
		snprintf(buffer, SZ_PATH, "%s/%s", sd->job.ld_preload, IMPI_C_LIB_PATH);
	}

	setenv_agnostic(sp, "LD_PRELOAD", buffer, 1);
	plug_verbose(sp, 2, "updated LD_PRELOAD envar '%s'", buffer);

	return ESPANK_SUCCESS;
}
