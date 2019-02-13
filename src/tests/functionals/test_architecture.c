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
#include <cpufreq.h>
#include <papi.h>
#include <metrics/custom/hardware_info.h>

#define _GNU_SOURCE
#define __USE_GNU

int cpu_model;
unsigned int ear_num_cpus;
unsigned long *ear_cpufreq;
unsigned int ear_num_p_states=0;
unsigned long *ear_cpufreq_pstates;
unsigned long ear_nominal_freq;
const PAPI_hw_info_t *ear_cpufreq_hwinfo;
struct cpufreq_policy *current_policy;
struct cpufreq_policy prev_policy;

void frequency_set_userspace_governor_all_cpus()
{
    int i;
    for (i=0;i<ear_num_cpus;i++) cpufreq_modify_policy_governor(i,"userspace");
}

unsigned int frequency_freq_to_pstate(unsigned long f)
{
	unsigned int i=0,found=0;
	while ((i<ear_num_p_states)&&(found==0)){
		if (ear_cpufreq_pstates[i]!=f) i++;
		else found=1;
	}
	return i;
}

void frequency_dispose()
{
	int i;
	verbose(0, "Restoring previous governor %s", prev_policy.governor);
	for (i=0;i<ear_num_cpus;i++){
		cpufreq_set_policy(i,&prev_policy);
	}
}

unsigned long ear_max_f()
{
    return ear_cpufreq_pstates[1];
}
unsigned long ear_min_f()
{
    return ear_cpufreq_pstates[ear_num_p_states-1];
}


unsigned long frequency_get_num_pstates(unsigned int cpuid)
{
    unsigned long f;
    if (cpuid>ear_num_cpus) return 0;
    f=cpufreq_get(cpuid);
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
        cpufreq_set_frequency(cpuid,newfreq);
        return ret;
    }else return 0;

}

unsigned long frequency_set_all_cpus(unsigned long newfreq)
{
    unsigned int i=0;
    if (ear_is_valid_frequency(newfreq)){
        verbose(0, "Setting node: freq = %u", newfreq);
        for (i=0;i<ear_num_cpus;i++){
            ear_cpufreq[i]=newfreq;
            // This is a privileged function
            cpufreq_set_frequency(i,newfreq);
        }

        return newfreq;
    }else return ear_cpufreq[0];
}

unsigned long frequency_get_nominal_freq()
{
    return ear_nominal_freq;
}

unsigned int frequency_get_num_pstates()
{
    return ear_num_p_states;
}

unsigned long *frequency_get_freq_rank_list()
{
    return ear_cpufreq_pstates;
}

unsigned long frequency_pstate_to_freq(unsigned int i)
{
    return ear_cpufreq_pstates[i];
}





int main(int argc,char *argv[])
{

	int status,i,retval;
	unsigned long f;
	struct cpufreq_available_frequencies *list_f,*first;
	//Init the PAPI library
	verbose(0, "Initializing PAPI to read machine configuration");
	if (PAPI_is_initialized()==PAPI_NOT_INITED){
		retval=PAPI_library_init(PAPI_VER_CURRENT );
		if ( (retval != PAPI_VER_CURRENT ) && (retval>0)){
			verbose(0, "Papi can not be initialised");
			return -1;
		}
	}
	ear_cpufreq_hwinfo= PAPI_get_hardware_info();
	if (ear_cpufreq_hwinfo==NULL){
		verbose(0, "PAPI_get_hardware_info returns NULL");
		return -1;
	}else{
		cpu_model=get_model();
		verbose(0, "CPU model %d",cpu_model);
		// 1- We detect the number of cpus based on papi information
		ear_num_cpus=ear_cpufreq_hwinfo->sockets*ear_cpufreq_hwinfo->cores*ear_cpufreq_hwinfo->threads;
		verbose(0, "%u cpus detected (sockets %u cores %u threads %u)",
			   ear_num_cpus, ear_cpufreq_hwinfo->sockets, ear_cpufreq_hwinfo->cores, ear_cpufreq_hwinfo->threads);
	}
	verbose(0, "Reading cpu frequencies information");
	ear_cpufreq=(unsigned long *)malloc(sizeof(unsigned long)*ear_num_cpus); 
	if (ear_cpufreq==NULL){
		verbose(0, "malloc return NULL in frequency_init");
		return -1;
	}
	// 2-We check all the cpus are online, we should detect cores but
	// we start with this approach
	for (i=0;i<ear_num_cpus;i++){
		// status=cpufreq_cpu_exists(i);
		status=0;
		ear_cpufreq[i]=0;
		if (status==0){
			ear_cpufreq[i]=cpufreq_get(i);
		}
	}
	// 3-We are assuming all the cpus supports the same set of frequencies
	// we check for cpu 0
	list_f=cpufreq_get_available_frequencies(0);
	first=list_f;
	while(list_f!=NULL){
		list_f=list_f->next;
		ear_num_p_states++;
	}
	verbose(0, "%d p_states available", ear_num_p_states);
	// 4-Detecting the list of pstates available
	ear_cpufreq_pstates=(unsigned long *)malloc(sizeof(unsigned long)*ear_num_p_states);
	if (ear_cpufreq_pstates==NULL){
		verbose(0, "malloc return NULL in frequency_init");
		return -1;
	}
	
	list_f=first;
	i=0;
	while(list_f!=NULL){
		ear_cpufreq_pstates[i]=list_f->frequency;	
		list_f=list_f->next;
		verbose(0, "P_state %d is %u",i,ear_cpufreq_pstates[i]);
		i++;
	}	
	ear_nominal_freq=ear_cpufreq_pstates[1];
	verbose(0, "Nominal frequency set to %u", ear_nominal_freq);
	// this functions releases the memory allocated in cpufreq_get_available_frequencies function
	cpufreq_put_available_frequencies(first);
	// we get (and latter put) the current policy=governor
	current_policy=cpufreq_get_policy(0);
	prev_policy.min=current_policy->min;
	prev_policy.max=current_policy->max;
	prev_policy.governor=(char *)malloc(strlen(current_policy->governor)+1);
	strcpy(prev_policy.governor,current_policy->governor);
	verbose(0, "Saving current governor %s", current_policy->governor);
	cpufreq_put_policy(current_policy);
	frequency_set_userspace_governor_all_cpus();
	// AFTER INIT
	verbose(0, "After init...checking");
	verbose(0, "nominal freq is %u", frequency_get_nominal_freq());
	verbose(0, "num_p_states %d", frequency_get_num_pstates());
	verbose(0, "ear_max_f %u", ear_max_f());
	verbose(0, "ear_min_f %u", ear_min_f());
	verbose(0, "changing node freq to %u", ear_max_f());
	frequency_set_all_cpus(ear_max_f());
	verbose(0, "frequency_get_num_pstates cpu %d =%u", ear_num_cpus-1, frequency_get_num_pstates(ear_num_cpus-1));
	verbose(0, "checking for valid frequency %u....%d(1=ok,0=not valid)", ear_max_f(), ear_is_valid_frequency(ear_max_f()));
	verbose(0, "checking for not valid frequency %u....%d(1=ok,0=not valid)", ear_max_f()+1000000, ear_is_valid_frequency(ear_max_f()+1000000));
	verbose(0, "changing freq for CPU 0 to min_f %u", ear_min_f());
	f=ear_cpufreq_set(0,ear_min_f());
	verbose(0, "after changing...frequency was %u and now it is %u", f, frequency_get_num_pstates(0));
	sleep(1);
	ear_cpufreq_set(0,f);
	verbose(0, "changing again....and checking %u", frequency_get_num_pstates(0));
	verbose(0, "p_state for nominal is %d,maxf %d and minf %d", frequency_freq_to_pstate(frequency_get_nominal_freq()),
		frequency_freq_to_pstate(ear_max_f()), frequency_freq_to_pstate(ear_min_f()));
	return 0;
}

