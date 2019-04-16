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

#include <slurm_plugin/slurm_plugin_rcom.h>

//
char eargmd_host[SZ_NAME_MEDIUM];
uint eargmd_port;
uint eargmd_nods;
uint eargmd_enbl;
int  eargmd_conn;

// Externs
extern char buffer1[SZ_PATH];
extern char buffer2[SZ_PATH];
extern char buffer3[SZ_PATH];

int plug_rcom_eargmd_job_start(spank_t sp)
{
	plug_verbose(sp, 2, "function plug_rcom_eargmd_job_start");

	// Not enabled
	if (!eargmd_enbl || plug_env_isenv(sp, "EARGMD_CONNECTED", "1")) {
		return ESPANK_SUCCESS;
	}

	// Gathering variables
	plug_env_getenv(sp, "SLURM_NNODES", buffer1, SZ_PATH);
	eargmd_nods = atoi(buffer1);

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
	plug_env_setenv(sp, "EARGMD_CONNECTED", "1", 1);

	return (ESPANK_SUCCESS);
}

int plug_rcom_eargmd_job_finish(spank_t sp)
{
	plug_verbose(sp, 2, "function plug_rcom_eargmd_job_finish");

	// Not connected, so doesn't need to be finished
	if (!eargmd_conn) {
		return ESPANK_SUCCESS;
	}

	// Disabling protection
	plug_env_setenv(sp, "EARGMD_CONNECTED", "0", 1);

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
