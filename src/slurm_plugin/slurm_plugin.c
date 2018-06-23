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

#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <linux/limits.h>
#include <slurm/slurm.h>
#include <slurm/spank.h>
#include <cpufreq.h>

#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_options.h>
#include <slurm_plugin/slurm_plugin_user_system.h>
#include <slurm_plugin/slurm_plugin_helper.h>
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
 * Environment variables list

 *   environment variable                   new
 * --------------------------------------------
 * - EAR
 * - EAR_LEARNING_PHASE
 * - EAR_VERBOSE
 * - EAR_POWER_POLICY
 * - EAR_P_STATE
 * - EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN
 * - EAR_PERFORMANCE_PENALTY
 * - EAR_TRACES          
 * - EAR_MPI_DIST
 * - EAR_USER_DB_PATHNAME
 * - EAR_POWER_POLICY_TH					x
 * - EAR_PREDIR
 * - EAR_ETCDIR
 * - EAR_TMPDIR                             x
 * - EAR_TMP
 * - EAR_APP_NAME
 * - LD_PRELOAD
 * - SLURM_CPU_FREQ_REQ
 * - SLURM_NNODES
 * - SLURM_JOB_ID
 * - SLURM_STEP_ID
 * - SLURM_JOB_USER
 * - SLURM_JOB_NAME
 * - SLURM_JOB_ACCOUNT
 */

static void remote_print_environment(spank_t sp)
{
	plug_verbose(sp, 2, "function remote_print_environment");

	struct rlimit sta, mem;
	int r_sta, r_mem;

	if (verbosity_test(sp, 2) == 1) {
		return;
	}

	r_sta = getrlimit(RLIMIT_STACK, &sta);
    r_mem = getrlimit(RLIMIT_MEMLOCK, &mem);

    plug_verbose_0("plugin compiled in %s", __DATE__);
	plug_verbose_0("buffers size %d", PATH_MAX);

    plug_verbose_0("stack size limit test (res %d, curr: %lld, max: %lld)",
                 r_sta, (long long) sta.rlim_cur, (long long) sta.rlim_max);
    plug_verbose_0("memlock size limit test (res %d, curr: %lld, max: %lld)",
                 r_mem, (long long) mem.rlim_cur, (long long) mem.rlim_max);

	if (verbosity_test(sp, 2) == 1)
	{	
		printenv_remote(sp, "EAR");
		printenv_remote(sp, "EAR_LEARNING_PHASE");
		printenv_remote(sp, "EAR_VERBOSE");
		printenv_remote(sp, "EAR_POWER_POLICY");
		printenv_remote(sp, "EAR_P_STATE");
		printenv_remote(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN");
		printenv_remote(sp, "EAR_PERFORMANCE_PENALTY");
		printenv_remote(sp, "EAR_POWER_POLICY_TH");
		printenv_remote(sp, "EAR_TRACES");
		printenv_remote(sp, "EAR_MPI_DIST");
		printenv_remote(sp, "EAR_USER_DB_PATHNAME");
		printenv_remote(sp, "EAR_PREDIR");
		printenv_remote(sp, "EAR_ETCDIR");
		printenv_remote(sp, "EAR_TMP");
		printenv_remote(sp, "EAR_APP_NAME");
		printenv_remote(sp, "LD_PRELOAD");
		printenv_remote(sp, "SLURM_CPU_FREQ_REQ");
		printenv_remote(sp, "SLURM_NNODES");
		printenv_remote(sp, "SLURM_JOB_ID");
		printenv_remote(sp, "SLURM_STEP_ID");
		printenv_remote(sp, "SLURM_JOB_USER");
		printenv_remote(sp, "SLURM_JOB_NAME");
		printenv_remote(sp, "SLURM_JOB_ACCOUNT");
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
	plug_verbose(sp, 2, "function remote_eard_report_finish");

	#if PRODUCTION
	return ESPANK_SUCCESS;
	#endif

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
    if (!getenv_remote(sp, "SLURM_JOB_ACCOUNT", buffer1, 64)) {
		strcpy(eard_appl.job.user_acc, "");
    } else {
		strcpy(eard_appl.job.user_acc, buffer1);
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
	if (!getenv_remote(sp, "EAR_LEARNING_PHASE", buffer1, NAME_MAX)) {
		eard_appl.is_learning = 0;
	} else {
		eard_appl.is_learning = atoi(buffer1);
	}
	if (!getenv_remote(sp, "EAR_P_STATE", buffer1, NAME_MAX)) {
		return (ESPANK_ERROR);	
	} else {
		eard_appl.job.def_f = atoi(buffer1);
	}

	// Getting EARD connection variables
	gethostname(eard_host, NAME_MAX);

	if (!getenv_remote(sp, "EARD_PORT", buffer1, NAME_MAX)) {
		plug_error(sp, 2, "EARD port not found");
		return (ESPANK_ERROR);
	} else {
		eard_port = (unsigned int) atoi(buffer1);
	}

	// Verbosity
	plug_verbose(sp, 2, "trying to connect EARD with host '%s' and port '%u'", eard_host, eard_port);


	// Connection
	if (eards_remote_connect(eard_host, eard_port) < 0) {
		plug_error("while connecting with EAR daemon");
	}
	if (!eards_new_job(&eard_appl)) {
		plug_error("while connecting with EAR daemon");
	}
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int remote_eard_report_finish(spank_t sp)
{
	plug_verbose(sp, 2, "function remote_eard_report_finish");

	#if PRODUCTION
    return ESPANK_SUCCESS;
	#endif

	// Getting EARD connection variables
	gethostname(eard_host, NAME_MAX);

	if (!getenv_remote(sp, "EARD_PORT", buffer1, NAME_MAX)) {
		plug_error(sp, 2, "EARD port not found");
		return (ESPANK_ERROR);
	} else {
		eard_port = (unsigned int) atoi(buffer1);
	}

	// Verbosity
	plug_verbose(sp, 2, "trying to connect EARD with host '%s' and port '%u'", eard_host, eard_port);

	if (eards_remote_connect(eard_host, eard_port) < 0) {
		plug_error("while connecting with EAR daemon");
	}
	eards_end_job(eard_appl.job.id, eard_appl.job.step_id);
	eards_remote_disconnect();

	return (ESPANK_SUCCESS);
}

int local_eargmd_report_start(spank_t sp)
{
	plug_verbose(sp, 2, "function local_eargmd_report_start");

	#if PRODUCTION
    return ESPANK_SUCCESS;
    #endif

	// Gathering variables
	getenv_local("SLURM_NNODES", buffer1);
	eargmd_nods = atoi(buffer1);

	// Verbosity
	plug_verbose(sp, 2, "trying to connect EARGMD with host '%s', port '%u', and nnodes '%u'",
				 eargmd_host, eargmd_port, eargmd_nods);

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

int local_eargmd_report_finish(spank_t sp)
{
	plug_verbose(sp, 2, "function local_eargmd_report_finish");

	#if PRODUCTION
    return ESPANK_SUCCESS;
    #endif

	plug_verbose(sp, 2, "trying to connect EARGMD with host '%s', port '%u', and nnodes '%u'",
				 eargmd_host, eargmd_port, eargmd_nods);

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
		local_eargmd_report_finish(sp);
		return ESPANK_SUCCESS;
	}

    if (spank_context() == S_CTX_REMOTE) {
        return remote_eard_report_finish(sp);
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
			//
			remote_update_slurm_vars(sp);
		}
		
		// Printing job remote information
		remote_print_environment(sp);

		//
		if ((r = remote_eard_report_start(sp)) != ESPANK_SUCCESS)
		{
			plug_error("unable to report to EAR node daemon, disabling EAR");
			remote_library_disable(sp);
		}
	}

	return (ESPANK_SUCCESS);
}
