#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <spank.h>
#include <fcntl.h>
#include <cpufreq.h>
#include <ear_slurm_plugin.h>
#include <config.h>

#define FUNCTION_INFO(function) \
	slurm_error("%s", function);
#define SPANK_ERROR(string)                            \
    slurm_error(string);
#define SPANK_STRERROR(string, var)                    \
    slurm_error(string " (%s)", var, strerror(errno));

SPANK_PLUGIN(EAR_PLUGIN, 1)

struct spank_option spank_options[] = {
    { "ear", NULL, "Enables Energy Aware Runtime",
      0, 1, (spank_opt_cb_f) _opt_ear
    },
    { "ear-policy", "type",
      "Selects an energy policy for EAR.\n" \
      "{type=MIN_ENERGY_TO_SOLUTION|MIN_TIME_TO_SOLUTION|MONITORING_ONLY}",
      1, 0, (spank_opt_cb_f) _opt_ear_policy
    },
    { "ear-policy-th", "value",
      "Specifies the threshold to be used by EAR policy" \
      " {value=[0..1]}",
      1, 0, (spank_opt_cb_f) _opt_ear_threshold
    },
    { "ear-user-db", "path",
      "Specifies the path of ",
      2, 0, (spank_opt_cb_f) _opt_ear_user_db
    },
    { "ear-verbose", "value",
      "Specifies the level of the verbosity\n" \
      "{value=[0..5]}; default is 0",
      2, 0, (spank_opt_cb_f) _opt_ear_verbose
    },
    { "ear-learning-phase", "value",
      "Enables the learning phase for a given P_STATE {value=[0..n]}",
      1, 0, (spank_opt_cb_f) _opt_ear_learning
    },
    { "ear-traces", NULL, "Generates application traces with metrics and internal details",
      0, 1, (spank_opt_cb_f) _opt_ear_traces
    },
    SPANK_OPTIONS_TABLE_END
};

/*
 *
 * String
 *
 */

static void strtoup(char *string)
{
    while (*string) {
        *string = toupper((unsigned char) *string);
        string++;
    }
}

static char* strclean(char *string, char chr)
{
    char *index = strchr(string, chr);
    if (index == NULL) return NULL;
    string[index - string] = '\0';
    return index;
}

static char* strend(char *string)
{
    int length = strlen(string);
    return &string[length];
}

static void strapn(char *string, char character)
{
    char *last_char = strend(string);
    last_char[0] = character;
    last_char[1] = '\0';
}

/*
 *
 * Environment
 *
 */
static void appendenv(char *destiny, char *source)
{
    int length = strlen(destiny);
    char *pointer;

    if (length > 0)
    {
        pointer = &destiny[length];
        strcpy(&pointer[1], source);
        pointer[0] = ':';
    } else {
        strcpy(destiny, source);
    }
}

static void printenv_remote(spank_t sp, char *name)
{
    spank_err_t result;
    char buffer[1024];
    result = spank_getenv (sp, name, buffer, 1024);

    if(result == ESPANK_SUCCESS) {
        slurm_error("%s = %s", name, buffer);
    } else if (result == ESPANK_NOSPACE) {
        slurm_error("%s = (TOO BIG)", name);
    } else {
        slurm_error("%s = NULL", name);
    }
}

static char* getenv_local(char *name, char *def_value)
{
    char *value = getenv (name);
    if(value != NULL && strlen(value) > 0) {
        return value;
    }
    return def_value;
}

static void setenv_local(const char *name, const char *value, int replace)
{
    if (setenv (name, value, replace) == -1) {
        SPANK_STRERROR("Error while setting envar %s", name);
    }
}

static int setenv_remote(spank_t sp, char *name, char *value, int replace)
{
    spank_unsetenv(sp, name);
    return spank_setenv (sp, name, value, replace) == ESPANK_SUCCESS;
}

static int getenv_remote(spank_t sp, char *name, char *value, int length)
{
    spank_err_t serrno = spank_getenv (sp, name, value, length);
    return serrno == ESPANK_SUCCESS && value != NULL && strlen(value);
}

static int existenv_remote(spank_t sp, char *name)
{
    char test[2];
    spank_err_t serrno = spank_getenv (sp, name, test, 2);
    return (serrno == ESPANK_SUCCESS || serrno == ESPANK_NOSPACE) &&
            test != NULL && strlen(test);
}

static int isenv_remote(spank_t sp, char *name, char *value)
{
    char buffer[16];
    if (getenv_remote(sp, name, buffer, 16)) {
        return strcmp(buffer, value) == 0;
    }
    return 0;
}

static void file_to_environment(spank_t sp, const char *path)
{
    const char *value = NULL;
    char option[512];
    FILE *file;

    if ((file = fopen(path, "r")) == NULL)
    {
        SPANK_STRERROR("Config file %s not found", path);
        exit(ESPANK_ERROR);
    }

    while (fgets(option, 100, file) != NULL)
    {
        if (strclean(option, '\n') && (value = strclean(option, '=')))
        {
            if (strlen(option) && strlen(++value))
            {
                strtoup(option);
                //slurm_error("%s %s", option, value);
                setenv_local(option, value, 0);
            }
        }
    }

    fclose(file);
}

/*
 *
 * Functionality
 *
 */

static int task_lock(spank_t sp)
{
    char lock_file[PATH_MAX];
    int lock_fd;

    getenv_remote(sp, "EAR_TASK_LOCK_FILE", lock_file, PATH_MAX);
    lock_fd = open(lock_file, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    if (lock_fd >= 0)
    {
        close(lock_fd);
        return 1;
    }
    return 0;
}

static void task_unlock(spank_t sp)
{
    char lock_file[PATH_MAX];
    getenv_remote(sp, "EAR_TASK_LOCK_FILE", lock_file, PATH_MAX);
    remove(lock_file);
}

static void exec_ear_daemon(spank_t sp)
{
    FUNCTION_INFO("exec_ear_daemon");
    char library_path[4096];
    char db_path[1024];
    char bin[1024];
    char tmp[1024];
    char verb[4];

    // Getting environment variables configuration
    getenv_remote(sp, "LD_LIBRARY_PATH", library_path, 4096);
    getenv_remote(sp, "EAR_INSTALL_PATH", bin, 1024);
    getenv_remote(sp, "EAR_DB_PATHNAME", db_path, 1024);
    getenv_remote(sp, "EAR_VERBOSE", verb, 4);
    getenv_remote(sp, "EAR_TMP", tmp, 1024);

    // Setting variables not in the arguments
    setenv_local("LD_LIBRARY_PATH", library_path, 1);
    setenv_local("EAR_DB_PATHNAME", db_path, 1);

    // Appending binary file
    sprintf(bin, "%s/bin/%s", bin, "ear_daemon");

    // Executing EAR daemon
    if (execl(bin, bin, "1", tmp, verb, (char *) 0) == -1) {
        SPANK_STRERROR("Error while executing %s", bin);
        exit(errno);
    }
}

static int fork_ear_daemon(spank_t sp)
{
    FUNCTION_INFO("fork_ear_daemon");
    pid_t pid;

    if (existenv_remote(sp, "EAR_INSTALL_PATH") &&
        existenv_remote(sp, "EAR_VERBOSE") &&
        existenv_remote(sp, "EAR_TMP"))
    {
        pid = fork();

        if (pid == 0) {
            exec_ear_daemon(sp);
        } else if (pid == -1) {
            SPANK_STRERROR("Fork returned %i", pid);
            return ESPANK_ERROR;
        }
    } else {
        SPANK_ERROR("Missing crucial environment variable");
        return (ESPANK_ENV_NOEXIST);
    }
    return ESPANK_SUCCESS;
}

static int freq_to_p_state(int freq)
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

static int prepare_environment(spank_t sp)
{
    char get_buffer[PATH_MAX];
    char set_buffer[PATH_MAX];
    char p_state[8];
    int p_freq = 1;

    if (!existenv_remote(sp, "EAR_INSTALL_PATH")) {
        SPANK_ERROR("EAR environment module not loaded");
        return (ESPANK_ENV_NOEXIST);
    }

    // If policy is monitoring
    if(isenv_remote(sp, "EAR_POWER_POLICY", "MONITORING_ONLY"))
    {
        // If learning phase is not enabled
        if (!existenv_remote(sp, "EAR_LEARNING_PHASE") ||
            isenv_remote(sp, "EAR_LEARNING_PHASE", "0"))
        {
            // If this is passed SLURM's --cpu-freq argument
            if (getenv_remote(sp, "SLURM_CPU_FREQ_REQ", p_state, 8))
            {
                p_freq = atoi(p_state);
                p_freq = freq_to_p_state(p_freq);

                sprintf(p_state, "%i", p_freq);
                setenv_remote(sp, "EAR_P_STATE", p_state, 0);
            }
        }
    }
    // Getting environment variables configuration
    set_buffer[0] = '\0';
    getenv_remote(sp, "LD_PRELOAD", set_buffer, PATH_MAX);
    getenv_remote(sp, "EAR_INSTALL_PATH", get_buffer, PATH_MAX);
    appendenv(set_buffer, get_buffer);

    // Appending libEAR.so or libEARgui.so
    if (isenv_remote(sp, "EAR_GUI", "1")) {
        sprintf(set_buffer, "%s/lib/%s", set_buffer, "libEARgui.so");
    } else {
        sprintf(set_buffer, "%s/lib/%s", set_buffer, "libEAR.so");
    }

    // Saving environment variable LD_PRELOAD
    setenv_remote(sp, "LD_PRELOAD", set_buffer, 1);

    // Appending this libraries to LD_LIBRARY_PATH
    getenv_remote(sp, "LD_LIBRARY_PATH", set_buffer, PATH_MAX);
    appendenv(set_buffer, CPUPOWER_LIB_PATH);
    appendenv(set_buffer, PAPI_LIB_PATH);
    appendenv(set_buffer, FREEIPMI_LIB_PATH);
    setenv_remote(sp, "LD_LIBRARY_PATH", set_buffer, 1);

    // Switching from SLURM_JOB_NAME to EAR_APP_NAME
    if (getenv_remote(sp, "SLURM_JOB_NAME", set_buffer, PATH_MAX)) {
        setenv_remote(sp, "EAR_APP_NAME", set_buffer, 1);
    }

    // Preparing daemon lock file
    getenv_remote(sp, "EAR_TMP", get_buffer, PATH_MAX);
    strapn(get_buffer, '/');
    getenv_remote(sp, "SLURM_JOB_ID", strend(get_buffer), PATH_MAX);
    strapn(get_buffer, '.');
    gethostname(strend(get_buffer), NAME_MAX);
    sprintf(set_buffer, "%s.lock", get_buffer);
    setenv_remote(sp,"EAR_TASK_LOCK_FILE", set_buffer, 1);

    // Adding LD_LIBRARY_PATH visibility
    if (isenv_remote(sp, "EAR_VERBOSE", "4"))
    {
        slurm_error("SLURM environment:");
        printenv_remote(sp, "SLURM_CPU_FREQ_REQ");
        printenv_remote(sp, "SLURMD_NODENAME");
        printenv_remote(sp, "SLURM_JOB_ID");
        slurm_error("System environment:");
        printenv_remote(sp, "LD_PRELOAD");
        printenv_remote(sp, "LD_LIBRARY_PATH");
        slurm_error("EAR special environment:");
        printenv_remote(sp, "EAR_INSTALL_PATH");
        printenv_remote(sp, "EAR_TASK_LOCK_FILE");
        slurm_error("EAR environment:");
        printenv_remote(sp, "EAR_APP_NAME");
        printenv_remote(sp, "EAR_DB_PATHNAME");
        printenv_remote(sp, "EAR_COEFF_DB_PATHNAME");
        printenv_remote(sp, "EAR_DYNAIS_LEVELS");
        printenv_remote(sp, "EAR_DYNAIS_WINDOW_SIZE");
        printenv_remote(sp, "EAR_LEARNING_PHASE");
        printenv_remote(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN");
        printenv_remote(sp, "EAR_PERFORMANCE_PENALTY");
        printenv_remote(sp, "EAR_POWER_POLICY");
        printenv_remote(sp, "EAR_PERFORMANCE_ACCURACY");
        printenv_remote(sp, "EAR_P_STATE");
        printenv_remote(sp, "EAR_RESET_FREQ");
        printenv_remote(sp, "EAR_TURBO");
        printenv_remote(sp, "EAR_TMP");
        printenv_remote(sp, "EAR_USER_DB_PATHNAME");
        printenv_remote(sp, "EAR_VERBOSE");
    }

    return ESPANK_SUCCESS;
}

/*
 *
 * SLURM FRAMEWORK
 *
 */

int slurm_spank_local_user_init (spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_local_user_init");
    int i;

    if(spank_context () == S_CTX_LOCAL)
    {
        if (strncmp ("ear_conf_file=", av[i], 14) == 0) {
            file_to_environment(sp, (const char *) &av[i][14]);
        }
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_user_init(spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_task_init");

    if(spank_context () == S_CTX_REMOTE && isenv_remote(sp, "EAR", "1"))
    {
        prepare_environment(sp);
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_init (spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_init");

    if(spank_context () == S_CTX_SLURMD)
    {
        // LAUNCH DAEMON
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_slurmd_init (spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_slurmd_init");

    if(spank_context () == S_CTX_SLURMD || //NOT LAUNCHED)
    {
        // LAUNCH DAEMON
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_exit (spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_exit");

    if(spank_context () == S_CTX_SLURMD)
    {
        // LAUNCH DAEMON
    }

    return (ESPANK_SUCCESS);
}

/*
 *
 *
 *
 *
 *
 */

static int _opt_ear (int val, const char *optarg, int remote)
{
    FUNCTION_INFO("_opt_ear");
    if (!remote) setenv_local("EAR", "1", 1);
    return (ESPANK_SUCCESS);
}

static int _opt_ear_learning (int val, const char *optarg, int remote)
{
    FUNCTION_INFO("_opt_ear_learning");
    char p_state[2];
    int ioptarg;

    if (!remote)
    {
        if (optarg == NULL) {
            return (ESPANK_BAD_ARG);
        }
        if ((ioptarg = atoi(optarg)) < 0) {
            return (ESPANK_BAD_ARG);
        }

        sprintf(p_state, "%d", ioptarg);
        setenv_local("EAR_P_STATE", p_state, 1);
        setenv_local("EAR_LEARNING_PHASE", "1", 1);
        setenv_local("EAR", "1", 1);
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_policy (int val, const char *optarg, int remote)
{
    FUNCTION_INFO("_opt_ear_policy");
    char policy[32];
    int index = 0;

    if (!remote)
    {
        if (optarg == NULL) {
            return (ESPANK_BAD_ARG);
        }

        strncpy(policy, optarg, 32);
        strtoup(policy);

        index += (strcmp(policy, "MIN_ENERGY_TO_SOLUTION") == 0);
        index += (strcmp(policy, "MIN_TIME_TO_SOLUTION") == 0) * 2;
        index += (strcmp(policy, "MONITORING_ONLY") == 0) * 3;

        if (index == 0) {
            return (ESPANK_BAD_ARG);
        }

        if (index == 1 || index == 2) {
            setenv_local("EAR_P_STATE", "1", 1);
        }
        if (index == 3) {
            setenv_local("EAR_P_STATE", "1", 0);
        }

        setenv_local("EAR_POWER_POLICY", policy, 1);
        setenv_local("EAR", "1", 1);
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_user_db (int val, const char *optarg, int remote)
{
    FUNCTION_INFO("_opt_ear_user_db");

    if (!remote) {
        if (optarg == NULL) return (ESPANK_BAD_ARG);
        setenv_local("EAR_USER_DB_PATHNAME", optarg, 1);
        setenv_local("EAR", "1", 1);
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_threshold (int val, const char *optarg, int remote)
{
    FUNCTION_INFO("_opt_ear_threshold");
    double foptarg = -1;
    char threshold[4];

    if (!remote)
    {
        if (optarg == NULL) {
            return (ESPANK_BAD_ARG);
        }
        if ((foptarg = atof(optarg)) < 0.0 || foptarg > 1.0) {
            return (ESPANK_BAD_ARG);
        }

        sprintf(threshold, "%0.2f", foptarg);
        setenv_local("EAR_PERFORMANCE_PENALTY", threshold, 1);
        setenv_local("EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", threshold, 1);
        setenv_local("EAR", "1", 1);
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_verbose (int val, const char *optarg, int remote)
{
    FUNCTION_INFO("_opt_ear_verbose");
    char verbosity[1];
    int ioptarg;

    if (!remote)
    {
        if (optarg == NULL) {
            return (ESPANK_BAD_ARG);
        }

        ioptarg = atoi(optarg);
        if (ioptarg < 0) ioptarg = 0;
        if (ioptarg > 4) ioptarg = 4;

        sprintf(verbosity, "%i", ioptarg);
        setenv_local("EAR_VERBOSE", verbosity, 1);
        setenv_local("EAR", "1", 1);
    }

    return (ESPANK_SUCCESS);
}

static int _opt_ear_traces (int val, const char *optarg, int remote)
{
    FUNCTION_INFO("_opt_ear_traces");
    if (!remote) {
        setenv_local("EAR_GUI", "1", 1);
        setenv_local("EAR", "1", 1);
    }
    return (ESPANK_SUCCESS);
}