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

int setenv_local(const char *var, const char *val, int ow);
int setenv_remote(spank_t sp, char *var, char *val, int ow);

int unsetenv_local(char *var);
int unsetenv_remote(spank_t sp, char *var);

int getenv_local(char *var, char *buf, int len);
int getenv_remote(spank_t sp, char *var, char *buf, int len);

int apenv(char *dst, char *src, int dst_capacity);

int isenv_local(char *var, char *val);
int isenv_remote(spank_t sp, char *var, char *val);

int exenv_local(char *var);
int exenv_remote(spank_t sp, char *var);

int repenv_local(char *var_old, char *var_new);
int repenv_remote(spank_t sp, char *var_old, char *var_new);

#endif //EAR_SLURM_PLUGIN_HELPER_H