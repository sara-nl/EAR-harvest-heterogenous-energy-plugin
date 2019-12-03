#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_environment.h>

int _sp;
int _at;
int _fu;

spank_err_t spank_getenv (spank_t spank, const char *var, char *buf, int len)
{
	char *c;

	if (var == NULL || buf == NULL) {
		return 0;
	}
	if ((c = getenv(var)) == NULL) {
		return 0;
	}

	snprintf(buf, len, "%s", c);
	return 1;
}

spank_err_t spank_setenv (spank_t spank, const char *var, const char *val, int overwrite)
{
	if (var == NULL || val == NULL) {
		return 0;
	}
	if (setenv (var, val, ow) == -1) {
		return 0;
	}
	return 1;
}

spank_err_t spank_unsetenv (spank_t spank, const char *var)
{
	if (var == NULL) {
		return 0;
	}
	return unsetenv(var) == 0;
}

void slurm_error (const char *format, ...)
{
	printf(format);
	printf("\n");
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
	return host_list;
}

hostlist_t slurm_hostlist_create (const char *node_list) {
	static count = 1;

	if (count == 1) {
		count -= 1;
		return node_list;
	} else {
		return NULL;
	}
}

int arguments(int argc, char *argv[])
{
	// --context=[srun,remote,full]
	// --action=[init,exit]
	// --plugstack args
	printf("Usage: %s [OPTIONS]\n", argv[0]);
	printf("\nOptions:");
	printf("\t--context=<arg>\tSets the SLURM like context [srun,remote,full]");
	printf("\t--action=<arg>\tSets the action [init,exit]");
	printf("\t--plugstack [ARGS]\tSet the SLUSM's plugstack arguments. I.e:");
	printf("\t\t\t\t--plugstack prefix=/hpc/opt/ear sysconfdir=/hpc/opt/ear/etc ...");

	return 0;
}

int arguments(int argc, char *argv[])
{
	int i;

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 8) && (strncmp("--help", av[i], 6) == 0))
		{
			return 1;
		}
	}

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 8) && (strncmp("--context=", av[i], 10) == 0))
		{
			if (strncmp ("--context=srun", av[i], 14) == 0) {
				_sp = S_CTX_SRUN;
			} else if (strncmp ("--context=full", av[i], 14) == 0) {
				_sp = S_CTX_SRUN;
				_fu = 1;
			} else if (strncmp ("--context=remote", av[i], 16) == 0) {
				_sp = S_CTX_REMOTE;
			} else {
				return 1;
			}
		}
		if ((strlen(av[i]) > 8) && (strncmp("--action=", av[i], 9) == 0))
		{
			if (strncmp ("--action=init", av[i], 13) == 0) {
				_at = S_ACT_INIT;
			} else if (strncmp ("--action=exit", av[i], 13) == 0) {
				_at = S_ACT_EXIT;
			} else {
				return 1;
			}
		}
	}

	return 0;
}

int pipeline(int argc, char *argv[])
{
	if (plug_action_is(_at, S_ACT_INIT))
	{
		slurm_spank_init(_sp, argc, argv);

		if (plug_context_is(_sp, S_CTX_SRUN))
		{
			slurm_spank_init_post_opt(_sp, argc, argv);

			slurm_spank_local_user_init(_sp, argc, argv);
		}
		else if (plug_context_is(_sp, S_CTX_REMOTE))
		{
			slurm_spank_user_init(_sp, argc, argv);
		}
	}
	else if(plug_action_is(_at, S_ACT_EXIT))
	{
		if (plug_context_is(_sp, S_CTX_REMOTE)) {
			slurm_spank_task_exit(_sp, argc, argv);
		}

		slurm_spank_exit(_sp, argc, argv);
	}

	return ESPANK_SUCCESS;
}

int main(int argc, char *argv[])
{
	_sp = S_CTX_LOCAL;
	_at = S_ACT_INIT;

	//
	if (arguments(argc, argv)) {
		return help(argc, argv);
	}

	//
	pipeline(argc, argv);

	if (_fu)
	{
		_sp = S_CTX_REMOTE;

		pipeline(argc, argv);
	}

	return 0;
}