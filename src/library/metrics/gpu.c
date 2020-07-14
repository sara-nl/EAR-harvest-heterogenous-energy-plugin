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
#include <common/output/verbose.h>
#include <common/output/debug.h>
#include <library/metrics/gpu.h>
#include <daemon/local_api/eard_api.h>

static gpu_ops_t *ops;
static uint dev_count;

state_t gpu_lib_load(settings_conf_t *settings)
{
	uint gpu_model;
	state_t ret;
	if ((ret=eards_gpu_model(&gpu_model))!=EAR_SUCCESS) return ret;
	debug("eards_gpu_model %u ",gpu_model);
	return gpu_load(&ops, gpu_model, NULL);
}

state_t gpu_lib_init(ctx_t *c)
{
	state_t ret;
	if ((ret=eards_gpu_dev_count(&dev_count))!=EAR_SUCCESS) return ret;
	debug("eards_gpu_dev_count %u",dev_count);
	preturn (ops->data_init, dev_count);
}

state_t gpu_lib_dispose(ctx_t *c)
{
	return EAR_SUCCESS;
}

state_t gpu_lib_read(ctx_t *c, gpu_t *data)
{
	state_t ret;
	char gpu_ste[256];
	debug("reading %u gpus",dev_count);
	if ((ret=eards_gpu_data_read(data,dev_count))!=EAR_SUCCESS) return ret;	
	gpu_lib_data_tostr(data,gpu_ste,sizeof(gpu_ste));
	debug("gpu data %s",gpu_ste);
	return EAR_SUCCESS;
}

state_t gpu_lib_read_copy(ctx_t *c, gpu_t *data2, gpu_t *data1, gpu_t *data_diff)
{
	state_t s;
	if (xtate_fail(s, gpu_lib_read(c, data2))) {
		return s;
	}
	if (xtate_fail(s, ops->data_diff(data2, data1, data_diff))) {
		return s;
	}
	return ops->data_copy(data1, data2);
}

state_t gpu_lib_count(ctx_t *c, uint *_dev_count)
{
	*_dev_count = dev_count;
	return EAR_SUCCESS;
}

state_t gpu_lib_data_diff(gpu_t *data2, gpu_t *data1, gpu_t *data_diff)
{
	preturn (ops->data_diff, data2, data1, data_diff);
}

state_t gpu_lib_data_alloc(gpu_t **data)
{
	preturn (ops->data_alloc, data);
}

state_t gpu_lib_data_free(gpu_t **data)
{
	preturn (ops->data_free, data);
}

state_t gpu_lib_data_null(gpu_t *data)
{
	preturn (ops->data_null, data);
}

state_t gpu_lib_data_copy(gpu_t *data_dst, gpu_t *data_src)
{
	preturn (ops->data_copy, data_dst, data_src);
}

state_t gpu_lib_data_print(gpu_t *data, int fd)
{
	preturn (ops->data_print, data, fd);
}

state_t gpu_lib_data_tostr(gpu_t *data, char *buffer, int length)
{
	preturn (ops->data_tostr, data, buffer, length);
}

state_t gpu_lib_data_merge(gpu_t *data_diff, gpu_t *data_merge)
{
  preturn (ops->data_merge, data_diff, data_merge);
}

