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

//#define SHOW_DEBUGS 1
#include <common/output/debug.h>
#include <metrics/flops/cpu/intel63.h>

static perf_t perf_064f;
static perf_t perf_064d;
static perf_t perf_128f;
static perf_t perf_128d;
static perf_t perf_256f;
static perf_t perf_256d;
static perf_t perf_512f;
static perf_t perf_512d;

static llong values_064f;
static llong values_064d;
static llong values_128f;
static llong values_128d;
static llong values_256f;
static llong values_256d;
static llong values_512f;
static llong values_512d;

static llong accum_064f;
static llong accum_064d;
static llong accum_128f;
static llong accum_128d;
static llong accum_256f;
static llong accum_256d;
static llong accum_512f;
static llong accum_512d;

int init_flops_metrics()
{
	state_t s;

	// Intel's Manual Volume 3 (FULL), look for 'FP_ARITH_INST'.
	s = perf_open(&perf_064f, &perf_064f, 0, PERF_TYPE_RAW, 0x02c7);
	s = perf_open(&perf_064d, &perf_064f, 0, PERF_TYPE_RAW, 0x01c7);
	s = perf_open(&perf_128f, &perf_064f, 0, PERF_TYPE_RAW, 0x08c7);
	s = perf_open(&perf_128d, &perf_064f, 0, PERF_TYPE_RAW, 0x04c7);
	s = perf_open(&perf_256f, &perf_256f, 0, PERF_TYPE_RAW, 0x20c7);
	s = perf_open(&perf_256d, &perf_256f, 0, PERF_TYPE_RAW, 0x10c7);
	s = perf_open(&perf_512f, &perf_256f, 0, PERF_TYPE_RAW, 0x80c7);
	s = perf_open(&perf_512d, &perf_256f, 0, PERF_TYPE_RAW, 0x40c7);
	
	// Remove warning
	(void) (s);

	return 1;
}

void reset_flops_metrics()
{
	state_t s;

	s = perf_reset(&perf_064f);
	s = perf_reset(&perf_256f);
	
	// Remove warning
	(void) (s);
}

void start_flops_metrics()
{
	state_t s;

	s = perf_start(&perf_064f);
	s = perf_start(&perf_256f);
	
	// Remove warning
	(void) (s);
}

void read_flops_metrics(long long *total_flops, long long *f_operations)
{
	state_t s;

	s = perf_read(&perf_064f, &values_064f);
	s = perf_read(&perf_256f, &values_256f);
	
	// Remove warning
	(void) (s);

	debug("read 064 f/d %lld/%lld", values_064f, values_064d);
	debug("read 128 f/d %lld/%lld", values_128f, values_128d);
	debug("read 256 f/d %lld/%lld", values_256f, values_256d);
	debug("read 512 f/d %lld/%lld", values_512f, values_512d);

	accum_064f += values_064f;
	accum_064d += values_064d;
	accum_128f += values_128f;
	accum_128d += values_128d;
	accum_256f += values_256f;
	accum_256d += values_256d;
	accum_512f += values_512f;
	accum_512d += values_512d;

	f_operations[0] = values_064f;
	f_operations[1] = values_064d;
	f_operations[2] = values_128f;
	f_operations[3] = values_128d;
	f_operations[4] = values_256f;
	f_operations[5] = values_256d;
	f_operations[6] = values_512f;
	f_operations[7] = values_512d;

	*total_flops  = 0;
	*total_flops += (accum_064f * 2);
	*total_flops += (accum_064d * 1);
	*total_flops += (accum_128f * 4);
	*total_flops += (accum_128d * 2);
	*total_flops += (accum_256f * 8);
	*total_flops += (accum_256d * 4);
	*total_flops += (accum_512f * 16);
	*total_flops += (accum_512d * 8);
	
	debug("total flops %lld", *total_flops);
}

void stop_flops_metrics(long long *total_flops, long long *f_operations)
{
	state_t s;

	s = perf_stop(&perf_064f);
	s = perf_stop(&perf_256f);
	
	// Remove warning
	(void) (s);

	read_flops_metrics(total_flops, f_operations);
}

int get_number_fops_events()
{
	return 8;
}

void get_total_fops(long long *metrics)
{
	metrics[0] = accum_064f;
	metrics[1] = accum_064d;
	metrics[2] = accum_128f;
	metrics[3] = accum_128d;
	metrics[4] = accum_256f;
	metrics[5] = accum_256d;
	metrics[6] = accum_512f;
	metrics[7] = accum_512d;
}

double gflops(ulong total_time, uint total_cores)
{
	double gflops;
	llong total;

	total  = 0;
	total += accum_064f * 2;
	total += accum_064d * 1;
	total += accum_128f * 4;
	total += accum_128d * 2;
	total += accum_256f * 8;
	total += accum_256d * 4;
	total += accum_512f * 16;
	total += accum_512d * 8;

	gflops = (double)(total*total_cores)/(double)(total_time*1000);
	return gflops;
}

void get_weigth_fops_instructions(int *weigth_vector)
{
	 weigth_vector[0] = 2;
	 weigth_vector[1] = 1;
	 weigth_vector[2] = 4;
	 weigth_vector[3] = 2;
	 weigth_vector[4] = 8;
	 weigth_vector[5] = 4;
	 weigth_vector[6] = 16;
	 weigth_vector[7] = 8;
}

void print_gflops(long long total_inst, ulong total_time, uint total_cores)
{
	printf("print_gflops\n");
}
