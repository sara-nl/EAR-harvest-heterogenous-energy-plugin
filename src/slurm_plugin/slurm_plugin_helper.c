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

#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <linux/limits.h>

#include <cpufreq.h>
#include <slurm/spank.h>

#include <slurm_plugin/slurm_plugin_helper.h>
#include <common/config.h>

static int auth_mode;

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

char* strclean(char *string, char chr)
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
	char value[PATH_MAX];

	if (name == NULL) {
		return;
	}
	
	if(getenv_remote(sp, name, value, PATH_MAX)) {
		slurm_error("%s %s", name, value);
	}
}

void appendenv(char *destiny, char *source, int destiny_length)
{
   	char buffer[PATH_MAX];
	char *pointer;
	int capacity;
	int length;

	if ((destiny == NULL) || (source == NULL) || (strlen(source) == 0)) {
		return;
	}

	length = strlen(destiny);
	capacity = strlen(source) + length + 2;

	if (capacity > destiny_length) {
		slurm_error("Variable could not be appended, too many characters");
		return;
	}

	if (length > 0)
	{
		//destiny = string1
		//source  = string2
		strcpy(buffer, destiny);
		//buffer  = string1
		length = strlen(source);
		//length  = 7
		pointer = &destiny[length];
		//pointer = string1_<-
		strcpy(&pointer[1], buffer);
		//destiny = string1_string1
		strcpy(destiny, source);
		//destiny = string2_string1
		pointer[0] = ':';
		//destiny = string2:string1
	} else {
		strcpy(destiny, source);
	}
}

int setenv_local(const char *name, const char *value, int replace)
{
	if (name == NULL || value == NULL) {
		return 0;
	}

    if (setenv (name, value, replace) == -1) {
       	slurm_error("Error while setting envar %s (%s)", name, strerror(errno));
        return 0;
    }

    return 1;
}

int setenv_remote(spank_t sp, char *name, char *value, int replace)
{
    return (spank_setenv (sp, name, value, replace) == ESPANK_SUCCESS);
}

int getenv_local(char *name, char **env)
{
	char *p;

	if ((name == NULL) || (env == NULL)) {
		return 0;
	}
	p = getenv(name);

	if ((p == NULL) || (strlen(p) == 0)) {
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

	return (serrno == ESPANK_SUCCESS) && (value != NULL) && (strlen(value) > 0);
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
    char test[2];

	if (name == NULL) {
		return 0;
	}

    serrno = spank_getenv (sp, name, test, 2);

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
    char buffer[128];

    if (name == NULL || value == NULL) {
		return 0;
	}

    if (getenv_remote(sp, name, buffer, 128)) {
        return (strcmp(buffer, value) == 0);
    }

    return 0;
}

/*
 *
 * Functionality
 *
 */

static void setenv_if_authorized(const char *option, const char *value)
{
	// Mode 0: authorized
	// Mode 1: normal user

	if (auth_mode == 1) {
		setenv_local(option, value, 0);
	} else {
		setenv_local(option, value, 0);
	}

	DEBUGGING("%s %s", option, value);
}

int file_to_environment(spank_t sp, const char *path)
{	
    FUNCTION_INFO("file_to_environment");
    const char *value = NULL;
    char option[PATH_MAX];
    FILE *file;

    if ((file = fopen(path, "r")) == NULL)
    {
        slurm_error("Config file %s not found (%s)", path, strerror(errno));
        return ESPANK_ERROR;
    }

    while (fgets(option, PATH_MAX, file) != NULL)
    {
        if ((strclean(option, '\n') != NULL) &&
			((value = strclean(option, '=')) != NULL))
        {
            if ((strlen(option) > 0))
            {
            	value += 1;

            	if (strlen(value) > 0) {
                	strtoup(option);
					setenv_if_authorized(option, value);
				}
            }
        }
    }

    fclose(file);
    return ESPANK_SUCCESS;
}

int find_ear_conf_file(spank_t sp, int ac, char **av)
{
	FUNCTION_INFO("find_ear_conf_file");
	char conf_path[PATH_MAX];
	char link_path[PATH_MAX];
	int i;

    for (i = 0; i < ac; ++i)
    {
        if (strncmp ("conf_dir=", av[i], 9) == 0)
        {
			sprintf(link_path, "%s/%s", &av[i][9], EAR_LINK_FILE);
			sprintf(conf_path, "%s/%s", &av[i][9], EAR_CONF_FILE);

			if(file_to_environment(sp, (const char *) conf_path) != ESPANK_SUCCESS) {
				return ESPANK_ERROR;
			}

			return file_to_environment(sp, (const char *) link_path);
        }
    }
    return ESPANK_ERROR;
}

static int find_user_by_string(char *string, char *id)
{
	char *p;

	if ((string == NULL) || (strlen(string) == 0) ||
			(id == NULL) || (strlen(id) == 0))
	{
		return;
	}

	p = strtok (string, ",");

	while (p != NULL)
	{
		DEBUGGING("%s %s", p, id);
		
		if (strcmp(p, id) == 0) {
			return 1;
		}

		p = strtok (NULL, ",");
	}
	return 0;
}

static int find_user_by_uint(char *string, unsigned int id)
{
	unsigned int nid;
	char *p;

	if (string == NULL || strlen(string) == 0) {
		return;
	}

	p = strtok (string, ",");

	while (p != NULL)
	{
		nid = (unsigned int) atoi(p);
		DEBUGGING("%u %u", id, nid);

		if (id == nid) {
			return 1;
		}

		p = strtok (NULL, ",");
	}
	return 0;
}

void find_ear_user_privileges(spank_t sp, int ac, char **av)
{
	FUNCTION_INFO("find_ear_user_privileges");
	int i, ruid, rgid, res = 0;
	char *aid;
	uid_t uid;
	gid_t gid;

	ruid = spank_get_item(sp, S_JOB_UID, &uid);
	rgid = spank_get_item(sp, S_JOB_GID, &gid);
	aid = getenv("SLURM_JOB_ACCOUNT");

	for (i = 0; i < ac; ++i)
	{
		if ((ruid == ESPANK_SUCCESS) &&
			(strncmp ("auth_users=", av[i], 11) == 0) &&
			(find_user_by_uint(&av[i][11], uid) ))
		{
			auth_mode = 1;
			return;
		}
		if ((rgid == ESPANK_SUCCESS) &&
			(strncmp ("auth_groups=", av[i], 12) == 0) &&
			(find_user_by_uint(&av[i][12], gid) ))
		{
			auth_mode = 1;
			return;
		}
		if ((aid != NULL) &&
			(strncmp ("auth_accounts=", av[i], 14) == 0) &&
			(find_user_by_string(&av[i][14], aid) ))
		{
			auth_mode = 1;
			return;
		}
	}
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
