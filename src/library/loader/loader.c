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
	
	return pid1 == pid2;
}

void  __attribute__ ((constructor)) loader()
{

	// Initialization
	if (!init()) {
		verbose(4, "LOADER: escaping the application '%s'", program_invocation_name);
		return;
	}
	verbose(3, "LOADER: loading for application '%s'", program_invocation_name);
	
	// Module MPI
	_loaded_mpi = module_mpi();
	// Module default
	if (!_loaded_mpi && strcmp(program_invocation_name,"/bin/bash")) {
		_loaded_con = module_constructor();
	}
	// New modules here...

	return;
}
