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

// Buffers
static char buffer[SZ_PATH];

int plug_shared_readservs(spank_t sp, plug_package_t *pack, plug_job_t *job)
{
	plug_verbose(sp, 2, "function plug_shared_readservs");

	services_conf_t *servs;

	get_services_conf_path(pack->path_tmp, buffer);
	servs = attach_services_conf_shared_area(buffer);

	if (servs == NULL) {
		plug_error(sp, "while reading the shared services memory in '%s'", "hostxxx");
		return ESPANK_ERROR;
	}

	memcpy(&pack->eard.servs, servs, sizeof(services_conf_t));
	pack->eards.port = servs->eard.port;
	dettach_services_conf_shared_area();

	return ESPANK_SUCCESS;
}

int plug_shared_readfreqs(spank_t sp, plug_package_t *pack, plug_job_t *job)
{
	plug_verbose(sp, 2, "function plug_shared_readfreqs");

	ulong *freqs;
	int n_freqs;

	get_frequencies_path(pack->path_tmp, buffer);
	freqs = attach_frequencies_shared_area(buffer, &n_freqs);

	if (freqs == NULL) {
		plug_error(sp, "while reading the shared services memory in '%s'", "hostxxx");
		return ESPANK_ERROR;
	}

	pack->eard.freqs.n_freqs = n_freqs / sizeof(ulong);
	malloc(&pack->eard.freqs.freqs, n_freqs * sizeof(ulong));
	memcpy(&pack->eard.freqs.freqs, freqs, n_freqs * sizeof(ulong));

	dettach_frequencies_shared_area();

	return ESPANK_SUCCESS;
}

int plug_shared_readsetts(spank_t sp, plug_package_t *pack, plug_job_t *job)
{
	plug_verbose(sp, 2, "function plug_shared_readsetts");

	settings_conf_t *setts;

	// Opening settings
	get_settings_conf_path(pack->path_tmp, buffer);
	setts = attach_settings_conf_shared_area(buffer);

	if (setts == NULL) {
		plug_error(sp, "while reading the shared configuration memory in node '%s'", "hostxxx");
		return ESPANK_ERROR;
	}

	memcpy(&pack->eard.setts, setts, sizeof(settings_conf_t));

	if (plug_verbosity_test(sp, 4)) {
		print_settings_conf(setts);
	}

	// Closing shared memory
	dettach_settings_conf_shared_area();

	// Variable EAR and LD_PRELOAD
	if (!setts->lib_enabled || setts->user_type == ENERGY_TAG) {
		return ESPANK_ERROR;
	}

	return ESPANK_SUCCESS;
}
