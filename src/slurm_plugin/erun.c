#include <common/system/file.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_environment.h>
#include <slurm_plugin/slurm_plugin_serialization.h>

extern plug_serialization_t sd;
char path_lock[SZ_PATH];
int    _master;
char **_argv;
int    _argc;
char *_pr;
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

	plug_verbose(sp, 0, "\t--%s\t\t", opt->name);
	if (strlen(opt->name) < 5) {
		plug_verbose(sp, 0, "\t");
	}
	if (n < 64) {
		plug_verbose(sp, 0, "%s\n", opt->usage);
		return ESPANK_SUCCESS;
	}
	while (i < n)
	{
		c = opt->usage[i];
		plug_verbose(sp, 0, "%c", c);
		o = (o == 1) | (i != 0 && (i % 64) == 0);
		if (o && (c == ' ' || c == ',')) {
			plug_verbose(sp, 0, "\n\t\t\t\t");
			o = 0;
		}
		i += 1;
	}
	plug_verbose(sp, 0, "\n");

	return ESPANK_SUCCESS;
}

spank_err_t spank_option_register_call(int argc, char *argv[], spank_t sp, struct spank_option *opt)
{
	char buffer[256];	
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
	plug_verbose(_sp, 0, "Usage: %s [OPTIONS]\n", argv[0]);
	plug_verbose(_sp, 0, "\nOptions:\n");
	plug_verbose(_sp, 0, "\t--program=<arg>\t\tSets the program to run.\n");
	plug_verbose(_sp, 0, "\t--plugstack [ARGS]\tSet the SLURM's plugstack arguments. I.e:\n");
	plug_verbose(_sp, 0, "\t\t\t\t--plugstack prefix=/hpc/opt/ear default=on...\n");
	plug_verbose(_sp, 0, "SLURM options:\n");

	return 0;
}

int arguments(int ac, char *av[])
{
	int i;

	_argc = ac;
	_argv = av;
	_sp = Context.error;
	_at = Action.error;

	for (i = 0; i < ac; ++i) {
		if ((strlen(av[i]) >= 6) && (strncmp("--help", av[i], 6) == 0)) {
			return 1;
		}
	}

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 8) && (strncmp("--program=", av[i], 10) == 0))
		{
			_pr = &av[i][10];
		}
	}

	if (_pr == NULL) {
		return 1;
	}

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
			
			slurm_spank_exit(_sp, argc, argv);
		}
			
		if (plug_context_is(_sp, Context.srun)) {
			slurm_spank_exit(_sp, argc, argv);
		}
	}

	return ESPANK_SUCCESS;
}

int job(int argc, char *argv[])
{
	setenv("SLURM_JOB_ID", "1", 1);
	setenv("SLURM_STEP_ID", "0", 1);
	setenv("SLURM_JOB_NAME", _pr, 1);

	//	
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
	
	//setenv("EAR_DYNAIS_WINDOW_SIZE", "200", 1);
	//setenv("EAR_DYNAIS_LEVELS", "10", 1);
	//setenv("SLURM_STEP_NODELIST", "cmp2545", 1);
	//setenv("SLURM_JOB_ACCOUNT", "xbsc", 1);
	//setenv("SLURM_STEPID", "0", 1);
	//setenv("SLURM_NNODES", "1", 1);
	//setenv("EAR_NUM_NODES", "1", 1);
	//setenv("SLURM_COMP_LIBRARY", "1", 1);
	//setenv("SLURM_COMP_PLUGIN", "1", 1);

	return 0;
}

int execute(int argc, char *argv[])
{
	system(_pr);
	return 0;
}

int main(int argc, char *argv[])
{
	//
	if (arguments(argc, argv))
	{
		help(argc, argv);
		//
		pipeline(argc, argv, Context.error, Action.init);
		//
		pipeline(argc, argv, Context.error, Action.exit);

		return 0;
	}


	job(argc, argv);
	
	if (lock(argc, argv)) {
		plug_verbose(_sp, 2, "got the lock file");
		
		_master = 1;
	} else {
		plug_verbosity_silence(_sp);
		
		spinlock(argc, argv);
		
		//fprintf(stderr, "skipping spinlock\n");
	}

	//
	pipeline(argc, argv, Context.srun, Action.init);
	//
	pipeline(argc, argv, Context.remote, Action.init);

	if (_master) {
		unlock(argc, argv);
	}

	execute(argc, argv);

	if (_master) {
		sleep(2);
	}

	//
	pipeline(argc, argv, Context.remote, Action.exit);
	//
	pipeline(argc, argv, Context.srun, Action.exit);
	
	return 0;
}
