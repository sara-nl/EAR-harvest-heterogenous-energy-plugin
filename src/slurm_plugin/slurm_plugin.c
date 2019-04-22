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
#include <slurm_plugin/slurm_plugin_rcom.h>
#include <slurm_plugin/slurm_plugin_options.h>
#include <slurm_plugin/slurm_plugin_environment.h>
#include <slurm_plugin/slurm_plugin_serialization.h>

// Spank
SPANK_PLUGIN(EAR_PLUGIN, 1)

//
static plug_serialization_t sd;

// Function order:
// 	- Local 1
// 	- Remote 1
int slurm_spank_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_init");

	_opt_register(sp);

	return ESPANK_SUCCESS;
}

// Function order:
// 	- Local 2
// 	- Remote 0
int slurm_spank_init_post_opt(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_init_post_opt");

	// Disabling policy
	// 	- Disable plugin means no plugin, so no serialization, no library
	//	- Disable library means the plugin works but no library is loaded
	// ADVISE! No need of testing the context, post_opt is always local

	if (plug_context_is(sp, Context.local)) {
		plug_component_setenabled(sp, Component.plugin, 1);
	}

	// Filling job data
	if (plug_deserialize_local(sp, &sd) != ESPANK_SUCCESS) {
		plug_component_setenabled(sp, Component.plugin, 0);
		return ESPANK_SUCCESS;
	}

	// Reading plugstack.conf file
	if (plug_read_plugstack(sp, ac, av, &sd) != ESPANK_SUCCESS) {
		plug_component_setenabled(sp, Component.plugin, 0);
		return ESPANK_SUCCESS;
	}

	// EARGMD connection
	if (plug_component_isenabled(sp, Component.plugin)) {
		plug_rcom_eargmd_job_start(sp, &sd);
	}

	// The serialization enables the LD_PRELOAD library
	if (plug_context_is(sp, Context.local) && plug_component_isenabled(sp, Component.library)) {
		plug_serialize_remote(sp, &sd);
	}
	
	return ESPANK_SUCCESS;
}

// Function order:
// 	- Local 0
// 	- Remote 2
int slurm_spank_user_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_user_init");

	if (!plug_context_is(sp, Context.remote)) {
		return ESPANK_SUCCESS;
	}

	//
	plug_deserialize_remote(sp, &sd);

	if (!plug_component_isenabled(sp, Component.plugin)) {
		return ESPANK_SUCCESS;
	}

	// If no shared services, EARD contact won't work, so plugin disabled
	if (plug_shared_readservs(sp, &sd) != ESPANK_SUCCESS) {
		plug_component_setenabled(sp, Component.plugin, 0);
		return ESPANK_SUCCESS;
	}

	// If no frequencies the EARD contact can be done, so library is disabled
	if (plug_shared_readfreqs(sp, &sd) != ESPANK_SUCCESS) {
		plug_component_setenabled(sp, Component.library, 0);
	}

	// The application can be filled as an empty object if something happen
	plug_read_application(sp, &sd);

	// The node list is filled with at least the current node
	plug_read_hostlist(sp, &sd);

	// EARD/s connection/s
	plug_rcom_eard_job_start(sp);

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

// Function order:
// 	- Local 0
// 	- Remote 3e
int slurm_spank_task_exit (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_task_exit");

	spank_err_t err;
	int status = 0;

	// ADVISE! No need of testing anything
	if (job.exit_status == 0)
	{
		err = spank_get_item (sp, S_TASK_EXIT_STATUS, &status);

		if (err == ESPANK_SUCCESS) {
			job.exit_status = WEXITSTATUS(status);
		}
	}

	return ESPANK_SUCCESS;
}

int slurm_spank_exit (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "slurm_spank_exit");

	// EARD disconnection
	if (plug_context_is(sp, Context.remote))
	{
		plug_read_hostlist(sp, &sd);

		plug_rcom_eard_job_finish(sp);
	}

	// EARGMD disconnection
	if (plug_context_is(sp, Context.local)) {
		plug_rcom_eargmd_job_finish(sp, &sd);
	}

	return ESPANK_SUCCESS;
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
