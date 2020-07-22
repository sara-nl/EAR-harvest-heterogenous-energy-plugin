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
#include <metrics/common/perf.h>
#include <metrics/cpi/cpu/intel63.h>

static llong values[2];
static perf_t perf_cyc;
static perf_t perf_ins;
static uint initialized;

state_t cpi_intel63_status(topology_t *tp)
{
	if (tp->vendor == VENDOR_AMD && tp->family >= FAMILY_ZEN) {
		return EAR_SUCCESS;
	}
	if (tp->vendor == VENDOR_INTEL && tp->model >= MODEL_HASWELL_X) {
		return EAR_SUCCESS;
	}
	return EAR_ERROR;
}

state_t cpi_intel63_init(ctx_t *c)
{
	state_t s;
	if (xtate_fail(s, perf_open(&perf_ins, &perf_ins, 0, PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS))) {
		return s;
	}
	if (xtate_fail(s, perf_open(&perf_cyc, &perf_ins, 0, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES))) {
		perf_close(&perf_ins);
		return s;
	}
	initialized = 1;
	return EAR_SUCCESS;
}

state_t cpi_intel63_dispose(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t cpi_intel63_reset(ctx_t *c)
{
	if (!initialized) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}
	return perf_reset(&perf_ins);
}

state_t cpi_intel63_start(ctx_t *c)
{
	if (!initialized) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}
	return perf_start(&perf_ins);
}

state_t cpi_intel63_stop(ctx_t *c, llong *cycles, llong *insts)
{
	state_t s;

	if (!initialized) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}
	if (xtate_fail(s, perf_stop(&perf_ins))) {
		return s;
	}

	return cpi_intel63_read(c, cycles, insts);
}

state_t cpi_intel63_read(ctx_t *c, llong *cycles, llong *insts)
{
	state_t s;

	*insts  = 0;
	*cycles = 0;

	if (!initialized) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}
	if (xtate_fail(s, perf_read(&perf_ins, values))) {
		return s;
	}

	*insts  = values[0];
	*cycles = values[1];

	debug("total ins %lld", *insts);
	debug("total cyc %lld", *cycles);

	return EAR_SUCCESS;
}
