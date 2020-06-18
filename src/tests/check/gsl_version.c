/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <gsl/gsl_version.h>
#include <assert.h>

// VERSION 1.4
#define MIN_GSL_MAJOR_VERSION 1
#define MIN_GSL_MINOR_VERSION 4

int main(void)
{
	assert(GSL_MAJOR_VERSION  > MIN_GSL_MAJOR_VERSION || 
	      (GSL_MAJOR_VERSION == MIN_GSL_MAJOR_VERSION &&
	       GSL_MINOR_VERSION >= MIN_GSL_MINOR_VERSION));

	return 0;
}

