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

#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <slurm/spank.h>
#include <cpufreq.h>

#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>
#include <slurm_plugin/slurm_plugin_options.h>
#include <common/types/application.h>
#include <common/types/job.h>
#include <common/config.h>

int EAR_VERBOSE_LEVEL = 0;
int verbosity = -1;

SPANK_PLUGIN(EAR_PLUGIN, 1)
pid_t daemon_pid = -1;

char buffer1[PATH_MAX];
char buffer2[PATH_MAX];
unsigned int num_nodes;
application_t app;

#define ESPANK_STOP -1

/*
 *
 *
 *
 *
 *
 */

static int local_update_ld_preload(spank_t sp)
{
    plug_verbose(sp, 2, "function local_update_ld_preload");
	
	char *ear_root_dir = NULL;
	char *ld_preload = NULL;

	buffer1[0] = '\0';
	buffer2[0] = '\0';

	/*if (getenv_local("LD_PRELOAD", &ld_preload) == 1) {
		strcpy(buffer1, ld_preload);
	}*/

    if (getenv_local("EAR_ROOT_DIR", &ear_root_dir) == 0)
    {
		plug_error("Error, missing EAR_ROOT_DIR");
        return ESPANK_ERROR;
    }
    appendenv(buffer1, ear_root_dir, PATH_MAX);
    
	// Appending libraries to LD_PRELOAD
    if (isenv_local("EAR_TRACES", "1") == 1)
	{
		if (isenv_local("EAR_MPI_DIST", "openmpi")) {
        	sprintf(buffer2, "%s/%s", buffer1, OMPI_TRACE_LIB_PATH);
		} else {
        	sprintf(buffer2, "%s/%s", buffer1, IMPI_TRACE_LIB_PATH);
		}
    }
	else
	{	
		if (isenv_local("EAR_MPI_DIST", "openmpi")) {
			sprintf(buffer2, "%s/%s", buffer1, OMPI_LIB_PATH);
		} else { 
			sprintf(buffer2, "%s/%s", buffer1, IMPI_LIB_PATH);
		}
    }

    //
    if(setenv_local("LD_PRELOAD", buffer2, 1) != 1) {
		return ESPANK_ERROR;
	}

	plug_verbose(sp, 2, "updated LD_PRELOAD envar '%s'", buffer2);

	return ESPANK_SUCCESS;
}

static void remote_update_slurm_vars(spank_t sp)
{
	plug_verbose(sp, 2, "function remote_update_slurm_vars");

    char p_state[64];
    int p_freq = 1;

    // If policy is monitoring
    if(isenv_remote(sp, "EAR_POWER_POLICY", "MONITORING_ONLY") == 1)
    {
        // If learning phase is not enabled
        if ((existenv_remote(sp, "EAR_LEARNING_PHASE") == 0) ||
            (isenv_remote(sp, "EAR_LEARNING_PHASE", "0") == 1))
        {
			printenv_remote(sp, "SLURM_CPU_FREQ_REQ");
            // If this is passed SLURM's --cpu-freq argument
            if (getenv_remote(sp, "SLURM_CPU_FREQ_REQ", p_state, 64) == 1)
            {
                p_freq = atoi(p_state);
                p_freq = freq_to_p_state(p_freq);

                sprintf(p_state, "%d", p_freq);
                setenv_remote(sp, "EAR_P_STATE", p_state, 0);

				plug_verbose(sp, 2, "Updating to P_STATE '%s' by '--cpu-freq=%d' command",
							 p_state, p_freq);
            }
        }
    }

    // Switching from SLURM_JOB_NAME to EAR_APP_NAME
    if (getenv_remote(sp, "SLURM_JOB_NAME", buffer2, PATH_MAX) == 1) {
        setenv_remote(sp, "EAR_APP_NAME", buffer2, 1);
    }
}

/*
 *
 * Configuration
 *
 */
int read_old_configuration_file(spank_t sp)
{
	plug_verbose(sp, 2, "function file_to_environment");


	const char *value = NULL;
	char *option = buffer2;
	char *path = buffer1;
	FILE *file;
	int r;

	if (!getenv_local("EAR_ETCDIR", &path)) {
		plug_error("while searching configuration file %s", path);
		return ESPANK_ERROR;
	}

	if ((file = fopen(path, "r")) == NULL)
	{
		plug_error("Config file %s not found (%s)", path, strerror(errno));
		return ESPANK_ERROR;
	}

	while (fgets(option, PATH_MAX, file) != NULL)
	{
		strclean(option, '\n');

		if ((value = strclean(option, '=')) != NULL)
		{
			if ((strlen(option) > 2))
			{
				value += 1;

				if (strlen(value) > 0)
				{
					strtoup(option);
					setenv_local(option, value, 0);
				}
			}
		}
	}

	fclose(file);
	return ESPANK_SUCCESS;
}

int read_cluster_configuration_file()
{
	//read_cluster_conf(char *conf_path,cluster_conf_t *my_conf);

	return (ESPANK_SUCCESS);
}

int find_paths(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function find_ear_conf_file");

	int i;

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 11) && (strncmp ("sysconfdir=", av[i], 11) == 0))
		{
			plug_verbose(sp, 3, "looking for configuration files in path '%s'", av[i]);
			setenv_local("EAR_ETCDIR", &av[i][11], 1);
		}
		if ((strlen(av[i]) > 8) && (strncmp ("rootdir=", av[i], 8) == 0))
		{
			plug_verbose(sp, 3, "looking for library files in path '%s'", av[i]);
			setenv_local("EAR_ROOT_DIR", &av[i][8], 1);

		}
	}
	return ESPANK_ERROR;
}

int library_disable()
{
	setenv_local("EAR", "0", 1);
}

/*
 *
 * Services
 *
 */

int eard_connection(spank_t sp)
{
	char buffer[64];

	gethostname(host_eard, 128);
	init_application(&app);

	if (!getenv_remote(sp, "SLURM_JOB_ID", buffer, 64)) {
		app.job.id = 0;
	} else {
		app.job.id = atoi(buffer);
	}
	if (!getenv_remote(sp, "SLURM_STEP_ID", buffer, 64)) {
		app.job.step_id = 0;
	} else {
		app.job.step_id = atoi(buffer);
	}
	if (!getenv_remote(sp, "SLURM_JOB_USER", app.job.user_id, GENERIC_NAME)) {
		strcpy(app.job.user_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_NAME",  app.job.app_id, GENERIC_NAME)) {
		strcpy(app.job.app_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_ACCOUNT", app.job.user_acc, GENERIC_NAME)) {
		strcpy(app.job.user_acc, "");
	}
	if (!getenv_remote(sp, "EAR_POWER_POLICY", app.job.policy, GENERIC_NAME)) {
		strcpy(app.job.policy, "");
	}
	if (!getenv_remote(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", buffer, 64)) {
		app.job.th = 0;
	} else {
		app.job.th = atof(buffer);
	}

	if (eards_remote_connect(host_eard) < 0) {
		plug_error("ERROR while connecting with EAR daemon");
	}
	if (!eards_new_job(&app)) {
		plug_error("ERROR while connecting with EAR daemon");
	}
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int eard_disconnection()
{
	if (eards_remote_connect(host_eard) < 0) {
		plug_error("ERROR while connecting with EAR daemon");
	}
	eards_end_job(app.job.id, app.job.step_id);
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int eargmd_connection()
{
	char *c_num_nodes;

	gethostname(host_eargmd, 128);
	getenv_local("SLURM_NNODES", &c_num_nodes);
	num_nodes = atoi(c_num_nodes);

	if (eargm_connect(host_eargmd, port_eargmd) < 0) {
		plug_error("ERROR while connecting with EAR global manager daemon");
	}
	if (!eargm_new_job(num_nodes)) {
		plug_error("ERROR while connecting with EAR global manager daemon");
	}
	eargm_disconnect();

	return (ESPANK_SUCCESS);
}

int eargmd_disconnection()
{
	if (eargm_connect(host_eargmd, port_eargmd) < 0) {
		plug_error("ERROR while connecting with EAR global manager daemon");
	}
	if (!eargm_end_job(num_nodes)) {
		plug_error("ERROR while connecting with EAR global manager daemon");
	}
	eargm_disconnect();

	return (ESPANK_SUCCESS);
}

/*
 *
 *
 * Framework
 *
 *
 */
int slurm_spank_init(spank_t sp, int ac, char **av)
{
	int i;

	for (i = 0; i < 9; ++i)
	{
		if (ESPANK_SUCCESS != spank_option_register(sp, &spank_options_manual[i]))
		{
        	slurm_error("unable to register a new option.");
        	return -1;
    	}
	}
}

int slurm_spank_local_user_init (spank_t sp, int ac, char **av)
{
    plug_verbose(sp, 2, "function slurm_spank_local_user_init");

    int r;

    if(spank_context () == S_CTX_LOCAL)
    {
		if ((r = find_paths(sp, ac, av)) != ESPANK_SUCCESS)
		{
			plug_error("plugstack.conf arguments are incorrect, disabling EAR");
			library_disable();

			return r;
		}

		//
		if ((r = read_cluster_configuration_file()) != ESPANK_SUCCESS) {
			plug_error("while reading configuration file, disabling EAR");
			library_disable();
		}

		//
		read_old_configuration_file(sp);

    	//
		if (isenv_local("EAR", "1"))
		{
			//
			eargmd_connection();

			//
			if ((r = local_update_ld_preload(sp)) != ESPANK_SUCCESS)
			{
				plug_error("unable to set LD_PRELOAD, disabling EAR");
				library_disable();

				return r;
			}
		}
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_user_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_user_init");

	if(spank_context() == S_CTX_REMOTE && isenv_remote(sp, "EAR", "1"))
	{
		// Printing job remote information
		print_general_info(sp);

		//
		remote_update_slurm_vars(sp);	
	}

	return (ESPANK_SUCCESS);
}

int slurm_spank_task_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_task_init");

	if (spank_context() == S_CTX_REMOTE) {
		return eard_connection(sp);
	}
	return (ESPANK_SUCCESS);
}

int slurm_spank_task_exit (spank_t sp, int ac, char **av)
{
	FUNCTION_INFO_("slurm_spank_task_exit");

	if (spank_context() == S_CTX_REMOTE) {
		return eard_disconnection();
	}
	return (ESPANK_SUCCESS);
}

int slurm_spank_exit (spank_t sp, int ac, char **av)
{
	FUNCTION_INFO_("slurm_spank_exit");

	if (spank_context() == S_CTX_LOCAL) {
		return eargmd_disconnection();
	}
	return (ESPANK_SUCCESS);
}
