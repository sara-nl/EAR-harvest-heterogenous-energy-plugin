//
// Created by xgomez on 7/03/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/version.h>
#include <cpufreq.h>
#include <papi.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
#include <cpupower.h>
#endif

#include <control/frequency.h>
#include <metrics/papi/generics.h>
#include <common/types/generic.h>
#include <common/ear_verbose.h>
#include <common/states.h>

static struct cpufreq_policy previous_cpu0_policy;
uint previous_cpu0_freq;
ulong *freq_list_rank; // List of frequencies of the whole rank (KHz)
ulong *freq_list_cpu; // List of frequencies of each CPU (KHz)
ulong freq_nom; // Nominal frequency (assuming CPU 0)
uint num_freqs;
uint num_cpus;

static const char* __NAME__ = "FREQUENCY_CONTROL";

//
static ulong *get_frequencies_cpu()
{
	ulong *freqs;
	int status, i;

	freqs = (ulong *) malloc(sizeof(ulong) * num_cpus);

	if (freqs == NULL) {
		VERBOSE_N(0, "ERROR while allocating memory, exiting");
		exit(1);
	}

	for (i = 0; i < num_cpus; i++)
	{
		#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 7, 0)
		// Returns:
		// X -> if not
		// 0 -> if the specified CPU is present
		status = cpufreq_cpu_exists(i);
		#else
		// Returns:
		// 1 -> if CPU is online
		// 0 -> if CPU is offline
		status = !cpupower_is_cpu_online(cpu);
		#endif
		freqs[i] = 0;

		if (status == 0) {
			freqs[i] = cpufreq_get(i);
			VERBOSE_N(0, "CPU %d is online and its frequency is %u \n", i, freqs[i]); // 4
		} else {
			VERBOSE_N(0, "CPU %d is offline\n", i; // 4
		}
	}

	return freqs;
}

//
static ulong *get_frequencies_rank()
{
	struct cpufreq_available_frequencies *list, *first;
	ulong *pointer;
	int i;

	// Kernel alloc
	list = cpufreq_get_available_frequencies(0);
	first = list;

	while(list != NULL)
	{
		list = list->next;
		num_freqs++;
	}

	VERBOSE_N(0, "%d frequencies available ", num_freqs); // 2

	//
	pointer = (ulong *) malloc(sizeof(ulong) * num_freqs);

	if (pointer == NULL) {
		VERBOSE_N(0, "ERROR while allocating memory, exiting");
		exit(1);
	}

	//
	list = first;
	i = 0;

	while(list!=NULL)
	{
		pointer[i] = list->frequency;
		list = list->next;
		i++;
	}

	// Kernel dealloc
	cpufreq_put_available_frequencies(first);

	return pointer;
}

// ear_cpufreq_init
int frequency_init()
{
	const PAPI_hw_info_t *hwinfo;
	struct cpufreq_policy *policy;
	int status, i;

	// TODO: metrics (PAPI) dependancy, remove and pass the number of cpus
	hwinfo = metrics_get_hw_info();

	if (hwinfo == NULL) {
		VERBOSE_N(0, "PAPI hardware scanning returned NULL, exiting");
		exit(1);
	}

	//
	num_cpus = hwinfo->sockets * hwinfo->cores * hwinfo->threads;
	VERBOSE_N(0, "detected %u CPUs");

	//
	freq_list_cpu = get_frequencies_cpu(num_cpus);

	//
	freq_list_rank = get_frequencies_rank();

	// Saving nominal freq = 1, because 0 is the turbo mode
	freq_nom = freq_list_rank[1];
	VERBOSE_N(0, "nominal frequency is %lu (KHz)", freq_nom);

	// Saving previous policy data
	previous_cpu0_freq = freq_list_cpu[0];

	// Kernel alloc
	policy = cpufreq_get_policy(0);

	previous_cpu0_policy.min = policy->min;
	previous_cpu0_policy.max = policy->max;

	previous_cpu0_policy.governor = (char *) malloc(strlen(policy->governor) + 1);
	strcpy(previous_cpu0_policy.governor, policy->governor);

	VERBOSE_N(0, "previous policy governor was %s", policy->governor);
	VERBOSE_N(0, "previous frequency was set to %lu (KHz)", previous_cpu0_freq);

	// Kernel dealloc
	cpufreq_put_policy(policy);

	return EAR_SUCCESS;
}

// ear_cpufreq_end
void frequency_dispose()
{
	int i;

	frequency_set_all_cpus(previous_cpu0_freq);

	for (i = 0; i < num_cpus; i++) {
		cpufreq_set_policy(i, &previous_cpu0_policy);
	}

	free(freq_list_rank);
	free(freq_list_cpu);
}


static int is_valid_frequency(ulong freq)
{
	int i = 0;

	while((i < num_freqs) && (freq_list_rank[i] != freq)) {
		i++;
	}

	if (i < num_freqs) return 1;
	else return 0;
}

uint frequency_get_num_online_cpus()
{
	return num_cpus;
}

// ear_cpufreq_set_node
ulong frequency_set_all_cpus(ulong freq)
{
	int i = 0;

	if (is_valid_frequency(freq))
	{
		VERBOSE_N(1, "setting all cpus to %lu KHz", freq);

		for (i = 0; i < num_cpus; i++)
		{
			freq_list_cpu[i] = freq;

			// This is a privileged function
			cpufreq_set_frequency(i, freq);
		}

		return freq;
	}
	return freq_list_cpu[0];
}

//
uint frequency_get_num_freqs()
{
	return num_freqs;
}

// ear_get_num_p_states
uint frequency_get_num_pstates()
{
	return num_freqs;
}

// ear_cpufreq_get
ulong frequency_get_cpu_freq(uint cpu)
{
	ulong f;

	if (cpu > num_cpus) {
		return 0;
	}

	// Kernel asking (not hardware)
	freq_list_cpu[cpu] = cpufreq_get(cpu);

	return freq_list_cpu[cpu];
}

// ear_get_nominal_frequency
ulong frequency_get_nominal_freq()
{
	return freq_nom;
}

// ear_get_pstate_list
ulong *frequency_get_freq_rank_list()
{
	return freq_list_rank;
}

// ear_get_freq
ulong frequency_pstate_to_freq(uint pstate)
{
	if (pstate >= num_freqs) return 1;
	return freq_list_rank[pstate];
}

// ear_get_pstate
ulong frequency_freq_to_pstate(uint freq)
{
	int i = 0, found = 0;

	while ((i < num_freqs) && (found == 0))
	{
		if (freq_list_rank[i] != freq) i++;
		else found = 1;
	}

	VERBOSE_N(0, "the P_STATE of %lu frequency is %u", freq, i);

	return i;
}

// ear_set_turbo TODO: ??
void frequency_set_performance_governor_all_cpus()
{
	int i;

	for (i = 0; i < num_cpus; i++) {
		cpufreq_modify_policy_governor(i, "performance");
	}
}

// ear_set_userspace TODO: ??
void frequency_set_userspace_governor_all_cpus()
{
	int i;

	for (i = 0; i < num_cpus; i++) {
		cpufreq_modify_policy_governor(i, "userspace");
	}
}
