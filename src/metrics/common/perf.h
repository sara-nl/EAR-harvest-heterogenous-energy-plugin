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

#ifndef METRICS_COMMON_PERF_H
#define METRICS_COMMON_PERF_H

#include <common/types.h>
#include <common/states.h>
#include <linux/perf_event.h>

typedef struct perf_s {
	struct perf_event_attr attr;
	void *group;
	int fd;
} perf_t;

state_t perf_open(perf_t *perf, perf_t *group, pid_t pid, uint type, ulong event);

state_t perf_reset(perf_t *perf);

state_t perf_start(perf_t *perf);

state_t perf_stop(perf_t *perf);

state_t perf_close(perf_t *perf);

state_t perf_read(perf_t *perf, llong *value);

#endif //METRICS_COMMON_PERF_H