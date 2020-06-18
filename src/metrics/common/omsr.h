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
#include <common/states.h>
#include <common/types/generic.h>

/* */

#define MAX_PACKAGES 16
#define NUM_SOCKETS 2

state_t omsr_open(uint cpu, int *fd);

/* */
state_t omsr_close(int *fd);

/* */
state_t omsr_read(int *fd, void *buffer, size_t count, off_t offset);

/* */
state_t omsr_write(int *fd, const void *buffer, size_t count, off_t offset);


int get_msr_ids(int *dest_fd_map);
int get_total_packages();
int is_msr_initialized();
int init_msr(int *dest_fd_map);

#endif //EAR_MSR_H
