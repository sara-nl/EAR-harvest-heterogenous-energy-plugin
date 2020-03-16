#include <library/api_loader/module_mpi.h>

void  __attribute__ ((constructor)) loader()
{
	module_mpi();
	return;
}