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

#include <common/symplug.h>
#include <common/includes.h>
#include <metrics/custom/energy.h>
#include <metrics/custom/energy/finder.h>
#include <metrics/custom/hardware_info.h>

struct energy_op
{
	state_t (*init)            (void **c);
	state_t (*dispose)         (void **c);
	state_t (*data_length_get) (void *c, size_t *size);
	state_t (*dc_read)         (void *c, ulong *emj);
	state_t (*dc_time_read)    (void *c, ulong *emj, ulong *tms);
	state_t (*dc_time_debug)   (void *c, ulong *ej, ulong *emj, ulong *ts, ulong *tms);
	state_t (*ac_read)         (void *c, ulong *em);
} energy_ops;
const int   energy_nops    = 7;
const char *energy_names[] = {
	"plug_energy_init",
	"plug_energy_dispose",
	"plug_energy_data_length_get",
	"plug_energy_dc_read",
	"plug_energy_dc_time_read",
	"plug_energy_dc_time_debug",
	"plug_energy_ac_read",
};

state_t energy_init(ehandler_t *eh)
{
	int ipmi = 0;
	int cpu_model;
	state_t ret;

	if (eh->connected) {
		return eh->status;
	}

	debug("initialized");

	//
	energy_ops.init            = NULL;
	energy_ops.dispose         = NULL;
	energy_ops.data_length_get = NULL;
	energy_ops.dc_read         = NULL;
	energy_ops.dc_time_read    = NULL;
	energy_ops.dc_time_debug   = NULL;
	energy_ops.ac_read         = NULL;

	//
	cpu_model = get_model();

	// IPMI
	ret = finder_ipmi_get_product_name(eh->name_manufacturer, eh->name_product);

	// No IPMI
	if (ret < 0) {
		// Test others (PAPI, msr..?)
	}

	switch (cpu_model)
	{
		case CPU_HASWELL_X:
		case CPU_BROADWELL_X:
			if (strstr(eh->name_product, "nx360") != NULL) {
				eh->interface = 1;
			} else {
				eh->interface = 0;
			}
			break;
		case CPU_SKYLAKE_X:
			if (strstr(eh->name_product, "SD530") != NULL) {
				sprintf(eh->path_object, "./plugins/energy/ipmi.sd530.so");
				sprintf(eh->path_object, "./energy/ipmi.lenovo.so");
				eh->interface = 1;
			} else if (strstr(eh->name_product, "SR650") != NULL) {
				eh->interface = 1;
			} else if (strstr(eh->name_product, "SD650") != NULL) {
				eh->interface = 1;
			} else {
				eh->interface = 0;
			}
			break;
		default:
			break;
	}

	if (eh->interface) {
		verbose(0, "energy: product name '%s' detected", eh->name_product);

		//
		ret = symplug_open(eh->path_object, (void **) &energy_ops, energy_names, energy_nops);
		debug("symplug_open() returned %d", ret);		

		if (state_fail(ret)) {
			return ret;
		}

		ret = energy_ops.init(&eh->context);
		debug("energy_ops.init() returned %d", ret);		
	
		eh->connected = 1;
		eh->status    = ret;
	} else {
		verbose(0, "energy: product name '%s' detected (not known)", eh->name_product);
		eh->connected = 0;
		eh->status    = EAR_ERROR;
	}

	return eh->status;
}

state_t energy_dispose(ehandler_t *eh)
{
	preturn (energy_ops.dispose, &eh->context);
}

state_t energy_data_length_get(ehandler_t *eh, size_t *size)
{
	preturn (energy_ops.data_length_get, eh->context, size);
}

state_t energy_data_frequency_get(ehandler_t *eh, ulong *freq)
{
	return EAR_SUCCESS;
}

state_t energy_dc_read(ehandler_t *eh, ulong *emj)
{
	preturn (energy_ops.dc_read, eh->context, emj);
}

state_t energy_dc_readtry(ehandler_t *eh, ulong *emj)
{
	return EAR_SUCCESS;
}

state_t energy_dc_time_read(ehandler_t *eh, ulong *emj, ulong *tms)
{
	preturn (energy_ops.dc_time_read, eh->context, emj, tms);
}

state_t energy_dc_time_readtry(ehandler_t *eh, ulong *energy, ulong *tms)
{
	return EAR_SUCCESS;
}

state_t energy_dc_time_debug(ehandler_t *eh, ulong *ej, ulong *emj, ulong *ts, ulong *tms)
{
	preturn (energy_ops.dc_time_debug, eh->context, ej, emj, ts, tms);
}

state_t energy_dc_time_debugtry(ehandler_t *eh, ulong *ej, ulong *emj, ulong *ts, ulong *tms)
{
	return EAR_SUCCESS;
}

state_t energy_ac_read(ehandler_t *eh, ulong *emj)
{
	preturn (energy_ops.ac_read, eh->context, emj);
}
