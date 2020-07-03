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

/*
 * Usage:
 * 1) Call init passing the processor model of clusters
 * nodes to initialize the uncores scan and allocate file
 * descriptors memory.
 * 2) Call reset and start when you want to begin to count
 * the bandwith for a period of time.
 * 3) Call stop passing an array of unsigned long long
 * to also get the final uncore counters values.
 * 4) Repeat steps 2 and 3 every time you need to obtain
 * counters values.
 * 4) Call dispose to close the file descriptors and also
 * free it's allocated memory.
 *
 * When an error occurs, those calls returns -1.
 */

#include <stdio.h>
#include <string.h>
#include <common/config.h>
#include <common/output/verbose.h>
#include <common/math_operations.h>
#include <common/hardware/hardware_info.h>
#include <metrics/bandwidth/cpu/intel_haswell.h>

struct uncore_op {
	state_t (*init)		(ctx_t *c, topology_t *tp);
	state_t (*count)	(ctx_t *c, uint *count);
	state_t (*check)	(ctx_t *c);
	state_t (*reset)	(ctx_t *c);
	state_t (*start)	(ctx_t *c);
	state_t (*stop)		(ctx_t *c, ullong *cas);
	state_t (*read)		(ctx_t *c, ullong *cas);
	state_t (*dispose)	(ctx_t *c);
} ops;

// Depending on the architecture delivered by cpu_model variable,
// the pmons structure would point to its proper reading functions.
int init_uncores(int cpu_model)
{
	debug("init_uncores");

	if (vendor == VENDOR_AMD)
	{
		switch (cpu_model)
		{
			case CPU_HASWELL_X:
			case CPU_BROADWELL_X:
			case CPU_SKYLAKE_X:
				ops.init  = pci_init_uncores;
				ops.count = pci_count_uncores;
				ops.check = NULL;
				ops.reset = pci_reset_uncores;
				ops.start = pci_start_uncores;
				ops.stop  = pci_stop_uncores;
				ops.read  = pci_read_uncores;
				ops.dispose = pci_dispose_uncores;
			break;
		}
	} else if (vendor == VENDOR_AMD)
	{
		if (family == FAMILY_ZEN) {
			ops.init    = bwidth_amd23_init;
			ops.count   = bwidth_amd23_count;
			ops.check   = NULL;
			ops.reset   = bwidth_amd23_reset;
			ops.start   = bwidth_amd23_start;
			ops.stop    = bwidth_amd23_stop;
			ops.read    = bwidth_amd23_read;
			ops.dispose = bwidth_amd23_dispose;
		}
	}

#if 0
	if (ops.init == NULL) {
		ops.init    = bwidth_dummy_init;
		ops.count   = bwidth_dummy_count;
		ops.check   = NULL;
		ops.reset   = bwidth_dummy_reset;
		ops.start   = bwidth_dummy_start;
		ops.stop    = bwidth_dummy_stop;
		ops.read    = bwidth_dummy_read;
		ops.dispose = bwidth_dummy_dispose;
	}
#endif

	if (state_ok(ops.init(cpu_model))) {
		return count_uncores();
	}
	return 0;
}

int count_uncores()
{
	int count = 0;
	if (ops.count != NULL) {
		ops.count(c, (uint *) &count);
	}
	return count;
	//preturn (ops.count, c);
}

int check_uncores()
{
	return EAR_SUCCESS;
	//preturn (ops.check, c);
}

int reset_uncores()
{
	preturn (ops.reset, c);
}

int start_uncores()
{
	preturn (ops.start, c);
}

int stop_uncores(ullong *values)
{
	preturn (ops.stop, c, values);
}

int read_uncores(ullong *values)
{
	preturn (ops.read, c, values);
}

int dispose_uncores()
{
	preturn (ops.dispose, c);
}