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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <metrics/custom/bandwidth.h>
#include <metrics/custom/hardware_info.h>
#include <common/types/generic.h>

int EAR_VERBOSE_LEVEL = 4;
static char _buffer[32];

static void _format_ull (ull n)
{
    if (n < 0) {
        printf ("-");
        _format_ull(-n);
        return;
    }

    if (n < 1000) {
        sprintf(_buffer, "%s%llu", _buffer, n);
        return;
    }

    _format_ull(n / 1000);
    sprintf(_buffer, "%s,%03llu", _buffer, (n % 1000));
}

char* format_ull(ull n)
{
    _buffer[0] = '\0';
    _format_ull(n);
    return _buffer;
}

void usage(char *app)
{
	printf("Usage: sudo ./%s num_seconds\n",app);
	printf("reports the memory bandwith every seconds during num_seconds seconds\n");
	exit(0);
}

int main (int argc, char *argv[])
{
    double gbytes;
    unsigned long long values[128];
    unsigned long long total_bytes;
    unsigned long long total_cas = 0;
    int num_counters;
    int cpu_model;
    int i, j = 0;
	int num_steps;

    if (argc<2) usage(argv[0]);

    num_steps=atoi(argv[1]);

    printf("Testing %s\n", argv[1]);
    cpu_model = get_model(85);
    printf("CPU Model: \t\t%i\n", cpu_model);
    num_counters = init_uncores(cpu_model);
    printf("Uncores detected: \t%i\n", num_counters);

    while(j < num_steps)
    {
		total_bytes=0;
		total_cas=0;
        printf("-------------------------- Working\n");
        start_uncores();
        reset_uncores();
        sleep(1);
        stop_uncores(values);

        printf("Reads\t  Writes\n");
        for (i = 0; i < num_counters; i += 2)
        {
            printf("%s\t  %s\n", format_ull(values[i]), format_ull(values[i+1]));
            total_cas += values[i] + values[i+1];
        }
        total_bytes = total_cas * 64;
        gbytes = ((double) total_bytes) / 1000000000.0;

        printf("-------------------------- Summary\n");
        printf("%0.3lf GB/sec.\n", gbytes);
        ++j;
    }

    dispose_uncores();
    return 0;
}
