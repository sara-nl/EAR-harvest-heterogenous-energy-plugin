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

// EARD variables
uint eard_port;

// Externs
extern char buffer1[SZ_PATH];
extern char buffer2[SZ_PATH];
extern char buffer3[SZ_PATH];

static void plug_rcom_eard_xxx(spank_t sp, hostlist_t nodes, application_t *app, int new_job)
{
	char *node;

	while ((node = slurm_hostlist_shift(nodes)) != NULL)
	{
		plug_verbose(sp, 2, "remote connect to host: '%s:%d'", node, eard_port);

		if (eards_remote_connect(node, eard_port) < 0) {
			plug_error(sp, "while connecting with EAR daemon");
			continue;
		}

		if (new_job) {
			eards_new_job(app);
		} else {
			eards_end_job(app->job.id, app->job.step_id);
		}

		eards_remote_disconnect();
		free(node);
	}
}

int plug_rcom_eard_job_start(spank_t sp)
{
	plug_verbose(sp, 2, "function plug_rcom_eard_job_start");

	// Frequency vars
	hostlist_t nodes;
	settings_conf_t *setts;
	services_conf_t *servs;
	application_t app;
	ulong *freqs;
	int n_freqs;

	// General data
	plug_env_getenv(sp, "PLG_PATH_TMP", buffer3, sizeof(buffer3));

	//
	plug_shared_readservs(sp, buffer3, servs);

	//
	plug_shared_readfreqs(sp, buffer3, freqs, &n_freqs);

	//
	plug_env_readapp(sp, &app, freqs, n_freqs);

	//
	plug_env_readnodes(sp, nodes);

	//
	plug_rcom_eard_xxx(sp, nodes, &app, 1);

	//
	if (plug_env_isenv(sp, ENV_LIB_EN, "1") && plug_env_isenv(sp, ENV_PLG_CTX, "SRUN"))
	{
		plug_shared_readsetts(sp, buffer3, setts);

		plug_env_setenviron(sp, setts);
	}

	return ESPANK_SUCCESS;
}

int plug_rcom_eard_job_finish(spank_t sp)
{
	plug_verbose(sp, 2, "function plug_rcom_eard_job_finish");

	//
	plug_rcom_eard_xxx(sp, NULL, NULL, 0);

	return (ESPANK_SUCCESS);
}
