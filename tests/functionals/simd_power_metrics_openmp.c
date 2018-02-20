/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/
#define _GNU_SOURCE
#include <math.h>
#include <sched.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <emmintrin.h> // -msse2
#include <immintrin.h> // -mavx -mfma -mfavx512
#include <papi.h>

#include <ear_rapl_metrics.h>
#include <ear_frequency.h>
#include <ear_turbo.h>

#include <metrics/flops.h>
#include <metrics/stalls.h>
#include <metrics/instructions.h>
#include <types/generic.h>
#include <hardware.h>

#define INST_ITER   64.0

static ulong n_iterations;
int EAR_VERBOSE_LEVEL = 4;
uint test = 0;

int open_csv()
{
    const char *HEADER = "JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;"  \
        "GFLOPS_1_proces;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;" \
        "TOTAL_INST;CORES\n";
    const char *FILE = "sim_metrics.csv";

    int fd = open(FILE, O_WRONLY);

    if (fd < 0)
    {
        #define OPTIONS O_WRONLY | O_CREAT
        #define PRIVILEGES S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
        fd = open(FILE, OPTIONS, PRIVILEGES);

        if (fd < 0)
	{
            fprintf(stderr, "Error creating csv file %s\n", strerror(errno));
            exit(1);
        }

        write(fd, HEADER, strlen(HEADER));
    }

    lseek(fd, 0, SEEK_END);
    return fd;
}

void usage()
{
    printf("Usage: simd_power_metrics n_sockets n_threads n_iterations csv\n");
    printf("- n_sockets: number of sockets in the node\n");
    printf("- n_threads: threads to create and bind\n");
    printf("- n_iterations: number of n_iterations to gather energy metrics\n");
    printf("- csv: print output in csv format (0,1)\n");
    printf("- frequency \n");
    exit(1);
}

int main (int argc, char *argv[])
{
    ull metrics[EAR_RAPL_EVENTS];
    long long total_flops[8];
    char nodename[256];
    char buffer[1024];

    ulong start_time, exec_time;
    ulong num_ops, frequency, aux;
    long long papi_flops, total_fp_inst;
    long long cycles, inst, stalls;
    unsigned long F,F_BASE;

    double power_dram_w, power_dc_w;
    double power_ins, power_w, power_raw, power_raw_w;
    unsigned long dc_energy_j, dc_energy_init, dc_energy_end;
    double energy_nj, energy_j, energy_raw, energy_raw_j;
    double energy_dram_raw, energy_dram_j;
    double time_s, flops_m, flops_x_watt;

    uint n_tests, n_sockets, n_threads;
    uint i_test, i_socket;
    int full_compatible;
    int cpu, fd, csv;

    if (argc != 6) {
        usage();
    }

    // Options
    n_sockets = atoi(argv[1]);
    n_threads = atoi(argv[2]);
    n_iterations = strtoul(argv[3], NULL, 10);
    csv = atoi(argv[4]);
	F= (unsigned long) atoi(argv[5]);

    // Node info
    cpu = get_model();
    gethostname(nodename, sizeof(nodename));
    full_compatible = (cpu == CPU_SKYLAKE) || (cpu == CPU_SKYLAKE_X);
    printf("CPU model detected %d\n", cpu);
    n_tests = 9;

    // CSV file
    if (csv) {
        fd = open_csv();
    }

    init_rapl_metrics();
    init_basic_metrics();

    if (full_compatible)
    {
    	init_flops_metrics();
    	init_stall_metrics();
    }

    ear_cpufreq_init();
    node_energy_init();
	F_BASE=ear_cpufreq_get(0);
    printf("Default frequency was %lu\n",F_BASE);
	printf("Setting frequency to %lu\n",F);
	printf("Using %d cores\n",n_threads);
    ear_cpufreq_set_node(F);

    // Creating the threads
    for (i_test = 0; i_test < 9; ++i_test)
    {
        printf("Starting test %d\n",i_test);
        test = i_test;
        energy_raw = 0;
        energy_dram_raw = 0;
		if (test_available(i_test,cpu)){

        #pragma omp parallel firstprivate(test) shared(dc_energy_init, dc_energy_end, exec_time, \
        frequency, metrics, papi_flops, total_flops,cycles,inst) num_threads(n_threads)
        {
            if (omp_get_thread_num() == 0)
            {
                start_rapl_metrics();

                if (full_compatible)
                {
                    start_flops_metrics();
                    start_stall_metrics();
                }

                ear_begin_compute_turbo_freq();
                read_dc_energy(&dc_energy_init);
                start_time = PAPI_get_real_usec();
                start_basic_metrics();
            }

            run_test(test,n_iterations,cpu);

            if (omp_get_thread_num() == 0)
            {
                stop_basic_metrics(&cycles,&inst);
                exec_time = (PAPI_get_real_usec() - start_time);
                read_dc_energy(&dc_energy_end);

                frequency = ear_end_compute_turbo_freq();
                stop_rapl_metrics(metrics);

                if (full_compatible)
                {
                    stop_flops_metrics(&papi_flops,&total_flops[0]);
                    stop_stall_metrics(&stalls);
                    reset_flops_metrics();
                    reset_stall_metrics();
                } else {
                    papi_flops = get_test_ops(test,cpu) * n_iterations * INST_ITER;
                }

                reset_rapl_metrics();
                reset_basic_metrics();
            }
        }

        dc_energy_j = (dc_energy_end-dc_energy_init) / 1000;

        if (full_compatible)
        {
            total_fp_inst = total_flops[0] + total_flops[1] + total_flops[2] + total_flops[3] +
                    total_flops[4] + total_flops[5] + total_flops[6] + total_flops[7];
        }

        for (i_socket = 0; i_socket < n_sockets; ++i_socket)
        {
            // Energy per socket in nano juls
            energy_raw += (double) metrics[2 + i_socket];
            energy_dram_raw += (double) metrics[i_socket];
        }

        // Energy (nJ -> J) = energy / 1 000 000 000
        // Time   (us -> s) = time   / 1 000 000

        // Exec time in seconds
        time_s = ((double) exec_time) / (double) 1000000;
        power_dc_w = (double) dc_energy_j / (double) time_s;

        // Raw data
        energy_raw_j = energy_raw / (double) 1000000000;
        power_raw_w = energy_raw_j / time_s;

        energy_dram_j = energy_dram_raw / (double) 1000000000;
        power_dram_w = energy_dram_j / time_s;

        // Energy per core (nJ)
        energy_nj = energy_raw / ((double) n_threads);
        // Energy per core (J)
        energy_j = energy_nj / (double) 1000000000;
        // Power (W) = energy (J) / time (s)
        power_w = energy_j / time_s;
        // Power divided per iteration
        power_ins = power_w / ((double) (n_iterations*INST_ITER) / time_s);
        // Power to uW
        power_ins = power_ins * 1000000;

        num_ops = (ull) get_test_ops(test, cpu) * n_iterations * INST_ITER;
        // Floating point operations per second
        flops_m = ((double) num_ops) / time_s;
        // Flops to Mflops
        flops_m = flops_m / (double) 1000000000;
        // Mflops x watt
        flops_x_watt = flops_m / power_w;

        printf("TEST (%d): %s\n", i_test, get_test_name(test,cpu));
        printf("\tEnergy (j): %.2lf\n", energy_raw_j);
        printf("\tAvg. Power pckg (W): %.2lf\n", power_raw_w);
        printf("\tAvg. Power core (W): %.2lf\n", power_w);
        printf("\tAvg. Power dram (W) %.2lf\n", power_dram_w);
        printf("\tAvg. Power dc (W): %.2lf\n", power_dc_w);
        printf("\tTime (s): %.2lf\n", time_s);
        printf("\tEnergy (core) (J): %.2lf\n", energy_j);
        printf("\tPower (instr) (uW): %.3lf\n", power_ins);
        printf("\tGflops (core): %.3lf\n", flops_m);
        printf("\tGflops/watt: %.3lf\n", flops_x_watt);
        printf("\tTotal FP_OPS: %lu\n", num_ops);
        printf("\tAvg. Frequency (MHz): %lu\n", frequency);

        // PAPI
        flops_m = ((double) papi_flops) / time_s;
        flops_m = flops_m / 1000000000.0;

        flops_x_watt  = (papi_flops * n_threads);
        flops_x_watt /= (power_raw_w * (double) 1000000000 * time_s);

        printf("\tPAPI flops: %.3lf\n", papi_flops);
        printf("\tPAPI Gflops: %.3lf\n", flops_m);
        printf("\tPAPI Gflops/watt: %.3lf\n", flops_x_watt);
        printf("\tPAPI Ops/sec: %lf\n", (double) n_iterations *INST_ITER / time_s);
        printf("\tPAPI Avg. CPI: %lf\n", (double) cycles / (double) inst);

        if (full_compatible)
        {
            printf("\tPAPI stall cycles: %lf\n", (double) stalls/ (double) cycles);
            printf("\tPAPI AVX512 percentage: %lf\n", (double) total_flops[7] / (double) inst);
            printf("\tPAPI FP percentage: %lf\n", (double) total_fp_inst / (double) inst);
            printf("\tPAPI total instructions: %llu\n", inst);
        }

        if (csv)
        {
            sprintf(buffer, "%d;%s;%s;%lu;%lf;%lf;%lf;%lf;%lf;%lf;%lu;" \
                "%lu;%u\n", getpid(), nodename, get_test_name(test, cpu),
                frequency, time_s, (double) cycles / (double) inst, flops_m, power_dc_w,
                power_dram_w, power_raw_w, F, (unsigned long) INST_ITER * n_iterations, n_threads);
            write(fd, buffer, strlen(buffer));
        }
		}else{ // This test is not available
            sprintf(buffer, "%d;%s;%s;%lu;%lf;%lf;%lf;%lf;%lf;%lf;%lu;" \
                "%lu;%u\n", getpid(), nodename, get_test_name(test, cpu),
                0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, F, 0, 0);
            write(fd, buffer, strlen(buffer));
		}
    }

    ear_cpufreq_set_node(F_BASE);
    ear_cpufreq_end();
    node_energy_dispose();

    return 0;
}
