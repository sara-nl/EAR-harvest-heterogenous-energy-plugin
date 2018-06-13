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

// Verbosity
#define plug_verbose(sp, level, ...) \
	if (verbosity_test(sp, level) == 1) { \
		slurm_error("EARPLUG, " __VA_ARGS__); \
	}	
#define plug_error(...) \
	slurm_error("EARPLUG ERROR," __VA_ARGS__)
#define plug_nude(...)

int verbosity_test(spank_t sp, int level);

// String
void strtoup(char *string);
char* strclean(char *string, char chr);

// Environment variables
void printenv_remote(spank_t sp, char *name);
void appendenv(char *destiny, char *source, int destiny_length);
int setenv_local(const char *name, const char *value, int replace);
int setenv_remote(spank_t sp, char *name, char *value, int replace);
int getenv_local(char *name, char **env);
int getenv_remote(spank_t sp, char *name, char *value, int length);
int existenv_local(char *name);
int existenv_remote(spank_t sp, char *name);
int isenv_local(char *name, char *value);
int isenv_remote(spank_t sp, char *name, char *value);

// Others
void print_general_info(spank_t sp);
int freq_to_p_state(int freq);

#endif //EAR_SLURM_PLUGIN_HELPER_H
