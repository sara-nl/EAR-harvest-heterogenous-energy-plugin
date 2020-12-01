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

#include <math.h>
#include <stdlib.h>
#include <metrics/common/msr.h>
#include <common/output/debug.h>
#include <common/string_enhanced.h>
#include <common/hardware/bithack.h>
#include <management/pstate/archs/amd17.h>

typedef struct pss_s {
	ullong fid;
	ullong did;
	ullong cof;
} pss_t;

#define CPU				0
#define MAX_PSTATES		128
#define MAX_REGISTERS	8
#define REG_HWCONF		0xc0010015
#define REG_LIMITS		0xc0010061
#define REG_CONTROL		0xc0010062
#define REG_STATUS		0xc0010063
#define REG_P0			0xc0010064
#define REG_P1			0xc0010065

static topology_t tp;

typedef struct amd17_ctx_s {
	ctx_t                driver_c;
	mgt_ps_driver_ops_t *driver;
	ullong               regs[MAX_REGISTERS];
	pss_t                psss[MAX_PSTATES];
	uint                 boost_enabled;
	uint                 pss_nominal;
	uint                 pss_count;
	uint                 user_mode;
	uint                 init;
} amd17_ctx_t;

state_t pstate_amd17_status(topology_t *_tp)
{
	state_t s;
	debug("testing AMD17 P_STATE control status");
	if (_tp->vendor != VENDOR_AMD) {
		return EAR_ERROR;
	}
	if (_tp->family < FAMILY_ZEN) {
		return EAR_ERROR;
	}
	// Thread control required
	if (tp.cpu_count == 0) {
		if(xtate_fail(s, topology_copy(&tp, _tp))) {
			return EAR_ERROR;
		}
	}
	return EAR_SUCCESS;
}

static state_t static_init_test(ctx_t *c, amd17_ctx_t **f)
{
	if (c == NULL) {
		return_msg(EAR_ERROR, Generr.input_null);
	}
	if (((*f = (amd17_ctx_t *) c->context) == NULL) || !(*f)->init) {
		return_msg(EAR_ERROR, Generr.api_uninitialized);
	}
	return EAR_SUCCESS;
}

static state_t static_dispose(amd17_ctx_t *f, uint close_msr_up_to, state_t s, char *msg)
{
	int cpu;

	for (cpu = 0; cpu < close_msr_up_to; ++cpu) {
		msr_close(tp.cpus[cpu].id);
	}
	if (f == NULL) {
		return_msg(s, msg);
	}
	//
	if (f->driver != NULL) {
		f->driver->dispose(&f->driver_c);
	}
	//
	f->init = 0;
	return_msg(s, msg);
}

state_t pstate_amd17_dispose(ctx_t *c)
{
	amd17_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	return static_dispose(f, tp.cpu_count, EAR_SUCCESS, NULL);
}

#if SHOW_DEBUGS
static void pstate_print(amd17_ctx_t *f)
{
	debug ("-----------------------------------------------------------------------------------");
	tprintf_init(STDERR_FILENO, STR_MODE_DEF, "4 18 4 8 8 11 8 7 8 10 10");
	tprintf("#||reg||en||IddDiv||IddVal||IddPerCore|||CpuVid|||CpuFid||  200||CpuDfsId||CoreCof");
	tprintf("-||---||--||------||------||----------|||------|||------||-----||--------||-------");

	int i;

	for (i = 0; i < MAX_REGISTERS; ++i)
	{
		ullong en        = getbits64(f->regs[i], 63, 63);
		ullong idd_div   = getbits64(f->regs[i], 31, 30);
		ullong idd_value = getbits64(f->regs[i], 29, 22);
		double idd_core  = ((double) idd_value) / pow(10.0, (double) idd_div);
		ullong vdd_cpu   = getbits64(f->regs[i], 21, 14);
		//double pow_core  = 1.350 - (((double) CpuVid) * 0.00625);
		ullong freq_fid  = getbits64(f->regs[i],  7,  0);
		ullong freq_did  = getbits64(f->regs[i], 13,  8);
		ullong freq_cof  = 0;

		if (freq_did > 0) {
			freq_cof = (freq_fid * 200LLU) / freq_did;
		}

		tprintf("%d||%LX||%llu||%llu||%llu||%0.2lf|||%llu|||%llu||* 200||/ %llu||= %llu",
				i, f->regs[i], en, idd_div, idd_value, idd_core, vdd_cpu, freq_fid, freq_did, freq_cof);
	}
	debug ("-----------------------------------------------------------------------------------");
}
#endif

static state_t pstate_build_psss(amd17_ctx_t *f)
{
	ullong did[31] = {  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
					   18, 19, 20, 21, 22, 23, 24, 25, 26, 28,
					   30, 32, 34, 36, 38, 40, 42, 44 };
	ullong p, m, d;
	ullong near;
	ullong diff;
	int i;

	// Getting P0 information
	f->psss[0].fid = getbits64(f->regs[0],  7,  0);
	f->psss[0].did = getbits64(f->regs[0], 13,  8);
	
	if (f->psss[0].did == 0) {
		return_msg(EAR_ERROR, "the frequency divisor is 0");
	}

	// Getting the P0(b) Core Frequency
	f->psss[0].cof = (f->psss[0].fid * 200LLU) / f->psss[0].did;

	// Getting P0 in case boost is not enabled
	i = 1;
	
	if (f->boost_enabled) {
		f->psss[1].cof = f->psss[0].cof;
		f->psss[1].fid = f->psss[0].fid;
		f->psss[1].did = f->psss[0].did;
		i = 2;
	}

	// Getting P1 Core Frequency (intervals of 100 MHz)
	if ((f->psss[0].cof % 100LLU) != 0) {
		p = f->psss[0].cof + (100LLU - (f->psss[0].cof % 100LLU)) - 100LLU;
	} else {
		p = f->psss[0].cof - 100LLU;
	}

	// Getting PSS objects from P1 to P7
	for (; p >= 1000 && i < MAX_PSTATES; p -= 100, ++i) {
		near = UINT_MAX;
		
	// Maximum multipier (255) and minimum (16)
		for (m = 255; m >= 16; --m) {
			// Divisors
			for (d = 0; d < 31; ++d)
			{
				if (((m * 1000LLU) * 200LLU) == ((p * 1000LLU) * did[d]))
				{
					diff = (f->psss[0].did + f->psss[0].fid) - (did[d] + m);
					diff = diff * diff;

					if (diff < near) {
						f->psss[i].cof = p;
						f->psss[i].fid = m;
						f->psss[i].did = did[d];
					}
				}
			}
		}
	}
	f->pss_count = i;
	#if SHOW_DEBUGS
	debug("PSS object list: ");
	for (i = 0; i < f->pss_count; ++i) {
		debug("P%d: %llum * 200 / %llud = %llu MHz", i,
			  f->psss[i].fid, f->psss[i].did, f->psss[i].cof);
	}
	#endif
	return EAR_SUCCESS;
}

state_t pstate_amd17_init_user(amd17_ctx_t *f)
{
	ullong *freqs_available;
	// Initializing in user mode
	f->user_mode = 1;
	// In user mode the driver is the main actor
	if (xtate_fail(s, f->driver->get_available_list(&f->driver_c, &freqs_available, &f->pss_count))) {
		return static_dispose(f, 0, s, state_msg);
	}
	// Saving registers is not necessary because there is no writing
	#if SHOW_DEBUGS
	pstate_print(f);
	#endif
	// Boost enabled
	/sys/devices/system/cpu/cpu0/cpufreq
	f->boost_enabled
	// Building PSS object list
	if (xtate_fail(s, pstate_build_psss(f))) {
		return static_dispose(f, tp.cpu_count, s, state_msg);
	}
	//
	debug("num P_STATEs: %d", f->pss_count);
	debug("nominal P_STATE: P%d", f->pss_nominal);
	debug("boost enabled: %d", f->boost_enabled);
	f->init = 1;
	
	return EAR_SUCCESS;
}

state_t pstate_amd17_init(ctx_t *c, mgt_ps_driver_ops_t *ops_driver)
{
	amd17_ctx_t *f;
	ullong data;
	state_t s;
	int cpu;
	int i;

	debug("initializing AMD17 P_STATE control");
	//
	if (state_ok(static_init_test(c, &f))) {
		return_msg(EAR_ERROR, Generr.api_initialized);
	}
	// Context
    if ((c->context = calloc(1, sizeof(amd17_ctx_t))) == NULL) {
        return_msg(EAR_ERROR, strerror(errno));
    }
    f = (amd17_ctx_t *) c->context;
	// Init driver
	f->driver = ops_driver;
	if (xtate_fail(s, f->driver->init(&f->driver_c))) {
		return static_dispose(f, tp.cpu_count, s, state_msg);
	}
	// Opening MSRs (switch to user mode if permission denied)
	for (cpu = 0; cpu < tp.cpu_count; ++cpu) {
		if (xtate_fail(s, msr_open(tp.cpus[cpu].id))) {
			if (state_is(s, EAR_NO_PERMISSIONS)) {
				return pstate_amd17_init_user();
			}
			return static_dispose(f, cpu, s, state_msg);
		}
	}
	// Reading min and max P_STATEs
	if (xtate_fail(s, msr_read(tp.cpus[0].id, &data, sizeof(ullong), REG_LIMITS))) {
		return static_dispose(f, tp.cpu_count, s, state_msg);
	}
	// Find maximum and minimum P_STATE
	ullong pstate_max = getbits64(data, 2, 0); // P_STATE max ex: P0
	ullong pstate_min = getbits64(data, 6, 4); // P_STATE min ex: P7
	if (pstate_max > 1LLU || pstate_min < 1LLU) {
		return static_dispose(f, tp.cpu_count, s, "Incorrect P_STATE limits");
	}
	// Save registers configuration
	for (data = 0LLU, i = MAX_REGISTERS - 1; i >= 0; --i) {
		if (xtate_fail(s, msr_read(0, &f->regs[i], sizeof(ullong), REG_P0+((ullong) i)))) {
			return static_dispose(f, tp.cpu_count, s, state_msg);
		}
		if (getbits64(f->regs[i], 63, 63) == 1LLU) {
			data = f->regs[i];
		}
	}
	#if SHOW_DEBUGS
	pstate_print(f);
	#endif
	// Boost enabled
	if (xtate_fail(s, msr_read(0, &data, sizeof(ullong), REG_HWCONF))) {
		return static_dispose(f, tp.cpu_count, s, state_msg);
	}
	f->boost_enabled = (uint) !getbits64(data, 25, 25);
	f->pss_nominal   = f->boost_enabled;
	// Building PSS object list
	if (xtate_fail(s, pstate_build_psss(f))) {
		return static_dispose(f, tp.cpu_count, s, state_msg);
	}
	//
	debug("num P_STATEs: %d", f->pss_count);
	debug("nominal P_STATE: P%d", f->pss_nominal);
	debug("boost enabled: %d", f->boost_enabled);
	f->init = 1;

	return EAR_SUCCESS;
}

/** Getters */
state_t pstate_amd17_count(ctx_t *c, uint *pstate_count)
{
	amd17_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	*pstate_count = f->pss_count;
	return EAR_SUCCESS;
}

static state_t static_get_index(amd17_ctx_t *f, ullong freq_khz, uint *pstate_index, uint closest)
{
	ullong cof_khz;
    int pst;
	if (closest && freq_khz > (f->psss[f->pss_nominal].cof * 1000LU)) {
        *pstate_index = f->pss_nominal;
        return EAR_SUCCESS;
    } 
	for (pst = f->pss_nominal; pst < f->pss_count; ++pst)
	{
		cof_khz = f->psss[pst].cof * 1000LLU;
		if (cof_khz == freq_khz) {
            *pstate_index = pst;
            return EAR_SUCCESS;
		}
		if (closest && cof_khz < freq_khz) {
			*pstate_index = pst-1;
            return EAR_SUCCESS;
		}
	}
	if (closest) {
		*pstate_index = f->pss_count-1;
		return EAR_SUCCESS;
	}
    return_msg(EAR_ERROR, "P_STATE not found");
}

state_t pstate_amd17_get_available_list(ctx_t *c, pstate_t *pstate_list, uint *pstate_count)
{
	amd17_ctx_t *f;
	state_t s;
	int i;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (f->pss_nominal) {
		pstate_list[0].idx = 0LLU;
		pstate_list[0].khz = (f->psss[0].cof * 1000LLU) + 1000LLU;
		pstate_list[0].mhz = (f->psss[0].cof + 1LLU);
	}
	for (i = f->pss_nominal; i < f->pss_count; ++i) {
		pstate_list[i].idx = (ullong) i;
		pstate_list[i].khz = f->psss[i].cof * 1000LLU;
		pstate_list[i].mhz = f->psss[i].cof;
	}
	if (pstate_count != NULL) {
		*pstate_count = f->pss_count;
	}
	return EAR_SUCCESS;
}

state_t pstate_amd17_get_current_list(ctx_t *c, pstate_t *pstate_list)
{
	ullong reg, fid, did, cof;
	amd17_ctx_t *f;
	uint governor;
	uint cpu, pst;
	state_t s;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	if (xtate_fail(s, f->driver->get_governor(&f->driver_c, &governor))) {
		return s;
	}
	for (cpu = 0; cpu < tp.cpu_count; ++cpu)
	{
		pstate_list[cpu].idx = f->pss_nominal;
		pstate_list[cpu].khz = f->psss[f->pss_nominal].cof * 1000LLU;
		pstate_list[cpu].mhz = f->psss[f->pss_nominal].cof;

		if (governor == Governor.userspace)
		{
			if (state_ok(msr_read(cpu, &reg, sizeof(ullong), REG_P1)))
			{
				fid = getbits64(reg, 7, 0);
				did = getbits64(reg, 13, 8);
				cof = 0LLU;

				if (did != 0)
				{
					cof = ((fid * 200LLU) / did) * 1000LLU;

					if (state_ok(static_get_index(f, cof, &pst, 0))) {
						pstate_list[cpu].idx = pst;
						pstate_list[cpu].khz = f->psss[pst].cof * 1000LLU;
						pstate_list[cpu].mhz = f->psss[pst].cof;
					}
				}
			}
		}
	}

	return EAR_SUCCESS;
}

state_t pstate_amd17_get_nominal(ctx_t *c, uint *pstate_index)
{
	amd17_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	*pstate_index = f->boost_enabled;
	return EAR_SUCCESS;
}

state_t pstate_amd17_get_governor(ctx_t *c, uint *governor)
{
	amd17_ctx_t *f;
	state_t s;
	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	return f->driver->get_governor(&f->driver_c, governor);
}

state_t pstate_amd17_get_index(ctx_t *c, ullong freq_khz, uint *pstate_index, uint closest)
{
    amd17_ctx_t *f;
    state_t s;

    if (xtate_fail(s, static_init_test(c, &f))) {
        return s;
    }
    return static_get_index(f, freq_khz, pstate_index, closest);
}

/** Setters */
static state_t set_frequency_step2(amd17_ctx_t *f, uint cpu, uint i)
{
	ullong reg;
	state_t s;

	if (cpu >= tp.cpu_count) {
		return_msg(EAR_ERROR, Generr.cpu_invalid);
	}

	reg = f->regs[0];
	reg = setbits64(reg, f->psss[i].fid,  7,  0);
	reg = setbits64(reg, f->psss[i].did, 13,  8);
	reg = setbits64(reg,           1LLU, 63, 63);

	if (xtate_fail(s, msr_write(cpu, &reg, sizeof(ullong), REG_P1))) {
		return s;
	}
	debug("edited CPU%d P1 MSR to (%llum * 200 / %llud) = %llu MHz",
		  cpu, f->psss[i].fid, f->psss[i].did, f->psss[i].cof);

	return EAR_SUCCESS;
}

static state_t set_frequency_step1(amd17_ctx_t *f)
{
	//const ullong *list;
	state_t s;
	//int i;

	// Call the driver to set userspace governor and P1
	debug("calling driver to set P1");

	#if 0
	if (xtate_fail(s, driver->frequencies_get_list(&driver_c, &list, &i))) {
		return s;
	}
	if (i == 0) {
		return_msg(s, "AMD family 17h requires at least 2 P_STATEs available");
	}
	#endif
	if (xtate_fail(s, f->driver->set_governor(&f->driver_c, Governor.userspace))) {
		return s;
	}
	if (xtate_fail(s, f->driver->set_current(&f->driver_c, 1, all_cpus))) {
		return s;
	}

	return EAR_SUCCESS;
}

state_t pstate_amd17_set_current_list(ctx_t *c, uint *pstate_index)
{
	state_t s2 = EAR_SUCCESS;
	state_t s1 = EAR_SUCCESS;
	amd17_ctx_t *f;
	uint cpu;

	if (xtate_fail(s1, static_init_test(c, &f))) {
		return s1;
	}
	if (xtate_fail(s1, set_frequency_step1(f))) {
		return s1;
	}
	for (cpu = 0; cpu < tp.cpu_count; ++cpu) {
		if (xtate_fail(s1, set_frequency_step2(f, cpu, pstate_index[cpu]))) {
			s2 = s1;
		}
	}
	return s2;
}

state_t pstate_amd17_set_current(ctx_t *c, uint pstate_index, int _cpu)
{
	state_t s1 = EAR_SUCCESS;
	state_t s2 = EAR_SUCCESS;
	amd17_ctx_t *f;
	uint cpu;

	if (xtate_fail(s1, static_init_test(c, &f))) {
		return s1;
	}
	if (xtate_fail(s1, set_frequency_step1(f))) {
		return s1;
	}
	if (_cpu != all_cpus) {
		return set_frequency_step2(f, (uint) _cpu, pstate_index);
	}
	for (cpu = 0; cpu < tp.cpu_count; ++cpu) {
		if (xtate_fail(s1, set_frequency_step2(f, cpu, pstate_index))) {
			s2 = s1;
		}
	}
	return s2;
}

state_t pstate_amd17_set_governor(ctx_t *c, uint governor)
{
	amd17_ctx_t *f;
	state_t s;
	int cpu;

	if (xtate_fail(s, static_init_test(c, &f))) {
		return s;
	}
	
	for (cpu = 0; cpu < tp.cpu_count; ++cpu) {
		if (xtate_fail(s, msr_write(cpu, &f->regs[1], sizeof(ullong), REG_P1))) {
			return s;
		}
	}
	if (xtate_fail(s, f->driver->set_governor(&f->driver_c, governor))) {
		return s;
	}

	return EAR_SUCCESS;
}
