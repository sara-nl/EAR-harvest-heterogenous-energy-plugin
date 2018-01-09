#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ear_uncores.h>
#include <ear_arch_type.h>
#include <config.h>

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

int main (int argc, char *argv[])
{
    double gbytes;
    unsigned long long values[128];
    unsigned long long total_bytes;
    unsigned long long total_cas = 0;
    int num_counters;
    int cpu_model;
    int i, j = 0;

    printf("Testing %s\n", argv[1]);
    cpu_model = get_model(85);
    printf("CPU Model: \t\t%i\n", cpu_model);
    num_counters = init_uncores(cpu_model);
    printf("Uncores detected: \t%i\n", num_counters);

    while(j < 10)
    {
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
        printf("Total Gbytes transferred: %0.3lf bytes\n", gbytes);
        ++j;
    }

    dispose_uncores();
    return 0;
}
