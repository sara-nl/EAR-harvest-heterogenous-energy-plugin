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

#include <metrics/custom/hardware_info.h>
#include <metrics/custom/bandwidth/uncores_pci.h>
#include <common/output/verbose.h>
#include <common/math_operations.h>

#define FUNCVERB(function)                               \
    debug( "EAR_DAEMON(UNCORES) " function "\n");

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

// Depending on the architecture delivered by cpu_model variable,
// the pmons structure would point to its proper reading functions.
int init_uncores(int cpu_model)
{
    FUNCVERB("init_uncores");

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
            return -1;
    }
}

int count_uncores()
{
    FUNCVERB("count_uncores");
    return pmons.count();
}

int check_uncores()
{
    return pmons.check();
}

int reset_uncores()
{
    FUNCVERB("reset_uncores");
    return pmons.reset();
}

int start_uncores()
{
    FUNCVERB("start_uncores");
    return pmons.start();
}

int stop_uncores(unsigned long long *values)
{
    FUNCVERB("stop_uncores");
    return pmons.stop(values);
}

int read_uncores(unsigned long long *values)
{
    FUNCVERB("read_uncore");
    return pmons.read(values);
}

int dispose_uncores()
{
    FUNCVERB("dispose_uncores");
    return pmons.dispose();
}


