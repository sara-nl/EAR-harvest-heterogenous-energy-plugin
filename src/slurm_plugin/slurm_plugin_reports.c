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

#include <slurm.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <daemon/eard_rapi.h>
#include <daemon/shared_configuration.h>
#include <global_manager/eargm_rapi.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>

// EARD variables
char eard_host[SZ_NAME_MEDIUM];
unsigned int  eard_port;
unsigned int  eard_conn;
application_t eard_appl;
int           eard_exst;

// EARGMD variables
char eargmd_host[SZ_NAME_MEDIUM];
unsigned int eargmd_port;
unsigned int eargmd_nods;
unsigned int eargmd_enbl;
unsigned int eargmd_conn;

// Externs
extern char buffer1[SZ_PATH];
extern char buffer2[SZ_PATH];

int remote_read_shared_data_set_environment(spank_t sp)
{
	plug_verbose(sp, 2, "function remote_read_shared_data_set_environment");

	settings_conf_t *conf_sett = NULL;

	// Getting TMP path
	getenv_remote(sp, "PLG_PATH_TMP", buffer1, sizeof(buffer1));

	// Opening settings
	get_settings_conf_path(buffer1, buffer2);
	conf_sett = attach_settings_conf_shared_area(buffer2);

	if (conf_sett == NULL) {
		plug_error(sp, "while reading the shared configuration memory in node '%s'", eard_host);
		return ESPANK_ERROR;
	}

	if (verbosity_test(sp, 4)) {
		print_settings_conf(conf_sett);
	}

	// Variable EAR and LD_PRELOAD
	if (!conf_sett->lib_enabled || conf_sett->user_type == ENERGY_TAG)
	{
		dettach_settings_conf_shared_area();
		return (ESPANK_ERROR);
	}

	// Variable EAR_ENERGY_TAG, unset
	if (conf_sett->user_type != ENERGY_TAG) {
		unsetenv_remote(sp, "EAR_ENERGY_TAG");
	}

	// Variable EAR_P_STATE
	snprintf(buffer2, 16, "%u", conf_sett->def_p_state);
	setenv_remote(sp, "EAR_P_STATE", buffer2, 1);

	// Variable EAR_FREQUENCY
	snprintf(buffer2, 16, "%lu", conf_sett->def_freq);
	setenv_remote(sp, "EAR_FREQUENCY", buffer2, 1);

	// Variable EAR_POWER_POLICY, overwrite
	if(policy_id_to_name(conf_sett->policy, buffer2) == EAR_ERROR)
	{
		// Closing shared memory
		dettach_settings_conf_shared_area();
		plug_error(sp, "invalid policy returned");
		return (ESPANK_ERROR);
	}
	setenv_remote(sp, "EAR_POWER_POLICY", buffer2, 1);

	// Variable EAR_POWER_POLICY_TH, overwrite
	snprintf(buffer2, 8, "%0.2f", conf_sett->th);
	setenv_remote(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer2, 1);
	setenv_remote(sp, "EAR_PERFORMANCE_PENALTY", buffer2, 1);

	// Variable EAR_LEARNING and EAR_P_STATE
	if(!conf_sett->learning) {
		unsetenv_remote(sp, "EAR_P_STATE");
		unsetenv_remote(sp, "EAR_LEARNING_PHASE");
	}

	// Final library tweaks
	if (getenv_remote(sp, "SLURM_JOB_NAME", buffer2, sizeof(buffer2)) == 1) {
		setenv_remote(sp, "EAR_APP_NAME", buffer2, 1);
	}
	if (getenv_remote(sp, "PLG_PATH_TMP", buffer2, sizeof(buffer2)) == 1) {
		setenv_remote(sp, "EAR_TMP", buffer2, 1);
	}

	// Closing shared memory
	dettach_settings_conf_shared_area();

	return (ESPANK_SUCCESS);
}

static int frequency_exists(ulong *frequencies, int n_frequencies, ulong frequency)
{
	int i;

	for (i = 0; i < n_frequencies; ++i) {
		if (frequencies[i] == frequency) {
			return 1;
		}
	}

	return 0;
}

int remote_eard_report_start(spank_t sp)
{
	plug_verbose(sp, 2, "function remote_eard_report_start");

	services_conf_t *conf_serv = NULL;
	ulong *frequencies;
	int n_frequencies;
	uint32_t aux_val;

	// General variables
	getenv_remote(sp, "PLG_PATH_TMP", buffer1, sizeof(buffer1));
	gethostname(eard_host, SZ_NAME_MEDIUM);

	// Opening shared services memory
	get_services_conf_path(buffer1, buffer2);
	conf_serv = attach_services_conf_shared_area(buffer2);

	if (conf_serv == NULL) {
		plug_error(sp, "while reading the shared services memory in '%s'", eard_host);
		return ESPANK_ERROR;
	}

	// Getting EARD connection variables
	eard_port = conf_serv->eard.port;
	
	// Closing shared services memory
	dettach_services_conf_shared_area();

	// Shared memory reading for frequencies
	get_frequencies_path(buffer1, buffer2);
	frequencies = attach_frequencies_shared_area(buffer2, &n_frequencies);

	// Number of frequencies
	n_frequencies = n_frequencies / sizeof(ulong);

	// EARD report
	init_application(&eard_appl);

	// Gathering variables
	if (!getenv_remote(sp, "EAR_LIBRARY", buffer1, SZ_NAME_SHORT)) {
		eard_appl.is_mpi = 0;
	} else {
		if (strcmp(buffer1,"0") == 0) {
			eard_appl.is_mpi = 0;
		} else {
			eard_appl.is_mpi = 1;
		}
	}
        if (spank_get_item (sp, S_JOB_ID, &aux_val) == ESPANK_SUCCESS) {
                eard_appl.job.id = aux_val;
        } else {
                eard_appl.job.id = NO_VAL;
        }
	if (spank_get_item (sp, S_JOB_STEPID, &aux_val) == ESPANK_SUCCESS) {
		eard_appl.job.step_id = aux_val;
	} else {
		eard_appl.job.step_id = NO_VAL;
	}
	if (!getenv_remote(sp, "SLURM_JOB_ACCOUNT", eard_appl.job.user_acc, SZ_NAME_SHORT)) {
		strcpy(eard_appl.job.user_acc, "");
	}
	if (!getenv_remote(sp, "EAR_USER", eard_appl.job.user_id, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.user_id, "");
	}
	if (!getenv_remote(sp, "EAR_GROUP", eard_appl.job.group_id, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.group_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_NAME",  eard_appl.job.app_id, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.app_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_ACCOUNT", eard_appl.job.user_acc, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.user_acc, "");
	}
	if (!getenv_remote(sp, "EAR_POWER_POLICY", eard_appl.job.policy, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.policy, "");
	}
	if (!getenv_remote(sp, "EAR_POWER_POLICY_TH", buffer1, SZ_NAME_SHORT)) {
		eard_appl.job.th = -1.0;
	} else {
		eard_appl.job.th = atof(buffer1);
	}
	if (!getenv_remote(sp, "EAR_FREQUENCY", buffer1, SZ_NAME_MEDIUM)) {
		eard_appl.job.def_f = 0;
	} else {
		eard_appl.job.def_f = (ulong) atol(buffer1);
		if (!frequency_exists(frequencies, n_frequencies, eard_appl.job.def_f)) {
			eard_appl.job.def_f = 0;
		}
	}
	if (!getenv_remote(sp, "EAR_LEARNING_PHASE", buffer1, SZ_NAME_MEDIUM)) {
		eard_appl.is_learning = 0;
	} else {
		eard_appl.is_learning = 1;
		eard_appl.job.def_f = frequencies[atoi(buffer1)];
	}
	if (!getenv_remote(sp, "EAR_ENERGY_TAG", eard_appl.job.energy_tag, 32)) {
		strcpy(eard_appl.job.energy_tag, "");
	}

	/*
 	 *
 	 */

	dettach_frequencies_shared_area();
	
	/*
	 *
	 */

	hostlist_t hostlist = NULL;
	int local_connect = 1;
	char *host;

	plug_verbose(sp, 2, "trying to connect EARD with host '%s' and port '%u'", eard_host, eard_port);
	eard_conn = 1;
	
	if (isenv_remote(sp, "PLG_LST_CTX", "SBATCH"))
	{
		if(getenv_remote(sp, "SLURM_STEP_NODELIST", buffer1, SZ_PATH))
		{
			hostlist = slurm_hostlist_create(buffer1);
			local_connect = 0;
			
			while ((host = slurm_hostlist_shift(hostlist)) != NULL)
			{
				plug_verbose(sp, 2, "remote connect to host: '%s:%d'", host, eard_port);
				eards_remote_connect(host, eard_port);
				eards_new_job(&eard_appl);
				eards_remote_disconnect();
				free(host);
			}
		}
	}
	
	if (local_connect)
	{
		plug_verbose(sp, 2, "remote (local) connect to host: '%s:%d'", eard_host, eard_port);
		eards_remote_connect(eard_host, eard_port);
		eards_new_job(&eard_appl);
		eards_remote_disconnect();
	}
	
	return ESPANK_SUCCESS;
}

int remote_eard_report_finish(spank_t sp)
{
	plug_verbose(sp, 2, "function remote_eard_report_finish");

	// Not connected before, so doesn't need to be finished
	if (!eard_conn) {
		return (ESPANK_SUCCESS);
	}

	// Verbosity
	plug_verbose(sp, 2, "trying to connect EARD with host '%s' and port '%u'", eard_host, eard_port);

	if (eards_remote_connect(eard_host, eard_port) < 0) {
		plug_error(sp, "while connecting with EAR daemon");
	}

	// New value (eard_exst)
	eards_end_job(eard_appl.job.id, eard_appl.job.step_id);

	//
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int local_eargmd_report_start(spank_t sp)
{
	plug_verbose(sp, 2, "function local_eargmd_report_start");
	char *c_eargmd_nods;

	// Not enabled
	if (!eargmd_enbl || isenv_local("EARGMD_CONNECTED", "1")) {
		return ESPANK_SUCCESS;
	}

	// Gathering variables
	getenv_local("SLURM_NNODES", &c_eargmd_nods);
	eargmd_nods = atoi(c_eargmd_nods);

	// Verbosity
	plug_verbose(sp, 2, "trying to connect EARGMD with host '%s', port '%u', and nnodes '%u'",
				 eargmd_host, eargmd_port, eargmd_nods);

	// Connection
	if (eargm_connect(eargmd_host, eargmd_port) < 0) {
		plug_error(sp, "while connecting with EAR global manager daemon");
		return ESPANK_ERROR;
	}
	if (!eargm_new_job(eargmd_nods)) {
		plug_error(sp, "while speaking with EAR global manager daemon");
	}
	eargm_disconnect();

	// Informing that this report has to be finished
	eargmd_conn = 1;

	// Informing that no nested process has to connect to EARGMD again
	setenv_local("EARGMD_CONNECTED", "1", 1);

	return (ESPANK_SUCCESS);
}

int local_eargmd_report_finish(spank_t sp)
{
	plug_verbose(sp, 2, "function local_eargmd_report_finish");

	// Not connected, so doesn't need to be finished
	if (!eargmd_conn) {
		return ESPANK_SUCCESS;
	} else {
		// Disabling protection
		setenv_local("EARGMD_CONNECTED", "0", 1);
	}

	if (eargm_connect(eargmd_host, eargmd_port) < 0) {
		plug_error(sp, "while connecting with EAR global manager daemon");
		return ESPANK_ERROR;
	}
	if (!eargm_end_job(eargmd_nods)) {
		plug_error(sp, "while speaking with EAR global manager daemon");
		return ESPANK_ERROR;
	}
	eargm_disconnect();

	return (ESPANK_SUCCESS);
}
