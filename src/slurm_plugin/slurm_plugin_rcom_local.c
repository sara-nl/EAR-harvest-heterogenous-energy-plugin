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

//
extern int eard_port;

int plug_shared_readservs(spank_t sp, char *path, services_conf_t *servs)
{
	plug_verbose(sp, 2, "function plug_shared_readservs");

	get_services_conf_path(path, buffer);
	servs = attach_services_conf_shared_area(buffer);

	if (servs == NULL) {
		plug_error(sp, "while reading the shared services memory in '%s'", "hostxxx");
		return ESPANK_ERROR;
	}

	eard_port = servs->eard.port;
	dettach_services_conf_shared_area();
}

int plug_shared_readfreqs(spank_t sp, char *path, ulong *freqs, int *n_freqs)
{
	plug_verbose(sp, 2, "function plug_shared_readfreqs");

	get_frequencies_path(path, buffer);
	freqs = attach_frequencies_shared_area(buffer, n_freqs);
	*n_freqs = *n_freqs / sizeof(ulong);
	dettach_frequencies_shared_area();
}

int plug_shared_readsetts(spank_t sp, char *path, settings_conf_t *setts)
{
	plug_verbose(sp, 2, "function plug_shared_readsetts");

	// Opening settings
	get_settings_conf_path(path, buffer);
	setts = attach_settings_conf_shared_area(buffer);

	if (setts == NULL) {
		plug_error(sp, "while reading the shared configuration memory in node '%s'", "hostxxx");
		return ESPANK_ERROR;
	}

	if (plug_env_verbtest(sp, 4)) {
		print_settings_conf(setts);
	}

	// Closing shared memory
	dettach_settings_conf_shared_area();

	// Variable EAR and LD_PRELOAD
	if (!setts->lib_enabled || setts->user_type == ENERGY_TAG) {
		return (ESPANK_ERROR);
	}

	return (ESPANK_SUCCESS);
}
