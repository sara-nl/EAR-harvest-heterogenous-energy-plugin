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

// Verbosity
#define plug_verbose(sp, level, ...) \
	if (plug_env_verbtest(sp, level) == 1) { \
		slurm_error("EARPLUG, " __VA_ARGS__); \
	}
#define plug_error(sp, ...) \
	if (plug_env_verbtest(sp, 1) == 1) { \
		slurm_error("EARPLUG ERROR, " __VA_ARGS__); \
	}

int plug_env_verbtest(spank_t sp, int level);

/*
 * Agnostic environment manipulation
 */
int plug_env_islocal(spank_t sp);

int plug_env_isremote(spank_t sp);

int plug_env_unsetenv(spank_t sp, char *var);

int plug_env_setenv(spank_t sp, char *var, char *val, int ow);

int plug_env_getenv(spank_t sp, char *var, char *buf, int len);

int plug_env_isenv(spank_t sp, char *var, char *val);

int plug_env_repenv(spank_t sp, char *var_old, char *var_new);

/*
 * Environment clean
 */

void plug_env_clean(spank_t sp, int ac, char **av);

/*
 * Environment read
 */

int plug_env_readstack(spank_t sp, int ac, char **av);

int plug_env_readuser(spank_t sp);

int plug_env_readapp(spank_t sp, application_t *app, ulong *freqs, int n_freqs);

int plug_env_readnodes(spank_t sp, hostlist_t nodes);

/*
 * Environment set
 */

int plug_env_setpreload(spank_t sp);

int plug_env_setenviron(spank_t sp, settings_conf_t *setts);

#endif