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
#define SHOW_DEBUGS 1
#include <pthread.h>
#include <common/config.h>
#include <signal.h>
#include <common/colors.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/system/execute.h>
#include <metrics/energy/cpu.h>
#include <management/cpufreq/frequency.h>
#include <common/hardware/hardware_info.h>
#include <daemon/powercap/powercap_status_conf.h>
#include <daemon/powercap/powercap_status.h>
#include <common/system/monitor.h>


#define RAPL_VS_NODE_POWER 1
#define RAPL_VS_NODE_POWER_limit 0.85
#define DEBUG_PERIOD 15

#define DVFS_PERIOD 0.1
#define DVFS_BURST_DURATION 1000
#define DVFS_RELAX_DURATION 1000

static uint current_dvfs_pc=0,set_dvfs_pc=0;
static uint dvfs_pc_enabled=0;
static uint c_status=PC_STATUS_IDLE;
static uint c_mode=PC_MODE_LIMIT;
static ulong *c_req_f;
static ulong num_pstates;
static suscription_t *sus_dvfs;

/* This  subscription will take care automatically of the power monitoring */
static uint dvfs_pc_secs=0,num_packs;
static  unsigned long long *values_rapl_init,*values_rapl_end,*values_diff;
static int *fd_rapl;
static float power_rapl,my_limit;
static ulong *c_freq;
static uint  *c_pstate,*t_pstate;
static uint node_size;
static uint dvfs_status = PC_STATUS_OK;
static uint dvfs_ask_def = 0;

/************************ These functions must be implemented in cpufreq *****/
void frequency_nfreq_to_npstate(ulong *f,uint *p,uint cpus)
{
	int i;
	for (i=0;i<cpus;i++) p[i] = frequency_freq_to_pstate(f[i]);
}
void frequency_npstate_to_nfreq(uint *p,ulong *f,uint cpus)
{
	int i;
	for (i=0;i<cpus;i++) f[i] = frequency_pstate_to_freq(p[i]);
}

void extend_from_cpu_to_list(int cpu, uint *list,uint num_cpus)
{
	ulong value = list[cpu];
	uint i;
	for (i=0;i<num_cpus;i++) list[i] = value;
}

/************************ This function is called by the monitor before the iterative part ************************/
state_t dvfs_pc_thread_init(void *p)
{
  state_t s;
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
	c_freq = calloc(node_size,sizeof(ulong));
	c_req_f = calloc(node_size,sizeof(ulong));
	c_pstate = calloc(node_size,sizeof(uint));
	t_pstate = calloc(node_size,sizeof(uint));
	return EAR_SUCCESS;
}
/************************ This function is called by the monitor in iterative part ************************/
state_t dvfs_pc_thread_main(void *p)
{
		char rapl_energy_str[512];
		unsigned long long acum_energy;
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
      /* Aplicar limites */
      if ((current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN)){
			frequency_get_cpufreq_list(node_size,c_freq);
      if (power_rapl > my_limit){ /* We are above the PC , reduce freq*/
				if (dvfs_status == PC_STATUS_RELEASE){
					dvfs_ask_def = 1;
				}
        frequency_nfreq_to_npstate(c_freq,c_pstate,node_size);
				/* If we are running at the lowest frequency there is nothing else to do */
				if (c_pstate[0] < (num_pstates-1)){ /*Pending verctor extension*/
        	c_pstate[0]=ear_min(c_pstate[0]+1,num_pstates-1); /* Pending verctor extension */
					extend_from_cpu_to_list(0,c_pstate,node_size);
        	frequency_npstate_to_nfreq(c_pstate,c_freq,node_size);
					frequency_set_with_list(node_size,c_freq);
				}
      }else{ /* We are below the PC */
        frequency_nfreq_to_npstate(c_req_f,t_pstate,node_size);
        frequency_nfreq_to_npstate(c_freq,c_pstate,node_size);
        if (c_pstate[0]>t_pstate[0]){ /* PENDING */
          extra=compute_extra_power(power_rapl,1,t_pstate[0]);
          if (((power_rapl+extra)<my_limit) && (c_mode==PC_MODE_TARGET)){ /* PENDING */
            c_pstate[0]=c_pstate[0]-1; /* PENDING */
						extend_from_cpu_to_list(0,c_pstate,node_size);
        		frequency_npstate_to_nfreq(c_pstate,c_freq,node_size);
						frequency_set_with_list(node_size,c_freq);
          }
        }else if (c_pstate[0] < t_pstate[0]){
					frequency_npstate_to_nfreq(t_pstate,c_freq,node_size);
					frequency_set_with_list(node_size,c_freq);
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
	sus->time_relax = DVFS_RELAX_DURATION*DVFS_PERIOD;
	sus->time_burst = DVFS_BURST_DURATION*DVFS_PERIOD;
	sus->suscribe(sus);
	sus_dvfs = sus;
	debug("DVFS Subscription done");
	/* Init data */
	return EAR_SUCCESS;
}

state_t set_powercap_value(uint pid,uint domain,uint limit,uint *cpu_util)
{
	/* Set data */
	debug("%sDVFS:set_powercap_value %u%s",COL_BLU,limit,COL_CLR);
	current_dvfs_pc=limit;
	dvfs_status = PC_STATUS_OK;
	dvfs_ask_def = 0;
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
	int i;
	debug("DVFS:Requested application freq set to %lu",*f);
	for (i=0;i<node_size;i++) c_req_f[i] = *f;	
}

void set_verb_channel(int fd)
{
  WARN_SET_FD(fd);
  VERB_SET_FD(fd);
  DEBUG_SET_FD(fd);
}


#define MIN_CPU_POWER_MARGIN 10

/* Returns 0 when 1) No info, 2) No limit 3) We cannot share power. */
/* Returns 1 when power can be shared with other modules */
/* Status can be: PC_STATUS_OK, PC_STATUS_GREEDY, PC_STATUS_RELEASE, PC_STATUS_ASK_DEF */
/* tbr is > 0 when PC_STATUS_RELEASE , 0 otherwise */
/* X_status is the plugin status */
/* X_ask_def means we must ask for our power */

uint get_powercap_status(uint *status,uint *tbr)
{
	uint ctbr;
	*status = PC_STATUS_OK;
	*tbr = 0;
	/* Return 0 means we cannot release power */
	if (current_dvfs_pc == PC_UNLIMITED) return 0;
	/* If we don't know the req_f we cannot release power */
	if (c_req_f == 0) return 0;
	frequency_get_cpufreq_list(node_size,c_freq);
	if (c_freq[0] < c_req_f[0]){
		if (dvfs_ask_def) *status = PC_STATUS_ASK_DEF;
		else *status = PC_STATUS_GREEDY;
		return 0; /* Pending for NJObs */
	}
	ctbr = (my_limit -  power_rapl) * 0.5;
	*status = PC_STATUS_RELEASE;
	*tbr = ctbr; 
	if (ctbr < MIN_CPU_POWER_MARGIN){
		*tbr = 0;
		*status = PC_STATUS_OK;
		return 0;
	}
	dvfs_status = *status;
	return 1;
}
