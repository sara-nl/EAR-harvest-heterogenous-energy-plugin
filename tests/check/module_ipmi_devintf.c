#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <linux/limits.h>
#include <common/string_enhanced.h>

int find_module()
{
	char path[PATH_MAX];
	char line[NAME_MAX]; // the same as lsmod
	FILE *fd;

	sprintf(path, "/proc/modules");
	fd = fopen(path, "r");
     
	if (fd == NULL) {
		return 0;
	}
 
	while(fgets (line, 16, fd) != NULL) {
		if (strstr(line, "ipmi_devintf") != NULL) {
			fclose(fd);
			return 1;
		}
	}
	fclose(fd);
	return 0;
}

int main ()
{
	char path[PATH_MAX];
	char line[16];
	FILE *fd;

	assert(find_module());
 
    return 0;
}
