/**************************************************************
 * * Energy Aware Runtime (EAR)
 * * This program is part of the Energy Aware Runtime (EAR).
 * *
 * * EAR provides a dynamic, transparent and ligth-weigth solution for
 * * Energy management.
 * *
 * *     It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017  
 * * BSC Contact   mailto:ear-support@bsc.es
 * * Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * * EAR is free software; you can redistribute it and/or
 * * modify it under the terms of the GNU Lesser General Public
 * * License as published by the Free Software Foundation; either
 * * version 2.1 of the License, or (at your option) any later version.
 * * 
 * * EAR is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * * Lesser General Public License for more details.
 * * 
 * * You should have received a copy of the GNU Lesser General Public
 * * License along with EAR; if not, write to the Free Software
 * * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * * The GNU LEsser General Public License is contained in the file COPYING  
 * */

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
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/system/execute.h>
#include <metrics/energy/energy_cpu.h>
#include <common/hardware/frequency.h>
#include <common/hardware/hardware_info.h>
#include <daemon/powercap_status_conf.h>
#include <daemon/powercap_status.h>


#define POWERCAP_MON 0
#define RAPL_VS_NODE_POWER 1
#define RAPL_VS_NODE_POWER_limit 0.85
#define DEBUG_PERIOD 15

pthread_t dvfs_pc_th;
static uint current_dvfs_pc=0,set_dvfs_pc=0;
static uint dvfs_pc_enabled=0;
static uint c_status=PC_STATUS_IDLE;
static uint c_mode=PC_MODE_LIMIT;
static ulong c_req_f;

#define USE_SUBSCRIPTIONS 1
#if USE_SUBSCRIPTIONS
#include <common/monitor.h>
/* This  subscription will take care automatically of the power monitoring */
static suscription_t *sus;
static uint dvfs_pc_secs=0,num_packs;
static  unsigned long long *values_rapl_init,*values_rapl_end,*values_diff;
static int *fd_rapl;

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
	return EAR_SUCCESS;
}
/************************ This function is called by the monitor in iterative part ************************/
state_t dvfs_pc_thread_main(void *p)
{
  	float power_rapl;
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
    power_rapl=(float)acum_energy/(1*RAPL_MSR_UNITS);
    /*debug("%sTotal power in dvfs_pc %f Watts limit %u DRAM+PCK low-limit %f up-limit %f%s",COL_BLU,power_rapl,current_dvfs_pc,(float)current_dvfs_pc*RAPL_VS_NODE_POWER,current_dvfs_pc*RAPL_VS_NODE_POWER_limit,COL_CLR);*/
    if (c_status==PC_STATUS_RUN){
      if (!dvfs_pc_secs){
        debug("DVFS:%sTotal power in dvfs_pc %f Watts limit %u DRAM+PCK low-limit %f up-limit %f%s",COL_BLU,power_rapl,current_dvfs_pc,(float)current_dvfs_pc*RAPL_VS_NODE_POWER,current_dvfs_pc*RAPL_VS_NODE_POWER_limit,COL_CLR);
      }
      #if 0
      debug("DRAM0 %f DRAM1 %f PCK0 %f PCK1 %f",((float)values_diff[0]/(1*RAPL_MSR_UNITS)),((float)values_diff[1]/(1*RAPL_MSR_UNITS)),
      ((float)values_diff[2]/(1*RAPL_MSR_UNITS)),((float)values_diff[3]/(1*RAPL_MSR_UNITS)));
      #endif
      /* Aplicar limites */
      if ((current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN)){
      if (power_rapl>(current_dvfs_pc*RAPL_VS_NODE_POWER)){ /* We are above the PC */
        c_freq=frequency_get_cpu_freq(0);
        c_pstate=frequency_freq_to_pstate(c_freq);
        c_pstate=c_pstate+1;
        c_freq=frequency_pstate_to_freq(c_pstate);
        debug("DVFS:%sReducing freq to %lu%s",COL_RED,c_freq,COL_CLR);
        frequency_set_all_cpus(c_freq);
      }else{ /* We are below the PC */
        t_pstate=frequency_freq_to_pstate(c_req_f);
        c_freq=frequency_get_cpu_freq(0);
        c_pstate=frequency_freq_to_pstate(c_freq);
        if (c_pstate>t_pstate){
          extra=compute_extra_power(power_rapl,1,t_pstate);
          if (((power_rapl+extra)<current_dvfs_pc) && (c_mode==PC_MODE_TARGET)){
            c_pstate=c_pstate-1;
            c_freq=frequency_pstate_to_freq(c_pstate);
            debug("DVFS:%sIncreasing freq to %lu (t_pstate %u c_pstate %u)%s",COL_RED,c_freq,t_pstate,c_pstate,COL_CLR);
            frequency_set_all_cpus(c_freq);
          }else{
            if (!dvfs_pc_secs) debug("DVFS:Not increasing becase not enough power");
          }
        }else{
          if (!dvfs_pc_secs) debug("DVFS: Not increasing because c_pstate %u == t_pstate %u",c_pstate,t_pstate);
        }
      }

    } /* (current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN) */
    } /* c_status==PC_STATUS_RUN */
    /* Copiar init=end */
    memcpy(values_rapl_init,  values_rapl_end,num_packs * RAPL_POWER_EVS* sizeof(unsigned long long));
		return EAR_SUCCESS;
}
#else

void dvfs_pc_thread(void *d)
{
	uint num_packs,extra;
	state_t s;
	uint secs=0;
	uint node_size;
	unsigned long long *values_rapl_init,*values_rapl_end,*values_diff,acum_energy;
	float power_rapl;
	char rapl_energy_str[512];
	int *fd_rapl;
	uint c_pstate,t_pstate;
	ulong c_freq;
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
	while(1)
	{
		sleep(1);
		secs=(secs+1)%DEBUG_PERIOD;
		read_rapl_msr(fd_rapl,values_rapl_end);	
			/* Calcular power */
				diff_rapl_msr_energy(values_diff,values_rapl_end,values_rapl_init);
			rapl_msr_energy_to_str(rapl_energy_str,values_diff);
			/*debug(rapl_energy_str); */
			acum_energy=acum_rapl_energy(values_diff);
			power_rapl=(float)acum_energy/(1*RAPL_MSR_UNITS);
			/*debug("%sTotal power in dvfs_pc %f Watts limit %u DRAM+PCK low-limit %f up-limit %f%s",COL_BLU,power_rapl,current_dvfs_pc,(float)current_dvfs_pc*RAPL_VS_NODE_POWER,current_dvfs_pc*RAPL_VS_NODE_POWER_limit,COL_CLR);*/
		if (c_status==PC_STATUS_RUN){
			if (!secs){ 
				debug("DVFS:%sTotal power in dvfs_pc %f Watts limit %u DRAM+PCK low-limit %f up-limit %f%s",COL_BLU,power_rapl,current_dvfs_pc,(float)current_dvfs_pc*RAPL_VS_NODE_POWER,current_dvfs_pc*RAPL_VS_NODE_POWER_limit,COL_CLR);
			}
			#if 0
			debug("DRAM0 %f DRAM1 %f PCK0 %f PCK1 %f",((float)values_diff[0]/(1*RAPL_MSR_UNITS)),((float)values_diff[1]/(1*RAPL_MSR_UNITS)),
			((float)values_diff[2]/(1*RAPL_MSR_UNITS)),((float)values_diff[3]/(1*RAPL_MSR_UNITS)));
			#endif
			/* Aplicar limites */
			if ((current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN)){
			if (power_rapl>(current_dvfs_pc*RAPL_VS_NODE_POWER)){ /* We are above the PC */
				c_freq=frequency_get_cpu_freq(0);
				c_pstate=frequency_freq_to_pstate(c_freq);
				c_pstate=c_pstate+1;
				c_freq=frequency_pstate_to_freq(c_pstate);
				debug("DVFS:%sReducing freq to %lu%s",COL_RED,c_freq,COL_CLR);
				frequency_set_all_cpus(c_freq);
			}else{ /* We are below the PC */
				t_pstate=frequency_freq_to_pstate(c_req_f);
				c_freq=frequency_get_cpu_freq(0);
				c_pstate=frequency_freq_to_pstate(c_freq);
				if (c_pstate>t_pstate){
					extra=compute_extra_power(power_rapl,1,t_pstate);
					if (((power_rapl+extra)<current_dvfs_pc) && (c_mode==PC_MODE_TARGET)){
						c_pstate=c_pstate-1;
						c_freq=frequency_pstate_to_freq(c_pstate);
						debug("DVFS:%sIncreasing freq to %lu (t_pstate %u c_pstate %u)%s",COL_RED,c_freq,t_pstate,c_pstate,COL_CLR);
						frequency_set_all_cpus(c_freq);
					}else{
						if (!secs) debug("DVFS:Not increasing becase not enough power");
					}
				}else{
					if (!secs) debug("DVFS: Not increasing because c_pstate %u == t_pstate %u",c_pstate,t_pstate);
				}
			}

		} /* (current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN) */
		} /* c_status==PC_STATUS_RUN */
		/* Copiar init=end */	
		memcpy(values_rapl_init,  values_rapl_end,num_packs * RAPL_POWER_EVS* sizeof(unsigned long long));
	}
	
}
#endif


state_t disable()
{
	return EAR_SUCCESS;
	/* Stop thread */
	pthread_kill(dvfs_pc_th,SIGSTOP);
}

state_t enable()
{
	int ret;
	#if USE_SUBSCRIPTIONS
	sus=suscription();
	if (sus == NULL){
		debug("NULL subscription in DVFS powercap");
		return EAR_ERROR;
	}
	sus->call_main = dvfs_pc_thread_main;
	sus->call_init = dvfs_pc_thread_init;
	sus->time_relax = 1000;
	sus->time_burst = 1000;
	if (monitor_register(sus) != EAR_SUCCESS){	
		error("Error registering DVFS_pc subscription in the monitor");
		return EAR_ERROR;
	}
	#else
	/* Create thread */
	if ((ret=pthread_create(&dvfs_pc_th, NULL, (void*)dvfs_pc_thread, (void *)NULL))){
		error("error creating DVFS powercap  thread \n");
		return EAR_ERROR;
	}
	#endif
	/* Init data */
	return EAR_SUCCESS;
}

state_t set_powercap_value(uint pid,uint domain,uint limit)
{
	/* Set data */
	debug("DVFS:set_powercap_value %u",limit);
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



void set_app_req_freq(ulong f)
{
	debug("DVFS:Requested application freq set to %lu",f);
	c_req_f=f;	
}

void set_verb_channel(int fd)
{
  WARN_SET_FD(fd);
  VERB_SET_FD(fd);
  DEBUG_SET_FD(fd);
}

