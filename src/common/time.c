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

ulong time_gettimestamp_precise(ulong time_unit)
{
	struct timespec ts;
	ulong stamp;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	stamp = ts.tv_sec * 1000000000;
	stamp = stamp + ts.tv_nsec;
	stamp = stamp / time_unit;

	return stamp;
}

ulong time_gettimestamp_fast(ulong time_unit)
{
	struct timespec ts;
	ulong stamp;

	clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
	stamp = ts.tv_sec * 1000000000;
	stamp = stamp + ts.tv_nsec;
	stamp = stamp / time_unit;

	return stamp;
}

int main(int argc, char *argv[])
{
	double fstamp;
	ulong stamp;
	ulong aux;
	int i;

	stamp = time_gettimestamp_precise(TIME_GET_US);
	for (i = 0; i < 120000000;++i) {
		aux = time_gettimestamp_precise(TIME_GET_S);
		aux = time_gettimestamp_precise(TIME_GET_S) - aux;
	}
	stamp = time_gettimestamp_precise(TIME_GET_US) - stamp;
	fstamp = (double) stamp / 1000000.0;
	printf("stamp p: %0.2lf\n", fstamp);
}