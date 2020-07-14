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

#include <library/loader/loader.h>
#include <library/loader/module_mpi.h>
#include <library/loader/module_default.h>

int _loaded_con;
int _loaded_mpi;

void  __attribute__ ((constructor)) loader()
{
	VERB_SET_EN(1);
  VERB_SET_LV(4);

	verbose(3, "LOADER: loader for application '%s'", program_invocation_name);
	verbose(3, "LOADER: loader for PID %d", getpid());
	if (getenv("SLURM_SCRIPT_CONTEXT")!=NULL){ 
		verbose(3,"SLURM_SCRIPT_CONTEXT %s",getenv("SLURM_SCRIPT_CONTEXT"));
	}else{ 
		verbose(3,"SLURM_SCRIPT_CONTEXT undefined");
	}
	// Module MPI
	_loaded_mpi = module_mpi();
	// Module default
	if (!_loaded_mpi && strcmp(program_invocation_name,"/bin/bash")) {
		_loaded_con = module_constructor();
	}
	// New modules here...

	return;
}
