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
