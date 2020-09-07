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

#include <stdio.h>
#include <unistd.h>
#include <common/system/time.h>

void timestamp_get(timestamp *ts)
{
	timestamp_getfast(ts);
}

void timestamp_getprecise(timestamp *ts)
{
	clock_gettime(CLOCK_MONOTONIC, ts);
}

void timestamp_getfast(timestamp *ts)
{
	clock_gettime(CLOCK_MONOTONIC_COARSE, ts);
}

void timestamp_getreal(timestamp *ts)
{
	clock_gettime(CLOCK_REALTIME_COARSE, ts);
}

ullong timestamp_convert(timestamp *ts, ullong time_unit)
{
	ullong stamp;
	stamp  = (ullong) (ts->tv_sec * 1000000000);
	stamp += (ullong) (ts->tv_nsec);
	stamp /= time_unit;
	return stamp;
}

ullong timestamp_diff(timestamp *ts2, timestamp *ts1, ullong time_unit)
{
	ullong stamp;

	if (ts2->tv_nsec < ts1->tv_nsec) {
		ts2->tv_sec   = ts2->tv_sec - 1;
		ts2->tv_nsec += 1000000000;
	}

	stamp  = (ullong) ((ts2->tv_sec - ts1->tv_sec) * 1000000000);
	stamp += (ullong) ((ts2->tv_nsec - ts1->tv_nsec));
	stamp /= time_unit;

	return stamp;
}

ullong timestamp_getconvert(ullong time_unit)
{
	timestamp_t ts;
	timestamp_getfast(&ts);
	return timestamp_convert(&ts, time_unit);
}

void timestamp_revert(timestamp *ts, ullong *tr, ullong time_unit)
{
    ullong aux_ns = (*tr) * time_unit;
    ts->tv_sec    = aux_ns / 1000000000; 
    ts->tv_nsec   = aux_ns - (ts->tv_sec * 1000000000);
}

void print_timestamp(timestamp *ts)
{
	fprintf(stdout,"Secs %ld nsec %ld\n", ts->tv_sec,ts->tv_nsec);
}
