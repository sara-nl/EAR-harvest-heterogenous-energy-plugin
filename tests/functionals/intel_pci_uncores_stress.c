#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ear_uncores.h>
#include <ear_arch_type.h>
#include <config.h>

int EAR_VERBOSE_LEVEL = 4;

int main (int argc, char *argv[])
{
    char command[PATH_MAX];
    unsigned long long values[128];
    unsigned long long total_bytes;
    unsigned long long total_cas = 0;
    int num_counters;
    int cpu_model;
    int i;

    printf("Testing %s\n", argv[1]);
    cpu_model = get_model(85);
    printf("CPU Model: \t\t%i\n", cpu_model);
    num_counters = init_uncores(cpu_model);
    printf("Uncores detected: \t%i\n", num_counters);
    printf("-------------------------- Working\n");
    start_uncores();
    reset_uncores();

    sprintf(command, "%s &> /dev/null", argv[1]);
    system(command);

    stop_uncores(values);

    printf("Reads\t  Writes\n");
    for (i = 0; i < num_counters; i += 2)
    {
        printf("%s\t  %s\n", format_ull(values[i]), format_ull(values[i+1]));
        total_cas += values[i] + values[i+1];
    }
    total_bytes = total_cas * 64;
    printf("-------------------------- Summary\n");
    printf("Total cas operations: %s ops\n", format_ull(total_cas));
    printf("Total bytes transferred: %s bytes\n", format_ull(total_bytes));

    dispose_uncores();
    return 0;
}
