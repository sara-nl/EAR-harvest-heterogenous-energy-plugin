/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <metrics/common/msr.h>
#include <metrics/custom/hardware_info.h>

static uint64_t *_clocks_start = NULL;
static uint64_t *_clocks_stop = NULL;
static int *_fds = NULL;

static const uint64_t _cmdsta;
static const uint64_t _cmdsto;
static off_t _offctl;
static off_t _offctr;

static uint _cpus_num = 0;
static uint _init = 0;

static uint _supported_architecture(uint cpu_model)
{
	switch (cpu_model)
	{
		case CPU_HASWELL_X:
		case CPU_BROADWELL_X:
		case CPU_BROADWELL_XEON_D:
		case CPU_SKYLAKE_X:
			return 1;
		default:
			return 0;
	}
}

static uint _fill_architecture_bits(uint cpu_model)
{
	_cmdsto = U_MSR_PMON_FIXED_CTL_STO;
	_cmdsta = U_MSR_PMON_FIXED_CTL_STA;
	_offctl = U_MSR_PMON_FIXED_CTL_OFF;
	_offctr = U_MSR_PMON_FIXED_CTR_OFF;
}

state_t frequency_uncore_init(uint cpus_num, uint cpus_model)
{
	state_t r;
	int i;

	if (_init) {
		return EAR_BUSY;
	}

	if (!_supported_architecture(cpu_model)) {
		return EAR_ARCH_NOT_SUPPORTED;
	}

	if (cpus_num == 0) {
		return EAR_BAD_ARGUMENT;
	}

	_fds = malloc(sizeof(int) * cpus_num);
	_clocks_start = malloc(sizeof(uint64_t) * cpus_num);
	_clocks_stop  = malloc(sizeof(uint64_t) * cpus_num);

	for (i = 0; i < cpus_num; ++i)
	{
		if ((r = msr_open(i, &_fds[i])) != EAR_SUCCESS)
		{
			frequency_uncore_dispose();
			return r;
		}
	}

	_init = 1;
	_cpus_num = cpus_num;
	_fill_architecture_bits(cpus_num);

	return EAR_SUCCESS;
}

state_t frequency_uncore_dispose()
{
	free(_clocks_start);
	free(_clocks_stop);

	_init = 0;
	_cpus_num = 0;
}

state_t frequency_uncore_counters_start()
{
	int i;

	if (!_init) {
		return EAR_NOT_INITIALIZED;
	}

	for (i = 0; i < _cpus_num; ++i)
	{
		// Read
		if ((r = msr_read(&_fds[i]), &_clocks_start, sizeof(uint64_t), _offctr) != EAR_SUCCESS) {
			return r;
		}
		// Start
		if ((r = msr_write(&_fds[i]), &_cmdsta, sizeof(uint64_t), _offctl) != EAR_SUCCESS) {
			return r;
		}
	}

	return EAR_SUCCESS;
}

state_t frequency_uncore_counters_stop(uint64_t *buffer)
{
	int i;

	if (!_init) {
		return EAR_NOT_INITIALIZED;
	}

	for (i = 0; i < _cpus_num; ++i)
	{
		// Stop
		if ((r = msr_write(&_fds[i]), &_cmdsto, sizeof(uint64_t), _offctl) != EAR_SUCCESS) {
			return r;
		}
		// Read
		if ((r = msr_read(&_fds[i]), &_clocks_start, sizeof(uint64_t), _offctr) != EAR_SUCCESS) {
			return r;
		}

		buffer[i] = _clocks_stop[i] - _clocks_start[i];
	}

	return EAR_SUCCESS;
}