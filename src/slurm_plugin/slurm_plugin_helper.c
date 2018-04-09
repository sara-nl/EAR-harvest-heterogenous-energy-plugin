#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cpufreq.h>
#include <slurm/spank.h>

#include <common/string_enhanced.h>
#include <common/config.h>

/*
 *
 * Environment
 *
 */
void appendenv(char *destiny, char *source)
{
    char buffer[PATH_MAX];
        int length = strlen(destiny);
        char *pointer;

        if (length > 0)
        {
            strcpy(buffer, destiny);
            length = strlen(source);
            pointer = &destiny[length];
            strcpy(&pointer[1], buffer);
            strcpy(destiny, source);
            pointer[0] = ':';
        } else {
            strcpy(destiny, source);
        }
}

int setenv_local(const char *name, const char *value, int replace)
{
    if (setenv (name, value, replace) == -1) {
        SPANK_STRERROR("Error while setting envar %s", name);
        return ESPANK_ERROR;
    }
    return ESPANK_SUCCESS;
}

int setenv_remote(spank_t sp, char *name, char *value, int replace)
{
    spank_unsetenv(sp, name);
    return spank_setenv (sp, name, value, replace) == ESPANK_SUCCESS;
}

int getenv_remote(spank_t sp, char *name, char *value, int length)
{
    spank_err_t serrno = spank_getenv (sp, name, value, length);
    return serrno == ESPANK_SUCCESS && value != NULL && strlen(value);
}

int existenv_local(char *name)
{
    char *env = getenv(name);
    return env != NULL && strlen(env) > 0;
}

int existenv_remote(spank_t sp, char *name)
{
    char test[2];
    spank_err_t serrno = spank_getenv (sp, name, test, 2);
    return (serrno == ESPANK_SUCCESS || serrno == ESPANK_NOSPACE) &&
            test != NULL && strlen(test) > 0;
}

int isenv_local(char *name, char *value)
{
    char *env = getenv(name);
    if (env != NULL) {
        return strcmp(env, value) == 0;
    }
    return 0;
}

int isenv_remote(spank_t sp, char *name, char *value)
{
    char buffer[16];
    if (getenv_remote(sp, name, buffer, 16)) {
        return strcmp(buffer, value) == 0;
    }
    return 0;
}

/*
 *
 * Functionality
 *
 */

//TODO: Utilizar un common para esto
int freq_to_p_state(int freq)
{
    struct cpufreq_available_frequencies *list_freqs;
    list_freqs = cpufreq_get_available_frequencies(0);
    int i = 0;

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

int file_to_environment(spank_t sp, const char *path)
{
    const char *value = NULL;
    char option[512];
    FILE *file;

    if ((file = fopen(path, "r")) == NULL)
    {
        SPANK_STRERROR("Config file %s not found", path);
        return ESPANK_ERROR;
    }

    while (fgets(option, 100, file) != NULL)
    {
        if (strclean(option, '\n') && (value = strclean(option, '=')))
        {
            if (strlen(option) && strlen(++value))
            {
                strtoup(option);

                #if BUILD_TYPE(RELEASE_LRZ)
                setenv_local(option, value, 1);
				# else
                setenv_local(option, value, 0);
				#endif
                //slurm_error("%s %s", option, value);
            }
        }
    }

    fclose(file);
    return ESPANK_SUCCESS;
}

int find_ear_conf_file(spank_t sp, int ac, char **av)
{
	char conf_path[PATH_MAX];
	char link_path[PATH_MAX];
    int i, r;

    for (i = 0; i < ac; ++i)
    {
        if (strncmp ("ear_conf_dir=", av[i], 13) == 0)
        {
			sprintf(link_path, "%s/%s", &av[i][13], EAR_LINK_FILE);
			sprintf(conf_path, "%s/%s", &av[i][13], EAR_CONF_FILE);

			if(file_to_environment(sp, (const char *) conf_path) != ESPANK_SUCCESS) {
				return ESPANK_ERROR;
			}

			return file_to_environment(sp, (const char *) link_path);
        }
    }
    return ESPANK_ERROR;
}
