#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <linux/limits.h>
#include <common/string_enhanced.h>

int main ()
{
	char path[PATH_MAX];
	char line[16];
	FILE *fd;

    sprintf(path, "/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed");
    fd = fopen(path, "r");
	
	assert(fd != NULL);
	assert(fgets (line, 16, fd) != NULL);
	
	strclean(line, '\n');
	assert(strcmp("<unsupported>", line) != 0);

	fclose(fd);

	sprintf(path, "/sys/devices/system/cpu/cpu0/cpufreq/scaling_driver");
    fd = fopen(path, "r");
    
	assert(fd != NULL);
    assert(fgets (line, 16, fd) != NULL);

	strclean(line, '\n');
    assert(strcmp("acpi-cpufreq", line) == 0);
 
	fclose(fd);
 
    return 0;
}
