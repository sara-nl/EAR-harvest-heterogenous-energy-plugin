#include <common/system/file.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_environment.h>
#include <slurm_plugin/slurm_plugin_serialization.h>

//
char plug_tmp[SZ_NAME_LARGE];
char plug_etc[SZ_NAME_LARGE];
char plug_pfx[SZ_NAME_LARGE];
char plug_def[SZ_NAME_LARGE];
char path_lock[SZ_PATH];
char path_prog[SZ_PATH];
char buffer[SZ_PATH];

//
char **_argv;
int    _argc;

//
extern plug_serialization_t sd;

//
int   _master;
int   _fd;
int   _sp;
int   _at;

spank_err_t spank_getenv (spank_t spank, const char *var, char *buf, int len)
{
	char *c;

	if (var == NULL || buf == NULL) {
		return ESPANK_ERROR;
	}
	if ((c = getenv(var)) == NULL) {
		return ESPANK_ERROR;
	}

	snprintf(buf, len, "%s", c);
	return ESPANK_SUCCESS;
}

spank_err_t spank_setenv (spank_t spank, const char *var, const char *val, int overwrite)
{
	if (var == NULL || val == NULL) {
		return ESPANK_ERROR;
	}
	if (setenv (var, val, overwrite) == -1) {
		return ESPANK_ERROR;
	}
	return ESPANK_SUCCESS;
}

spank_err_t spank_unsetenv (spank_t spank, const char *var)
{
	if (var == NULL) {
		return ESPANK_ERROR;
	}
	return unsetenv(var) == 0;
}

int plug_action_is(int _ac, int action)
{
	return _ac == action;
}

spank_context_t spank_context (void)
{
	return _sp;
}

spank_err_t spank_get_item (spank_t spank, spank_item_t item, int *p)
{
	if (item == S_JOB_ID) {
		*p = atoi(getenv("SLURM_JOB_ID"));
	} else if (item == S_JOB_STEPID) {
		*p = atoi(getenv("SLURM_STEP_ID"));
	} else {
		*p = 0;
	}
	return ESPANK_SUCCESS;
}

char *slurm_hostlist_shift (hostlist_t host_list)
{
	static int count = 1;
        
	if (count == 1) {
                count = 0;
                return host_list;
        } else {
		count = 1;
                return NULL;
        }
}

hostlist_t slurm_hostlist_create (char *node_list)
{
	char *copy = malloc(strlen(node_list));
	return strcpy(copy, node_list);
}

spank_err_t spank_option_register_print(spank_t sp, struct spank_option *opt)
{
	int n = strlen(opt->usage);
	int i = 0;
	int o = 0;
	char c;

	printf("\t--%s\t\t", opt->name);
	if (strlen(opt->name) < 5) {
		printf("\t");
	}
	if (n < 64) {
		printf("%s\n", opt->usage);
		return ESPANK_SUCCESS;
	}
	while (i < n)
	{
		c = opt->usage[i];
		printf("%c", c);
		o = (o == 1) | (i != 0 && (i % 64) == 0);
		if (o && (c == ' ' || c == ',')) {
			printf("\n\t\t\t\t");
			o = 0;
		}
		i += 1;
	}
	printf("\n");

	return ESPANK_SUCCESS;
}

spank_err_t spank_option_register_call(int argc, char *argv[], spank_t sp, struct spank_option *opt)
{
	char *p;

	sprintf(buffer, "--%s=", opt->name);
	p = plug_acav_get(argc, argv, buffer);
	
	if (p == NULL) {
		return ESPANK_SUCCESS;
	} if (strlen(p) == 0) {
		return ESPANK_SUCCESS;
	}

	opt->cb(0, p, 0);
	
	return ESPANK_SUCCESS;
}

spank_err_t spank_option_register(spank_t sp, struct spank_option *opt)
{
	if (plug_context_is(sp, Context.error)) {
		return spank_option_register_print(sp, opt);
	} else if (plug_context_is(sp, Context.srun)) {
		return spank_option_register_call(_argc, _argv, sp, opt);
	}
	return ESPANK_SUCCESS;
}

int help(int argc, char *argv[])
{
	printf("Usage: %s [OPTIONS]\n", argv[0]);
	printf("\nOptions:\n");
	printf("\t--nodes=<arg>\t\tSets the number of nodes.\n");
	printf("\t--program=<arg>\t\tSets the program to run.\n");
	printf("\t--plugstack [ARGS]\tSet the SLURM's plugstack arguments. I.e:\n");
	printf("\t\t\t\t--plugstack prefix=/hpc/opt/ear default=on...\n");
	printf("SLURM options:\n");

	return 0;
}

int lock(int argc, char *argv[])
{
	sprintf(path_lock, "/tmp/earplug.lock");

	_fd = file_lock_master(path_lock);

	if (_fd > 0) {
		return 1;
	}
	
	_fd = 0;
	return 0;
}

int spinlock(int argc, char *argv[])
{
	while (access(path_lock, F_OK) == 0);
}

int unlock(int argc, char *argv[])
{
	if (_fd == 0) {
		return 0;
	}
	
	file_unlock_master(_fd, path_lock);
	
	return 1;
}

int fake_slurm_spank_user_init(spank_t sp, int ac, char **av)
{
	int plug_shared_readsetts(spank_t sp, plug_serialization_t *sd);
	plug_verbose(sp, 2, "function fake_slurm_spank_user_init");

	//
	plug_deserialize_remote(sp, &sd);
	
	if (!plug_component_isenabled(sp, Component.plugin)) {
		return ESPANK_SUCCESS;
	}

	//
	if (sd.subject.context_local == Context.srun)
	{
		if (plug_component_isenabled(sp, Component.library))
		{
			plug_shared_readsetts(sp, &sd);

			plug_serialize_task(sp, &sd);
		}
	}
	
	return ESPANK_SUCCESS;
}

int pipeline(int argc, char *argv[], int sp, int at)
{
	_sp = sp;
	_at = at;

	if (plug_action_is(_at, Action.init))
	{
		slurm_spank_init(_sp, argc, argv);

		if (plug_context_is(_sp, Context.srun))
		{
			slurm_spank_init_post_opt(_sp, argc, argv);

			slurm_spank_local_user_init(_sp, argc, argv);
		}
		else if (plug_context_is(_sp, Context.remote))
		{
			if (_master) {
				slurm_spank_user_init(_sp, argc, argv);
			} else {
				fake_slurm_spank_user_init(_sp, argc, argv);
			}

		}
	}
	else if(plug_action_is(_at, Action.exit))
	{
		if (plug_context_is(_sp, Context.remote))
		{
			slurm_spank_task_exit(_sp, argc, argv);

			if (_master) {
				slurm_spank_exit(_sp, argc, argv);
			}
		}
			
		if (plug_context_is(_sp, Context.srun)) {
			slurm_spank_exit(_sp, argc, argv);
		}
	}

	return ESPANK_SUCCESS;
}

int print_argv(int argc, char *argv[])
{
	int i;
	int j;

	for (i = 0, j = 0; i < argc; ++i) {
		strcpy(&buffer[j], argv[i]);
		j += strlen(argv[i])+1;
		buffer[j-1] = ' ';
	}
	buffer[j] = '\0';
	plug_verbose(_sp, 3, "input: %s", buffer);
}

int job(int argc, char *argv[])
{
	char *p = NULL;
	int i = 0;
	int h = 0;

	// Input parameters initial
	print_argv(argc, argv);

	// Help?
	for (i = 0; i < argc; ++i) {
		if ((strlen(argv[i]) >= 6) && (strncmp("--help", argv[i], 6) == 0)) {
			h = 1;
		}
	}

	// Setting the program and the job name
	for (i = 0; i < argc; ++i) {
		if ((strlen(argv[i]) > 8) && (strncmp("--program=", argv[i], 10) == 0)) {
			p = &argv[i][10];
		}
	}

	if (p != NULL) {
		// Setting the job name
		sprintf(path_prog, "%s", p);
		setenv("SLURM_JOB_NAME", p, 1);
	} else {
		h = 1;
	}

	// Converting configuration enrivonment variables 
	// (INSTALL_PATH, ETC, TMP) in input parameters.
	_argc = argc + 4;
	_argv = malloc(sizeof(char **) * _argc);

	_argv[argc + 0] = plug_def;
	_argv[argc + 1] = plug_pfx;
	_argv[argc + 2] = plug_etc;
	_argv[argc + 3] = plug_tmp;
	
	for (i = 0; i < argc; ++i) {
		_argv[i] = argv[i];
	}

	sprintf(plug_def, "default=on");
	if ((p = getenv("EAR_INSTALL_PATH")) != NULL) {
		sprintf(plug_pfx, "prefix=%s", p);
	}
	if ((p = getenv("EAR_ETC")) != NULL) {
		sprintf(plug_etc, "sysconfdir=%s", p);
	}
	if ((p = getenv("EAR_TMP")) != NULL) {
		sprintf(plug_tmp, "localstatedir=%s", p);
	}
	
	for (i = 0; i < argc; ++i) {
		_argv[i] = argv[i];
	}

	// Getting the job id (it cant be created,
	// depends on the queue manager:
	if ((p = getenv("SLURM_JOB_ID")) == NULL) {
		setenv("SLURM_JOB_ID", "0", 1);
	}
	if (getenv("SLURM_STEP_ID") == NULL) {
		setenv("SLURM_STEP_ID", "0", 1);
	}

	// Getting the number of nodes
	for (i = 0; i < argc; ++i) {
		if ((strlen(argv[i]) > 7) && (strncmp("--nodes=", argv[i], 8) == 0)) {
			setenv("SLURM_NNODES", &argv[i][8], 1);
		}
	}

	char *nnodes = getenv("SLURM_NNODES");
	
	if (nnodes == NULL)
	{
		char *size_world = getenv("OMPI_COMM_WORLD_SIZE");
		char *size_local = getenv("OMPI_COMM_WORLD_LOCAL_SIZE");
		char nnodes_b[8];
		int  nnodes_i;

		if (size_world != NULL && size_local != NULL) {
			nnodes_i = atoi(size_world) / atoi(size_local);
			sprintf(nnodes_b, "%d", nnodes_i);
			nnodes = nnodes_b;
		}
	}
	
	if (nnodes != NULL) {
		setenv("SLURM_STEP_NUM_NODES", nnodes, 1);
	}

	// Input parameters final
	print_argv(_argc, _argv);
	plug_verbose(_sp, 3, "program: '%s'", path_prog);

	return h;
}

int execute(int argc, char *argv[])
{
	system(path_prog);
	return 0;
}

int plugin_enabled(int argc, char *argv[])
{
	return getenv(Var.context.rem) != NULL;
}

int main(int argc, char *argv[])
{
	// Arguments reading
	if (job(argc, argv))
	{
		help(_argc, _argv);
		// No context initializations
		pipeline(_argc, _argv, Context.error, Action.init);
		// no context finalization
		pipeline(_argc, _argv, Context.error, Action.exit);

		return 0;
	}

	// In case the plugin was enabled, just execute
	if (plugin_enabled(_argc, _argv)) {
		return execute(_argc, _argv);
	}

	// The master simulation
	if (lock(_argc, _argv)) {
fprintf(stderr, "MASTER\n");
		plug_verbose(_sp, 2, "got the lock file");
		_master = 1;
	} else {
fprintf(stderr, "NO MASTER\n");
		plug_verbosity_silence(_sp);
		spinlock(_argc, _argv);
	}

	// Local context initialization
	pipeline(_argc, _argv, Context.srun, Action.init);
	// Remote context initialization
	pipeline(_argc, _argv, Context.remote, Action.init);

	if (_master) {
		sleep(1);
		unlock(_argc, _argv);
	}

	execute(_argc, _argv);

	if (_master) {
		sleep(2);
	}

	// Remote context finalization
	pipeline(_argc, _argv, Context.remote, Action.exit);
	// Local context finalization
	pipeline(_argc, _argv, Context.srun, Action.exit);
	
	return 0;
}
