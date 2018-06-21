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
#include <sys/resource.h>
#include <linux/limits.h>
#include <slurm/spank.h>
#include <cpufreq.h>

#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>
#include <slurm_plugin/slurm_plugin_options.h>
#include <slurm_plugin/slurm_plugin_user_system.h>
#include <common/types/cluster_conf.h>
#include <common/types/application.h>
#include <common/types/job.h>
#include <common/config.h>

int EAR_VERBOSE_LEVEL = 0;

SPANK_PLUGIN(EAR_PLUGIN, 1)
static pid_t daemon_pid = -1;
int verbosity = -1;

char buffer1[PATH_MAX];
char buffer2[PATH_MAX];
static int job_created;

// EARGMD variables
static unsigned char eargmd_host[NAME_MAX+1];
static unsigned int  eargmd_port;
static unsigned int  eargmd_nods;

// EARD variables
unsigned char eard_host[NAME_MAX+1];
unsigned int  eard_port;
application_t eard_appl;

/*
 *
 * Environment variables list
 *
 */



/*
 *
 * Configuration
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

    if (getenv_local("EAR_PREDIR", &ear_root_dir) == 0)
    {
		plug_error("Error, wrong environment for setting LD_PRELOAD");
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

static int plugstack_process(spank_t sp, int ac, char **av)
{
	int found_predir = 0;
	int found_etcdir = 0;
	char *etc_dir;
	char *pre_dir;
	int i;

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 8) && (strncmp ("default=on", av[i], 10) == 0))
		{
			setenv_local("EAR", "1", 1);
		}
		if ((strlen(av[i]) > 11) && (strncmp ("sysconfdir=", av[i], 11) == 0))
		{
			etc_dir = &av[i][11];
			found_etcdir = 1;

			plug_verbose(sp, 3, "looking for configuration files in path '%s'", etc_dir);
			setenv_local("EAR_ETCDIR", etc_dir, 1);
		}
		if ((strlen(av[i]) > 7) && (strncmp ("prefix=", av[i], 7) == 0))
		{
			pre_dir = &av[i][7];
			found_predir = 1;

			plug_verbose(sp, 3, "looking for library files in path '%s'", pre_dir);
			setenv_local("EAR_PREDIR", pre_dir, 1);
		}
	}

	if (!found_etcdir || !found_predir) {
		return (ESPANK_ERROR);
	}
}

static int local_configuration(spank_t sp, int ac, char **av)
{
	static cluster_conf_t conf_clus;
	char *conf_path = buffer1;
	char *etc_dir;
	int r;

	//
	if ((r = plugstack_process(sp, ac, av)) != ESPANK_SUCCESS) {
		return r;
	}

	// Getting variables
	gethostname(eard_host, NAME_MAX);
	getenv_local("ETC_DIR", &etc_dir);

	plug_verbose(0, 2, "Trying to read config file in '%s' for node '%s'", etc_dir, eard_host);

	// Passing parameters to get_ear_conf_path (LOL)
	setenv_local("ETC", etc_dir, 1);
	conf_path[0] = '\0';

	// Gathering data
	if (get_ear_conf_path(conf_path) == EAR_ERROR) {
		return (ESPANK_ERROR);
	}

	if (read_cluster_conf(conf_path, &conf_clus) != EAR_SUCCESS) {
		return (ESPANK_ERROR);
	}

	// Setting variables for EARD connection
	sprintf(buffer1, "%u", conf_clus.eard.port);
	setenv_local("EARD_HOST", eard_host, 1);
	setenv_local("EARD_PORT", buffer1,   1);

	// Setting variables for EARGMD connection
	sprintf(eargmd_host, "%s", conf_clus.eargm.host);
	eargmd_port = conf_clus.eargm.port;

	// Setting LIBEAR variables
	setenv_local("EAR_TMP", conf_clus.tmp_dir, 1);

	// User system for LIBEAR
	local_user_system_configuration(&conf_clus);

	// Freeing cluster configuration resources
	free_cluster_conf(&conf_clus);

	return (ESPANK_SUCCESS);
}

/*
 *
 * Disabling
 *
 */

int local_library_disable()
{
	setenv_local("EAR", "0", 1);
}

int remote_library_disable(spank_t sp)
{
	if(isenv_remote(sp, "EAR", "1")) {
		setenv_remote(sp, "LD_PRELOAD", "", 1);
		setenv_remote(sp, "EAR", "0", 1);
	}
}

/*
 *
 * Services
 *
 */

int remote_eard_report_start(spank_t sp)
{
	unsigned int eard_port;

	init_application(&eard_appl);

	// Gathering variables
	if (!getenv_remote(sp, "SLURM_JOB_ID", buffer1, 64)) {
		eard_appl.job.id = 0;
	} else {
		eard_appl.job.id = atoi(buffer1);
	}
	if (!getenv_remote(sp, "SLURM_STEP_ID", buffer1, 64)) {
		eard_appl.job.step_id = 0;
	} else {
		eard_appl.job.step_id = atoi(buffer1);
	}
	if (!getenv_remote(sp, "SLURM_JOB_USER", eard_appl.job.user_id, GENERIC_NAME)) {
		strcpy(eard_appl.job.user_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_NAME",  eard_appl.job.app_id, GENERIC_NAME)) {
		strcpy(eard_appl.job.app_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_ACCOUNT", eard_appl.job.user_acc, GENERIC_NAME)) {
		strcpy(eard_appl.job.user_acc, "");
	}
	if (!getenv_remote(sp, "EAR_POWER_POLICY", eard_appl.job.policy, GENERIC_NAME)) {
		strcpy(eard_appl.job.policy, "");
	}
	if (!getenv_remote(sp, "EAR_POWER_POLICY_TH", buffer1, 64)) {
		eard_appl.job.th = 0;
	} else {
		eard_appl.job.th = atof(buffer1);
	}
	if (!getenv_remote(sp, "EARD_HOST", eard_host, NAME_MAX)) {
		return (ESPANK_ERROR);
	}	
	if (!getenv_remote(sp, "EARD_PORT", buffer1, NAME_MAX)) {
		return (ESPANK_ERROR);
	} else {
		eard_port = (unsigned int) atoi(buffer1);
	}
	// TODO:
	//eard_appl.is_learning =
	//eard.job.def_f =

	// Verbosity
	plug_verbose(sp, 2, "trying to connect EARD with host '%s' and port '%u'", eard_host, eard_port);

	if (eards_remote_connect(eard_host, eard_port) < 0) {
		plug_error("while connecting with EAR daemon");
	}
	if (!eards_new_job(&eard_appl)) {
		plug_error("while connecting with EAR daemon");
	}
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int remote_eard_report_finish()
{
	if (eards_remote_connect(eard_host) < 0) {
		plug_error("while connecting with EAR daemon");
	}
	eards_end_job(eard_appl.job.id, eard_appl.job.step_id);
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int local_eargmd_report_start(spank_t sp)
{
	char *c_num_nodes;

	// Gathering variables
	getenv_local("SLURM_NNODES", &c_num_nodes);
	eargmd_nods = atoi(c_num_nodes);

	// Verbosity
	plug_verbose(sp, 2, "trying to connect EARGMD with host '%s' and port '%u'", eargmd_host, eargmd_port);

	if (eargm_connect(eargmd_host, eargmd_port) < 0) {
		plug_error("while connecting with EAR global manager daemon");
		return ESPANK_ERROR;
	}
	if (!eargm_new_job(eargmd_nods)) {
		plug_error("while connecting with EAR global manager daemon");
	}
	eargm_disconnect();

	return (ESPANK_SUCCESS);
}

int local_eargmd_report_finish()
{
	if (eargm_connect(eargmd_host, eargmd_port) < 0) {
		plug_error("while connecting with EAR global manager daemon");
		return ESPANK_ERROR;
	}
	if (!eargm_end_job(eargmd_nods)) {
		plug_error("while connecting with EAR global manager daemon");
		return ESPANK_ERROR;
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

int slurm_spank_init_post_opt(spank_t sp, int ac, char **av)
{
    plug_verbose(sp, 2, "function slurm_spank_init_post_opt");
	int r;

	if(spank_context () == S_CTX_LOCAL)
	{
		if ((r = local_configuration(sp, ac, av)) != ESPANK_SUCCESS)
		{
			plug_error("while reading configuration file, disabling EAR");
			local_library_disable();

			return r;
		}
	}

	return (ESPANK_SUCCESS);
}

int slurm_spank_local_user_init (spank_t sp, int ac, char **av)
{
    plug_verbose(sp, 2, "function slurm_spank_local_user_init");
    int r;

    if(spank_context () == S_CTX_LOCAL)
    {
		job_created = 1;

		//
		local_eargmd_report_start(sp);

		//
		if (isenv_local("EAR", "1"))
		{
			//
			if ((r = local_update_ld_preload(sp)) != ESPANK_SUCCESS)
			{
				plug_error("unable to set LD_PRELOAD, disabling EAR");
				local_library_disable();

				return r;
			}
		}
    }

    return (ESPANK_SUCCESS);
}

int slurm_spank_exit (spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "slurm_spank_exit");

	if (spank_context() == S_CTX_LOCAL && job_created == 1) {
		local_eargmd_report_finish();
		return ESPANK_SUCCESS;
	}

    if (spank_context() == S_CTX_REMOTE) {
        return remote_eard_report_finish();
    }

	return (ESPANK_SUCCESS);
}

int slurm_spank_user_init(spank_t sp, int ac, char **av)
{
	plug_verbose(sp, 2, "function slurm_spank_user_init");
	int r;

	if (spank_context() == S_CTX_REMOTE)
	{
		if(isenv_remote(sp, "EAR", "1"))
		{
			// Printing job remote information
			print_general_info(sp);

			//
			remote_update_slurm_vars(sp);
		}

		if ((r = remote_eard_report_start(sp)) != ESPANK_SUCCESS)
		{
			plug_error("unable to report to EAR node daemon, disabling EAR");
			remote_library_disable(sp);
		}
	}

	return (ESPANK_SUCCESS);
}
