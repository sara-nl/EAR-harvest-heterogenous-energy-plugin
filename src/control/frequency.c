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
#include <unistd.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
#include <cpupower.h>
#else
#include <cpufreq.h>
#endif

#include <control/frequency.h>
#include <common/types/generic.h>
#include <common/ear_verbose.h>
#include <common/states.h>

static const char* __NAME__ = "FREQUENCY_CONTROL";

static struct cpufreq_policy previous_cpu0_policy;
static ulong previous_cpu0_freq;
static int saved_previous_policy;
static int saved_previous_freq;

static ulong *freq_list_rank; // List of frequencies of the whole rank (KHz)
static ulong *freq_list_cpu; // List of frequencies of each CPU (KHz)
static ulong freq_nom; // Nominal frequency (assuming CPU 0)
static uint num_freqs;
static uint num_cpus;


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

			if (freqs[i] == 0) {
				VERBOSE_N(0, "ERROR, CPU %d is online but the returned freq is 0", i);
			}
		} else {
			VERBOSE_N(0, "ERROR, CPU %d is offline", i); // 4
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

	if (list == NULL) {
		VERBOSE_N(0, "unable to find an available frequencies list");
	}

	while(list != NULL)
	{
		list = list->next;
		num_freqs++;
	}

	VERBOSE_N(2, "%d frequencies available ", num_freqs); // 2

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
int frequency_init(unsigned int _num_cpus)
{
	num_cpus = _num_cpus;

	//
	freq_list_cpu = get_frequencies_cpu();

	//
	freq_list_rank = get_frequencies_rank();

	// Saving nominal freq = 1, because 0 is the turbo mode
	freq_nom = freq_list_rank[1];
	VERBOSE_N(2, "nominal frequency is %lu (KHz)", freq_nom);

	return EAR_SUCCESS;
}

// ear_cpufreq_end
void frequency_dispose()
{
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

// Privileged function
ulong frequency_set_all_cpus(ulong freq)
{
	int result, i = 0;

	VERBOSE_N(1, "setting all cpus to %lu KHz", freq);
	if (is_valid_frequency(freq))
	{

		for (i = 0; i < num_cpus; i++)
		{
			freq_list_cpu[i] = freq;

			// This is a privileged function
			result = cpufreq_set_frequency(i, freq);

			if (result < 0) {
				//VERBOSE_N(0, "ERROR while switching cpu %d frequency to %lu (%s)", i, freq, strerror(-result));
				VERBOSE_N(0, "ERROR while switching cpu %d frequency to %lu ", i, freq);
			}
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
	if (pstate >= num_freqs) {
		VERBOSE_N(0, "higher P_STATE (%u) than the maximum (%u), returning nominal", pstate, num_freqs);
		return 1;
	}
	return freq_list_rank[pstate];
}

// ear_get_pstate
uint frequency_freq_to_pstate(ulong freq)
{
	int i = 0, found = 0;

	while ((i < num_freqs) && (found == 0))
	{
		if (freq_list_rank[i] != freq) i++;
		else found = 1;
	}

	VERBOSE_N(4, "the P_STATE of the frequency %lu is %d", freq, i);

	return i;
}

// Privileged function
void frequency_set_performance_governor_all_cpus()
{
	int i;

	for (i = 0; i < num_cpus; i++) {
		cpufreq_modify_policy_governor(i, "performance");
	}
}

// Privileged function
void frequency_set_ondemand_governor_all_cpus()
{
    int i;

    for (i = 0; i < num_cpus; i++) {
        cpufreq_modify_policy_governor(i, "ondemand");
    }
}


// Privileged function
void frequency_set_userspace_governor_all_cpus()
{
	int i;

	for (i = 0; i < num_cpus; i++) {
		cpufreq_modify_policy_governor(i, "userspace");
	}
}

void frequency_save_previous_frequency()
{
	// Saving previous policy data
	previous_cpu0_freq = freq_list_cpu[0];
	
	VERBOSE_N(1, "previous frequency was set to %lu (KHz)", previous_cpu0_freq);
	
	saved_previous_freq = 1;
}

// Privileged function
void frequency_recover_previous_frequency()
{
	if (!saved_previous_freq) {
		VERBOSE_N(2, "previous frequency not saved");
		return;
	}

	frequency_set_all_cpus(previous_cpu0_freq);
	saved_previous_freq = 0;
}

void frequency_save_previous_policy()
{
	struct cpufreq_policy *policy;
	
	// Kernel alloc
	policy = cpufreq_get_policy(0);

	previous_cpu0_policy.min = policy->min;
	previous_cpu0_policy.max = policy->max;

	previous_cpu0_policy.governor = (char *) malloc(strlen(policy->governor) + 1);
	strcpy(previous_cpu0_policy.governor, policy->governor);

	VERBOSE_N(2, "previous policy governor was %s", policy->governor);

	// Kernel dealloc
	cpufreq_put_policy(policy);
	saved_previous_policy = 1;
}

void frequency_recover_previous_policy()
{
	int status, i;

	if (!saved_previous_policy) {
		VERBOSE_N(2, "previous policy not saved");
		return;
	}

	for (i = 0; i < num_cpus; i++)
	{
		status = cpufreq_set_policy(i, &previous_cpu0_policy);

		if (status < 0) {
			//VERBOSE_N(0, "ERROR while switching policy for cpu %d (%s)", i, strerror(-status));
			VERBOSE_N(0, "ERROR while switching policy for cpu %d ", i);
		}
	}

	free(previous_cpu0_policy.governor);
	saved_previous_policy = 0;
}
