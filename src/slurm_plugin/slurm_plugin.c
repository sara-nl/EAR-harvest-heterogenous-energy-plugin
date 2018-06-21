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

static unsigned char host_name[256];

static unsigned char eargmd_host[256];
static unsigned int  eargmd_port;
static unsigned int  eargmd_nods;

static application_t app;
static int job_created;

/*
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

/*
 *
 * Configuration
 *
 */

int local_read_cluster_conf_file(spank_t sp, int ac, char **av)
{
	static cluster_conf_t conf_clus;
	my_node_conf_t *conf_node;
	policy_conf_t  *conf_plcy;
	
	char *conf_path = buffer1;
	char *hetc_path;
	
	int found_predir = 0;
	int found_etcdir = 0;
	int i;

	for (i = 0; i < ac; ++i)
	{
		if ((strlen(av[i]) > 11) && (strncmp ("sysconfdir=", av[i], 11) == 0))
		{
			hetc_path = &av[i][11];
			plug_verbose(sp, 3, "looking for configuration files in path '%s'", hetc_path);
			setenv_local("EAR_ETCDIR", hetc_path, 1);
			found_etcdir = 1;

		}
		if ((strlen(av[i]) > 7) && (strncmp ("prefix=", av[i], 7) == 0))
		{
			plug_verbose(sp, 3, "looking for library files in path '%s'", &av[i][7]);
			setenv_local("EAR_PREDIR", &av[i][7], 1);
			found_predir = 1;
		}
	}

	if (!found_etcdir || !found_predir) {
		return (ESPANK_ERROR);
	}

	// Getting hostname
	gethostname(host_name, 128);

	//sprintf(buffer1, "%s/%s", conf_path, "ear/ear.conf");
	plug_verbose(0, 2, "Trying to read config file in '%s' for node '%s'", conf_path, host_name);

	// Passing parameters to get_ear_conf_path (LOL)
	setenv_local("ETC", hetc_path, 1);
	conf_path[0] = '\0';	

	// Gathering data
	if (get_ear_conf_path(conf_path) == EAR_ERROR) {
		return (ESPANK_ERROR);
	}
	
	if (read_cluster_conf(conf_path, &conf_clus) != EAR_SUCCESS) {
		return (ESPANK_ERROR);
	}

	if ((conf_node = get_my_node_conf(&conf_clus, host_name)) == NULL) {
		return (ESPANK_ERROR);
	}

	if ((conf_plcy = get_my_policy_conf(&conf_clus, conf_node, conf_clus.default_policy)) == NULL) {
		return (ESPANK_ERROR);
	}

	// Setting variables for EARD connection
	sprintf(buffer1, "%u", conf_clus.eard.port);
	setenv_local("EARD_HOST", host_name, 1);
	setenv_local("EARD_PORT", buffer1,   1);

	// Setting variables for EARGMD connection
	sprintf(eargmd_host, "%s", conf_clus.eargm.host);
	eargmd_port = conf_clus.eargm.port;

	// Setting LIBEAR variables
	setenv_local("EAR_TMP", conf_clus.tmp_dir, 1);

	//
	free_cluster_conf(&conf_clus);

	return (ESPANK_SUCCESS);
}

int local_environment_post_process()
{
	// It means that the user has already set up
	if(existenv_local("EAR_POWER_POLICY")) {
		if(existenv_local("EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN")) {}
		if(existenv_local("EAR_PERFORMANCE_PENALTY")) {}
	}
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

	init_application(&app);

	if (!getenv_remote(sp, "SLURM_JOB_ID", buffer1, 64)) {
		app.job.id = 0;
	} else {
		app.job.id = atoi(buffer1);
	}
	if (!getenv_remote(sp, "SLURM_STEP_ID", buffer1, 64)) {
		app.job.step_id = 0;
	} else {
		app.job.step_id = atoi(buffer1);
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
	if (!getenv_remote(sp, "EAR_POWER_POLICY_TH", buffer1, 64)) {
		app.job.th = 0;
	} else {
		app.job.th = atof(buffer1);
	}
	if (!getenv_remote(sp, "EARD_HOST", host_name, GENERIC_NAME)) {
		return (ESPANK_ERROR);
	}	
	if (!getenv_remote(sp, "EARD_PORT", buffer1, GENERIC_NAME)) {
		return (ESPANK_ERROR);
	} else {
		eard_port = (unsigned int) atoi(buffer1);
	}

	// Verbosity
	plug_verbose(sp, 2, "trying to connect EARD with host '%s' and port '%u'", host_name, eard_port);

	if (eards_remote_connect(host_name, eard_port) < 0) {
		plug_error("ERROR while connecting with EAR daemon");
	}
	if (!eards_new_job(&app)) {
		plug_error("ERROR while connecting with EAR daemon");
	}
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int remote_eard_report_finish()
{
	if (eards_remote_connect(host_name) < 0) {
		plug_error("while connecting with EAR daemon");
	}
	eards_end_job(app.job.id, app.job.step_id);
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int local_eargmd_report_start(spank_t sp)
{
	char *c_num_nodes;

	getenv_local("SLURM_NNODES", &c_num_nodes);
	eargmd_nods = atoi(c_num_nodes);

	// Verbosity
	plug_verbose(sp, 2, "EARGMD host: %s", eargmd_host);
	plug_verbose(sp, 2, "EARGMD port: %u", eargmd_port);

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
		if ((r = local_read_cluster_conf_file(sp, ac, av)) != ESPANK_SUCCESS)
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
