/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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
#include <ear_daemon_client.h>
#include <config.h>

int EAR_VERBOSE_LEVEL = 4;

int main (int argc, char *argv[])
{
    struct timespec requirement, remaining;
    long long start_time_us, call_time_us_ac, call_time_us_dc;
    double call_time_avg_ac, call_time_avg_dc;
    unsigned long energy;
    int iterations;
    int i;

    ear_daemon_client_connect();

    // Initializations
    requirement.tv_sec  = 0;
    requirement.tv_nsec = 100000000L;
    iterations          = 10000;
    call_time_us_dc     = 0;
    call_time_us_ac     = 0;

    for (i = 0; i < iterations; ++i)
    {
        {
            start_time_us = PAPI_get_real_usec();
            ear_daemon_client_node_dc_energy(&energy);
            call_time_us_dc += PAPI_get_real_usec() - start_time_us;

        }
    }


    call_time_avg_dc = (double) call_time_us_dc / (double) iterations;
    call_time_avg_dc = call_time_avg_dc / 1000;

    printf("read_dc_energy overhead: %0.3lf ms\n", call_time_avg_dc);

    ear_daemon_client_disconnect();

    return 0;
}
