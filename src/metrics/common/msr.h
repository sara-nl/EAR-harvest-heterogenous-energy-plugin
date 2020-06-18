/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef EAR_MSR_H
#define EAR_MSR_H

#include <unistd.h>
#include <common/types.h>
#include <common/states.h>

#define msr_clean(fd) \
	*fd = -1

/* */
state_t msr_open(uint cpu);

/* */
state_t msr_close(uint cpu);

/* */
state_t msr_read(uint cpu, void *buffer, size_t count, off_t offset);

/* */
state_t msr_write(uint cpu, const void *buffer, size_t count, off_t offset);


#endif //EAR_MSR_H
