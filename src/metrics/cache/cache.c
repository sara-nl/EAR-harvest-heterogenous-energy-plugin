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

#include <metrics/frequency/cpu.h>
#include <metrics/frequency/cpu/intel63.h>

static struct cache_ops
{
	state_t (*init)		(topology_t *tp);
	state_t (*dispose)	();
	state_t (*reset)	();
	state_t (*start)	();
	state_t (*stop)		(llong *L1_misses, llong *LL_misses);
	state_t (*read)		(llong *L1_misses, llong *LL_misses);
	state_t (*print)	(llong *L1_misses, llong *LL_misses);
} ops;

state_t cache_init(topology_t *tp)
{
	if (state_ok(cache_perf_status(tp)))
	{
		ops.init	= cache_perf_init;
		ops.dispose	= cache_perf_dispose;
		ops.reset	= cache_perf_reset;
		ops.start	= cache_perf_start;
		ops.stop	= cache_perf_stop;
		ops.read	= cache_perf_read;
		ops.print	= cache_perf_print;
		return ops.init(tp);
	} else {
		return_msg(EAR_INCOMPATIBLE, Generr.api_incompatible);
	}
}

state_t cache_dispose()
{
	opreturn(ops.dispose,);
}

state_t cache_reset()
{
	opreturn(ops.reset,);
}

state_t cache_start()
{
	opreturn(ops.start,);
}

state_t cache_stop(llong *L1_misses, llong *LL_misses)
{
	opreturn(ops.stop, L1_misses, LL_misses);
}

state_t cache_read(llong *L1_misses, llong *LL_misses)
{
	opreturn(ops.read, L1_misses, LL_misses);
}

state_t cache_print()
{
	opreturn(ops.print, L1_misses, LL_misses);
}