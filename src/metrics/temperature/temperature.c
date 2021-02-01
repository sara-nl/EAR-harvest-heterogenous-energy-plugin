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
#include <metrics/temperature/temperature.h>
#include <metrics/temperature/archs/amd17.h>
#include <metrics/temperature/archs/intel63.h>
#include <metrics/temperature/archs/dummy.h>

// /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -I ../../ -o temp temperature.c ../libmetrics.a ../../common/libcommon.a -lpthread

#if 1
static struct temp_ops
{
	state_t (*init)			(ctx_t *c);
	state_t (*dispose)		(ctx_t *c);
	state_t (*read)			(ctx_t *c, llong *temp, llong *average);
	state_t (*data_alloc)	(llong **temp);
	state_t (*data_free)	(llong **temp);
	state_t (*data_copy)	(llong *temp1, llong *temp2);
} ops;

state_t temp_load(topology_t *tp)
{
	if (ops.init != NULL) {
		return EAR_SUCCESS;
	}
	if (state_ok(temp_dummy_status(tp)))
	{
		ops.init       = temp_dummy_init;
		ops.dispose    = temp_dummy_dispose;
		ops.read       = temp_dummy_read;
		ops.data_alloc = temp_dummy_data_alloc;
		ops.data_free  = temp_dummy_data_free;
		ops.data_copy  = temp_dummy_data_copy;
		debug("selected dummy temperature");
	}
	if (state_ok(temp_intel63_status(tp)))
	{
		ops.init       = temp_intel63_init;
		ops.dispose    = temp_intel63_dispose;
		ops.read       = temp_intel63_read;
		debug("selected intel63 temperature");
	}
	else if (state_ok(temp_amd17_status(tp)))
	{
		ops.init       = temp_amd17_init;
		ops.dispose    = temp_amd17_dispose;
		ops.read       = temp_amd17_read;
		debug("selected amd17 temperature");
	}
	if (ops.init == NULL) {
		return_msg(EAR_ERROR, Generr.api_incompatible);
	}
	return EAR_SUCCESS;
}

state_t temp_init(ctx_t *c)
{
	preturn(ops.init, c);
}

state_t temp_dispose(ctx_t *c)
{
	preturn(ops.dispose, c);
}

state_t temp_read(ctx_t *c, llong *temp, llong *average)
{
	preturn(ops.read, c, temp, average);
}

state_t temp_data_alloc(llong **temp)
{
	preturn(ops.data_alloc, temp);
}

state_t temp_data_free(llong **temp)
{
	preturn(ops.data_free, temp);
}

state_t temp_data_copy(llong *temp2, llong *temp1)
{
	preturn(ops.data_copy, temp2, temp1);
}

#else
#include <library/dynais/dynais.h>

topology_t topo;
llong *temp2;
llong *temp1;
llong avg;
state_t s;
ctx_t c;
uint n;

int main()
{
    int i;
    
	topology_init(&topo);

    state_assert(s, temp_load(&topo),)
    
	state_assert(s, temp_init(&c),)
	
	state_assert(s, temp_data_alloc(&temp2),)
	state_assert(s, temp_data_alloc(&temp1),)
    
	state_assert(s, temp_read(&c, temp1, &avg),)

	state_assert(s, temp_data_copy(temp2, temp1),)
	
	printf("0: %lld\n", temp2[0]);
	printf("1: %lld\n", temp2[1]);
	printf("A: %lld\n", avg);
	
	state_assert(s, temp_data_free(&temp2),)
	state_assert(s, temp_data_free(&temp1),)

	state_assert(s, temp_dispose(&c),)

	printf("finished temp\n");

	int r = dynais_init(256, 10);
	printf("dynais returned %d\n", r);
	dynais_dispose();

	sleep(1);

	return 0;
}
#endif
