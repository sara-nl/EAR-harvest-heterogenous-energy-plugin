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
				if (!isenv_agnostic(sp, Var.comp_libr.cmp, "0")) {
					plug_component_setenabled(sp, Component.library, 1);
				}
			// If disabled by default or de administrator have misswritten
			} else {
				// Library == 1: nothing
				// Library == 0: disable
				// Library == whatever: disable (bug protection)
				if (!isenv_agnostic(sp, Var.comp_libr.cmp, "1")) {
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

static int frequency_exists(spank_t sp, ulong *freqs, int n_freqs, ulong freq)
{
	int i;
	
	plug_verbose(sp, 3, "number of frequencies %d (looking for %lu)", n_freqs, freq);

	for (i = 0; i < n_freqs; ++i) {
		plug_verbose(sp, 3, "freq #%d: %lu", i, freqs[i]);
		if (freqs[i] == freq) {
			return 1;
		}
	}
	return 0;
}

int plug_read_application(spank_t sp, plug_serialization_t *sd)
{
	plug_verbose(sp, 2, "function plug_read_application");
	
	application_t *app = &sd->job.app;
	ulong *freqs = sd->pack.eard.freqs.freqs;
	int n_freqs = sd->pack.eard.freqs.n_freqs;
	uint32_t item;

	init_application(app);

	// Gathering variables
	app->is_mpi = plug_component_isenabled(sp, Component.library);
	
	strcpy(app->job.user_id, sd->job.user.user);
	strcpy(app->job.group_id, sd->job.user.group);
	strcpy(app->job.user_acc, sd->job.user.account);
	
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
	if (!getenv_agnostic(sp, Var.name_app.rem, app->job.app_id, SZ_NAME_MEDIUM)) {
		strcpy(app->job.app_id, "");
	}
	if (!getenv_agnostic(sp, Var.policy.ear, app->job.policy, SZ_NAME_MEDIUM)) {
		strcpy(app->job.policy, "");
	}
	if (!getenv_agnostic(sp, Var.policy_th.ear, buffer, SZ_NAME_SHORT)) {
		app->job.th = -1.0;
	} else {
		app->job.th = atof(buffer);
	}
	if (!getenv_agnostic(sp, Var.frequency.ear, buffer, SZ_NAME_MEDIUM)) {
		app->job.def_f = 0;
	} else {
		app->job.def_f = (ulong) atol(buffer);
		if (!frequency_exists(sp, freqs, n_freqs, app->job.def_f)) {
			app->job.def_f = 0;
		}
	}
	if (!getenv_agnostic(sp, Var.learning.ear, buffer, SZ_NAME_MEDIUM)) {
		app->is_learning = 0;
	} else {
		if ((unsigned int) atoi(buffer) < n_freqs) {
			app->job.def_f = freqs[atoi(buffer)];
			app->is_learning = 1;
		}
	}
	if (!getenv_agnostic(sp, Var.tag.ear, app->job.energy_tag, 32)) {
		strcpy(app->job.energy_tag, "");
	}

	plug_verbose(sp, 3, "application summary");
	plug_verbose(sp, 3, "job/step/name '%lu'/'%lu'/'%s'", app->job.id, app->job.step_id, app->job.app_id);
	plug_verbose(sp, 3, "user/group/acc '%s'/'%s'/'%s'", app->job.user_id, app->job.group_id, app->job.user_acc);
	plug_verbose(sp, 3, "policy/th/freq '%s'/'%f'/'%lu'", app->job.policy, app->job.th, app->job.def_f);
	plug_verbose(sp, 3, "learning/tag '%lu'/'%s'", app->is_learning, app->job.energy_tag);

	return ESPANK_SUCCESS;
}

int plug_read_hostlist(spank_t sp, plug_serialization_t *sd)
{
	plug_verbose(sp, 2, "function plug_read_hostlist");
	
	if (sd->subject.context_local == Context.sbatch)
	{
		if (plug_component_isenabled(sp, Component.monitor))
		{
			if (getenv_agnostic(sp, Var.node_list.rem, buffer, SZ_PATH))  {
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

int plug_clean_components(spank_t sp)
{
	int test;

	plug_component_setenabled(sp, Component.plugin, 0);
	plug_component_setenabled(sp, Component.library, 0);
	plug_component_setenabled(sp, Component.monitor, 0);
	plug_component_setenabled(sp, Component.test, 0);

	/*
	 * Components
	 */
	if (valenv_agnostic(sp, Var.comp_test.cmp, &test)) {
			plug_component_setenabled(sp, Component.test, e);
	}
	if (test || !isenv_agnostic(sp, Var.comp_plug.cmp, "0")) {
			plug_component_setenabled(sp, Component.plugin, 1);
	}
	if (test || isenv_agnostic(sp, Var.comp_libr.cmp, "1")) {
			plug_component_setenabled(sp, Component.library, 1);
	}
	if (isenv_agnostic(sp, Var.comp_moni.cmp, "1")) {
			plug_component_setenabled(sp, Component.monitor, 1);
	}

	return ESPANK_SUCCESS;
}

int plug_deserialize_local(spank_t sp, plug_serialization_t *sd)
{
	plug_verbose(sp, 2, "function plug_deserialize_local");

	/*
	 * Components
	 */
	unsetenv_agnostic(sp, Var.verbose.ear);
	unsetenv_agnostic(sp, Var.policy.ear);
	unsetenv_agnostic(sp, Var.policy_th.ear);
	unsetenv_agnostic(sp, Var.perf_pen.ear);
	unsetenv_agnostic(sp, Var.eff_gain.ear);
	unsetenv_agnostic(sp, Var.frequency.ear);
	unsetenv_agnostic(sp, Var.p_state.ear);
	unsetenv_agnostic(sp, Var.learning.ear);
	unsetenv_agnostic(sp, Var.tag.ear);
	unsetenv_agnostic(sp, Var.path_usdb.ear);
	unsetenv_agnostic(sp, Var.path_trac.ear);
	unsetenv_agnostic(sp, Var.path_temp.ear);
	unsetenv_agnostic(sp, Var.name_app.ear);

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

	strncpy(sd->job.user.user,  upw->pw_name, SZ_NAME_MEDIUM);
	strncpy(sd->job.user.group, gpw->gr_name, SZ_NAME_MEDIUM);
	plug_verbose(sp, 2, "user '%u' ('%s')", uid, sd->job.user.user);
	plug_verbose(sp, 2, "user group '%u' ('%s')", gid, sd->job.user.group);

	/*
	 * Job
	 */
	getenv_agnostic(sp, Var.node_num.loc, buffer, SZ_PATH);
	sd->job.n_nodes = atoi(buffer);

	return ESPANK_SUCCESS;
}

int plug_serialize_remote(spank_t sp, plug_serialization_t *sd)
{
	plug_verbose(sp, 2, "function plug_serialize_remote");

	/*
	 * User
	 */
	setenv_agnostic(sp, Var.user.rem,  sd->job.user.user,  1);
	setenv_agnostic(sp, Var.group.rem, sd->job.user.group, 1);
	setenv_agnostic(sp, Var.path_temp.rem, sd->pack.path_temp, 1);
	setenv_agnostic(sp, Var.path_inst.rem, sd->pack.path_inst, 1);

	/*
	 * Subject
	 */

	if (plug_context_is(sp, Context.srun)) {
		setenv_agnostic(sp, Var.context.rem, "SRUN", 1);
	} else if (plug_context_is(sp, Context.sbatch)) {
		setenv_agnostic(sp, Var.context.rem, "SBATCH", 1);
	}

	return ESPANK_SUCCESS;
}

int plug_deserialize_remote(spank_t sp, plug_serialization_t *sd)
{
	plug_verbose(sp, 2, "function plug_deserialize_remote");

	/*
	 * Options
	 */
	repenv_agnostic(sp, Var.verbose.loc,   Var.verbose.ear);
	repenv_agnostic(sp, Var.policy.loc,    Var.policy.ear);
	repenv_agnostic(sp, Var.policy_th.loc, Var.policy_th.ear);
	repenv_agnostic(sp, Var.frequency.loc, Var.frequency.ear);
	repenv_agnostic(sp, Var.learning.loc,  Var.learning.ear);
	repenv_agnostic(sp, Var.tag.loc,       Var.tag.ear);
	repenv_agnostic(sp, Var.path_usdb.loc, Var.path_usdb.ear);
	repenv_agnostic(sp, Var.path_trac.loc, Var.path_trac.ear);
	repenv_agnostic(sp, Var.mpi_dist.loc,  Var.mpi_dist.rem);

	/*
	 * User
	 */
	getenv_agnostic(sp, Var.user.rem,  sd->job.user.user,  SZ_NAME_MEDIUM);
	getenv_agnostic(sp, Var.group.rem, sd->job.user.group, SZ_NAME_MEDIUM);
	getenv_agnostic(sp, Var.account.rem, sd->job.user.account, SZ_NAME_MEDIUM);
	getenv_agnostic(sp, Var.path_temp.rem, sd->pack.path_temp, SZ_PATH);
	getenv_agnostic(sp, Var.path_inst.rem, sd->pack.path_inst, SZ_PATH);

	/*
  	 * Subject
  	 */
	gethostname(sd->subject.host, SZ_NAME_MEDIUM);

	getenv_agnostic(sp, Var.mpi_dist.rem, buffer, SZ_PATH);
	sd->job.mpi = atoi(buffer);

	if (isenv_agnostic(sp, Var.context.rem, "SRUN")) {
		sd->subject.context_local = Context.srun;
	} else if (isenv_agnostic(sp, Var.context.rem, "SBATCH")) {
		sd->subject.context_local = Context.sbatch;
	}

	/*
	 * Clean
	 */
	unsetenv_agnostic(sp, Var.user.rem);
	unsetenv_agnostic(sp, Var.group.rem);
	unsetenv_agnostic(sp, Var.path_temp.rem);
	unsetenv_agnostic(sp, Var.path_inst.rem);
	unsetenv_agnostic(sp, Var.mpi_dist.rem);
	unsetenv_agnostic(sp, Var.context.rem);

	return ESPANK_SUCCESS;
}

int plug_serialize_task(spank_t sp, plug_serialization_t *sd)
{
	plug_verbose(sp, 2, "function plug_serialize_task");

	/*
  	 * EAR variables
  	 */
	settings_conf_t *setts = &sd->pack.eard.setts;

	// Variable EAR_ENERGY_TAG, unset
	if (setts->user_type != ENERGY_TAG) {
		unsetenv_agnostic(sp, Var.tag.ear);
	}

	snprintf(buffer, 16, "%u", setts->def_p_state);
	setenv_agnostic(sp, Var.p_state.ear, buffer, 1);

	snprintf(buffer, 16, "%lu", setts->def_freq);
	setenv_agnostic(sp, Var.frequency.ear, buffer, 1);

	if(policy_id_to_name(setts->policy, buffer) != EAR_ERROR) {
		setenv_agnostic(sp, Var.policy.ear, buffer, 1);
	}

	snprintf(buffer, 16, "%0.2f", setts->th);
	setenv_agnostic(sp, Var.eff_gain.ear, buffer, 1);
	setenv_agnostic(sp, Var.perf_pen.ear, buffer, 1);

	if(!setts->learning) {
		unsetenv_agnostic(sp, Var.p_state.ear);
		unsetenv_agnostic(sp, Var.learning.ear);
	}

	if (getenv_agnostic(sp, Var.name_app.rem, buffer, sizeof(buffer)) == 1) {
		setenv_agnostic(sp, Var.name_app.ear, buffer, 1);
	}
	if (getenv_agnostic(sp, Var.path_temp.rem, buffer, sizeof(buffer)) == 1) {
		setenv_agnostic(sp, Var.path_temp.ear, buffer, 1);
	}

	/*
	 * LD_PRELOAD
	 */
	apenv_agnostic(sd->job.user.env.ld_preload, sd->pack.path_inst, SZ_PATH);

	// Appending libraries to LD_PRELOAD
	if (sd->job.mpi) {
		snprintf(buffer, SZ_PATH, "%s/%s", sd->job.user.env.ld_preload, OMPI_C_LIB_PATH);
	} else {
		snprintf(buffer, SZ_PATH, "%s/%s", sd->job.user.env.ld_preload, IMPI_C_LIB_PATH);
	}

	setenv_agnostic(sp, Var.ld_prel.ear, buffer, 1);
	
	return ESPANK_SUCCESS;
}
