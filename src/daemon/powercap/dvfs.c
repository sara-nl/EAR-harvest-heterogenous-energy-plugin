/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#define _GNU_SOURCE
#include <pthread.h>
#include <common/config.h>
#include <signal.h>
#include <common/colors.h>
#include <common/states.h>
//#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/system/execute.h>
#include <metrics/energy/cpu.h>
#include <management/cpufreq/connector.h>
#include <common/hardware/hardware_info.h>
#include <daemon/powercap/powercap_status_conf.h>
#include <daemon/powercap/powercap_status.h>
#include <common/system/monitor.h>


#define RAPL_VS_NODE_POWER 1
#define RAPL_VS_NODE_POWER_limit 0.85
#define DEBUG_PERIOD 15

#define DVFS_PERIOD 0.1

static uint current_dvfs_pc=0,set_dvfs_pc=0;
static uint dvfs_pc_enabled=0;
static uint c_status=PC_STATUS_IDLE;
static uint c_mode=PC_MODE_LIMIT;
static ulong c_req_f;
static ulong num_pstates;

/* This  subscription will take care automatically of the power monitoring */
static uint dvfs_pc_secs=0,num_packs;
static  unsigned long long *values_rapl_init,*values_rapl_end,*values_diff;
static int *fd_rapl;
static float power_rapl,my_limit;

/************************ This function is called by the monitor before the iterative part ************************/
state_t dvfs_pc_thread_init(void *p)
{
  state_t s;
  uint node_size;
  topology_t node_desc;

  s = topology_init(&node_desc);
  num_packs=node_desc.socket_count;
  if (num_packs==0){
    error("Num packages cannot be detected in dvfs_pc thread initialization");
    pthread_exit(NULL);
  }
  debug("DVFS:Num pcks detected in dvfs_pc thread %u",num_packs);
  values_rapl_init=(unsigned long long*)calloc(num_packs*RAPL_POWER_EVS,sizeof(unsigned long long));
  values_rapl_end=(unsigned long long*)calloc(num_packs*RAPL_POWER_EVS,sizeof(unsigned long long));
  values_diff=(unsigned long long*)calloc(num_packs*RAPL_POWER_EVS,sizeof(unsigned long long));
  if ((values_rapl_init==NULL) || (values_rapl_end==NULL) || (values_diff==NULL))
  {
    error("values_rapl returns NULL in dvfs_pc thread initialization");
    pthread_exit(NULL);
  }
  fd_rapl=(int*)calloc(sizeof(int),num_packs);
  if (fd_rapl==NULL){
    error("fd_rapl cannot be allocated in dvfs_pc thread initialization");
    pthread_exit(NULL);
  }
  debug("DVFS:Initializing RAPL in dvfs_pc");
  if (init_rapl_msr(fd_rapl)<0){
    error("Error initializing rapl in dvfs_pc thread initialization");
    pthread_exit(NULL);
  }
  dvfs_pc_enabled=1;
  verbose(1,"Power measurement initialized in dvfs_pc thread initialization");
  /* RAPL is initialized */
  read_rapl_msr(fd_rapl,values_rapl_init);
  node_size = node_desc.cpu_count;
  debug("DVFS:Initializing frequency in dvfs_pc %u cpus",node_size);
  frequency_init(node_size);
	num_pstates = frequency_get_num_pstates();
	return EAR_SUCCESS;
}
/************************ This function is called by the monitor in iterative part ************************/
state_t dvfs_pc_thread_main(void *p)
{
		char rapl_energy_str[512];
		unsigned long long acum_energy;
  	uint c_pstate,t_pstate;
  	ulong c_freq;
		uint extra;

    dvfs_pc_secs=(dvfs_pc_secs+1)%DEBUG_PERIOD;
    read_rapl_msr(fd_rapl,values_rapl_end);
    /* Calcular power */
    diff_rapl_msr_energy(values_diff,values_rapl_end,values_rapl_init);
    rapl_msr_energy_to_str(rapl_energy_str,values_diff);
    /*debug(rapl_energy_str); */
    acum_energy=acum_rapl_energy(values_diff);
    power_rapl=(float)acum_energy/(DVFS_PERIOD*RAPL_MSR_UNITS);
		my_limit=(float)current_dvfs_pc;
		// debug("DVFS monitoring exectuted power_computed=%f limit %u",power_rapl,current_dvfs_pc);
    /*debug("%sTotal power in dvfs_pc %f Watts limit %u DRAM+PCK low-limit %f up-limit %f%s",COL_BLU,power_rapl,current_dvfs_pc,(float)current_dvfs_pc*RAPL_VS_NODE_POWER,current_dvfs_pc*RAPL_VS_NODE_POWER_limit,COL_CLR);*/
    if (c_status==PC_STATUS_RUN){
			#if 0
      if (!dvfs_pc_secs){
        debug("DVFS:%sTotal power in dvfs_pc %f Watts limit %u DRAM+PCK low-limit %f up-limit %f%s",COL_BLU,power_rapl,current_dvfs_pc,my_limit,my_limit,COL_CLR);
      }
			#endif
      /* Aplicar limites */
      if ((current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN)){
      if (power_rapl > my_limit){ /* We are above the PC */
        c_freq=frequency_get_cpu_freq(0);
        c_pstate=frequency_freq_to_pstate(c_freq);
        c_pstate=ear_min(c_pstate+1,num_pstates-1);
        c_freq=frequency_pstate_to_freq(c_pstate);
        debug("DVFS:%sReducing freq to %lu (pstate %u): power %f limit %f%s",COL_RED,c_freq,c_pstate,power_rapl,my_limit,COL_CLR);
        frequency_set_all_cpus(c_freq);
      }else{ /* We are below the PC */
        t_pstate=frequency_freq_to_pstate(c_req_f);
        c_freq=frequency_get_cpu_freq(0);
        c_pstate=frequency_freq_to_pstate(c_freq);
        if (c_pstate>t_pstate){
          extra=compute_extra_power(power_rapl,1,t_pstate);
          if (((power_rapl+extra)<my_limit) && (c_mode==PC_MODE_TARGET)){
            c_pstate=c_pstate-1;
            c_freq=frequency_pstate_to_freq(c_pstate);
            debug("DVFS:%sIncreasing freq to %lu (t_pstate %u c_pstate %u) estimated %f = %f + %u, limit %f%s",COL_RED,c_freq,t_pstate,c_pstate,power_rapl+extra,power_rapl,extra,my_limit,COL_CLR);
            frequency_set_all_cpus(c_freq);
          }else{
            if (!dvfs_pc_secs){ 
							debug("DVFS:Not increasing becase not enough power");
						}
          }
        }else if (c_pstate < t_pstate){
					debug("c_pstate %u < t_pstate %u. Power %f limit %f",c_pstate,t_pstate,power_rapl,my_limit);
					c_freq=frequency_pstate_to_freq(t_pstate);
					debug("Setting freq to %.2f",(float)c_freq/1000000.0);
					frequency_set_all_cpus(c_freq);
				}else{
          if (!dvfs_pc_secs) debug("DVFS: Not increasing because c_pstate %u == t_pstate %u. Power %f limit %f",c_pstate,t_pstate,power_rapl,my_limit);
        }
      }

    } /* (current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN) */
    } /* c_status==PC_STATUS_RUN */
    /* Copiar init=end */
    memcpy(values_rapl_init,  values_rapl_end,num_packs * RAPL_POWER_EVS* sizeof(unsigned long long));
		return EAR_SUCCESS;
}


state_t disable()
{
	return EAR_SUCCESS;
}

state_t enable(suscription_t *sus)
{
	int ret;
	debug("DVFS");
	if (sus == NULL){
		debug("NULL subscription in DVFS powercap");
		return EAR_ERROR;
	}
	sus->call_main = dvfs_pc_thread_main;
	sus->call_init = dvfs_pc_thread_init;
	sus->time_relax = DVFS_PERIOD*1000;
	sus->time_burst = DVFS_PERIOD*1000;
	sus->suscribe(sus);
	debug("DVFS Subscription done");
	/* Init data */
	return EAR_SUCCESS;
}

state_t set_powercap_value(uint pid,uint domain,uint limit,uint *cpu_util)
{
	/* Set data */
	debug("%sDVFS:set_powercap_value %u%s",COL_BLU,limit,COL_CLR);
	current_dvfs_pc=limit;
	return EAR_SUCCESS;
}

state_t get_powercap_value(uint pid,uint *powercap)
{
	/* copy data */
	debug("DVFS:get_powercap_value");
	*powercap=current_dvfs_pc;
	return EAR_SUCCESS;
}

uint is_powercap_policy_enabled(uint pid)
{
	return dvfs_pc_enabled;
}

void print_powercap_value(int fd)
{
	dprintf(fd,"dvfs_powercap_value %u\n",current_dvfs_pc);
}
void powercap_to_str(char *b)
{
	sprintf(b,"%u",current_dvfs_pc);
}

void set_status(uint status)
{
	debug("DVFS:set_status %u",status);
	c_status=status;
}
uint get_powercap_strategy()
{
	debug("DVFS:get_powercap_strategy");
	return PC_DVFS;
}

void set_pc_mode(uint mode)
{
	debug("DVFS:set_pc_mode");
	c_mode=mode;
}



void set_app_req_freq(ulong *f)
{
	debug("DVFS:Requested application freq set to %lu",*f);
	c_req_f=*f;	
}

void set_verb_channel(int fd)
{
  WARN_SET_FD(fd);
  VERB_SET_FD(fd);
  DEBUG_SET_FD(fd);
}

uint get_powercap_status(uint *in_target,uint *tbr)
{
	ulong c_freq;
	uint ctbr;
	/* If we don't know the req_f we cannot release power */
	if (c_req_f == 0) return 0;
	c_freq=frequency_get_cpu_freq(0);
	if (c_freq != c_req_f) return 0;
	ctbr = (my_limit -  power_rapl) * 0.75;
	debug("We can reuse %u W of power from the CPU since target %lu and current %lu",ctbr,c_req_f,c_freq);
	*in_target=1;
	*tbr = ctbr; 
	return 1;
}
