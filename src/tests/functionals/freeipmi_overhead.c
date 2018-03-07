/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/

#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <metrics/ipmi/energy_node.h>
#include <common/config.h>

int EAR_VERBOSE_LEVEL = 4;

void usage(char *bin)
{
    printf("Usage: %s n_iterations\n", bin);
    printf("- n_iterations: number of executing iterations to determine the average overhead\n");
    exit(1);
}

int main (int argc, char *argv[])
{
    struct timespec requirement, remaining;
    long long start_time_us, call_time_us_ac, call_time_us_dc;
    double call_time_avg_ac, call_time_avg_dc;
    unsigned long energy;
    int iterations;
    int i;

	if (argc != 2) {
		usage(argv[0]);
	}

    if (node_energy_init() < 0)
    {
        printf("Error\n");
        return 0;
    }

    // Initializations
    requirement.tv_sec  = 0;
    requirement.tv_nsec = 100000000L;
    iterations          = atoi(argv[1]);
    call_time_us_dc     = 0;
    call_time_us_ac     = 0;

    for (i = 0; i < iterations; ++i)
    {
        //if (nanosleep(&requirement, &remaining) != -1)
        {
            start_time_us = PAPI_get_real_usec();
            read_dc_energy(&energy);
            call_time_us_dc += PAPI_get_real_usec() - start_time_us;

            start_time_us = PAPI_get_real_usec();
            read_ac_energy(&energy);
            call_time_us_ac += PAPI_get_real_usec() - start_time_us;
        }
    }

    node_energy_dispose();

    call_time_avg_dc = (double) call_time_us_dc / (double) iterations;
    call_time_avg_ac = (double) call_time_us_ac / (double) iterations;
    call_time_avg_dc = call_time_avg_dc / 1000;
    call_time_avg_ac = call_time_avg_ac / 1000;

    printf("read_dc_energy overhead: %0.3lf ms\n", call_time_avg_dc);
    printf("read_ac_energy overhead: %0.3lf ms\n", call_time_avg_ac);

    return 0;
}
