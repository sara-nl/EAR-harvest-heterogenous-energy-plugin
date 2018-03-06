#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <linux/limits.h>
#include <common/string_enhanced.h>
#include <check.h>

int main ()
{
	char path[PATH_MAX];
	char line[16];
	FILE *fd;

	// General test
	assert(find_module("ipmi_devintf"));

	// Test if scaling is supported
	sprintf(path, "/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed");
    fd = fopen(path, "r");
	
	assert(fd != NULL);
	assert(fgets (line, 16, fd) != NULL);
	
	strclean(line, '\n');
	assert(strcmp("<unsupported>", line) != 0);

	fclose(fd);

	// Test if the scaling driver is effectively acpi-cpufreq
	sprintf(path, "/sys/devices/system/cpu/cpu0/cpufreq/scaling_driver");
    fd = fopen(path, "r");
    
	assert(fd != NULL);
    assert(fgets (line, 16, fd) != NULL);

	strclean(line, '\n');
    assert(strcmp("acpi-cpufreq", line) == 0);
 
	fclose(fd);
 
    return 0;
}
