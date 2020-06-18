/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef METRICS_CACHE_H
#define METRICS_CACHE_H

int init_cache_metrics();

void reset_cache_metrics();

void start_cache_metrics();

void stop_cache_metrics(long long *l1);

void print_cache_metrics(long long *L);

#endif //METRICS_CACHE_H
