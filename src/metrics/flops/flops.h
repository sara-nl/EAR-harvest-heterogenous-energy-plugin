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

#ifndef METRICS_FLOPS_H
#define METRICS_FLOPS_H

#include <common/states.h>
#include <common/plugins.h>
#include <common/hardware/topology.h>

#define INDEX_064F 0
#define INDEX_064D 4
#define INDEX_128F 1
#define INDEX_128D 5
#define INDEX_256F 2
#define INDEX_256D 6
#define INDEX_512F 3
#define INDEX_512D 7



int init_flops_metrics();

void reset_flops_metrics();

void start_flops_metrics();

void read_flops_metrics(llong *flops, llong *ops);

void stop_flops_metrics(llong *flops, llong *ops);

int get_number_fops_events();

void get_total_fops(llong *metrics);

void get_weigth_fops_instructions(int *weigths);

#endif
