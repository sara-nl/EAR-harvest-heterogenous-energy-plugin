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

/*
 * Usage:
 * 1) Call init passing the processor model of clusters
 * nodes to initialize the uncores scan and allocate file
 * descriptors memory.
 * 2) Call reset and start when you want to begin to count
 * the bandwith for a period of time.
 * 3) Call stop passing an array of unsigned long long
 * to also get the final uncore counters values.
 * 4) Repeat steps 2 and 3 every time you need to obtain
 * counters values.
 * 4) Call dispose to close the file descriptors and also
 * free it's allocated memory.
 *
 * When an error occurs, those calls returns -1.
 */

#include <stdio.h>
#include <string.h>
#include <common/config.h>
#include <common/output/verbose.h>
#include <common/math_operations.h>
#include <common/hardware/hardware_info.h>
#include <metrics/bandwidth/cpu/intel_haswell.h>


struct uncore_op
{
    int (*init) (int cpu_model);
    int (*count) ();
    int (*check) ();
    int (*reset) ();
    int (*start) ();
    int (*stop) (unsigned long long *values);
    int (*read) (unsigned long long *values);
    int (*dispose) ();
} pmons;

#define DEF_UNC_SIZE 1



// Depending on the architecture delivered by cpu_model variable,
// the pmons structure would point to its proper reading functions.
int init_uncores(int cpu_model)
{
    debug("init_uncores");

    switch (cpu_model) {
        case CPU_HASWELL_X:
        case CPU_BROADWELL_X:
        case CPU_SKYLAKE_X:
            pmons.init = pci_init_uncores;
            pmons.count = pci_count_uncores;
            pmons.check = pci_check_uncores;
            pmons.reset = pci_reset_uncores;
            pmons.start = pci_start_uncores;
            pmons.stop = pci_stop_uncores;
            pmons.read = pci_read_uncores;
            pmons.dispose = pci_dispose_uncores;
            return pmons.init(cpu_model);
        default:
            pmons.init = NULL;
            pmons.count = NULL;
            pmons.check = NULL;
            pmons.reset =NULL;
            pmons.start = NULL;
            pmons.stop = NULL;
            pmons.read = NULL;
            pmons.dispose = NULL;
            return DEF_UNC_SIZE;
    }
}

int count_uncores()
{
    debug("count_uncores");
		if (pmons.count!=NULL) return pmons.count();
		else return DEF_UNC_SIZE;
}

int check_uncores()
{
		if (pmons.check!=NULL) return pmons.check();
		else return EAR_ERROR;
}

int reset_uncores()
{
    debug("reset_uncores");
		if (pmons.reset!=NULL) return pmons.reset();
		else return EAR_ERROR;
}

int start_uncores()
{
    debug("start_uncores");
		if (pmons.start!=NULL) return pmons.start();
		else return EAR_ERROR;
}

int stop_uncores(unsigned long long *values)
{
    debug("stop_uncores");
		if (pmons.stop!=NULL) return pmons.stop(values);
		else {
			memset(values,0,DEF_UNC_SIZE*sizeof(unsigned long long));
			return EAR_ERROR;
		}
}

int read_uncores(unsigned long long *values)
{
    debug("read_uncore");
		if (pmons.read!=NULL) return pmons.read(values);
		else {
      memset(values,0,DEF_UNC_SIZE*sizeof(unsigned long long));
      return EAR_ERROR;
    }

}

int dispose_uncores()
{
    debug("dispose_uncores");
   	if (pmons.dispose!=NULL) return pmons.dispose();
		else return EAR_ERROR;
}


