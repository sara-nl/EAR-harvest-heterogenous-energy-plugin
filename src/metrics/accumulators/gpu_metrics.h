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

#ifndef ACCUMULATORS_GPU_METRICS_H
#define ACCUMULATORS_GPU_METRICS_H

#include <metrics/energy/energy_gpu.h>

state_t acc_gpu_metrics_init(pcontext_t *c);

state_t acc_gpu_metrics_disclose(pcontext_t *c);

state_t acc_gpu_metrics_count(pcontext_t *c, uint *count);

state_t acc_gpu_mertics_sample(pcontext_t *c, uint loop_ms);

state_t acc_gpu_metrics_read(pcontext_t *c, gpu_power_t *data_read);

state_t acc_gpu_metrics_data_alloc(pcontext_t *c, gpu_power_t **data_read);

state_t acc_gpu_metrics_data_free(pcontext_t *c, gpu_power_t **data_read);

state_t acc_gpu_metrics_data_diff(pcontext_t *c, gpu_power_t *data_read1, gpu_power_t *data_read2, gpu_power_t *data_avrg);

state_t acc_gpu_metrics_data_null(pcontext_t *c, gpu_power_t *data_read);

#endif // ACCUMULATORS_GPU_METRICS_H
