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

//#define SHOW_DEBUGS 1
#include <common/output/debug.h>
#include <metrics/cpi/cpu/intel63.h>

static llong values[2];
static perf_t perf_cyc;
static perf_t perf_ins;

int init_basic_metrics()
{
	state_t s;
	s = perf_open(&perf_ins, &perf_ins, 0, PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
	s = perf_open(&perf_cyc, &perf_ins, 0, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);

	// Remove warning
	(void) (s);

	return EAR_SUCCESS;
}

void reset_basic_metrics()
{
	state_t s;
	s = perf_reset(&perf_ins);

	// Remove warning
	(void) (s);
}

void start_basic_metrics()
{
	state_t s;
	s = perf_start(&perf_ins);

	// Remove warning
	(void) (s);
}

void stop_basic_metrics(long long *cycles, long long *instructions)
{
	state_t s;

	s = perf_stop(&perf_ins);

	get_basic_metrics(cycles, instructions);

	// Remove warning
	(void) (s);
}

void get_basic_metrics(long long *cycles, long long *instructions)
{
	state_t s;

	*instructions = 0;
	*cycles       = 0;
	
	s = perf_read(&perf_ins, values);

	*instructions = values[0];
	*cycles       = values[1];

	debug("total ins %lld", *instructions);
	debug("total cyc %lld", *cycles);

	// Remove warning
	(void) (s);
}
