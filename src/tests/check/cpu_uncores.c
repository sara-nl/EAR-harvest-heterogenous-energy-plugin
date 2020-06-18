/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <stdio.h>
#include <assert.h>
#include <metrics/bandwidth/bandwidth.h>
#include <common/hardware/hardware_info.h>
#include <common/states.h>

int main(void)
{
	int result;

	init_uncores(get_model());
	result = check_uncores();
	dispose_uncores();
	assert(result == EAR_SUCCESS || result == EAR_WARNING);

	return 0;
}

