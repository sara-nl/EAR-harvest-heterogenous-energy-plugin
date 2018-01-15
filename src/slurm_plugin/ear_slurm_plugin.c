#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <spank.h>
#include <fcntl.h>
#include <cpufreq.h>
#include <config.h>
#include <signal.h>

#include <string_enhanced.h>
#include <ear_slurm_plugin.h>

#define FUNCTION_INFO(function) 
#define SPANK_ERROR(string)                            \
    slurm_error(string);
#define SPANK_STRERROR(string, var)                    \
    slurm_error(string " (%s)", var, strerror(errno));

SPANK_PLUGIN(EAR_PLUGIN, 1)
pid_t daemon_pid = -1;

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

static int setenv_local(const char *name, const char *value, int replace)
{
    if (setenv (name, value, replace) == -1) {
        SPANK_STRERROR("Error while setting envar %s", name);
        return ESPANK_ERROR;
    }
    return ESPANK_SUCCESS;
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

static int existenv_local(char *name)
{
    char *env = getenv(name);
    return env != NULL && strlen(env) > 0;
}

static int existenv_remote(spank_t sp, char *name)
{
    char test[2];
    spank_err_t serrno = spank_getenv (sp, name, test, 2);
    return (serrno == ESPANK_SUCCESS || serrno == ESPANK_NOSPACE) &&
            test != NULL && strlen(test) > 0;
}

static int isenv_local(char *name, char *value)
{
    char *env = getenv(name);
    if (env != NULL) {
        return strcmp(env, value) == 0;
    }
    return 0;
}

static int isenv_remote(spank_t sp, char *name, char *value)
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

static int file_to_environment(spank_t sp, const char *path)
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
                setenv_local(option, value, 0);
                //slurm_error("%s %s", option, value);
            }
        }
    }

    fclose(file);
    return ESPANK_SUCCESS;
}

static int find_ear_conf_file(spank_t sp, int ac, char **av)
{
    int i;
    for (i = 0; i < ac; ++i)
    {
        if (strncmp ("ear_conf_file=", av[i], 14) == 0) {
            return file_to_environment(sp, (const char *) &av[i][14]);
        }
    }
    return ESPANK_ERROR;
}

/*
 *
 *
 *
 *
 *
 */

static void exec_ear_daemon(spank_t sp)
{
    FUNCTION_INFO("exec_ear_daemon");
    char ear_lib[PATH_MAX];
    char *ear_install_path;
    char *ear_verbose;
    char *ear_tmp;

    // Getting environment variables configuration
    ear_install_path = getenv("EAR_INSTALL_PATH");
    sprintf(ear_lib, "%s/%s", ear_install_path, EAR_DAEMON_PATH);

    ear_verbose = getenv("EAR_VERBOSE");
    ear_tmp = getenv("EAR_TMP");

    // Executing EAR daemon
    if (execl(ear_lib, ear_lib, "1", ear_tmp, ear_verbose, (char *) 0) == -1) {
        SPANK_STRERROR("Error while executing %s", ear_lib);
        exit(errno);
    }
}

static int fork_ear_daemon(spank_t sp)
{
    FUNCTION_INFO("fork_ear_daemon");

    if (existenv_local("EAR_INSTALL_PATH") &&
        existenv_local("EAR_VERBOSE") &&
        existenv_local("EAR_TMP"))
    {
        daemon_pid = fork();

        if (daemon_pid == 0) {
            exec_ear_daemon(sp);
        } else if (daemon_pid < 0) {
            SPANK_STRERROR("Fork returned %i", daemon_pid);
            return ESPANK_ERROR;
        }
    } else {
        SPANK_ERROR("Missing crucial environment variable");
        return ESPANK_ENV_NOEXIST;
    }
    return ESPANK_SUCCESS;
}

/*
 *
 *
 *
 *
 *
 */

static int local_update_ld_preload(spank_t sp)
{
    char *ld_preload, *ear_install_path;
    char buffer[PATH_MAX];

    buffer[0] = '\0';
    ld_preload = getenv("LD_PRELOAD");
    ear_install_path = getenv("EAR_INSTALL_PATH");

    if (ld_preload != NULL) {
        strcpy(buffer, ld_preload);
    }

    // Apending: x:y or y
    appendenv(buffer, ear_install_path);

    // Appending libraries to LD_PRELOAD
    if (isenv_local("EAR_TRACES", "1")) {
        sprintf(buffer, "%s/%s", buffer, EAR_LIB_TRAC_PATH);
    } else {
        sprintf(buffer, "%s/%s", buffer, EAR_LIB_PATH);
    }

    //
    return setenv_local("LD_PRELOAD", buffer, 1);
}


static int local_update_ear_install_path()
{
    FUNCTION_INFO("local_update_ear_install_path");
    char *ear_install_path = getenv("EAR_INSTALL_PATH");
    
    if (ear_install_path == NULL || strlen(ear_install_path) == 0)
    {
        slurm_error("Warning: $EAR_INSTALL_PATH not found, using %s", EAR_INSTALL_PATH);
        slurm_error("Please, read the documentation and load the environment module.");
        return setenv_local("EAR_INSTALL_PATH", EAR_INSTALL_PATH, 1);
    }
    return ESPANK_SUCCESS;
}

static int local_update_ld_library_path()
{
    FUNCTION_INFO("local_update_ld_library_path");
    char *ld_library_path;
    char buffer[PATH_MAX];

    //
    buffer[0] = '\0';
    ld_library_path = getenv("LD_LIBRARY_PATH");
    
    if (ld_library_path != NULL) {
        strcpy(buffer, ld_library_path);
    }

    //
    appendenv(buffer, CPUPOWER_LIB_PATH);
    appendenv(buffer, PAPI_LIB_PATH);
    appendenv(buffer, FREEIPMI_LIB_PATH);
    
    //
    return setenv_local("LD_LIBRARY_PATH", buffer, 1);
}

//TODO: comprobar todos los tipos de --cpu-freq
static void remote_update_slurm_vars(spank_t sp)
{
    char buffer[PATH_MAX];
    char p_state[8];
    int p_freq = 1;

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

    // Switching from SLURM_JOB_NAME to EAR_APP_NAME
    if (getenv_remote(sp, "SLURM_JOB_NAME", buffer, PATH_MAX)) {
        setenv_remote(sp, "EAR_APP_NAME", buffer, 1);
    }
}

/*
 *
 *
 * SLURM FRAMEWORK
 *
 *
 */
#define NO_OK(function) \
    if ((r = function) != ESPANK_SUCCESS) return r;

int slurm_spank_local_user_init (spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_local_user_init");
    int r;

    if(spank_context () == S_CTX_LOCAL)
    {
        NO_OK(find_ear_conf_file(sp, ac, av));

        if (isenv_local("EAR", "1"))
        {
            NO_OK(local_update_ear_install_path());
            NO_OK(local_update_ld_library_path());
            NO_OK(local_update_ld_preload(sp));
        }
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_user_init(spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_task_init");

    if(spank_context() == S_CTX_REMOTE && isenv_remote(sp, "EAR", "1"))
    {
        remote_update_slurm_vars(sp);
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_init (spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_init");

    if(spank_context() == S_CTX_SLURMD) {
        return slurm_spank_slurmd_init(sp, ac, av);
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_slurmd_init (spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_slurmd_init");
    int r;

    if(spank_context() == S_CTX_SLURMD && daemon_pid < 0)
    {
        NO_OK(find_ear_conf_file(sp, ac, av));
        NO_OK(local_update_ear_install_path());
        NO_OK(local_update_ld_library_path());
        return fork_ear_daemon(sp);
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_slurmd_exit (spank_t sp, int ac, char **av)
{
    FUNCTION_INFO("slurm_spank_slurmd_exit");

    if(spank_context() == S_CTX_SLURMD && daemon_pid > 0)
    {
        kill(daemon_pid, SIGTERM);
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

//TODO: añadir más control de errores aquí
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
        setenv_local("EAR_TRACES", "1", 1);
        setenv_local("EAR", "1", 1);
    }
    return (ESPANK_SUCCESS);
}
