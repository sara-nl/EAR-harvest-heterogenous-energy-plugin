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
}

void reset_basic_metrics()
{
	state_t s;
	s = perf_reset(&perf_ins);
}

void start_basic_metrics()
{
	state_t s;
	s = perf_start(&perf_ins);
}

void stop_basic_metrics(long long *cycles, long long *instructions)
{
	state_t s;

	s = perf_stop(&perf_ins);

	get_basic_metrics(cycles, instructions);
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
}
