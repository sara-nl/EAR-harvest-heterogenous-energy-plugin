#include <library/loader/module_mpi.h>
#include <library/loader/module_default.h>

void  __attribute__ ((constructor)) loader()
{
	// Module MPI
	module_mpi();
	// Module default
	module_constructor();
	return;
}
