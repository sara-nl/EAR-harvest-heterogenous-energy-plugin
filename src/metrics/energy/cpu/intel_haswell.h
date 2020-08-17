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

#ifndef _RAPL_METRICS_H_
#define _RAPL_METRICS_H_

#include <metrics/common/omsr.h>

#define RAPL_POWER_EVS            2
#define RAPL_DRAM0          0
#define RAPL_DRAM1          1
#define RAPL_PACKAGE0       2
#define RAPL_PACKAGE1       3
#define RAPL_MSR_UNITS 			1000000000.0

#define RAPL_ENERGY_EV 2
#define RAPL_DRAM_EV 0
#define RAPL_PCK_EV 1

/** Opens the necessary fds to read the MSR registers. Returns 0 on success
* 	and -1 on error. fd_map is an already allocated vector with num_packages positions*/
int init_rapl_msr(int *fd_map);

/** */
void dispose_rapl_msr(int *fd_map);

/** Reads rapl counters and stores them in values array. Returns 0 on success 
*	and -1 on error. */
/* DRAM 0, DRAM 1,..DRAM N, PCK0,PCK1,...PCKN  */
int read_rapl_msr(int *fd_map,unsigned long long *_values);

void rapl_msr_energy_to_str(char *b,unsigned long long *values);
unsigned long long acum_rapl_energy(unsigned long long *values);
void diff_rapl_msr_energy(unsigned long long *diff,unsigned long long *end, unsigned long long *init);


#endif
