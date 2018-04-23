/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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

#define DEBUGGING(string, ...)
//    	slurm_error(string, __VA_ARGS__)
#define FUNCTION_INFO(function)
//	slurm_error(function)
#define FUNCTION_INFO_(function)
//	if (isenv_local("EAR_VERBOSE", "1") || isenv_remote(sp, "EAR_VERBOSE", "1")) { \
//		slurm_error(function); \
//		printenv_remote(sp, "LD_LIBRARY_PATH"); \
//	}
#define SPANK_ERROR(string) \
    slurm_error(string);
#define SPANK_STRERROR(string, var) \
    slurm_error(string " (%s)", var, strerror(errno));

void strtoup(char *string);
char* strclean(char *string, char chr);

void printenv_remote(spank_t sp, char *name);
void appendenv(char *destiny, char *source);
int setenv_local(const char *name, const char *value, int replace);
int setenv_remote(spank_t sp, char *name, char *value, int replace);
int getenv_local(char *name, char **env);
int getenv_remote(spank_t sp, char *name, char *value, int length);
int existenv_local(char *name);
int existenv_remote(spank_t sp, char *name);
int isenv_local(char *name, char *value);
int isenv_remote(spank_t sp, char *name, char *value);

//int freq_to_p_state(int freq);
int file_to_environment(spank_t sp, const char *path);
int find_ear_conf_file(spank_t sp, int ac, char **av);

#endif //EAR_SLURM_PLUGIN_HELPER_H
