/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/version.h>
#include <cpufreq.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
#include <cpupower.h>
#endif

#include <metrics/papi/generics.h>
#include <library/common/externs.h>
#include <library/ear_frequency/ear_cpufreq.h>
#include <common/ear_daemon_client.h>
#include <common/types/generic.h>
#include <common/ear_verbose.h>

static const PAPI_hw_info_t *ear_cpufreq_hwinfo;
static struct cpufreq_policy *current_policy;
static struct cpufreq_policy prev_policy;
static ulong *ear_cpufreq_pstates;
static ulong ear_nominal_freq;
static ulong ear_prev_freq;
static ulong *ear_cpufreq;
static uint ear_num_p_states;
static uint ear_num_cpus;

unsigned int ear_get_pstate(unsigned long f)
{
	unsigned int i=0,found=0;
	while ((i<ear_num_p_states)&&(found==0)){
		if (ear_cpufreq_pstates[i]!=f) i++;
		else found=1;
	}
	return i;
}

void ear_cpufreq_end()
{
	int i;

	ear_cpufreq_set_node(ear_prev_freq);
#if 0
	ear_verbose(2,"EAR:  ear_cpufreq_end: Restoring previous governor %s and frequency %lu\n",
				prev_policy.governor, ear_prev_freq);
	for (i=0; i < ear_num_cpus; i++) {
		cpufreq_set_policy(i,&prev_policy);
	}
#endif
}

void ear_cpufreq_init()
{
	int status,i,retval;
	struct cpufreq_available_frequencies *list_f,*first;
	char *ear_tmp;
	char nodename[128];
	char ear_commreq[128],ear_commack[128];

	ear_verbose(4,"EAR: ear_cpufreq_init\n");

	//
	ear_cpufreq_hwinfo = metrics_get_hw_info();

	// We should detect cpus
	ear_num_cpus=ear_cpufreq_hwinfo->sockets*ear_cpufreq_hwinfo->cores*ear_cpufreq_hwinfo->threads;
	ear_verbose(2,"EAR: %u cpus detected (sockets %u cores %u threads %u)\n",ear_cpufreq_hwinfo->ncpu,ear_cpufreq_hwinfo->sockets,ear_cpufreq_hwinfo->cores,ear_cpufreq_hwinfo->threads);
	ear_cpufreq=(unsigned long *)malloc(sizeof(unsigned long)*ear_num_cpus);

	if (ear_cpufreq==NULL){
		ear_verbose(0,"EAR: malloc returns null for ear_cpulist.Exiting\n");
		exit(1);
	}

	// We check all the cpus are online, we should detect cores but
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
			ear_verbose(4,"EAR: Curent cpu frequency for cpu %u is %u \n", i, ear_cpufreq[i]);
		}
	}

	ear_prev_freq=ear_cpufreq[0];
	// We are assuming all the cpus supports the same set of frequencies
	// we check for cpu 0
        list_f=cpufreq_get_available_frequencies(0);
        first=list_f;
        while(list_f!=NULL){
        	list_f=list_f->next;
		ear_num_p_states++;
        }
	ear_verbose(2,"EAR: %d p_states available\n",ear_num_p_states);
	ear_cpufreq_pstates=(unsigned long *)malloc(sizeof(unsigned long)*ear_num_p_states);
	if (ear_cpufreq_pstates==NULL){
			ear_verbose(0,"EAR: malloc return NULL for ear_cpufreq_pstates.Exiting\n");
			exit(1);
	}
	
	list_f=first;
	i=0;
        while(list_f!=NULL){
		ear_cpufreq_pstates[i]=list_f->frequency;	
        	list_f=list_f->next;
		ear_verbose(4,"EAR: P_state %d is %u\n",i,ear_cpufreq_pstates[i]);
		i++;
	}	
	cpufreq_put_available_frequencies(first);

	ear_nominal_freq=ear_cpufreq_pstates[1];
	ear_verbose(2,"EAR: Nominal frequency detected %u\n",ear_nominal_freq);
#if 0
	current_policy=cpufreq_get_policy(0);
	prev_policy.min=current_policy->min;
	prev_policy.max=current_policy->max;
	prev_policy.governor=(char *)malloc(strlen(current_policy->governor)+1);
	strcpy(prev_policy.governor,current_policy->governor);
	ear_verbose(2,"EAR: Saving current governor %s\n",current_policy->governor);
	cpufreq_put_policy(current_policy);
#endif
	ear_verbose(4,"EAR: ear_cpufreq_init success\n");
}

unsigned long ear_max_f()
{
	return ear_cpufreq_pstates[1];
}
unsigned long ear_min_f()
{
	return ear_cpufreq_pstates[ear_num_p_states-1];
}

unsigned long ear_cpufreq_get(unsigned int cpuid)
{
	unsigned long f;
	ear_verbose(4,"EAR: ear_cpufreq_get for cpu %u\n",cpuid);
	if (cpuid>ear_num_cpus){ 
		ear_verbose(0,"EAR: Invalid cpu number in ear_cpufreq_get (%s)\n",cpuid);
		return 0;
	}
	f=cpufreq_get(cpuid);
	ear_verbose(4,"EAR: ear_cpufreq_get for cpu %u f=%u\n",cpuid,f);
	return ear_cpufreq[cpuid]=f;
}

unsigned int ear_is_valid_frequency(unsigned long f)
{
	unsigned int i=0;
	while((i<ear_num_p_states) && (ear_cpufreq_pstates[i]!=f)) i++;
	if (i<ear_num_p_states) return 1;
	else return 0;
}

unsigned long ear_cpufreq_set(unsigned int cpuid,unsigned long newfreq)
{
	unsigned long ret;
	if (cpuid>ear_num_cpus) return 0;
	ret=ear_cpufreq[cpuid];
	if (ear_is_valid_frequency(newfreq)){
		ear_cpufreq[cpuid]=newfreq;
		// cpufreq_set_frequency(cpuid,newfreq);
		ret=eards_change_freq(newfreq);
		return ret;
	}else return 0;

}

unsigned long ear_cpufreq_set_node(unsigned long newfreq)
{
	ear_verbose(4,"EAR: ear_cpufreq_set_node newfreq=%u\n", newfreq);
	ear_frequency = eards_change_freq(newfreq);
	return ear_frequency;
}

unsigned long ear_get_nominal_frequency()
{
	return ear_nominal_freq;
}

unsigned int ear_get_num_p_states()
{
	return ear_num_p_states;
}

unsigned long *ear_get_pstate_list()
{
	return ear_cpufreq_pstates;
}

unsigned long ear_get_freq(unsigned int i)
{
	return ear_cpufreq_pstates[i];
}

unsigned long ear_my_frequency()
{
	return ear_frequency;
}
