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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#define _GNU_SOURCE
#include <pthread.h>
#include <common/config.h>
#include <signal.h>
#include <common/colors.h>
#include <common/states.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/system/execute.h>
#include <daemon/powercap_status_conf.h>



static uint current_gpu_pc=0,set_gpu_pc=0;
static uint gpu_pc_enabled=0;
static uint c_status=PC_STATUS_IDLE;
static uint c_mode=PC_MODE_LIMIT;

#define NVIDIA_GPU_SET_POWER_LIMIT_CMD "nvidia-smi –pl %u"
#define NVIDIA_CPU_TEST_CMD						 "nvidia-smi -h"

state_t disable()
{
	return EAR_SUCCESS;
}

state_t enable()
{
	int ret;
	char cmd[256];
	debug("GPU: power cap  enable");
	gpu_pc_enabled=1;
	return EAR_SUCCESS;
}

state_t set_powercap_value(uint pid,uint domain,uint limit)
{
	char cmd[256];
	/* Set data */
	debug("GPU: set_powercap_value %u",limit);
	current_gpu_pc=limit;
	return EAR_SUCCESS;
}

state_t get_powercap_value(uint pid,uint *powercap)
{
	/* copy data */
	debug("GPU::get_powercap_value");
	*powercap=current_gpu_pc;
	return EAR_SUCCESS;
}

uint is_powercap_policy_enabled(uint pid)
{
	return gpu_pc_enabled;
}

void print_powercap_value(int fd)
{
	dprintf(fd,"gpu_powercap_value %u\n",current_gpu_pc);
}
void powercap_to_str(char *b)
{
	sprintf(b,"%u",current_gpu_pc);
}

void set_status(uint status)
{
	debug("GPU. set_status %u",status);
	c_status=status;
}
uint get_powercap_strategy()
{
	debug("GPU. get_powercap_strategy");
	return PC_POWER;
}

void set_pc_mode(uint mode)
{
	debug("GPU. set_pc_mode");
	c_mode=mode;
}


void set_verb_channel(int fd)
{
  WARN_SET_FD(fd);
  VERB_SET_FD(fd);
  DEBUG_SET_FD(fd);
}

