/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>
#include <cpufreq.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <ear_turbo.h>
#include <ear_verbose.h>


unsigned int ear_num_cpus;
unsigned long *ear_cpufreq;
unsigned int ear_num_p_states=0;
unsigned long *ear_cpufreq_pstates;
unsigned long ear_nominal_freq;
const PAPI_hw_info_t *ear_cpufreq_hwinfo;
struct cpufreq_policy *current_policy;
struct cpufreq_policy prev_policy;

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
	ear_debug(1,"EAR_DAEMON:: Restoring previous governor %s\n",prev_policy.governor);
	for (i=0;i<ear_num_cpus;i++){
		cpufreq_set_policy(i,&prev_policy);
	}
#ifdef EAR_BOOST
	aperf_dispose();
#endif
}

// TURBO FUNCTIONALLITY
// Start computing the avg freq for the whole application
void ear_begin_app_compute_turbo_freq()
{
	ear_debug(2,"EAR_DAEMON:: ear_begin_app_compute_turbo_freq\n");
#ifdef EAR_BOOST
	int i;
	for (i=0;i<ear_num_cpus;i++) aperf_start_computing_app_avg_freq(i);
#endif
}

// Ends computing the avg freq for the whole application
unsigned long ear_end_app_compute_turbo_freq()
{
#ifdef EAR_BOOST
        unsigned long freq=0,new_freq;
        int i;
        ear_debug(2,"EAR_DAEMON:: EAR_end_compute_turbo_freq\n");
        for (i=0;i<ear_num_cpus;i++){
                aperf_end_computing_app_avg_freq(i,&new_freq);
                freq+=new_freq;
        }
        return(freq/ear_num_cpus);
#endif
        return ear_nominal_freq;
}

void ear_begin_compute_turbo_freq()
{
	ear_debug(2,"EAR_DAEMON:: EAR_begin_compute_turbo_freq\n");
#ifdef EAR_BOOST
	int i;
	for (i=0;i<ear_num_cpus;i++) aperf_get_avg_frequency_init(i);
#endif
}
unsigned long ear_end_compute_turbo_freq()
{
#ifdef EAR_BOOST
	unsigned long freq=0,new_freq;
	int i;
	ear_debug(2,"EAR_DAEMON:: EAR_end_compute_turbo_freq\n");
	for (i=0;i<ear_num_cpus;i++){ 
		aperf_get_avg_frequency_end(i,&new_freq);
		freq+=new_freq;
	}
	return(freq/ear_num_cpus);
#endif
	return ear_nominal_freq;

}

void ear_set_turbo()
{
	int i;
	for (i=0;i<ear_num_cpus;i++) cpufreq_modify_policy_governor(i,"performance");
}

void ear_set_userspace()
{
	int i;
	for (i=0;i<ear_num_cpus;i++) cpufreq_modify_policy_governor(i,"userspace");
}

// END TURBO FUNCTIONALLITY

int ear_cpufreq_init()
{
	struct cpufreq_available_frequencies *list_f, *first;
	int status, i, retval;

	ear_debug(3, "eard:: Reading cpu frequencies information\n");
        
	//TODO: generalize
	if (PAPI_is_initialized()==PAPI_NOT_INITED)
	{
		retval = PAPI_library_init(PAPI_VER_CURRENT);

		if ((retval != PAPI_VER_CURRENT) && (retval>0))
		{
			ear_verbose(0,"eard: Papi can not be initialised \n");
			return -1;
		}
	}

	ear_cpufreq_hwinfo = PAPI_get_hardware_info();

	if (ear_cpufreq_hwinfo == NULL)
	{
		ear_verbose(0,"eard: PAPI_get_hardware_info \n");
		return -1;
	} else {
		// 1- We detect the number of cpus based on papi information
		ear_num_cpus = ear_cpufreq_hwinfo->sockets *
					   ear_cpufreq_hwinfo->cores *
					   ear_cpufreq_hwinfo->threads;
		
		ear_verbose(2, "eard: %u cpus detected (sockets %u cores %u threads %u)\n", ear_num_cpus,
			ear_cpufreq_hwinfo->sockets, ear_cpufreq_hwinfo->cores, ear_cpufreq_hwinfo->threads);
	}

	ear_cpufreq = (unsigned long *) malloc(sizeof(unsigned long) * ear_num_cpus);

	if (ear_cpufreq == NULL)
	{
		ear_verbose(0,"eard: malloc return NULL in ear_cpufreq_init\n");
		return -1;
	}
	
	// 2-We check all the cpus are online, we should detect cores but
	// we start with this approach
	for (i = 0; i < ear_num_cpus; i++)
	{
		status = cpufreq_cpu_exists(i);
		ear_cpufreq[i]=0;

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
		ear_verbose(0, "%s: no P_STATEs detected, make sure the CPUPower driver is working properly\n", __FILE__);
		return EAR_ERROR;
	}
	// else
	ear_verbose(2, "%s: %d P_STATEs detected\n", __FILE__, ear_num_p_states);

	// 4-Detecting the list of pstates available
	ear_cpufreq_pstates = (unsigned long *) malloc(sizeof(unsigned long) * ear_num_p_states);
			
	if (ear_cpufreq_pstates == NULL) {
		ear_verbose(0," eard: malloc return NULL in ear_cpufreq_init\n");
		return -1;
	}
	
	list_f = first;
	i=0;
        	
	while(list_f!=NULL)
	{
		ear_cpufreq_pstates[i]=list_f->frequency;	
		list_f=list_f->next;
		ear_debug(3,"eard::: P_state %d is %u\n",i,ear_cpufreq_pstates[i]);
		i++;
	}

		ear_nominal_freq=ear_cpufreq_pstates[1];
		ear_verbose(2,"eard: Nominal frequency set to %u\n",ear_nominal_freq);

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
		// Initializing code to estimate cpu frequency when turbo is activated
		aperf_init(ear_num_cpus);
		for (i=0;i<ear_num_cpus;i++) aperf_init_cpu(i,ear_nominal_freq);
	#endif
	
	return 0;
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
	ear_debug(4,"eard::: cpufreq_get for cpu %u\n",cpuid);
	if (cpuid>ear_num_cpus) return 0;
	f=cpufreq_get(cpuid);
	ear_debug(4,"eard::: getcpufreq cpu(%u) freq = %u\n",cpuid,f);
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
	//TODO: Why?
	int EAR_VERBOSE_LEVEL = 0;
	unsigned long ret;

	if (cpuid>ear_num_cpus) return 0;
	ret = ear_cpufreq[cpuid];

	if (ear_is_valid_frequency(newfreq))
	{
		ear_cpufreq[cpuid] = newfreq;
		cpufreq_set_frequency(cpuid, newfreq);
		ear_debug(1,"eard::: Setting cpu(%u) freq = %u\n", cpuid,newfreq);

		return ret;
	}
	return 0;
}

unsigned long ear_cpufreq_set_node(unsigned long newfreq)
{
	unsigned int i=0;

	if (ear_is_valid_frequency(newfreq))
	{
		ear_verbose(1,"eard: Setting node: freq = %u\n",newfreq);

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
