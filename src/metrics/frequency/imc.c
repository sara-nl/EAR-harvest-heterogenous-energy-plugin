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

#include <metrics/frequency/imc.h>
#include <metrics/frequency/imc/dummy.h>
#include <metrics/frequency/imc/intel63.h>

#define opreturn(call, ...) \
	if (call == NULL) { \
		return_msg(EAR_UNDEFINED, Generr.api_undefined); \
	} \
	return call (__VA_ARGS__);

static struct imc_freq_ops
{
	state_t (*init) (topology_t *tp);
	state_t (*dispose) ();
	state_t (*read) (freq_imc_t *ef);
	state_t (*read_diff) (freq_imc_t *ef2, freq_imc_t *ef1, ulong *freqs, ulong *average);
	state_t (*read_copy) (freq_imc_t *ef2, freq_imc_t *ef1, ulong *freqs, ulong *average);
	state_t (*data_alloc) (freq_imc_t *ef, ulong *freqs[], ulong *freqs_count);
	state_t (*data_count) (uint *count);
	state_t (*data_copy) (freq_imc_t *ef_dst, freq_imc_t *ef_src);
	state_t (*data_free) (freq_imc_t *ef, ulong *freqs[]);
	state_t (*data_diff) (freq_imc_t *ef2, freq_imc_t *ef1, ulong *freqs, ulong *average);
	state_t (*data_print) (ulong *freqs, ulong *average);
} ops;

state_t freq_imc_init(topology_t *tp)
{
	if (state_ok(ifreq_intel63_status(tp)))
	{
		ops.init		= ifreq_intel63_init;
		ops.dispose		= ifreq_intel63_dispose;
		ops.read		= ifreq_intel63_read;
		ops.read_diff	= ifreq_intel63_read_diff;
		ops.read_copy	= ifreq_intel63_read_copy;
		ops.data_alloc	= ifreq_intel63_data_alloc;
		ops.data_count	= ifreq_intel63_data_count;
		ops.data_copy	= ifreq_intel63_data_copy;
		ops.data_free   = ifreq_intel63_data_free;
		ops.data_diff   = ifreq_intel63_data_diff;
		ops.data_print  = ifreq_intel63_data_print;
	} else {
		ops.init		= ifreq_dummy_init;
		ops.dispose		= ifreq_dummy_dispose;
		ops.read		= ifreq_dummy_read;
		ops.read_diff	= ifreq_dummy_read_diff;
		ops.read_copy	= ifreq_dummy_read_copy;
		ops.data_alloc	= ifreq_dummy_data_alloc;
		ops.data_count	= ifreq_dummy_data_count;
		ops.data_copy	= ifreq_dummy_data_copy;
		ops.data_free   = ifreq_dummy_data_free;
		ops.data_diff   = ifreq_dummy_data_diff;
		ops.data_print  = ifreq_dummy_data_print;
	}

	return ops.init(tp);
}

state_t freq_imc_dispose()
{
	opreturn(ops.dispose,);
}

state_t freq_imc_read(freq_imc_t *ef)
{
	opreturn(ops.read, ef);
}

state_t freq_imc_read_diff(freq_imc_t *ef2, freq_imc_t *ef1, ulong *freqs, ulong *average)
{
	opreturn(ops.read_diff, ef2, ef1, freqs, average);
}

state_t freq_imc_read_copy(freq_imc_t *ef2, freq_imc_t *ef1, ulong *freqs, ulong *average)
{
	opreturn(ops.read_copy, ef2, ef1, freqs, average);
}

state_t freq_imc_data_alloc(freq_imc_t *ef, ulong *freqs[], ulong *freqs_count)
{
	opreturn(ops.data_alloc, ef, freqs, freqs_count);
}

state_t freq_imc_data_count(uint *count)
{
	opreturn(ops.data_count, count);
}

state_t freq_imc_data_copy(freq_imc_t *ef_dst, freq_imc_t *ef_src)
{
	opreturn(ops.data_copy, ef_dst, ef_src);
}

state_t freq_imc_data_free(freq_imc_t *ef, ulong *freqs[])
{
	opreturn(ops.data_free, ef, freqs);
}

state_t freq_imc_data_diff(freq_imc_t *ef2, freq_imc_t *ef1, ulong *freqs, ulong *average)
{
	opreturn(ops.data_diff,  ef2, ef1, freqs, average);
}

state_t freq_imc_data_print(ulong *freqs, ulong *average)
{
	opreturn(ops.data_print, freqs, average);
}

/*
 *
 *
 *
 */

#define U_MSR_UNCORE_RATIO_LIMIT	0x000620
#define U_MSR_UNCORE_RL_MASK_MAX	0x00007F
#define U_MSR_UNCORE_RL_MASK_MIN	0x007F00

state_t mgt_imcfreq_set_current(ctx_t *c, ulong max_khz, ulong min_khz)
{
	off_t address = U_MSR_UNCORE_RATIO_LIMIT;
	uint64_t set0 = 0;
	uint64_t set1 = 0;
	state_t r;
	int i;

	max_khz = max_khz / 100000LU;
	min_khz = min_khz / 100000LU;

	for (i = 0; i < 1; ++i)
	{
		set0 = (min_khz << 8) & U_MSR_UNCORE_RL_MASK_MIN;
		set1 = (max_khz << 0) & U_MSR_UNCORE_RL_MASK_MAX;
		set0 = set0 | set1;

		if ((r = msr_write(0, &set0, sizeof(uint64_t), address)) != EAR_SUCCESS) {
			return r;
		}
	}

	return EAR_SUCCESS;
}

state_t mgt_imcfreq_set_current(ctx_t *c, ulong *max_khz, ulong *min_khz)
{
	off_t address = U_MSR_UNCORE_RATIO_LIMIT;
	uint64_t result = 0;
	state_t r;
	int i;

	for (i = 0; i < 1; ++i)
	{
		// Read
		if ((r = msr_read(0, &result, sizeof(uint64_t), address)) != EAR_SUCCESS) {
			return r;
		}

		*max_mhz = (result & U_MSR_UNCORE_RL_MASK_MAX) >> 0;
		*min_mhz = (result & U_MSR_UNCORE_RL_MASK_MIN) >> 8;
		*max_khz = max_khz * 100000LU;
		*min_mhz = min_mhz * 100000LU;
	}

	return EAR_SUCCESS;
}