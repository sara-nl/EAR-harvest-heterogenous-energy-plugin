/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef METRICS_STALLS_H
#define METRICS_STALLS_H

int init_stall_metrics();

void reset_stall_metrics();

void start_stall_metrics();

void stop_stall_metrics(long long *stall_cycles);

void get_stall_metrics(long long *total_stall_cycles);

#endif //METRICS_STALLS_H