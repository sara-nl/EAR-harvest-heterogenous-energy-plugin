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

#define SHOW_DEBUGS 1

#include <common/output/debug.h>
#include <metrics/common/perf.h>
#include <metrics/flops/cpu/amd49.h>

// References:
// PPR for AMD Family 17h. Topic 2.1.15.4.1 Floating Point (FP) Events.
//
// The event FpRetSseAvxOps works next MergeEvent in the set of generic
// AMD Performance Monitoring Counters. MergeEvent (0xFFF) have to be
// set in an odd numbered PMC (there are 6 per thread). Then, the event
// FpRetSseAvxOps have to be set in the corresponding even counter.
// Supposedly, the FpRetSseAvxOps have to be set in the counter 0, 2 or
// 4 and the MergeEvent in 1, 3 and 5.
//
// Working scheme:
//	0 event + 1 merge
//	2 event + 3 merge
//	4 event + 5 merge
//
// We found that in the an AMD node a fixed event was always present
// sitting in the counter 0. So we manage to create a combination of
// even/odd FpRetSseAvxOps and MergeEvent to satisfy at least one
// working scheme (F M M F M).

const ulong	cmd_avx_insts = 0x00000000000004cb; // AVX instructions event
const ulong cmd_avx_flops = 0x0000000000000f03; // Flops event
const ulong cmd_avx_merge = 0x0000000f000000ff; // Merge event

static perf_t perf_evn0;
static perf_t perf_mer1;
static perf_t perf_mer2;
static perf_t perf_evn3;
static perf_t perf_mer4;

// The counters are 48 bit values. But it can be added the MergeEvent
// counter which supposedly expands the counters to 64 bits.
static llong values[5];
static llong accums[5];

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
	s = perf_open(&perf_evn0, &perf_evn0, 0, PERF_TYPE_RAW, cmd_avx_flops);
	s = perf_open(&perf_mer1, &perf_evn0, 0, PERF_TYPE_RAW, cmd_avx_merge);
	s = perf_open(&perf_mer2, &perf_evn0, 0, PERF_TYPE_RAW, cmd_avx_merge);
	s = perf_open(&perf_evn3, &perf_evn0, 0, PERF_TYPE_RAW, cmd_avx_flops);
	s = perf_open(&perf_mer4, &perf_evn0, 0, PERF_TYPE_RAW, cmd_avx_merge);

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

	s = perf_reset(&perf_evn0);

	// Remove warning
	(void) (s);

	return EAR_SUCCESS;
}

state_t flops_amd49_start(ctx_t *c)
{
	state_t s;

	s = perf_start(&perf_evn0);

	// Remove warning
	(void) (s);

	return EAR_SUCCESS;
}

state_t flops_amd49_read(ctx_t *c, llong *flops, llong *ops)
{
	llong aux;
	state_t s;

	s = perf_read(&perf_evn0, values);

	// Remove warning
	(void) (s);

	debug("read 0 %lld", values[0]);
	debug("read 1 %lld", values[1]);
	debug("read 2 %lld", values[2]);
	debug("read 3 %lld", values[3]);
	debug("read 4 %lld", values[4]);

	if ((aux = values[0]) == 0) {
		aux = values[3];
	}

	accums[0] += aux;
	accums[1]  = 0;
	accums[2]  = 0;
	accums[3]  = 0;
	accums[4]  = 0;

	if ((aux = values[0]) == 0) {
		aux = values[3];
	}

	if (ops != NULL) {
		ops[0] = 0;
		ops[1] = 0;
		ops[2] = 0;
		ops[3] = 0;
		ops[4] = aux;
		ops[5] = 0;
		ops[6] = 0;
		ops[7] = 0;
	}

	if (flops != NULL) {
		*flops  = aux;
	}

	debug("total flops %lld", *flops);

	return EAR_SUCCESS;
}

state_t flops_amd49_stop(ctx_t *c, llong *flops, llong *ops)
{
	state_t s;

	s = perf_stop(&perf_gen);
	
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
	ops[4] = accums[0];
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
