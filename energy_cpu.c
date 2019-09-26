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


#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/states.h>

#include "msr.h"
#include "energy_cpu.h"
#include "hardware_info.h"
#include "mymath.h"


#define MSR_INTEL_RAPL_POWER_UNIT		0x606

/* PKG RAPL Domain */
#define MSR_PKG_RAPL_POWER_LIMIT		0x610 
#define MSR_INTEL_PKG_ENERGY_STATUS		0x611
#define MSR_PKG_PERF_STATUS				0x613
#define MSR_PKG_POWER_INFO				0x614 

/* DRAM RAPL Domain */
#define MSR_DRAM_POWER_LIMIT			0x618
#define MSR_DRAM_ENERGY_STATUS			0x619
#define MSR_DRAM_PERF_STATUS			0x61B
#define MSR_DRAM_POWER_INFO				0x61C

/* Thermal Domain */
#define IA32_THERM_STATUS               0x19C
#define IA32_PKG_THERM_STATUS           0x1B1
#define MSR_TEMPERATURE_TARGET          0x1A2

#define MAX_PACKAGES	16


int num_cpus = -1;
int num_cores = -1;
int msr_initialised = 0;

static int total_cores=0,total_packages=0;
static int package_map[MAX_PACKAGES];

double power_units, cpu_energy_units, time_units, dram_energy_units;

static int detect_packages(void) {

	char filename[BUFSIZ];
	FILE *fff;
	int package;
	int i;
    topology_t topo;
    hardware_gettopology(&topo);
    num_cpus = topo.sockets;
    num_cores = topo.cores*topo.sockets;
    
	if (num_cpus < 1 || num_cores < 1) {
        	return EAR_ERROR;
	}
	
	for(i=0;i<MAX_PACKAGES;i++) {
		package_map[i]=-1;
	}

	for(i=0;i<num_cores;i++)
	{
		sprintf(filename,"/sys/devices/system/cpu/cpu%d/topology/physical_package_id",i);
		fff=fopen(filename,"r");
		if (fff==NULL) break;
		fscanf(fff,"%d",&package);
		fclose(fff);

		if (package_map[package]==-1) {
			total_packages++;
			package_map[package]=i;
		}
	}
	total_cores=i;

	return 0;
}

int init_rapl_msr(int *fd_map)
{
    if (msr_initialised) return EAR_SUCCESS;
    if (detect_packages() == EAR_ERROR)
    {
        return EAR_ERROR;
    }

	unsigned long long result;
	int fd, j;
	for(j=0;j<NUM_SOCKETS;j++) {
       	int ret;
	    fd = -1;
		if ((ret = msr_open(package_map[j], &fd)) != EAR_SUCCESS)
       	{
		    return EAR_ERROR;
        }
		fd_map[j] = fd;
		if (msr_read(&fd, &result, sizeof result, MSR_INTEL_RAPL_POWER_UNIT))
			return EAR_ERROR;

		power_units=pow(0.5,(double)(result&0xf));
		cpu_energy_units=pow(0.5,(double)((result>>8)&0x1f));
		time_units=pow(0.5,(double)((result>>16)&0xf));
		dram_energy_units=pow(0.5,(double)16);

	}
    msr_initialised = 1;
	return EAR_SUCCESS;
}


int read_rapl_msr(int *fd_map,unsigned long long *_values)
{
	unsigned long long result;
	int j;

	for(j=0;j<NUM_SOCKETS;j++) {
		/* PKG reading */	    
	    if (msr_read(&fd_map[j], &result, sizeof result, MSR_INTEL_PKG_ENERGY_STATUS))
			return EAR_ERROR;
		result &= 0xffffffff;
		_values[j+2] = (unsigned long long)result*(cpu_energy_units*1000000000);

		/* DRAM reading */
	    if (msr_read(&fd_map[j], &result, sizeof result, MSR_DRAM_ENERGY_STATUS))
			return EAR_ERROR;
		result &= 0xffffffff;

		_values[j] = (unsigned long long)result*(dram_energy_units*1000000000);
		
    }
	return EAR_SUCCESS;
}

void dispose_rapl_msr(int *fd_map)
{
	int j;
	for (j = 0; j < total_packages; j++)
		msr_close(&fd_map[j]);
    msr_initialised = 0;
}

