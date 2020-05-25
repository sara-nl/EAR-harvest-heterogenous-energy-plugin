#include <library/loader/module_mpi.h>
#include <library/loader/module_constructor.h>

void  __attribute__ ((constructor)) loader()
{
	module_mpi();
	module_constructor();
	return;
}
