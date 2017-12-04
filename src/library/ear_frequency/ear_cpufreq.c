/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <cpufreq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <ear_verbose.h>
#include <ear_daemon_client.h>

#define _GNU_SOURCE
#define __USE_GNU



unsigned int ear_num_cpus;
unsigned long *ear_cpufreq;
unsigned int ear_num_p_states=0;
unsigned long *ear_cpufreq_pstates;
unsigned long ear_nominal_freq;
const PAPI_hw_info_t *ear_cpufreq_hwinfo;
extern int ear_papi_init;
extern unsigned long ear_frequency;
struct cpufreq_policy *current_policy;
struct cpufreq_policy prev_policy;
extern int ear_my_rank;

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
	ear_verbose(2,"EAR:  ear_cpufreq_end: Restoring previous governor %s\n",prev_policy.governor);
	for (i=0;i<ear_num_cpus;i++){
		cpufreq_set_policy(i,&prev_policy);
	}
}

void ear_cpufreq_init()
{
	int status,i,retval;
	struct cpufreq_available_frequencies *list_f,*first;
	char *ear_tmp;
	char nodename[128];
	char ear_commreq[128],ear_commack[128];
	ear_verbose(4,"EAR: ear_cpufreq_init\n");
    //Init the PAPI library
    if (!ear_papi_init){
		if (PAPI_is_initialized()==PAPI_NOT_INITED){
        	retval=PAPI_library_init(PAPI_VER_CURRENT );
        	if ( (retval != PAPI_VER_CURRENT ) && (retval>0)){
        		ear_verbose(0,"EAR: Error intializing the PAPI library init %d current %d.Exiting\n",retval,PAPI_VER_CURRENT);
        	    exit(1);
        	}
		}
        ear_papi_init=1;
	}                                    
	ear_cpufreq_hwinfo= PAPI_get_hardware_info();
    if (ear_cpufreq_hwinfo==NULL){
        ear_verbose(0,"EAR: ERROR PAPI_get_hardware_info returns NULL.Exiting\n");
		exit(1);
     }else{
		// We should detect cpus
		ear_num_cpus=ear_cpufreq_hwinfo->sockets*ear_cpufreq_hwinfo->cores*ear_cpufreq_hwinfo->threads;
		ear_verbose(1,"EAR: %u cpus detected (sockets %u cores %u threads %u)\n",ear_cpufreq_hwinfo->ncpu,ear_cpufreq_hwinfo->sockets,ear_cpufreq_hwinfo->cores,ear_cpufreq_hwinfo->threads);
		ear_cpufreq=(unsigned long *)malloc(sizeof(unsigned long)*ear_num_cpus); 
		if (ear_cpufreq==NULL){
			ear_verbose(0,"EAR: malloc returns null for ear_cpulist.Exiting\n");
			exit(1);
		}
		// We check all the cpus are online, we should detect cores but
		// we start with this approach
		for (i=0;i<ear_num_cpus;i++){
        	status=cpufreq_cpu_exists(i);
			ear_cpufreq[i]=0;
            if (status==0){
            	ear_cpufreq[i]=cpufreq_get(i);
            	ear_verbose(4,"EAR: Curent cpu frequency for cpu %u is %u \n",i,ear_cpufreq[i]);
            }
        }
		ear_nominal_freq=ear_cpufreq[1];
		ear_verbose(2,"EAR: Nominal frequency detected %u\n",ear_nominal_freq);
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
	}
	current_policy=cpufreq_get_policy(0);
	prev_policy.min=current_policy->min;
	prev_policy.max=current_policy->max;
	prev_policy.governor=(char *)malloc(strlen(current_policy->governor)+1);
	strcpy(prev_policy.governor,current_policy->governor);
	ear_verbose(2,"EAR: Saving current governor %s\n",current_policy->governor);
	cpufreq_put_policy(current_policy);
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
		ret=ear_daemon_client_change_freq(newfreq);
		return ret;
	}else return 0;

}

unsigned long ear_cpufreq_set_node(unsigned long newfreq)
{
	ear_verbose(4,"EAR: ear_cpufreq_set_node newfreq=%u\n",newfreq);
	ear_frequency=ear_daemon_client_change_freq(newfreq);
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
