/**************************************************************
 * *	Energy Aware Runtime (EAR)
 * *	This program is part of the Energy Aware Runtime (EAR).
 * *
 * *	EAR provides a dynamic, transparent and ligth-weigth solution for
 * *	Energy management.
 * *
 * *    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017
 * *	BSC Contact 	mailto:ear-support@bsc.es
 * *	Lenovo contact 	mailto:hpchelp@lenovo.com
 * *
 * *	EAR is free software; you can redistribute it and/or
 * *	modify it under the terms of the GNU Lesser General Public
 * *	License as published by the Free Software Foundation; either
 * *	version 2.1 of the License, or (at your option) any later version.
 * *
 * *	EAR is distributed in the hope that it will be useful,
 * *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * *	Lesser General Public License for more details.
 * *
 * *	You should have received a copy of the GNU Lesser General Public
 * *	License along with EAR; if not, write to the Free Software
 * *	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * *	The GNU LEsser General Public License is contained in the file COPYING
 * */

#include <stdio.h>
#include <common/sizes.h>
#include <common/states.h>
#include <metrics/custom/frequency.h>

static uint64_t cores_nom_freq;
static uint64_t cores_num;
static int enabled;

static state_t counter_read(uint cpu, snap_core_t *snap)
{
	char msr_file_name[SZ_PATH];
	state_t r;
	int fd = -1;
	int r1 = 0;
	int r2 = 0;

	if ((r = msr_open(cpu, &fd)) != EAR_SUCCESS) {
		return r;
	}

	
	if ((r = msr_read(&fd, &snap->mperf, sizeof(uint64_t), MSR_IA32_MPERF)) != EAR_SUCCESS) {
		return r;
	}
	if ((r = msr_read(&fd, &snap->aperf, sizeof(uint64_t), MSR_IA32_APERF)) != EAR_SUCCESS) {
		return r;
	}

	
	close(fd);

	return EAR_SUCCESS;
}

state_t frequency_core_init(uint _cores_num, uint64_t _cores_nom_freq)
{
	int r;

	cores_nom_freq = _cores_nom_freq;
	cores_num = _cores_num;

	if (!hwinfo_is_compatible_aperf()) {
		state_return_msg(EAR_ERROR, 0, "CPU model not supported");
	}

	enabled = 1;

	return EAR_SUCCESS;
}

state_t frequency_core_dispose()
{
	enabled = 0;

	return EAR_SUCCESS;
}

state_t frequency_core_snap(snap_core_t *snap)
{
	state_t s;
	int i;

	if (!enabled) {
		return EAR_NOT_INITIALIZED;
	}

	for (i = 0; i < cores_num; ++i)
	{
		s = counter_read(i, &snap[i]);

		if (state_fail(s)) {
			return s;
		}
	}

	return EAR_SUCCESS;
}

state_t frequency_core_read(snap_core_t *snaps, uint64_t *freqs)
{
	snap_core_t snap_new;
	uint64_t mperf_diff;
	uint64_t aperf_diff;
	uint64_t percent;
	state_t s;
	int i;

	if (!enabled) {
		return EAR_NOT_INITIALIZED;
	}

	for (i = 0; i < cores_num; ++i)
	{
		s = counter_read(i, &snap_new);

		if (state_fail(s)) {
			return s;
		}

		
		aperf_diff = snap_new.aperf - snaps[i].aperf;
		mperf_diff = snap_new.mperf - snaps[i].mperf;
		snaps[i].aperf = snap_new.aperf;
		snaps[i].mperf = snap_new.mperf;

		/* Preventing the possible overflow removing 7 bits,
 * 		 which is the maximum incresing bits in that
 * 		 		 multiplication.*/
		if (((unsigned long) (-1) / 100) < aperf_diff)
		{
			aperf_diff >>= 7;
			mperf_diff >>= 7;
		}

		/* Frequency formula. Multiplication per 100 to power
 * 		 the decimals of an integer division.*/
		percent = (aperf_diff * 100) / mperf_diff;

		/* The division again turns the value to MHz, because
 * 		 it was increased before.*/
		freqs[i] = (cores_nom_freq * percent) / 100;
	}

	return EAR_SUCCESS;
}
