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

#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <linux/limits.h>
#include <sys/resource.h>
#include <slurm/spank.h>
#include <cpufreq.h>

#include <slurm_plugin/slurm_plugin_helper.h>
#include <common/config.h>

extern char buffer1[PATH_MAX];
extern char buffer2[PATH_MAX];
extern int verbosity;

/*
 *
 * Verbosity
 *
 */

int verbosity_test(spank_t sp, int level)
{
	char env_remote[8];
	char *env_local;

	if (verbosity == -1) {
		if (getenv_remote(sp, "EAR_VERBOSE", env_remote, 8) == 1) {
			verbosity = atoi(env_remote);
		} else if (getenv_local("EAR_VERBOSE", &env_local) == 1) {
			verbosity = atoi(env_local);
		} else {
			verbosity = 0;
		}
	}

	return verbosity >= level;
}

/*
 *
 * Strings
 *
 */

void strtoup(char *string)
{
	if (string == NULL) {
		return;
	}

    while (*string) {
        *string = toupper((unsigned char) *string);
        string++;
    }
}

char *strclean(char *string, char chr)
{
    char *index;

    if (string == NULL) {
    	return NULL;
    }

	index = strchr(string, chr);

    if (index == NULL) {
    	return NULL;
    }

    string[index - string] = '\0';

    return index;
}

/*
 *
 * Environment
 *
 */
void printenv_remote(spank_t sp, char *name)
{
	if (name == NULL) {
		return;
	}
	
	if(getenv_remote(sp, name, buffer1, PATH_MAX)) {
		plug_error("%s %s", name, buffer1);
	}
}

void appendenv(char *dst, char *src, int dst_capacity)
{
	char *pointer;
	int new_cap;
	int len_src;
	int len_dst;

	if ((dst == NULL) || (src == NULL) || (strlen(src) == 0)) {
		plug_error("Something is null in appendenv");
		return;
	}

	len_dst = strlen(dst);
	len_src = strlen(src);
	new_cap = len_dst + len_src + (len_dst > 0) + 1;

	if (new_cap > dst_capacity) {
		plug_error("Variable could not be appended, too many characters on %d",new_cap);
		return;
	}

	if (len_dst > 0)
	{
		strcpy(buffer2, dst);
		pointer = &dst[len_src];
		strcpy(&pointer[1], buffer2);
		strcpy(dst, src);
		pointer[0] = ':';
	} else {
		strcpy(dst, src);
	}
}

int setenv_local(const char *name, const char *value, int replace)
{
	if (name == NULL || value == NULL) {
		return 0;
	}

    if (setenv (name, value, replace) == -1) {
       	plug_error("Error while setting envar %s (%s)", name, strerror(errno));
        return 0;
    }

    return 1;
}

int setenv_remote(spank_t sp, char *name, char *value, int replace)
{
	if (name == NULL || value == NULL) {
		return 0;
	}

    return (spank_setenv (sp, name, value, replace) == ESPANK_SUCCESS);
}

int getenv_local(char *name, char **env)
{
	char *p;

	if ((name == NULL) || (env == NULL)) {
		return 0;
	}
	p = getenv(name);

	if (p == NULL) {
		return 0;
	}
	if (strlen(p) == 0) {
		return 0;
	}
	*env = p;

	return 1;
}

int getenv_remote(spank_t sp, char *name, char *value, int length)
{
	spank_err_t serrno;

	if (name == NULL || value == NULL) {
		return 0;
	}

	serrno = spank_getenv (sp, name, value, length);

	if (serrno != ESPANK_SUCCESS) {
		return 0;
	}
	if (value == NULL) {
		return 0;
	}
	if (strlen(value) <= 0) {
		return 0;
	}

	return 1;
}

int existenv_local(char *name)
{
    char *env;

    if (name == NULL) {
    	return 0;
    }

	env = getenv(name);
	return (env != NULL) && (strlen(env) > 0);
}

int existenv_remote(spank_t sp, char *name)
{
	spank_err_t serrno;
    char test[4];

	if (name == NULL) {
		return 0;
	}

    serrno = spank_getenv (sp, name, test, 4);

    return (serrno == ESPANK_SUCCESS || serrno == ESPANK_NOSPACE) &&
			(test != NULL) && (strlen(test)) > 0;
}

int isenv_local(char *name, char *value)
{
	char *env;

	if (name == NULL || value == NULL) {
		return 0;
	}

	if ((env = getenv(name)) != NULL) {
        return (strcmp(env, value) == 0);
    }

    return 0;
}

int isenv_remote(spank_t sp, char *name, char *value)
{
    if (name == NULL || value == NULL) {
		return 0;
	}

    if (getenv_remote(sp, name, buffer2, 128)) {
        return (strcmp(buffer2, value) == 0);
    }

    return 0;
}

/*
 *
 * Others
 *
 */
void print_general_info(spank_t sp)
{
	plug_verbose(sp, 2, "function print_general_info");

	struct rlimit sta, mem;
	int r_sta, r_mem;

	r_sta = getrlimit(RLIMIT_STACK, &sta);
	r_mem = rgetrlimit(RLIMIT_MEMLOCK, &mem);

	plug_verbose(sp, 2, "plugin compiled in %s", __DATE__);
	plug_verbose(sp, 2, "stack size limit test (res %d, curr: %lld, max: %lld)",
				 r_sta, (long long) sta.rlim_cur, (long long) sta.rlim_max);
	plug_verbose(sp, 2, "memlock size limit test (res %d, curr: %lld, max: %lld)",
				 r_mem, (long long) mem.rlim_cur, (long long) mem.rlim_max);
	plug_verbose(sp, 2, "buffers size %d", PATH_MAX);
}

int freq_to_p_state(int freq)
{
    struct cpufreq_available_frequencies *list_freqs;
    list_freqs = cpufreq_get_available_frequencies(0);
    int i = 0;

    if (list_freqs == NULL) {
    	return 1;
    }

    if (freq > (int) list_freqs->frequency) {
        return 0;
    }

    while(list_freqs != NULL)
    {
        if (freq == (int) list_freqs->frequency) {
            return i;
        }

        list_freqs = list_freqs->next;
        i += 1;
    }

    return 1;
}
