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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/config.h>
#include <common/config/config_install.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/output/debug.h>
#if POWERCAP
#include <common/types/pc_app_info.h>
#include <daemon/powercap/powercap_status_conf.h>

void pcapp_info_new_job(pc_app_info_t *t)
{
	t->req_f=0;
	t->req_power=0;
	t->pc_status=PC_STATUS_IDLE;
	#if USE_GPUS
	t->num_gpus_used=0;
	memset(t->req_gpu_f,0,sizeof(ulong)*MAX_GPUS_SUPPORTED);
	t->req_gpu_power=0;
	#endif
}
void pcapp_info_end_job(pc_app_info_t *t)
{
	t->req_f=0;
	t->req_power=0;
	t->pc_status=PC_STATUS_IDLE;
	#if USE_GPUS
	t->num_gpus_used=0;
	memset(t->req_gpu_f,0,sizeof(ulong)*MAX_GPUS_SUPPORTED);
	t->req_gpu_power=0;
	#endif
}
void pcapp_info_set_req_f(pc_app_info_t *t,ulong f)
{
	t->req_f=f;
	t->pc_status=PC_STATUS_OK;
}

void debug_pc_app_info(pc_app_info_t *t)
{
	float f;
	int i;
	f=(float)t->req_f/1000000.0;
	debug("PC_APP_INFO: mode %u CPU (req_f %.2f power %lu status %u)",t->mode, f,t->req_power,t->pc_status);
	#if USE_GPUS
	for (i=0;i<t->num_gpus_used;i++){
		f=(float)t->req_gpu_f[i]/1000000.0;
		debug("\t(GPU[%d] req_f %.2f power %lu status %u)",i,f,t->req_gpu_power,t->pc_gpu_status);
	}
	#endif
}

#endif

