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
#include <slurm_plugin/slurm_plugin_env.h>

// Buffers
static char buffer[SZ_PATH];

static int plug_rcom_eard(spank_t sp, plug_pack_t *pack, plug_job_t *job, int new_job)
{
	char *node;

	while ((node = slurm_hostlist_shift(job->hostlist)) != NULL)
	{
		plug_verbose(sp, 2, "remote connect to host: '%s:%d'", node, pack->eard.port);

		if (eards_remote_connect(node, pack->eard.port) < 0) {
			plug_error(sp, "while connecting with EAR daemon");
			continue;
		}

		if (new_job) {
			eards_new_job(&job->app);
		} else {
			eards_end_job(job->app.job.id, job->app.job.step_id);
		}

		eards_remote_disconnect();
		free(node);
	}

	return ESPANK_SUCCESS;
}


int plug_rcom_eard_job_start(spank_t sp, plug_pack_t *pack, plug_job_t *job)
{
	plug_verbose(sp, 2, "function plug_rcom_eard_job_start");
	return plug_rcom_eard(sp, pack, job, 1);
}

int plug_rcom_eard_job_finish(spank_t sp, plug_pack_t *pack, plug_job_t *job)
{
	plug_verbose(sp, 2, "function plug_rcom_eard_job_finish");
	return plug_rcom_eard(sp, pack, job, 0);
}
