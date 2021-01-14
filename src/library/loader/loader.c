/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#define _GNU_SOURCE
#include <sched.h>


#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <library/loader/loader.h>
#include <library/loader/module_mpi.h>
#include <library/loader/module_default.h>
#include <common/config/config_env.h>

static int _loaded_con;
static int _loaded_mpi;

static int init()
{
	char *verb;
	char *task;
	pid_t pid1 = 1;
	pid_t pid2 = 0;
	
	// Activating verbose
	if ((verb = getenv(FLAG_LOAD_VERB)) != NULL) {
		VERB_SET_EN(1);
		VERB_SET_LV(atoi(verb));
	}
	
	// SLURM prolog/epilog control
	if ((task = getenv(FLAG_TASK_PID)) == NULL) {
		return 0;
	}
	
	pid1 = getpid();
	pid2 = (pid_t) atoi(task);
	verbose(4, "LOADER: loader pids: %d/%d", pid1, pid2);
	return 1;	
	return pid1 == pid2;
}

int must_load()
{
	if ((strstr(program_invocation_name, "bash") == NULL) &&
	(strstr(program_invocation_name, "hydra") == NULL)){
		verbose(2,"LOADER: Program %s loaded with EAR",program_invocation_name);
		return 1;
	}
	verbose(2,"LOADER: Program %s loaded without EAR",program_invocation_name);
	return 0;
}

int load_no_mpi()
{
	char *app_to_load=getenv(SCHED_LOADER_LOAD_NO_MPI_LIB);
	if (app_to_load == NULL) return 0;
	if (strstr(program_invocation_name,app_to_load) == NULL) return 0;
	return 1;
}

void  __attribute__ ((constructor)) loader()
{
	// Initialization
	if (!init()) {
		verbose(4, "LOADER: escaping the application '%s'", program_invocation_name);
		return;
	}
	verbose(2, "LOADER: loading for application '%s'", program_invocation_name);
	
	if (must_load()){ 
		// Module MPI
		verbose(2,"Tring MPI module");
		_loaded_mpi = module_mpi();

		// Module default
		if (!_loaded_mpi && load_no_mpi()) {
			verbose(2,"Tring default module");
			_loaded_con = module_constructor();
		}
	}
	// New modules here...
#if 0
	char *s = NULL;
	char *o = NULL;

	if ((s = getenv("SLURM_LOCALID")) != NULL && (o = getenv("SLURM_JEJE")) != NULL)
	{
		cpu_set_t mask;
		int cpu = atoi(s);
		int off = atoi(o); 

		cpu = cpu * off;
		CPU_ZERO(&mask);
		CPU_SET(cpu, &mask);
		
		int result = sched_setaffinity(0, sizeof(cpu_set_t), &mask);
		verbose(0, "TASK %s: set to CPU %d (%d, %d, %s) %d", s, cpu, result, errno, strerror(errno), mask);
	}
#endif

	return;
}
