/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <common/types/application.h>
#include <common/types/periodic_metric.h>

int main(int argc,char *argv[])
{
	printf("Application size if %lu\n",sizeof(application_t));
	printf("Periodic metric size is %lu\n",sizeof(periodic_metric_t));
	return 0;
}
