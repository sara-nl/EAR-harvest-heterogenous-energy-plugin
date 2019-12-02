typedef int spank_t;
typedef int spank_err_t;
typedef int spank_item_t;
typedef int spank_context_t;

enum spank_context {
    S_CTX_ERROR,             /* Error obtaining current context              */
    S_CTX_LOCAL,             /* Local context (srun)                         */
    S_CTX_REMOTE,            /* Remote context (slurmstepd)                  */
    S_CTX_ALLOCATOR
};

typedef int (*spank_opt_cb_f) (int val, const char *optarg, int remote);

struct spank_option {
    char *         name;    /* long option provided by plugin               */
    char *         arginfo; /* one word description of argument if required */
    char *         usage;   /* Usage text                                   */
    int            has_arg; /* Does option require argument?                */
    int            val;     /* value to return using callback               */
    spank_opt_cb_f cb;      /* Callback function to check option value      */
};

int slurm_spank_init_post_opt(spank_t sp, int ac, char **av);

enum spank_err {
    ESPANK_SUCCESS     = 0, /* Success.                                      */
    ESPANK_ERROR       = 1, /* Generic error.                                */
    ESPANK_BAD_ARG     = 2, /* Bad argument.                                 */
    ESPANK_NOT_TASK    = 3, /* Not in task context.                          */
    ESPANK_ENV_EXISTS  = 4, /* Environment variable exists && !overwrite     */
    ESPANK_ENV_NOEXIST = 5, /* No such environment variable                  */
    ESPANK_NOSPACE     = 6, /* Buffer too small.                             */
    ESPANK_NOT_REMOTE  = 7, /* Function only may be called in remote context */
    ESPANK_NOEXIST     = 8, /* Id/pid doesn't exist on this node             */
    ESPANK_NOT_EXECD   = 9, /* Lookup by pid requested, but no tasks running */
    ESPANK_NOT_AVAIL   = 10,/* SPANK item not available from this callback   */
    ESPANK_NOT_LOCAL   = 11,/* Function only valid in local/alloc context    */
};

enum spank_item {
    S_JOB_UID,               /* User id (uid_t *)                            */
    S_JOB_GID,               /* Primary group id (gid_t *)                   */
    S_JOB_ID,                /* SLURM job id (uint32_t *)                    */
    S_JOB_STEPID,            /* SLURM job step id (uint32_t *)               */
    S_JOB_NNODES,            /* Total number of nodes in job (uint32_t *)    */
    S_JOB_NODEID,            /* Relative id of this node (uint32_t *)        */
    S_JOB_LOCAL_TASK_COUNT,  /* Number of local tasks (uint32_t *)           */
    S_JOB_TOTAL_TASK_COUNT,  /* Total number of tasks in job (uint32_t *)    */
    S_JOB_NCPUS,             /* Number of CPUs used by this job (uint16_t *) */
    S_JOB_ARGV,              /* Command args (int *, char ***)               */
    S_JOB_ENV,               /* Job env array (char ***)                     */
    S_TASK_ID,               /* Local task id (int *)                        */
    S_TASK_GLOBAL_ID,        /* Global task id (uint32_t *)                  */
    S_TASK_EXIT_STATUS,      /* Exit status of task if exited (int *)        */
    S_TASK_PID,              /* Task pid (pid_t *)                           */
    S_JOB_PID_TO_GLOBAL_ID,  /* global task id from pid (pid_t, uint32_t *)  */
    S_JOB_PID_TO_LOCAL_ID,   /* local task id from pid (pid_t, uint32_t *)   */
    S_JOB_LOCAL_TO_GLOBAL_ID,/* local id to global id (uint32_t, uint32_t *) */
    S_JOB_GLOBAL_TO_LOCAL_ID,/* global id to local id (uint32_t, uint32_t *) */
    S_JOB_SUPPLEMENTARY_GIDS,/* Array of suppl. gids (gid_t **, int *)       */
    S_SLURM_VERSION,         /* Current SLURM version (char **)              */
    S_SLURM_VERSION_MAJOR,   /* SLURM version major release (char **)        */
    S_SLURM_VERSION_MINOR,   /* SLURM version minor release (char **)        */
    S_SLURM_VERSION_MICRO,   /* SLURM version micro release (char **)        */
    S_STEP_CPUS_PER_TASK,    /* CPUs allocated per task (=1 if --overcommit
                              * option is used, uint32_t *)                  */
    S_JOB_ALLOC_CORES,       /* Job allocated cores in list format (char **) */
    S_JOB_ALLOC_MEM,         /* Job allocated memory in MB (uint64_t *)      */
    S_STEP_ALLOC_CORES,      /* Step alloc'd cores in list format  (char **) */
    S_STEP_ALLOC_MEM,        /* Step alloc'd memory in MB (uint64_t *)       */
    S_SLURM_RESTART_COUNT    /* Job restart count (uint32_t *)               */
};



