/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING
*/

#include <stdio.h>
#include <unistd.h>
#include <common/time.h>

void time_gettimestamp_precise(timestamp *ts)
{
	clock_gettime(CLOCK_MONOTONIC, spec);
}

void time_gettimestamp_fast(timestamp *ts)
{
	clock_gettime(CLOCK_MONOTONIC_COARSE, spec);
}

ulong time_gettimestamp_diff(timestamp *ts2, timestamp *ts1, ulong time_unit)
{
	ulong stamp;

	if (ts2->tv_nsec < ts1->tv_nsec) {
		ts2->tv_nsec += 1000000000;
	}

	stamp  = (ts2->tv_sec - ts1->tv_sec) * 1000000000;
	stamp += (ts2->tv_nsec - ts1->tv_nsec) / time_unit;
	return stamp;
}

int main(int argc, char *argv[])
{
	timestamp ts1;
	timestamp ts2;
	timestamp ts3;
	timestamp ts4;
	double fstamp;
	ulong stamp;
	ulong aux;
	int i;

	time_gettimespec_precise(&ts3);

	for (i = 0; i < 120000000;++i)
	{
		time_gettimespec_precise(&ts1);
		time_gettimespec_precise(&ts2);

		aux = time_difftimespec(&ts2, &ts1, TIME_GET_S);
	}

	time_gettimespec_precise(&ts4);
	stamp  = time_difftimespec(&ts2, &ts1, TIME_GET_S);

	fstamp = (double) stamp / 1000000.0;
	printf("stamp tp: %0.2lf\n", fstamp);
}