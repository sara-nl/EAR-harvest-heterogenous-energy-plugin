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

state_t flops_dummy_init(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t flops_dummy_dispose(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t flops_dummy_reset(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t flops_dummy_start(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t flops_dummy_read(ctx_t *c, llong *flops, llong *ops)
{
	*flops = 0;
	*ops   = 0;
	return EAR_SUCCESS;
}

state_t flops_dummy_stop(ctx_t *c, llong *flops, llong *ops)
{
	*flops = 0;
	*ops   = 0;
	return EAR_SUCCESS;
}

state_t flops_dummy_count(ctx_t *c, uint *count)
{
	*count = 1;
	return EAR_SUCCESS;
}

state_t flops_dummy_read_accum(ctx_t *c, llong *flops)
{
	*flops = 0;
	return EAR_SUCCESS;
}

state_t flops_dummy_weights(uint *weigths)
{
	*weigths = 1;
	return EAR_SUCCESS;
}