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

#include <slurm_plugin/slurm_plugin_env.h>
#include <slurm_plugin/slurm_plugin_rcom.h>

// Externs
static char buffer[SZ_PATH];

int plug_rcom_eargmd_job_start(spank_t sp, plug_pack_t *pack, plug_job_t *job)
{
	plug_verbose(sp, 2, "function plug_rcom_eargmd_job_start");

	// Disabled
	return ESPANK_SUCCESS;

	// Pack deserialization
	getenv_agnostic(sp, "EARGMD_CONNECTED", buffer, SZ_PATH);
	pack->eargmd.connected = atoi(buffer);

	if (pack->eargmd.enabled && !pack->eargmd.connected) {
		return ESPANK_ERROR;
	}

	// Verbosity
	plug_verbose(sp, 2, "trying to connect EARGMD with host '%s', port '%u', and nnodes '%u'",
		pack->eargmd.host, pack->eargmd.port, job->n_nodes);

	// Connection
	if (eargm_connect(pack->eargmd.host, pack->eargmd.port) < 0) {
		plug_error(sp, "while connecting with EAR global manager daemon");
		return ESPANK_ERROR;
	}
	if (!eargm_new_job(job->n_nodes)) {
		plug_error(sp, "while speaking with EAR global manager daemon");
	}
	eargm_disconnect();

	// Informing that this report has to be finished
	pack->eargmd.connected = 1;

	// Enabling protection
	setenv_agnostic(sp, "EARGMD_CONNECTED", "1", 1);

	return (ESPANK_SUCCESS);
}

int plug_rcom_eargmd_job_finish(spank_t sp, plug_pack_t *pack, plug_job_t *job)
{
	plug_verbose(sp, 2, "function plug_rcom_eargmd_job_finish");

	// Disabled
	return ESPANK_SUCCESS;

	if (eargm_connect(pack->eargmd.host, pack->eargmd.port) < 0) {
		plug_error(sp, "while connecting with EAR global manager daemon");
		return ESPANK_ERROR;
	}
	if (!eargm_end_job(job->n_nodes)) {
		plug_error(sp, "while speaking with EAR global manager daemon");
		return ESPANK_ERROR;
	}
	eargm_disconnect();

	// Disabling protection
	setenv_agnostic(sp, "EARGMD_CONNECTED", "0", 1);

	return (ESPANK_SUCCESS);
}
