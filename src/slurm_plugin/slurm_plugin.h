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

#ifndef EAR_SLURM_PLUGIN_H
#define EAR_SLURM_PLUGIN_H

#include <slurm/spank.h>
#include <slurm/slurm.h>
#include <common/sizes.h>
#include <common/config.h>
#include <common/output/error.h>
#include <common/output/verbose.h>
#include <common/types/generic.h>
#include <common/types/application.h>
#include <daemon/eard_rapi.h>
#include <daemon/shared_configuration.h>
#include <global_manager/eargm_rapi.h>
//#include <slurm_plugin/slurm_plugin_env.h>
//#include <slurm_plugin/slurm_plugin_helper.h>
//#include <slurm_plugin/slurm_plugin_options.h>
//#include <slurm_plugin/slurm_plugin_rcom.h>

#define ESPANK_STOP		-1
#define S_CTX_SRUN		S_CTX_LOCAL
#define S_CTX_SBATCH	S_CTX_ALLOCATOR
#define ENV_PLUG_CTX		"PLUG_CONTEXT"
#define ENV_PLUG_EN		"PLUG_PLUGIN"
#define ENV_LIB_EN		"PLUG_LIBRARY"
#define ENV_MON_EN		"PLUG_MONITOR"

/*
 * Job data
 */

typedef struct plug_vars {
	char var_ld_preload[SZ_PATH];
	char var_ld_library[SZ_PATH];
} plug_vars_t;

typedef struct plug_user {
	char user[SZ_NAME_MEDIUM];
	char group[SZ_NAME_MEDIUM];
	char account[SZ_NAME_MEDIUM];
	plug_vars_t environment;
} plug_user_t;

typedef struct plug_job
{
	hostlist_t hostlist;
	plug_user_t user;
	uint n_nodes;
	// Current context
	char host[SZ_NAME_MEDIUM];
	application_t app;
	int context_local;
	int exit_status;
} plug_job_t;

/*
 * Package data
 */

typedef struct plug_freqs {
	ulong *freqs;
	int n_freqs;
} plug_freqs_t;

typedef struct plug_eard {
	settings_conf_t setts;
	services_conf_t servs;
	plug_freqs_t freqs;
	uint port;
} plug_eard_t;

typedef struct plug_eargmd {
	char host[SZ_NAME_MEDIUM];
	uint connected;
	uint enabled;
	uint port;
} plug_eargmd_t;

typedef struct plug_pack {
	char path_temp[SZ_PATH];
	char path_inst[SZ_PATH];
	plug_eargmd_t eargmd;
	plug_eard_t eard;
} plug_pack_t;

#endif
