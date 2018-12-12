/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/



#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <metrics/ipmi/energy_node.h>
#include <common/config.h>
#include <papi.h>

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

    if (getuid()!=0){
        printf("Warning, this test need root privileges, execute it as root or with sudo\n");
    }


	if (argc != 2) {
		usage(argv[0]);
	}

    if (node_energy_init() < 0)
    {
        printf("Error\n");
        return 0;
    }

    // Initializations
    iterations          = atoi(argv[1]);
	call_time_us_dc     = 0;
    start_time_us = PAPI_get_real_usec();
    for (i = 0; i < iterations; ++i)
    {
            read_dc_energy(&energy);
    }
    call_time_us_dc += PAPI_get_real_usec() - start_time_us;

    node_energy_dispose();

    call_time_avg_dc = (double) call_time_us_dc / (double) iterations;
    call_time_avg_dc = call_time_avg_dc / 1000;


    printf("read_dc_energy overhead: %0.3lf ms\n", call_time_avg_dc);

    return 0;
}
