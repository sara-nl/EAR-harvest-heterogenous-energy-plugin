/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/version.h>
#include <cpufreq.h>
#include <papi.h>

#define _GNU_SOURCE
#define __USE_GNU

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
#include <cpupower.h>
#endif

#include <metrics/custom/frequency.h>
#include <daemon/ear_frequency.h>
#include <common/ear_verbose.h>
#include <common/states.h>

static const char *__NAME__ = "EARD_FREQ";

static const PAPI_hw_info_t *hwinfo;
static struct cpufreq_policy *current_policy;
static struct cpufreq_policy prev_policy;
static ulong *ear_cpufreq_pstates;
static ulong ear_nominal_freq;
static ulong *ear_cpufreq;
static uint ear_num_p_states;
static uint ear_num_cpus;

int ear_cpufreq_init()
{
	struct cpufreq_available_frequencies *list_f, *first;
	int status, i, retval;

	//TODO: generalize
	if (PAPI_is_initialized()==PAPI_NOT_INITED)
	{
		retval = PAPI_library_init(PAPI_VER_CURRENT);

		if ((retval != PAPI_VER_CURRENT) && (retval>0))
		{
			ear_verbose(0,"eard: Papi can not be initialised \n");
			return EAR_ERROR;
		}
	}

	// Reading hardware information
	if ((hwinfo = PAPI_get_hardware_info()) == NULL)
	{
		VERBOSE_N(0, "PAPI hardware scanning returned NULL");
		return EAR_ERROR;
	} else {
		// 1- We detect the number of cpus based on papi information
		ear_num_cpus = hwinfo->sockets *
					   hwinfo->cores *
					   hwinfo->threads;

		VERBOSE_N(2, "detected %u sockets, %u CPUs and %u threads", hwinfo->sockets,
				  hwinfo->cores, hwinfo->threads);
	}

	ear_cpufreq = (ulong *) malloc(sizeof(ulong) * ear_num_cpus);

	if (ear_cpufreq == NULL)
	{
		VERBOSE_N(0, "ERROR, can't allocate memory");
		return EAR_ERROR;
	}

	// 2-We check all the cpus are online, we should detect cores but
	// we start with this approach
	for (i = 0; i < ear_num_cpus; i++)
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
		// negative errno values in error case
		status = !cpupower_is_cpu_online(cpu);
		#endif
		ear_cpufreq[i] = 0;

		if (status == 0) {
			ear_cpufreq[i] = cpufreq_get(i);
		}
	}

	// 3-We are assuming all the cpus supports the same set of frequencies
	// we check for cpu 0
	list_f = cpufreq_get_available_frequencies(0);
	first = list_f;

	while(list_f != NULL)
	{
		list_f = list_f->next;
		ear_num_p_states++;
	}

	if (ear_num_p_states == 0) {
		VERBOSE_N(0, "ERROR, no P_STATEs detected, make sure the CPUPower driver is working properly");
		return EAR_ERROR;
	}
	// else
	VERBOSE_N(2, "%d P_STATEs detected", ear_num_p_states);

	// 4-Detecting the list of pstates available
	ear_cpufreq_pstates = (ulong *) malloc(sizeof(ulong) * ear_num_p_states);

	if (ear_cpufreq_pstates == NULL) {
		VERBOSE_N(0, "ERROR, can't allocate memory");
		return EAR_ERROR;
	}

	list_f = first;
	i = 0;

	while(list_f!=NULL)
	{
		ear_cpufreq_pstates[i]=list_f->frequency;
		list_f=list_f->next;
		ear_debug(3,"eard::: P_state %d is %lu\n", i, ear_cpufreq_pstates[i]);
		i++;
	}

	ear_nominal_freq=ear_cpufreq_pstates[1];
	ear_verbose(2,"eard: Nominal frequency set to %lu\n", ear_nominal_freq);

	// this functions releases the memory allocated in cpufreq_get_available_frequencies function
	cpufreq_put_available_frequencies(first);

	// we get (and latter put) the current policy=governor
	current_policy=cpufreq_get_policy(0);
	prev_policy.min=current_policy->min;
	prev_policy.max=current_policy->max;

	prev_policy.governor=(char *)malloc(strlen(current_policy->governor)+1);
	strcpy(prev_policy.governor,current_policy->governor);

	ear_verbose(1,"eard::: Saving current governor %s\n",current_policy->governor);
	cpufreq_put_policy(current_policy);
	ear_set_userspace();

	#ifdef EAR_BOOST
	aperf_init(ear_num_cpus);

	for (i = 0; i < ear_num_cpus; i++) {
		aperf_init_cpu(i, ear_nominal_freq);
	}
	#endif

	return 0;
}

void ear_cpufreq_end()
{
	int i;

	ear_debug(1,"EAR_DAEMON:: Restoring previous governor %s\n", prev_policy.governor);

	for (i = 0; i < ear_num_cpus; i++) {
		cpufreq_set_policy(i, &prev_policy);
	}

	#ifdef EAR_BOOST
	aperf_dispose();
	#endif
}

void ear_set_userspace()
{
	int i;

	for (i = 0; i < ear_num_cpus; i++) {
		cpufreq_modify_policy_governor(i,"userspace");
	}
}

static uint ear_is_valid_frequency(ulong f)
{
	uint i = 0;
	while((i<ear_num_p_states) && (ear_cpufreq_pstates[i]!=f)) i++;
	if (i < ear_num_p_states) return 1;
	else return 0;
}

ulong ear_cpufreq_set_node(ulong newfreq)
{
	uint i = 0;

	if (ear_is_valid_frequency(newfreq))
	{
		ear_verbose(1,"eard: Setting node: freq = %lu\n", newfreq);

		for (i = 0; i < ear_num_cpus; i++)
		{
			ear_cpufreq[i]=newfreq;
			// This is a privileged function
			cpufreq_set_frequency(i, newfreq);
		}

		return newfreq;
	}
	return ear_cpufreq[0];
}

uint ear_get_num_p_states()
{
	return ear_num_p_states;
}

ulong ear_get_freq(uint i)
{
	return ear_cpufreq_pstates[i];
}

void ear_set_turbo()
{
	int i;

	for (i = 0; i < ear_num_cpus; i++) {
		cpufreq_modify_policy_governor(i,"performance");
	}
}

/*
 *
 *
 * NO MORE
 *
 *
 */
void ear_begin_compute_turbo_freq()
{
	int i;

	ear_debug(2,"EAR_DAEMON:: EAR_begin_compute_turbo_freq\n");

#ifdef EAR_BOOST
	for (i = 0; i < ear_num_cpus; i++) {
		aperf_get_avg_frequency_init(i);
	}
#endif
}

void ear_begin_app_compute_turbo_freq()
{
	ear_debug(2,"EAR_DAEMON:: ear_begin_app_compute_turbo_freq\n");

#ifdef EAR_BOOST
	int i;
	for (i = 0; i < ear_num_cpus; i++) {
		aperf_start_computing_app_avg_freq(i);
	}
#endif
}

ulong ear_end_compute_turbo_freq()
{
	ulong new_freq, freq = 0;
	int i;

#ifdef EAR_BOOST
	ear_debug(2,"EAR_DAEMON:: EAR_end_compute_turbo_freq\n");

	for (i = 0; i < ear_num_cpus; i++)
	{
		aperf_get_avg_frequency_end(i,&new_freq);
		freq += new_freq;
	}

	return (freq / ear_num_cpus);
#endif
	return ear_nominal_freq;
}

ulong ear_end_app_compute_turbo_freq()
{
#ifdef EAR_BOOST
	ulong freq = 0, new_freq;
	int i;

	ear_debug(2,"EAR_DAEMON:: EAR_end_compute_turbo_freq\n");
	for (i = 0; i <ear_num_cpus; i++)
	{
		aperf_end_computing_app_avg_freq(i,&new_freq);
		freq += new_freq;
	}
	return (freq / ear_num_cpus);
#endif
	return ear_nominal_freq;
}


//TODO: Not for daemon
unsigned long ear_cpufreq_get(unsigned int cpuid)
{
	unsigned long f;
	ear_debug(4,"eard::: cpufreq_get for cpu %u\n",cpuid);
	if (cpuid>ear_num_cpus) return 0;
	f=cpufreq_get(cpuid);
	ear_debug(4,"eard::: getcpufreq cpu(%u) freq = %lu\n", cpuid, f);
	return ear_cpufreq[cpuid]=f;

}
