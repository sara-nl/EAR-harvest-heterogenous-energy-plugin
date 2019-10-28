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

#ifndef METRICS_ENERGY_NVSMI_GPU_H
#define METRICS_ENERGY_NVSMI_GPU_H

#include <metrics/energy/energy_gpu.h>

/** Initializes and allocates the GPU context (c) memory for the data readings (data_read).
  * Also the data average structure (data_avrg) is allocated if its pointer it is not NULL.
  * This data computes the average of the current read metric and the previous one, and also
  * computes the energy and time between two different readings. **/
state_t nvsmi_gpu_init(pcontext_t *c, gpu_power_t **data_read, gpu_power_t **data_avrg, uint num_cpus);

/** Frees the allocated memory of the context and data readings structures. **/
state_t nvsmi_gpu_disclose(pcontext_t *c, gpu_power_t **data_read, gpu_power_t **data_avrg);

/** Reads the GPU metrics and returns its values (data_read). If previous data reading is
  * given (data_read), the averages of the current reading and the previous is computed and
  * returned (data_avrg). **/
state_t nvsmi_gpu_read(pcontext_t *c, gpu_power_t *data_read, gpu_power_t *data_avrg, uint num_gpus);

/** **/
state_t nvsmi_gpu_status();

#endif // METRICS_ENERGY_NVSMI_GPU_H