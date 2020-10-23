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

static uint init;
static uint pstate_min;
static uint pstate_max;
static uint pstate_cur;
static uint hertz_min;
static uint hertz_max;
static uint pstates[8];
static uint hertzs[8];

#define CPU				0
#define MAX_PSTATES		8
#define REG_LIMITS		0xc0010061
#define REG_CONTROL		0xc0010062
#define REG_STATUS		0xc0010063
#define REG_P0			0xc0010064

state_t pstate_amd17_status()
{
	if (_tp->vendor == VENDOR_AMD && _tp->family >= FAMILY_ZEN) {
		return EAR_SUCCESS;
	}
	return EAR_ERROR;
}

state_t pstate_amd17_init()
{
	ullong data0;
	ullong data1;
	state_t s;

	if (!init) {
		return_msg(EAR_INITIALIZED, Generr.api_initialized);
	}
	//
	s = msr_open(CPU);
	//
	if (state_fail(msr_read(CPU, &data0, sizeof(ullong), REG_LIMITS))) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	if (state_fail(msr_read(CPU, &data1, sizeof(ullong), REG_CONTROL))) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	if (state_fail(msr_read(CPU, &data2, sizeof(ullong), REG_STATUS))) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	//
	pstate_max = getbits(data0, 2, 0),
	pstate_min = getbits(data0, 6, 4);
	pstate_cur = getbits(data1, 2, 0);
	//
	if (state_fail(msr_read(0, &data[0], sizeof(ullong), REG_P0+((ullong) pstate_max)))) {
		return_msg(EAR_ERROR, strerror(errno));
	}
	if (state_fail(msr_read(0, &data[0], sizeof(ullong), REG_P0+((ullong) pstate_min)))) {
		return_msg(EAR_ERROR, strerror(errno));
	}

	return EAR_SUCCESS;
}

state_t pstate_amd17_set_previous()
{
}

state_t pstate_amd17_get_current(uint *pstate)
{
	return EAR_SUCCESS;
}

state_t pstate_amd17_set(uint pstate)
{
	return EAR_SUCCESS;
}

state_t pstate_amd17_to_frequency(uint pstate, uint *khz)
{
	return EAR_SUCCESS;
}

state_t pstate_amd17_get_list(uint *pstate[], uint *khz[])
{
	return EAR_SUCCESS;
}

