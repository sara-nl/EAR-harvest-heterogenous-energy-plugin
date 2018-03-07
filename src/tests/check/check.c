/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

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
