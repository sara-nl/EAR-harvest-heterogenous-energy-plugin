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

#ifndef METRICS_BANDWIDTH_CPU
#define METRICS_BANDWIDTH_CPU

#include <common/states.h>
#include <common/plugins.h>
#include <common/hardware/topology.h>

#define BW_KB 1024.0
#define BW_MB 1048576.0
#define BW_GB 1073741824.0

int init_uncores(int nothing);

int dispose_uncores();

int count_uncores();

int check_uncores();

int start_uncores();

int reset_uncores();

int stop_uncores(ullong *cas);

int read_uncores(ullong *cas);

int compute_uncores(ullong *cas2, ullong *cas1, double *bytes, double units);

int alloc_array_uncores(ullong **array);

#endif
