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

#ifndef METRICS_CPI_INTEL63
#define METRICS_CPI_INTEL63

#include <metrics/common/perf.h>
#include <metrics/cpi/cpi.h>

int init_basic_metrics();

void reset_basic_metrics();

void start_basic_metrics();

void stop_basic_metrics(long long *cycles, long long *instructions);

void get_basic_metrics(long long *total_cycles, long long *instructions);

#endif //METRICS_CPI_INTEL63
