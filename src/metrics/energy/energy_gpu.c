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

state_t energy_gpu_init(pcontext_t *c, gpu_power_t **dread, gpu_power_t **davrg, uint num_cpus)
{
	if (state_ok(nvsmi_gpu_status())) {
		return nvsmi_gpu_init(c, dread, davrg, num_cpus);
	} else {
		return EAR_INCOMPATIBLE;
	}
}

state_t energy_gpu_disclose(pcontext_t *c, gpu_power_t **dread, gpu_power_t **davrg)
{
	return nvsmi_gpu_disclose(c, dread, davrg);
}

state_t energy_gpu_read(pcontext_t *c, gpu_power_t *dread, gpu_power_t *davrg, uint num_gpus)
{
	return nvsmi_gpu_read(c, dread, davrg, num_gpus);
}