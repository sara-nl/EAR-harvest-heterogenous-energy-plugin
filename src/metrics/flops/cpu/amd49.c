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

#include <common/output/debug.h>
#include <metrics/common/perf.h>
#include <metrics/flops/cpu/amd49.h>

static perf_t perf_gen;
static perf_t perf_256;
static perf_t perf_mer;

static llong values[3];
static llong accums[3];

state_t flops_amd49_status(topology_t *tp)
{
	if (tp->vendor == VENDOR_AMD && tp->family >= FAMILY_ZEN) {
		return EAR_SUCCESS;
	}
	return EAR_ERROR;
}

state_t flops_amd49_init(ctx_t *c)
{
	state_t s;

	//
	s = perf_open(&perf_gen,      NULL, 0, PMU_GENERAL, 0x0000004cb);
	s = perf_open(&perf_256, &perf_gen, 0, PMU_GENERAL, 0x000000f03);
	s = perf_open(&perf_mer, &perf_gen, 0, PMU_GENERAL, 0xf000000ff);

	// Remove warning
	(void) (s);

	return EAR_SUCCESS;
}

state_t flops_amd49_dispose(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t flops_amd49_reset(ctx_t *c)
{
	state_t s;

	s = perf_reset(&perf_gen);

	// Remove warning
	(void) (s);

	return EAR_SUCCESS;
}

state_t flops_amd49_start(ctx_t *c)
{
	state_t s;

	s = perf_start(&perf_gen);

	// Remove warning
	(void) (s);

	return EAR_SUCCESS;
}

state_t flops_amd49_read(ctx_t *c, llong *flops, llong *ops)
{
	state_t s;

	s = perf_read(&perf_gen, &values);

	// Remove warning
	(void) (s);

	debug("read 0 %lld", values[0]);
	debug("read 1 %lld", values[1]);
	debug("read 2 %lld", values[2]);

	accums[0] += values[0];
	accums[1] += values[1];
	accums[2]  = 0;

	if (ops != NULL) {
		ops[0] = 0;
		ops[1] = 0;
		ops[2] = 0;
		ops[3] = 0;
		ops[4] = values[1];
		ops[5] = 0;
		ops[6] = 0;
		ops[7] = 0;
	}

	if (flops != NULL) {
		*flops  = values[1];
	}

	debug("total flops %lld", *flops);

	return EAR_SUCCESS;
}

state_t flops_amd49_stop(ctx_t *c, llong *flops, llong *ops)
{
	state_t s;

	s = perf_stop(&perf_064f);
	s = perf_stop(&perf_256f);
	
	// Remove warning
	(void) (s);

	return flops_amd49_read(c, flops, ops);
}

state_t flops_amd49_count(ctx_t *c, uint *count)
{
	*count = 8;
	return 0;
}

state_t flops_amd49_read_accum(ctx_t *c, llong *ops)
{
	ops[0] = 0;
	ops[1] = 0;
	ops[2] = 0;
	ops[3] = 0;
	ops[4] = accums[1];
	ops[5] = 0;
	ops[6] = 0;
	ops[7] = 0;
	return EAR_SUCCESS;
}

state_t flops_amd49_weights(uint *weigths)
{
	weigths[0] = 0;
	weigths[1] = 0;
	weigths[2] = 0;
	weigths[3] = 0;
	weigths[4] = 1;
	weigths[5] = 0;
	weigths[6] = 0;
	weigths[7] = 0;
	return EAR_SUCCESS;
}
