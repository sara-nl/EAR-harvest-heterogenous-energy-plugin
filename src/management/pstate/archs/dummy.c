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

#define SHOW_DEBUGS 1

#include <stdlib.h>
#include <common/output/debug.h>
#include <common/hardware/cpuid.h>
#include <common/hardware/bithack.h>
#include <management/pstate/archs/dummy.h>

static topology_t tp;

typedef struct dummy_ctx_s {
	ctx_t                driver_c;
	mgt_ps_driver_ops_t *driver;
	const ullong        *freqs_available;
	uint                 freqs_count;
	uint                 pstate_nominal;
	uint                 boost_enabled;
	uint                 init;
} dummy_ctx_t;

state_t pstate_dummy_status(topology_t *_tp)
{
	state_t s;
	debug("testing Dummy P_STATE control status");
	if (_tp->vendor != VENDOR_INTEL) {
		return EAR_ERROR;
	}
	// Thread control required
	if (tp.cpu_count == 0) {
		if(xtate_fail(s, topology_copy(&tp, _tp))) {
			return s;
		}
	}
	return EAR_SUCCESS;
}

static state_t static_init_test(ctx_t *c, dummy_ctx_t **f)
{
	if (c == NULL || c->context == NULL) {
		return_msg(EAR_ERROR, Generr.input_null);
	}
	if (((*f = (dummy_ctx_t *) c->context) == NULL) || !(*f)->init) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}
	return EAR_SUCCESS;
}

static state_t static_dispose(dummy_ctx_t *f, state_t s, char *msg)
{
	if (f == NULL) {
		return_msg(s, msg);
	}
	f->driver->dispose(&f->driver_c);
	f->init = 0;
	return_msg(s, msg);
}

state_t pstate_dummy_dispose(ctx_t *c)
{
	dummy_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	return static_dispose(f, EAR_SUCCESS, NULL);
}

state_t static_init(dummy_ctx_t *f)
{
	cpuid_regs_t r;
	// - Intel System Programming Guide Vol. 2A, Software Developer Manual 3-193,
	// INSTRUCTION SET REFERENCE, A-L, Thermal and Power Management Leaf.
	CPUID(r,6,0);
	// Getting the nominal P_STATE (boost enabled or not)
	f->boost_enabled  = getbits32(r.eax, 1, 1);
	f->pstate_nominal = f->boost_enabled;
	return EAR_SUCCESS;
}

state_t pstate_dummy_init(ctx_t *c, mgt_ps_driver_ops_t *ops_driver)
{
	dummy_ctx_t *f;
	state_t s;

	debug("initializing Dummy P_STATE control");
	//
	if (state_ok(static_init_test(c, &f))) {
		return_msg(EAR_ERROR, Generr.api_initialized);
	}
	// Context
    if ((c->context = calloc(1, sizeof(dummy_ctx_t))) == NULL) {
        return_msg(EAR_ERROR, strerror(errno));
    }
    f = (dummy_ctx_t *) c->context;
	//
	f->driver = ops_driver;
	if (xtate_fail(s, f->driver->init(&f->driver_c))) {
		return static_dispose(f, s, state_msg);
	}
	//
	if (xtate_fail(s, f->driver->get_available_list(&f->driver_c, &f->freqs_available, &f->freqs_count))) {
		return static_dispose(f, s, state_msg);
	}
	//
	if (xtate_fail(s, static_init(f))) {
		return static_dispose(f, s, state_msg);
	}
	//
	debug("initialized correctly with %d P_STATEs", f->freqs_count);
	f->init = 1;

	return EAR_SUCCESS;
}

/** Getters */
state_t pstate_dummy_count(ctx_t *c, uint *pstate_count)
{
	dummy_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	*pstate_count = f->freqs_count;
	return EAR_SUCCESS;
}

static state_t static_get_index(dummy_ctx_t *f, ullong freq_khz, uint *pstate_index, uint closest)
{
	int pst;
	// Boost test
	if (f->boost_enabled && f->freqs_available[0] == freq_khz) {
		*pstate_index = 0;
		return EAR_SUCCESS;
	}
	// Closest test
	if (closest && freq_khz > f->freqs_available[f->pstate_nominal]) {
		*pstate_index = f->pstate_nominal;
		return EAR_SUCCESS;
	}
	// Searching
	for (pst = f->pstate_nominal; pst < f->freqs_count; ++pst)
	{
		debug("comparing frequencies %llu == %llu", f->freqs_available[pst], freq_khz);
		if (f->freqs_available[pst] == freq_khz) {
			*pstate_index = pst;
			return EAR_SUCCESS;
		}
		if (closest && f->freqs_available[pst] < freq_khz) {
			*pstate_index = pst-1;
			return EAR_SUCCESS;
		}
	}
	if (closest) {
		*pstate_index = f->freqs_count-1;
		return EAR_SUCCESS;
	}
	return_msg(EAR_ERROR, "P_STATE not found");
}

state_t pstate_dummy_get_available_list(ctx_t *c, pstate_t *pstate_list, uint *pstate_count)
{
	dummy_ctx_t *f;
	state_t s;
	int i;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	for (i = 0; i < f->freqs_count; ++i) {
		pstate_list[i].idx = (ullong) i;
		pstate_list[i].khz = f->freqs_available[i];
	}
	if (pstate_count != NULL) {
		*pstate_count = f->freqs_count;
	}
	return EAR_SUCCESS;
}

state_t pstate_dummy_get_current_list(ctx_t *c, pstate_t *pstate_list)
{
	const ullong *current_list;
	dummy_ctx_t *f;
	uint cpu, pst;
	state_t s;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (xtate_fail(s, f->driver->get_current_list(&f->driver_c, &current_list))) {
		return s;
	}
	for (cpu = 0; cpu < tp.cpu_count; ++cpu) {
		pstate_list[cpu].idx = 0;
		pstate_list[cpu].khz = f->freqs_available[0];

		if (state_ok(static_get_index(f, current_list[cpu], &pst, 0)))
		{
			pstate_list[cpu].idx = pst;
			pstate_list[cpu].khz = f->freqs_available[pst];
		}
	}

	return EAR_SUCCESS;
}

state_t pstate_dummy_get_nominal(ctx_t *c, uint *pstate_index)
{
	dummy_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	*pstate_index = f->pstate_nominal;
	return EAR_SUCCESS;
}

state_t pstate_dummy_get_governor(ctx_t *c, uint *governor)
{
	dummy_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	return f->driver->get_governor(&f->driver_c, governor);
}

state_t pstate_dummy_get_index(ctx_t *c, ullong freq_khz, uint *pstate_index, uint closest)
{
	dummy_ctx_t *f;
	state_t s;
	
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	return static_get_index(f, freq_khz, pstate_index, closest);
}

/** Setters */
state_t pstate_dummy_set_current_list(ctx_t *c, uint *pstate_index)
{
	dummy_ctx_t *f;
	state_t s;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (xtate_fail(s, f->driver->set_governor(&f->driver_c, Governor.userspace))) {
		return s;
	}
	if (xtate_fail(s, f->driver->set_current_list(&f->driver_c, pstate_index))) {
		return s;
	}

	return EAR_SUCCESS;
}

state_t pstate_dummy_set_current(ctx_t *c, uint pstate_index, int cpu)
{
	dummy_ctx_t *f;
	state_t s;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (xtate_fail(s, f->driver->set_governor(&f->driver_c, Governor.userspace))) {
		return s;
	}
	if (xtate_fail(s, f->driver->set_current(&f->driver_c, pstate_index, cpu))) {
		return s;
	}

	return EAR_SUCCESS;
}

state_t pstate_dummy_set_governor(ctx_t *c, uint governor)
{
	dummy_ctx_t *f;
	state_t s;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (xtate_fail(s, f->driver->set_governor(&f->driver_c, governor))) {
		return s;
	}

	return EAR_SUCCESS;
}
