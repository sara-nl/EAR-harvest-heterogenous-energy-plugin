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
#include <common/sizes.h>
#include <common/config.h>
#include <common/types/job.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_helper.h>
#include <slurm_plugin/slurm_plugin_options.h>
#include <slurm_plugin/slurm_plugin_user_environment.h>

int EAR_VERBOSE_LEVEL = 0;

SPANK_PLUGIN(EAR_PLUGIN, 1)
int verbosity = -1;

char buffer1[PATH_MAX];
char buffer2[PATH_MAX];
static int job_created;

// EARGMD variables
unsigned char eargmd_host[NAME_MAX+1];
unsigned int  eargmd_port;
unsigned int  eargmd_nods;

// EARD variables
unsigned char eard_host[NAME_MAX+1];
unsigned int  eard_port;
application_t eard_appl;

// Contexts
#define S_CTX_SRUN 		S_CTX_LOCAL
#define S_CTX_SBATCH	S_CTX_ALLOCATOR

/*
 * Manual
 * ------
 * SRUN pipeline:
 * srun   -> slurm_spank_init()
 * srun   -> srun arguments are received int ‘opt()’ functions arguments
 *           are transformed into environment variables if the values are
 *           in the correct range.
 * srun   -> slurm_spank_init_post_op()
 * srun   -> job is created and queued
 * srun   -> job is launched
 * srun   -> slurm_spank_srun _user_init()
 * srun   -> environment variables are serialized and transformed into
 *           spank remote variables
 * remote -> processes are spawned into remote nodes
 * remote -> slurm_spank_user_init()
 * remote -> running task
 * remote -> job exit functions
 * srun   -> job exit functions
 *
 * SBATCH pipeline:
 * The same of the SRUN pipeline, except that it returns inmediately after the
 * job is created, queued and launched. SRUN waits until the end.
 *
 * SBATCH launching processes step by step:
 * 1) Process 38966 launches sbatch
 * 2) It creates the job process 41186 and its local context returns inmediately
 * 3) Job process 41186 contacts with a node nxt0347
 * 4) Node nxt0347 SLURMD process 18432 creates a sbatch read process 19668
 * 5) Read process 19668 reads the SBATCH script file
 * 6) Read process 19668 finds and SRUN command in the sbatch script file and
 *    it creates the job process 19676 which starts with the SRUN local pipeline
 *    whie the 19668 waits.
 * 7) It is a two task job so the processes 19688 and 41930 processes are
 *    spawned in the nodes nxt0347 and nxt0348 respectively, following the
 *    SRUN remote pipeline.
 * 8) The running processes 19693 and 41935, children of 19688 and 41930
 *    respectively are created and runs the application while the parents
 *    wait.
 * 9) The app ends and the parents call the exit remote pipeline functions.
 * 10) The read process 19668 continue the reading of the SBATCH.
 *
 * FAQ:
 * - The environment is propagated when the flag is set in the SBATCH (i.e.
 *   sbatch --ear=on)?
 * + Yes.
 * - The environment is propagated when the flag is set in #SBATCH script
 *   options (i.e. #SBATCH --ear=on)?
 * + Yes.
 * - It is called the respective option function (i.e. function '_opt_ear') when
 *   the #SBATCH script option is set?
 * + Is called in the SBATCH context, not in the SRUN context.
 * - Is called the EARD and EARGMD job connection functions in every SRUN and
 *   SBATCH remote conext?
 * + Yes, so we have to control it.
 * - It is possible to identify when is SRUN and when is SBATCH/SALLOC?
 * + Yes. The local context in SBATCH/SALLOC is defined as 'S_CTX_ALLOCATOR'
 *   while in SRUN is defined as 'S_CTX_LOCAL'. So in remote context some
 *   control have to be added.
 *
 * Environment variables list:
 *   Var                                    | Opt | Local | Local Job | Remote | Remote PU
 * ---------------------------------------------------------------------------------------
 * - EAR									| x   | x     |           |        |
 * - EAR_DEFAULT							|     | x     |           |        |
 * - EAR_USER                               |     | x     |           |        |
 * - EAR_LEARNING_PHASE						| x   |       |           |        |
 * - EAR_VERBOSE							| x   | x     |           |        |
 * - EAR_POWER_POLICY						| x   | x     |           |        |
 * - EAR_P_STATE							| x   | x     |           | t      |
 * - EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN	| -   | -     | -         | -      |
 * - EAR_PERFORMANCE_PENALTY				| -   | -     | -         | -      |
 * - EAR_POWER_POLICY_TH					| x   | x     |           | t      |
 * - EAR_TRACES          					| x   |       |           |        |
 * - EAR_MPI_DIST                           | x   |       |           |        |
 * - EAR_USER_DB_PATHNAME                   | x   |       |           |        |
 * - EAR_DB_PATHNAME                   		|     | x     |           |        |
 * - EAR_COEFF_PATHNAME                     |     | x     |           |        |
 * - EAR_PREDIR								|     | x     |           |        |
 * - EAR_ETCDIR								|     | x     |           |        |
 * - EAR_TMPDIR                             |     | x     |           |        |
 * - EAR_TMP								| -   | -     | -         | -      |
 * - TMP									| -   | -     | -         | -      |
 * - EAR_APP_NAME							|     | x     |           |        |
 * - LD_PRELOAD                             |     | x     |           |        |
 * - SLURM_CPU_FREQ_REQ                     |     |       | x         |        |
 * - SLURM_NNODES                           |     |       | x         |        |
 * - SLURM_JOB_ID							|     |       | x         |        |
 * - SLURM_STEP_ID							|     |       | x         |        | 
 * - SLURM_JOB_USER							|     |       |           |        | x
 * - SLURM_JOB_NAME							|     |       | x         |        | 
 * - SLURM_JOB_ACCOUNT						|     |       | x         |        | 
 *
 * Reasons to stop the job:
 * 1) A problem inside option functions.
 * 2) The P_STATE is not found in 'ear.conf' P_STATE list.
 * 3) Unprivileged USER trying to use privileged options: EAR, EAR_VERBOSE,
 *    EAR_LEARNING_PHASE, EAR_POWER_POLICY, EAR_P_STATE, EAR_POWER_POLICY_TH,
 *    EAR_TRACES, EAR_DB_PATHNAME, EAR_COEFF_PATHNAME
 *
 * Reasons to disable EAR:
 * 1) Set environment variable fails.
 * 2) Writing in a sized buffer fails.
 *
 * Reading configuration times:
 * 1) In local context, prior to job creation
 * 2) In remote context, for specific node configuration
 *
 * Preguntas:
 * 1) Que pasa con el threshold? Si cada nodo tiene uno, que ocurre cuando
 *    se escribe uno manualmente con la opción? Lo mismo para el P_STATE. Ello
 *    implica la relectura de la configuración.
 * 2) Que variables son estrictamente necesarias tanto de EAR, como de EARD
 *    y EARGMD? Que hacer en caso de no encontrarse.
 *
 * Under construction:
 * 1) Energy tag
 * 2) Detection system to avoid EARD and EARGMD job end functions if there
 *    were an error in the job start functions.
 * 3) SBATCH/SALLOC context detection system.
 *
 */

static void remote_print_environment(spank_t sp)
{
	plug_verbose(sp, 2, "function remote_print_environment");

	struct rlimit sta, mem;
	int r_sta, r_mem;

	if (verbosity_test(sp, 2) == 0) {
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

	printenv_remote(sp, "EAR");
	printenv_remote(sp, "EAR_USER");
	printenv_remote(sp, "EAR_GROUP");
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
	printenv_remote(sp, "EAR_ENERGY_TAG");
	printenv_remote(sp, "LD_PRELOAD");
	printenv_remote(sp, "SLURM_CPU_FREQ_REQ");
	printenv_remote(sp, "SLURM_NNODES");
	printenv_remote(sp, "SLURM_JOB_ID");
	printenv_remote(sp, "SLURM_STEP_ID");
	printenv_remote(sp, "SLURM_JOB_USER");
	printenv_remote(sp, "SLURM_JOB_NAME");
	printenv_remote(sp, "SLURM_JOB_ACCOUNT");
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
	if (!getenv_remote(sp, "SLURM_JOB_ID", buffer1, SZ_NAME_SHORT)) {
		eard_appl.job.id = 0;
	} else {
		eard_appl.job.id = atoi(buffer1);
	}
	if (!getenv_remote(sp, "SLURM_STEP_ID", buffer1, SZ_NAME_SHORT)) {
		eard_appl.job.step_id = 0;
	} else {
		eard_appl.job.step_id = atoi(buffer1);
	}
    if (!getenv_remote(sp, "SLURM_JOB_ACCOUNT", eard_appl.job.user_acc, SZ_NAME_SHORT)) {
		strcpy(eard_appl.job.user_acc, "");
    }
	if (!getenv_remote(sp, "EAR_USER", eard_appl.job.user_id, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.user_id, "");
	}
	if (!getenv_remote(sp, "EAR_GROUP", eard_appl.job.group_id, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.group_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_NAME",  eard_appl.job.app_id, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.app_id, "");
	}
	if (!getenv_remote(sp, "SLURM_JOB_ACCOUNT", eard_appl.job.user_acc, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.user_acc, "");
	}
	if (!getenv_remote(sp, "EAR_POWER_POLICY", eard_appl.job.policy, SZ_NAME_MEDIUM)) {
		strcpy(eard_appl.job.policy, "");
	}
	if (!getenv_remote(sp, "EAR_POWER_POLICY_TH", buffer1, SZ_NAME_SHORT)) {
		eard_appl.job.th = 0;
	} else {
		eard_appl.job.th = atof(buffer1);
	}
	if (!getenv_remote(sp, "EAR_LEARNING_PHASE", buffer1, SZ_NAME_MEDIUM)) {
		eard_appl.is_learning = 0;
	} else {
		eard_appl.is_learning = atoi(buffer1);
	}
	if (!getenv_remote(sp, "EAR_P_STATE", buffer1, SZ_NAME_MEDIUM)) {
		return (ESPANK_ERROR);	
	} else {
		eard_appl.job.def_f = atoi(buffer1);
	}
	if (!getenv_remote(sp, "EAR_ENERGY_TAG", eard_appl.job.energy_tag, SZ_NAME_SHORT)) {
		strcpy(eard_appl.job.energy_tag, "");
	}

	// Getting EARD connection variables
	gethostname(eard_host, NAME_MAX);

	if (!getenv_remote(sp, "EARD_PORT", buffer1, NAME_MAX)) {
		plug_error("EARD port not found");
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
		plug_error("EARD port not found");
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
	char *c_eargmd_nods;
	
	#if PRODUCTION
    return ESPANK_SUCCESS;
    #endif

	// Gathering variables
	getenv_local("SLURM_NNODES", &c_eargmd_nods);
	eargmd_nods = atoi(c_eargmd_nods);

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
		if ((r = local_pre_job_configuration(sp, ac, av)) != ESPANK_SUCCESS)
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
			if ((r = local_post_job_configuration(sp)) != ESPANK_SUCCESS)
			{
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
			if ((r = remote_configuration(sp)) != ESPANK_SUCCESS)
			{
				remote_library_disable(sp);
				return (ESPANK_ERROR);
			}
		}
		
		// Printing job remote information
		remote_print_environment(sp);

		//
		if ((r = remote_eard_report_start(sp)) != ESPANK_SUCCESS)
		{
			remote_library_disable(sp);
			return (ESPANK_ERROR);
		}
	}

	return (ESPANK_SUCCESS);
}
