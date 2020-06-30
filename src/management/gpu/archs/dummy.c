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

#include <stdlib.h>
#include <management/gpu/archs/dummy.h>

static struct error_s {
	char *null_data;
} Error = {
	.null_data = "data pointer is NULL",
};

state_t mgt_dummy_status()
{
	return 1;
}

state_t mgt_dummy_init(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t mgt_dummy_dispose(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t mgt_dummy_count(ctx_t *c, uint *_dev_count)
{
	if (_dev_count != NULL) {
		*_dev_count = 1;
	}
	return EAR_SUCCESS;
}

state_t dummy_clock_limit_get_current(ctx_t *c, uint *khz)
{
	if (khz != NULL) {
		khz[0] = 0;
	}
	return EAR_SUCCESS;
}

state_t dummy_clock_limit_get_default(ctx_t *c, uint *khz)
{
	if (khz != NULL) {
		khz[0] = 0;
	}
	return EAR_SUCCESS;
}

state_t dummy_clock_limit_get_max(ctx_t *c, uint *khz)
{
	if (khz != NULL) {
		khz[0] = 0;
	}
	return EAR_SUCCESS;
}

state_t dummy_clock_limit_reset(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t dummy_clock_limit_set(ctx_t *c, uint *khz)
{
	if (khz != NULL) {
		khz[0] = 0;
	}
	return EAR_SUCCESS;
}

state_t dummy_power_limit_get_current(ctx_t *c, uint *watts)
{
	if (watts != NULL) {
		watts[0] = 0;
	}
	return EAR_SUCCESS;
}

state_t dummy_power_limit_get_default(ctx_t *c, uint *watts)
{
	if (watts != NULL) {
		watts[0] = 0;
	}
	return EAR_SUCCESS;
}

state_t dummy_power_limit_get_max(ctx_t *c, uint *watts)
{
	if (watts != NULL) {
		watts[0] = 0;
	}
	return EAR_SUCCESS;
}

state_t dummy_power_limit_reset(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t dummy_power_limit_set(ctx_t *c, uint *watts)
{
	if (watts != NULL) {
		watts[0] = 0;
	}
	return EAR_SUCCESS;
}