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


#define POWERCAP_MON 0
#define RAPL_VS_NODE_POWER 0.85
#define RAPL_VS_NODE_POWER_limit 0.7
#define DEBUG_PERIOD 1

pthread_t dvfs_pc_th;
static uint current_dvfs_pc=0,set_dvfs_pc=0;
static uint dvfs_pc_enabled=0;
static uint c_status=PC_STATUS_IDLE;
static uint c_mode=PC_MODE_LIMIT;


void dvfs_pc_thread(void *d)
{
	uint num_packs;
	state_t s;
	uint secs=0;
	uint node_size;
	unsigned long long *values_rapl_init,*values_rapl_end,*values_diff,acum_energy;
	float power_rapl;
	char rapl_energy_str[512];
	int *fd_rapl;
	uint c_pstate;
	ulong c_freq;
	topology_t node_desc;
  num_packs=detect_packages(NULL);
  if (num_packs==0){ 
		error("Num packages cannot be detected in dvfs_pc thread initialization");
		pthread_exit(NULL);
	}
	debug("Num pcks detected in dvfs_pc thread %u",num_packs);
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
	debug("Initializing RAPL in dvfs_pc");
	if (init_rapl_msr(fd_rapl)<0){ 
		error("Error initializing rapl in dvfs_pc thread initialization");
		pthread_exit(NULL);
	}
	dvfs_pc_enabled=1;
	verbose(1,"Power measurement initialized in dvfs_pc thread initialization");
	/* RAPL is initialized */
	read_rapl_msr(fd_rapl,values_rapl_init);
  s = hardware_gettopology(&node_desc);
  node_size = node_desc.sockets * node_desc.cores * node_desc.threads;
	debug("Initializing frequency in dvfs_pc %u cpus",node_size);
	frequency_init(node_size);
	while(1)
	{
		sleep(1);
		secs=(secs+1)%DEBUG_PERIOD;
		read_rapl_msr(fd_rapl,values_rapl_end);	
			/* Calcular power */
				diff_rapl_msr_energy(values_diff,values_rapl_end,values_rapl_init);
			rapl_msr_energy_to_str(rapl_energy_str,values_diff);
			//debug(rapl_energy_str);
			acum_energy=acum_rapl_energy(values_diff);
			power_rapl=(float)acum_energy/(1*RAPL_MSR_UNITS);
			debug("%sTotal power in dvfs_pc %f Watts limit %u DRAM+PCK low-limit %f up-limit %f%s",COL_BLU,power_rapl,current_dvfs_pc,(float)current_dvfs_pc*RAPL_VS_NODE_POWER,current_dvfs_pc*RAPL_VS_NODE_POWER_limit,COL_CLR);
		if (c_status==PC_STATUS_RUN){
			if (!secs){ 
		//		debug("%sTotal power in dvfs_pc %f Watts limit %u DRAM+PCK low-limit %f up-limit %f%s",COL_BLU,power_rapl,current_dvfs_pc,(float)current_dvfs_pc*RAPL_VS_NODE_POWER,current_dvfs_pc*RAPL_VS_NODE_POWER_limit,COL_CLR);
			}
			#if 0
			debug("DRAM0 %f DRAM1 %f PCK0 %f PCK1 %f",((float)values_diff[0]/(1*RAPL_MSR_UNITS)),((float)values_diff[1]/(1*RAPL_MSR_UNITS)),
			((float)values_diff[2]/(1*RAPL_MSR_UNITS)),((float)values_diff[3]/(1*RAPL_MSR_UNITS)));
			#endif
			/* Aplicar limites */
			if ((current_dvfs_pc>0)  && (c_status==PC_STATUS_RUN)){
			if (power_rapl>(current_dvfs_pc*RAPL_VS_NODE_POWER)){
				c_freq=frequency_get_cpu_freq(0);
				c_pstate=frequency_freq_to_pstate(c_freq);
				c_pstate=c_pstate+1;
				c_freq=frequency_pstate_to_freq(c_pstate);
				debug("%sReducing freq to %lu%s",COL_RED,c_freq,COL_CLR);
				frequency_set_all_cpus(c_freq);
			}
			}
			if ((power_rapl<(current_dvfs_pc*RAPL_VS_NODE_POWER_limit) && (c_mode==PC_MODE_TARGET))){
				c_freq=frequency_get_cpu_freq(0);
				c_pstate=frequency_freq_to_pstate(c_freq);
				if (c_pstate>frequency_get_nominal_pstate()){
					c_pstate=c_pstate-1;
					c_freq=frequency_pstate_to_freq(c_pstate);
					debug("%sIncreasing freq to %lu%s",COL_RED,c_freq,COL_CLR);
					frequency_set_all_cpus(c_freq);
				}
		}
		}
		/* Copiar init=end */	
		memcpy(values_rapl_init,  values_rapl_end,num_packs * RAPL_POWER_EVS* sizeof(unsigned long long));
	}
	
}


state_t disable()
{
	return EAR_SUCCESS;
	/* Stop thread */
	pthread_kill(dvfs_pc_th,SIGSTOP);
}

state_t enable()
{
	int ret;
	/* Create thread */
	if ((ret=pthread_create(&dvfs_pc_th, NULL, (void*)dvfs_pc_thread, (void *)NULL))){
		error("error creating DVFS powercap  thread \n");
		return EAR_ERROR;
	}
	/* Init data */
	return EAR_SUCCESS;
}

state_t set_powercap_value(uint pid,uint domain,uint limit)
{
	/* Set data */
	debug("set_powercap_value %u",limit);
	current_dvfs_pc=limit;
	return EAR_SUCCESS;
}

state_t get_powercap_value(uint pid,uint *powercap)
{
	/* copy data */
	debug("get_powercap_value");
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
	debug("set_status %u",status);
	c_status=status;
}
uint get_powercap_strategy()
{
	debug("get_powercap_strategy");
	return PC_DVFS;
}

void set_pc_mode(uint mode)
{
	debug("set_pc_mode");
	c_mode=mode;
}


