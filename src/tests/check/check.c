/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <tests/check/check.h>

int find_module(char *module)
{
	char line[NAME_MAX]; // the same as lsmod
	FILE *fd;

	fd = fopen("/proc/modules", "r");

	if (fd == NULL) {
		return 0;
	}

	while(fgets (line, 16, fd) != NULL) {
		if (strstr(line, module) != NULL) {
			fclose(fd);
			return 1;
		}
	}
	fclose(fd);
	return 0;
}
