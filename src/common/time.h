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

#ifndef EAR_COMMON_TIME_H
#define EAR_COMMON_TIME_H

#include <time.h>
#include <common/types/generic.h>

#define TIME_GET_S	1000000000
#define TIME_GET_MS	1000000
#define TIME_GET_US	1000
#define TIME_GET_NS	1

/* Returns a monotonic timestamp
 * 	- precision: ns
 * 	- resolution: 1ns
 */
ulong time_gettimestamp_precise(ulong time_unit);

/* Returns a monotonic timestamp
 * 	- precision: ns
 * 	- resolution: 1-4ms
 * 	- Performance: 5x faster than 'time_gettimestamp_precise'
 */
ulong time_gettimestamp_fast(ulong time_unit);

ulong time_contimestamp();

#endif //EAR_COMMON_TIME_H
