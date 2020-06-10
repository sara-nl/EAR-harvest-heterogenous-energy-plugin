#include <library/loader/module_mpi.h>
#include <library/loader/module_default.h>

static int loaded_default;

void  __attribute__ ((constructor)) loader()
{
	// Module default
	loaded_default = module_constructor();

	if (!loaded_default) {
		// Module MPI
		module_mpi();
	}

	return;
}
