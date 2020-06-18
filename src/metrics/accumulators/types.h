/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef EAR_PRIVATE_TYPES_H
#define EAR_PRIVATE_TYPES_H

#include <common/sizes.h>
#include <metrics/energy/energy_cpu.h>

typedef void *edata_t;

typedef struct ehandler {
	char manufacturer[SZ_NAME_MEDIUM];
	char product[SZ_NAME_MEDIUM];
	void *context;
	int fds_rapl[MAX_PACKAGES];
} ehandler_t;

typedef long long rapl_data_t;
typedef edata_t node_data_t;

#endif //EAR_PRIVATE_TYPES_H

