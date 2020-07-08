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

#ifndef _HW_INFO_H_
#define _HW_INFO_H_

#include <common/sizes.h>
#include <common/states.h>
#include <common/system/file.h>
#include <common/types/generic.h>
#include <common/hardware/cpuid.h>
#include <common/hardware/topology.h>

/** Returns if the cpu is examinable by this library */
int is_cpu_examinable();

/** Returns the vendor ID */
int get_vendor_id(char *vendor_id);

/** Returns the family of the processor */
int get_family();

/** Returns 1 if the CPU is APERF/MPERF compatible. */
int is_aperf_compatible();

/** Returns the size of a cache line of the higher cache level. */
int get_cache_line_size();

/** Returns an EAR architecture index (top). */
int get_model();

/** Returns if the processor is HTT capable */
int is_cpu_hyperthreading_capable();

/** Returns true if turbo is enabled */
int is_cpu_boost_enabled();

/** Returns the number of packages detected*/
int detect_packages(int **package_map);

/** Prints the affinity mask of the current process */
void print_affinity_mask(topology_t *topo);
/** Checks if process with pid=pid has some cpu forbiden to run , then is_set is set to 1 */
state_t is_affinity_set(topology_t *topo,int pid,int *is_set);

#endif
