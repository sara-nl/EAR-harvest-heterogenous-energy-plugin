/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <papi.h>
#include <assert.h>
#include <stdio.h>

// VERSION 5.4.3.0

int main(void)
{
	assert(PAPI_VERSION >= PAPI_VERSION_NUMBER(5,4,3,0));
	return 0;
}
