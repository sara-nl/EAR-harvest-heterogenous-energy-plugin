/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
/*
 * Usage:
 * This library gets the average frequency of a physical core
 * (boost included) during a period of time. To do that, a
 * snapshot of the current core state has to be taken before
 * that period of time.
 *
 * Then aperf_get_avg_frequency_init() has to be called before
 * the working period, passing to it the MHz of the nominal
 * frequency (TSC) and the number of the core that you want to
 * get the frequency.
 *
 * When the work is done, aperf_get_avg_frequency_end() has
 * to be called passing the core number to get the average
 * frequency (MHz) of that CPU during that period of time.
 *
 * You HAVE to call aperf_init() passing the number of CPUs
 * your system has, before all any other call, to allocate
 * memory for the state savings. Later, aperf_cpu_init()
 * passing to initialize a core with it's nominal frequency.
 * And also aperf_dispose() has to be called to free that
 * memory when at the end.
 *
 * Errors:
 * Non static functions return as an error EAPERF (-1).
 *
 * Requirements:
 * - The MSR module has to be loaded (/modprobe msr).
 * - You need permissions to read MSR file.
 *
 * Test:
 * - You can compare the frequency values with this call:
 * sudo watch -n1 cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq
 * - You can use sleep() to simulate a period of working
 * time.
 * - You can see your power governor here:
 * sudo cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include <metrics/custom/frequency.h>
#include <metrics/custom/hardware_info.h>
#include <common/states.h>

// INTEL APERF/MPERF MSR registers
//
// MSR:
// - The MSRs are per logical processor; they measure performance
// only when the targeted processor is in the C0 state.
// - MSRs with a scope of "thread" are separate for each logical
// processor and can only be accessed by the specific logical
// processor.
// - MSRs with a scope of "core" are separate for each core, so
// they can be accessed by any logical processor (thread context)
// running on that core.
// - MSRs with a scope of "package" are global to the package, so
// access from any core or thread context in that package will
// access the same register.
//
// Frequency:
// - IA32_MPERF MSR (E7H) increments in proportion to a fixed
// frequency, only increases in C0 state and stops ticking in
// any idle state.
// - IA32_APERF MSR (E8H) increments in proportion to actual
// performance.
// - Only the IA32_APERF / IA32_MPERF ratio is architecturally
// defined; software should not attach meaning to the content of
// the individual of IA32_APERF or IA32_MPERF MSRs.
// - When either MSR overflows, both MSRs are reset to zero and
// continue to increment.
// - Both MSRs are full 64-bits counters. Each MSR can be written
// to independently.
//
// Frequency formula:
// - performance_percent = (delta_APERF / delta_MPERF)
// - freq_mhz = base_MHz * performance_percent
//
// Compatibility:
// - Use CPUID to check the P-State hardware coordination feedback
// capability bit. CPUID.06H.ECX[Bit 0] = 1 indicates IA32_MPERF
// MSR and IA32_APERF MSR are present.
// - May work poorly on Linux-2.6.20 through 2.6.29, as the
// acpi-cpufreq kernel frequency driver periodically cleared
// aperf/mperf registers in those kernels.
//
// Documentation:
// - Intel® 64 and IA-32 Architectures Software Developer’s Manual
// chapter 35.


unsigned int _num_cpus;

struct avg_perf_cpu_info
{
    unsigned long nominal_freq;
    unsigned long saved_aperf;
    unsigned long saved_mperf;
    unsigned int snapped;
} *cpu_info, *cpu_info_global;

inline static int read_ulong(int fd, unsigned int pos, unsigned long *value)
{
    if (lseek(fd, pos, SEEK_SET) == -1) {
        close(fd);
        return EAPERF;
    }

    return read(fd, value, 8);
}

static int get_aperf_mperf(int cpu, unsigned long *aperf, unsigned long *mperf)
{
    char msr_file_name[64];
    int fd, result1, result2;

    result1 = result2 = 0;
    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    fd = open(msr_file_name, O_RDONLY);

    if (fd < 0) {
		perror("EAR_APERF: opening MSR file");
        return EAPERF;
    }

    result1 = read_ulong(fd, MSR_IA32_MPERF, mperf);
    result2 = read_ulong(fd, MSR_IA32_APERF, aperf);
    result1 = -(result1 != 8 || result2 != 8);

    close(fd);
    return result1;
}

int aperf_init(unsigned int num_cpus)
{
    if (!is_aperf_compatible()) {
        return EAPERF;
    }

    _num_cpus = num_cpus;
    int size = sizeof(struct avg_perf_cpu_info);
    int result = posix_memalign((void *) &cpu_info, size, size * num_cpus);
    if (result!=0) return EAPERF;
    result = posix_memalign((void *) &cpu_info_global, size, size * num_cpus);
    return -(result != 0) ;
}

int aperf_init_cpu(unsigned int cpu, unsigned long max_freq)
{
    if (cpu >= _num_cpus) {
        return EAR_ERROR;
    }
    cpu_info[cpu].nominal_freq = max_freq;
    cpu_info_global[cpu].nominal_freq = max_freq;

    return EAR_SUCCESS;
}

int apert_init_all_cpus(unsigned int num_cpus, unsigned long max_freq)
{
    int i;

    aperf_init(num_cpus);

    for (i = 0; i < num_cpus; i++) {
        aperf_init_cpu(i, max_freq);
    }

    return EAR_SUCCESS;
}

void aperf_dispose()
{
	if (cpu_info != NULL) free(cpu_info);
	if (cpu_info_global!=NULL) free(cpu_info_global);
}

int aperf_start_avg_freq(int cpu,struct avg_perf_cpu_info *my_cpu_info)
{
    uint64_t aperf, mperf;
    int result;
	
    if (cpu >= _num_cpus) {
        return EAPERF;
    }

    result = get_aperf_mperf(cpu, &aperf, &mperf);

    if (result != 0) {
        return EAPERF;
    }

    my_cpu_info[cpu].saved_aperf = aperf;
    my_cpu_info[cpu].saved_mperf = mperf;
    my_cpu_info[cpu].snapped = 1;

    return 0;
}

int aperf_end_avg_freq(unsigned int cpu, struct avg_perf_cpu_info *my_cpu_info, unsigned long *frequency)
{
    unsigned long current_aperf, current_mperf;
    unsigned long mperf_diff, aperf_diff;
    unsigned int perf_percent = 0;
    int result;

    if (cpu >= _num_cpus) {
        return EAPERF;
    }

    if (!my_cpu_info[cpu].snapped) {
        return EAPERF;
    }

    result = get_aperf_mperf(cpu, &current_aperf, &current_mperf);

    if (result != 0) {
        return EAPERF;
    }

    aperf_diff = current_aperf - my_cpu_info[cpu].saved_aperf;
    mperf_diff = current_mperf - my_cpu_info[cpu].saved_mperf;
    my_cpu_info[cpu].saved_aperf = current_aperf;
    my_cpu_info[cpu].saved_mperf = current_mperf;

    // Preventing the possible overflow removing 7 bits,
    // which is the maximum incresing bits in that
    // multiplication.
    if (((unsigned long) (-1) / 100) < aperf_diff)
    {
        aperf_diff >>= 7;
        mperf_diff >>= 7;
    }

    // Frequency formula. Multiplication per 100 to power
    // the decimals of an integer division.
    perf_percent = (aperf_diff * 100) / mperf_diff;

    // The division again turns the value to MHz, because
    // it was increased before.
    *frequency = (my_cpu_info[cpu].nominal_freq * perf_percent) / 100;
    return 0;
}

/*
 *
 * Extra functions
 *
 */
int aperf_get_avg_frequency_init(unsigned int cpu)
{
    return aperf_start_avg_freq(cpu, cpu_info);
}

// ear_begin_compute_turbo_freq
void aperf_get_avg_frequency_init_all_cpus(unsigned int num_cpus)
{
    int i;

    for (i = 0; i < ear_num_cpus; i++) {
        aperf_get_avg_frequency_init(i);
    }
}

int aperf_get_global_avg_frequency_init(unsigned int cpu)
{
    return aperf_start_avg_freq(cpu, cpu_info_global);
}

// ear_begin_app_compute_turbo_freq
void aperf_get_global_avg_frequency_init_all_cpus(unsigned int num_cpus)
{
    int i;

    for (i = 0; i < num_cpus; i++) {
        aperf_get_global_avg_frequency_init(i);
    }
}

int aperf_get_avg_frequency_end(unsigned int cpu, unsigned long *frequency)
{
    return aperf_end_avg_freq(cpu, cpu_info, frequency);
}

// ear_end_compute_turbo_freq
unsigned long aperf_get_avg_frequency_end_all_cpus(unsigned int num_cpus)
{
    unsigned long new_freq, freq = 0;
    int i;

	for (i = 0; i < num_cpus; i++)
	{
        aperf_get_avg_frequency_end(i, &new_freq);
		freq += new_freq;
	}

	return (freq / num_cpus);
}

int aperf_get_global_avg_frequency_end(unsigned int cpu, unsigned long *frequency)
{
	return aperf_end_avg_freq(cpu, cpu_info_global, frequency);
}

// ear_end_app_compute_turbo_freq
unsigned long aperf_get_global_avg_frequency_end_all_cpus(unsigned int num_cpus)
{
    unsigned long new_freq, freq = 0;
    int i;

    for (i = 0; i < num_cpus; i++)
    {
        aperf_get_global_avg_frequency_end(i, &new_freq);
        freq += new_freq;
    }

    return (freq / num_cpus);
}
