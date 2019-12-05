#include <common/system/file.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_environment.h>

char *program;
int _fd;
int _sp;
int _at;

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

spank_err_t spank_option_register (spank_t sd, struct spank_option *opt)
{
	plug_verbose(sd, 0, "--%s\t%s",
				 opt->name, opt->usage);

	return ESPANK_SUCCESS;
}

spank_err_t spank_get_item (spank_t spank, spank_item_t item, ...)
{
	return ESPANK_ERROR;
}

char *slurm_hostlist_shift (hostlist_t host_list)
{
	static int count = 1;
        
	if (count == 1) {
                count -= 1;
                return host_list;
        } else {
                return NULL;
        }
}

hostlist_t slurm_hostlist_create (char *node_list)
{
	char *copy = malloc(strlen(node_list));
	return strcpy(copy, node_list);
}

int help(int argc, char *argv[])
{
	// --context=[srun,remote,full]
	// --action=[init,exit]
	// --plugstack args
	printf("Usage: %s [OPTIONS]\n", argv[0]);
	printf("\nOptions:\n");
	printf("\t--program=<arg>\t\tSets the program to run.\n");
	printf("\t--plugstack [ARGS]\tSet the SLUSM's plugstack arguments. I.e:\n");
	printf("\t\t\t\t--plugstack prefix=/hpc/opt/ear default=on...\n");
	printf("\n");

	return 0;
}

int arguments(int ac, char *av[])
{
	int i;

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
			program = &av[i][10];
		}
	}

	return 0;
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
			slurm_spank_user_init(_sp, argc, argv);
		}
	}
	else if(plug_action_is(_at, Action.exit))
	{
		if (plug_context_is(_sp, Context.remote)) {
			slurm_spank_task_exit(_sp, argc, argv);
		}

		slurm_spank_exit(_sp, argc, argv);
	}

	return ESPANK_SUCCESS;
}

int lock(int argc, char *argv[])
{
	_fd = file_lock_master("fake_slurm.lock");

	if (_fd > 0) {
		plug_verbose(_sp, 2, "got the lock file, pipelining");
		return 1;
	}
	
	_fd = 0;
	plug_verbose(_sp, 2, "file locked, skipping");
	
	return 0;
}

int unlock(int argc, char *argv[])
{
	if (_fd == 0) {
		return 0;
	}
	
	plug_verbose(_sp, 2, "sleeping 2 seconds to avoid fast programs to take the lock file");
	sleep(2);	

	file_unlock_master(_fd, "fake_slurm.lock");
	
	return 1;
}

int execute(int argc, char *argv[])
{
	system(program);
	return 0;
}

int main(int argc, char *argv[])
{
	//
	if (arguments(argc, argv))
	{
		help(argc, argv);

		pipeline(argc, argv, Context.error, Action.init);

		return 0;
	}

	//
	if (lock(argc, argv))
	{
		//
		pipeline(argc, argv, Context.srun, Action.init);
		//
		pipeline(argc, argv, Context.remote, Action.init);
	}

	execute(argc, argv);

	if (unlock(argc, argv)) {
		//
		pipeline(argc, argv, Context.remote, Action.exit);
		//
		pipeline(argc, argv, Context.srun, Action.exit);
	}
	
	return 0;
}
