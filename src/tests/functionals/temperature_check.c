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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <common/hardware/topology.h>
#include <metrics/temperature/temperature.h>

int main(int argc,char *argv[])
{
	llong values[32];
	topology_t topo;
	state_t s;
	ctx_t c;
	int i;

	state_assert(s, topology_init(&topo),);

    state_assert(s, temp_load(&topo),);
    
    state_assert(s, temp_init(&c),);

    while (1)
    {
		for (i = 0; i < topo.cpu_count; ++i) {
			state_assert(s, temp_read(&c, values, NULL),);
            printf("socket %d, temp: %llu\n", i, values[i]);
		}
		printf("--\n");
        sleep(3);
    }

	return 0;
}
