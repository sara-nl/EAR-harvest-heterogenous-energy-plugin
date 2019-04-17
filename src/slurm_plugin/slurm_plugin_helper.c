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

int setenv_local(const char *var, const char *val, int ow)
{
	if (var == NULL || val == NULL) {
		return 0;
	}
    if (setenv (var, val, ow) == -1) {
        return 0;
    }
    return 1;
}

int setenv_remote(spank_t sp, char *var, char *val, int ow)
{
	if (var == NULL || val == NULL) {
		return 0;
	}
    return (spank_setenv (sp, var, val, ow) == ESPANK_SUCCESS);
}

int unsetenv_local(char *var)
{
	if (var == NULL) {
		return 0;
	}
	return unsetenv(var) == 0;
}

int unsetenv_remote(spank_t sp, char *var)
{
	if (var == NULL) {
        	return 0;
    	}
	return (spank_unsetenv(sp, var) == ESPANK_SUCCESS);
}

int getenv_local(char *var, char *buf, int len)
{
	char *c;

	if (var == NULL || buf == NULL) {
		return 0;
	}
	if ((c = getenv(var)) == NULL) {
		return 0;
	}

	snprintf(buf, len, "%s", c);
	return 1;
}

int getenv_remote(spank_t sp, char *var, char *buf, int len)
{
	spank_err_t serrno;

	if (var == NULL || buf == NULL) {
		return 0;
	}
	
	serrno = spank_getenv (sp, var, buf, len);

	if (serrno != ESPANK_SUCCESS) {
		return 0;
	}
	if (strlen(buf) <= 0) {
		return 0;
	}

	return 1;
}

int apenv(char *dst, char *src, int dst_capacity)
{
	char *pointer;
	int new_cap;
	int len_src;
	int len_dst;

	if ((dst == NULL) || (src == NULL) || (strlen(src) == 0)) {
		return 0;
	}

	len_dst = strlen(dst);
	len_src = strlen(src);
	new_cap = len_dst + len_src + (len_dst > 0) + 1;

	if (new_cap > dst_capacity) {
		return 0;
	}

	if (len_dst > 0)
	{
		strcpy(buffer, dst);
		pointer = &dst[len_src];
		strcpy(&pointer[1], buffer);
		strcpy(dst, src);
		pointer[0] = ':';
	} else {
		strcpy(dst, src);
	}

	return 1;
}

int isenv_local(char *var, char *val)
{
	char *env;

	if (var == NULL || val == NULL) {
		return 0;
	}

	if ((env = getenv(var)) != NULL) {
        return (strcmp(env, val) == 0);
    }

    return 0;
}

int isenv_remote(spank_t sp, char *var, char *val)
{
    if (var == NULL || val == NULL) {
		return 0;
	}

    if (getenv_remote(sp, var, buffer, sizeof(buffer))) {
        return (strcmp(buffer, val) == 0);
    }

    return 0;
}

int exenv_local(char *var)
{
	char *env;

	if (var == NULL) {
		return 0;
	}

	env = getenv(var);
	return (env != NULL) && (strlen(env) > 0);
}

int exenv_remote(spank_t sp, char *var)
{
	spank_err_t serrno;

	if (var == NULL) {
		return 0;
	}

	serrno = spank_getenv (sp, var, buffer, SZ_PATH);

	return (serrno == ESPANK_SUCCESS || serrno == ESPANK_NOSPACE) &&
		   (buffer != NULL) && (strlen(buffer)) > 0;
}

int repenv_local(char *var_old, char *var_new)
{
	if (!getenv_local(var_old, buffer, SZ_PATH)) {
		return 0;
	}
	return setenv_local(var_new, buffer, 1);
}

int repenv_remote(spank_t sp, char *var_old, char *var_new)
{
	return 1;
}
