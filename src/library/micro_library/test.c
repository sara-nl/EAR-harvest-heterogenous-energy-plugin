#include <papi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <common/math_operations.h>

static unsigned long long ear_in_mpi=0;
static unsigned long long begin_in_mpi,end_mpi_time;
unsigned long long ear_total_in_mpi=0;
unsigned long long ear_iteration_in_mpi=0;

int main(int argc, char *argv[])
{
	int i;

	for (i = 0; i < 40000000; ++i)
	{
	begin_in_mpi=PAPI_get_real_usec();

	end_mpi_time=PAPI_get_real_usec();
	if (end_mpi_time>begin_in_mpi){ 
			ear_in_mpi=end_mpi_time-begin_in_mpi;
	}else{ 
			ear_in_mpi= ullong_diff_overflow(begin_in_mpi,end_mpi_time);
	}
	ear_total_in_mpi+=ear_in_mpi;
	ear_iteration_in_mpi+=ear_in_mpi;
	}

	return 0;
}
