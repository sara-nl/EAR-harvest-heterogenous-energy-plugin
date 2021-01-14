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

#include <errno.h>
#include <stdlib.h>
#include <metrics/temperature/archs/dummy.h>

static uint socket_count;

state_t temp_dummy_status(topology_t *topo)
{
	if (socket_count == 0) {
		if (topo->socket_count > 0) {
			socket_count = topo->socket_count;
		} else {
			socket_count = 1;
		}
	}
	return EAR_SUCCESS;
}

state_t temp_dummy_init(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t temp_dummy_dispose(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t temp_dummy_read(ctx_t *c, llong *temp, llong *average)
{
	if (temp == NULL) {
		return_msg(EAR_BAD_ARGUMENT, Generr.input_null);
	}
    if (memset((void *) temp, 0, sizeof(llong)*socket_count) != temp) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	if (average != NULL) {
		*average = 0;
	}
	return EAR_SUCCESS;
}

state_t temp_dummy_data_alloc(llong **temp)
{
	if (temp == NULL) {
		return_msg(EAR_BAD_ARGUMENT, Generr.input_null);
	}
	if ((*temp = (llong *) calloc(socket_count, sizeof(llong))) == NULL) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	return EAR_SUCCESS;
}

state_t temp_dummy_data_free(llong **temp)
{
	if (temp == NULL || *temp == NULL) {
		return_msg(EAR_BAD_ARGUMENT, Generr.input_null);
	}
	free(*temp);
	*temp = NULL;
	return EAR_SUCCESS;
}

state_t temp_dummy_data_copy(llong *temp2, llong *temp1)
{
	if (temp2 == NULL || temp1 == NULL) {
		return_msg(EAR_BAD_ARGUMENT, Generr.input_null);
	}
	if (memcpy((void *) temp2, (const void *) temp1, sizeof(llong)*socket_count) != temp2) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	return EAR_SUCCESS;
}
