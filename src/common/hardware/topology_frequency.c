/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/sizes.h>
#include <common/types.h>
#include <common/states.h>

static ulong read_freq(int fd)
{
	char freqc[8];
	ulong freq;
	int i = 0;
	char c;

	while((read(fd, &c, sizeof(char)) > 0) && ((c >= '0') && (c <= '9')))
	{
		freqc[i] = c;
    	i++;
	}

	freqc[i] = '\0';
	freq = atoi(freqc);
	return freq;
}

state_t topology_freq_getbase(uint cpu, ulong *freq_base)
{
	char path[1024];
	int fd;

	sprintf(path,"/sys/devices/system/cpu/cpu%d/cpufreq/scaling_available_frequencies", cpu);
	
	if ((fd = open(path, O_RDONLY)) < 0)
	{
		*freq_base = 1000000LU;
    	return EAR_ERROR;
	}
	
	ulong f0 = read_freq(fd);	
	ulong f1 = read_freq(fd);	
	ulong fx = f0 - f1;

	if (fx == 1000) {
		*freq_base = f1;
	} else {
		*freq_base = f0;
	}

	close(fd);

	return EAR_SUCCESS;
}
