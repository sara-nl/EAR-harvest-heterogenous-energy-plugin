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

#ifndef EAR_SLURM_PLUGIN_HELPER_H
#define EAR_SLURM_PLUGIN_HELPER_H

typedef char *plug_component_t;
typedef int   plug_context_t;

struct component {
	plug_component_t plugin;
	plug_component_t library;
	plug_component_t monitor;
} Component = {
		.plugin  = "PLUG_PLUGIN",
		.library = "PLUG_LIBRARY",
		.monitor = "PLUG_MONITOR"
};

struct context {
	int srun;
	int sbatch;
	int remote;
} Context = {
		.srun   = S_CTX_LOCAL,
		.sbatch = S_CTX_ALLOCATOR,
		.remote = S_CTX_REMOTE,
		.local  = -1;
};

/*
 * Agnostic environment manipulation
 */

int unsetenv_agnostic(spank_t sp, char *var);

int setenv_agnostic(spank_t sp, char *var, char *val, int ow);

int getenv_agnostic(spank_t sp, char *var, char *buf, int len);

int isenv_agnostic(spank_t sp, char *var, char *val);

int repenv_agnostic(spank_t sp, char *var_old, char *var_new);

int apenv_agnostic(char *dst, char *src, int len);


/*
 * Components
 */

int plug_component_setenabled(spank_t sp, plug_component_t comp, int enabled);

int plug_component_isenabled(spank_t sp, plug_component_t comp);

/*
 * Context
 */
int plug_context_is(spank_t sp, plug_context_t ctxt);

/*
 * Verbosity
 */

int plug_verbosity_test(spank_t sp, int level);

#endif //EAR_SLURM_PLUGIN_HELPER_H