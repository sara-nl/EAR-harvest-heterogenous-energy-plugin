/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef METRICS_FLOPS_INTEL63
#define METRICS_FLOPS_INTEL63

#include <metrics/common/perf.h>
#include <metrics/flops/flops.h>

int init_flops_metrics();

void reset_flops_metrics();

void start_flops_metrics();

void read_flops_metrics(long long *total_flops, long long *f_operations);

void stop_flops_metrics(long long *total_flops, long long *f_operations);

int get_number_fops_events();

void get_total_fops(long long *metrics);

double gflops(ulong total_timei, uint total_cores);

void get_weigth_fops_instructions(int *weigth_vector);

void print_gflops(long long total_inst, ulong total_time, uint total_cores);

#endif
