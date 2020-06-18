/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <slurm/slurm.h>
#include <assert.h>

// VERSION 17.02.6

int main(void)
{
	assert(SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(17,2,6));
	return 0;
}

