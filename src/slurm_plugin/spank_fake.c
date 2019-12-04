#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_environment.h>

int _sp;
int _at;
int _fu;

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

spank_err_t spank_option_register (spank_t spank, struct spank_option *opt)
{
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
	printf("\t--context=<arg>\t\tSets the SLURM like context [srun,remote,full]\n");
	printf("\t--action=<arg>\t\tSets the action [init,exit]\n");
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
		if ((strlen(av[i]) > 8) && (strncmp("--context=", av[i], 10) == 0))
		{
			if (strncmp ("--context=srun", av[i], 14) == 0) {
				_sp = Context.srun;
			} else if (strncmp ("--context=full", av[i], 14) == 0) {
				_fu = 1;
			} else if (strncmp ("--context=remote", av[i], 16) == 0) {
				_sp = Context.remote;
			} else {
				return 1;
			}
		}
		if ((strlen(av[i]) > 8) && (strncmp("--action=", av[i], 9) == 0))
		{
			if (strncmp ("--action=init", av[i], 13) == 0) {
				_at = Action.init;
			} else if (strncmp ("--action=exit", av[i], 13) == 0) {
				_at = Action.exit;
			} else {
				return 1;
			}
		}
	}


	plug_verbose(_sp, 2, "arguments %d %d %d", _sp, _at, _fu);

	if (_fu == 1 && _at == Action.init) {
		_sp = Context.srun;
		_fu = Context.remote;
	} else if (_fu == 1 && _at == Action.exit) {
		_sp = Context.remote;
		_fu = Context.srun;
	} else if (_fu == 1) {
		return 1;
	} if (_sp == Context.error || _at == Action.error) {
		return 1;
	}

	return 0;
}

int pipeline(int argc, char *argv[])
{
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

int main(int argc, char *argv[])
{
	//
	if (arguments(argc, argv)) {
		return help(argc, argv);
	}

	//
	pipeline(argc, argv);

	if (_fu) {
		_sp = _fu;
		pipeline(argc, argv);
	}

	return 0;
}
