/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef METRICS_TEMPERATURE_H
#define METRICS_TEMPERATURE_H

#define RAPL_TEMP_EVS 1
int init_temp_msr(int *fd);
int read_temp_msr(int *fd,unsigned long long *_values);
int read_temp_limit_msr(int *fds, unsigned long long *_values);
int reset_temp_limit_msr(int *fds);
#endif //METRICS_TEMPERATURE_H
