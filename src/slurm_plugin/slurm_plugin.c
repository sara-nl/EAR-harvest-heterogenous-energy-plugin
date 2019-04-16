/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
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

#include <slurm_plugin/slurm_plugin.h>

// Spank
SPANK_PLUGIN(EAR_PLUGIN, 1)

// Buffers
char buffer1[SZ_PATH];
char buffer2[SZ_PATH];
char buffer3[SZ_PATH];

//
extern int eard_exst;

int slurm_spank_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_init");

	_opt_register(sp);

	if (plug_env_islocal(sp))
	{
		if (spank_context() == S_CTX_SRUN) {
			plug_env_setenv(sp, ENV_PLG_CTX, "SRUN", 1);
		}
		if (spank_context() == S_CTX_SBATCH) {
			plug_env_setenv(sp, ENV_PLG_CTX, "SBATCH", 1);
		}

		plug_env_setenv(sp, ENV_PLG_EN, "1", 1);
	}

	return ESPANK_SUCCESS;
}

int slurm_spank_init_post_opt(spank_t sp, int ac, char **av)
{
    plug_verbose(sp, 2, "function slurm_spank_init_post_opt");

	// No need of testing the context, post_opt is always local
	if(!plug_env_isenv(sp, ENV_PLG_EN, "1")) {
		return ESPANK_SUCCESS;
	}

	plug_env_clean(sp, ac, av);

	// Reading plugstack.conf
	if (plug_env_readstack(sp, ac, av) != ESPANK_SUCCESS) {
		plug_env_setenv(sp, ENV_PLG_EN, "0", 1);
		return ESPANK_SUCCESS;
	}

	// Filling user data
	if (plug_env_readuser(sp) != ESPANK_SUCCESS) {
		plug_env_setenv(sp, ENV_LIB_EN, "0", 1);
		return ESPANK_SUCCESS;
	}

	//
	if (spank_context() == S_CTX_SRUN && plug_env_isenv(sp, ENV_LIB_EN, "1")) {
		plug_env_setpreload(sp);
	}

    return (ESPANK_SUCCESS);
}

int slurm_spank_user_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_user_init");

	if(!plug_env_isenv(sp, ENV_PLG_EN, "1")) {
		return (ESPANK_SUCCESS);
	}

	//
	if (spank_context() == S_CTX_REMOTE) {
		plug_rcom_eard_job_start(sp);
	}
	
	return (ESPANK_SUCCESS);
}

int slurm_spank_task_exit (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_task_exit");

	spank_err_t err;
	int status = 0;

	// Not needed to test if the plugin is enabled
	if (eard_exst == 0)
	{
		err = spank_get_item (sp, S_TASK_EXIT_STATUS, &status);

		if (err == ESPANK_SUCCESS) {
			eard_exst = WEXITSTATUS(status);
		}
	}

	return (ESPANK_SUCCESS);
}

int slurm_spank_exit (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "slurm_spank_exit");

	// Not needed to test if the plugin is enabled
	if (spank_context() == S_CTX_REMOTE) {
		plug_rcom_eard_job_finish(sp);
	}

	return (ESPANK_SUCCESS);
}

#if 0
int slurm_spank_slurmd_init (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_slurmd_init");
	return (ESPANK_SUCCESS);
}

int slurm_spank_slurmd_exit (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_slurmd_exit");
	return (ESPANK_SUCCESS);
}

int slurm_spank_job_prolog (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_job_prolog");
	return (ESPANK_SUCCESS);
}

int slurm_spank_task_init_privileged (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_task_init_privileged");
	return (ESPANK_SUCCESS);
}

int _slurm_spank_local_user_init (spank_t sp, int ac, char **av)
{
        plug_verbose(sp, 2, "function slurm_spank_local_user_init");
        return (ESPANK_SUCCESS);
}

int _slurm_spank_init_post_opt(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_init_post_opt");
	return (ESPANK_SUCCESS);
}

int slurm_spank_task_init (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_task_init");
	return (ESPANK_SUCCESS);
}

int slurm_spank_task_post_fork (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_task_post_fork");
	return (ESPANK_SUCCESS);
}

int slurm_spank_job_epilog (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_job_epilog");
	return (ESPANK_SUCCESS);
}
#endif
