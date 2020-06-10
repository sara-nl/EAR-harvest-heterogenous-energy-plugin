#include <stdio.h>
#include <library/loader/module_mpi.h>
#include <library/loader/module_default.h>

int _loaded_default;

void  __attribute__ ((constructor)) loader()
{
	// Module default
	//_loaded_default = module_constructor();
	// Module MPI
	module_mpi();

	return;
}
