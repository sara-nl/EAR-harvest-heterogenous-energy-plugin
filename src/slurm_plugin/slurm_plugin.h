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
#include <slurm_plugin/slurm_plugin_env.h>
#include <slurm_plugin/slurm_plugin_helper.h>
#include <slurm_plugin/slurm_plugin_options.h>
#include <slurm_plugin/slurm_plugin_rcom.h>

#define ESPANK_STOP		-1
#define S_CTX_SRUN		S_CTX_LOCAL
#define S_CTX_SBATCH	S_CTX_ALLOCATOR
#define ENV_PLG_CTX		"PLUG_CONTEXT"
#define ENV_PLG_EN		"PLUG_PLUGIN"
#define ENV_LIB_EN		"PLUG_LIBRARY"
#define ENV_MON_EN		"PLUG_MONITOR"

typedef struct plug_user {
	char *name_acc[SZ_NAME_MEDIUM];
	char *name_upw[SZ_NAME_MEDIUM];
	char *name_gpw[SZ_NAME_MEDIUM];
} plug_user_t;

typedef struct plug_node {
	char host[SZ_NAME_MEDIUM];
} plug_node_t;

typedef struct plug_job {
	plug_user_t user;
	char var_ld_preload[SZ_PATH];
	char var_ld_library[SZ_PATH];
	application_t app;
	int local_context;
	int exit_status;
	int verbosity;
	uint n_nodes;
} plug_job_t;

typedef struct plug_freqs {
	ulong *freqs;
	int n_freqs;
} plug_freqs_t;

typedef struct plug_eard {
	settings_conf_t setts;
	services_conf_t servs;
	plug_freqs_t freqs;
	hostlist_t hostlist;
	uint port;
} plug_eard_t;

typedef struct plug_eargmd {
	char host[SZ_NAME_MEDIUM];
	uint connected;
	uint enabled;
	uint port;
} plug_eargmd_t;

typedef struct plug_pack {
	plug_comps_t comps;
	plug_eargmd_t eargmd;
	plug_eards_sbatch eards;
	plug_eard_srun eard;
	char path_temp[SZ_PATH];
	char path_inst[SZ_PATH];
} plug_pack_t;

typedef char *plug_comp_t;

struct comp {
	plug_comp_t plugin = "PLUG_PLUGIN";
	plug_comp_t library = "PLUG_LIBRARY";
	plug_comp_t monitor = "PLUG_MONITOR";
};

struct cntx {
	int srun = S_CTX_LOCAL;
	int sbatch = S_CTX_ALLOCATOR;
	int remote = 0;
};

#endif
