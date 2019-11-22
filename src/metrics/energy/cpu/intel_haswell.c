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

#include <common/config.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/hardware/hardware_info.h>
#include <common/math_operations.h>
#include <metrics/energy/energy_cpu.h>
#include <metrics/common/msr.h>



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





double power_units, cpu_energy_units, time_units, dram_energy_units;

int init_rapl_msr(int *fd_map)
{	
    int j;
    unsigned long long result;
	/* If it is not initialized, I do it, else, I get the ids */
    if (is_msr_initialized()==0){ 
	    init_msr(fd_map);
    }else get_msr_ids(fd_map);
	
    /* Ask for msr info */
    for(j=0;j<NUM_SOCKETS;j++) 
    {
        if (msr_read(&fd_map[j], &result, sizeof result, MSR_INTEL_RAPL_POWER_UNIT)) return EAR_ERROR;

        power_units=pow(0.5,(double)(result&0xf));
        cpu_energy_units=pow(0.5,(double)((result>>8)&0x1f));
        time_units=pow(0.5,(double)((result>>16)&0xf));
        dram_energy_units=pow(0.5,(double)16);
    }
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
	int j,tp;
	tp=get_total_packages();
	for (j = 0; j < tp; j++)
		msr_close(&fd_map[j]);
}

