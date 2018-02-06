#include <slurm/slurm.h>
#include <assert.h>

// VERSION 17.02.6

int main(void)
{
	assert(SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(17,2,6));
	return 0;
}

