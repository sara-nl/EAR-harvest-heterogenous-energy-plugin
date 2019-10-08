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

#include <common/config.h>
#include <common/includes.h>
#include <common/output/verbose.h>
#include <common/system/symplug.h>
#include <common/hardware/hardware_info.h>
#include <metrics/energy/energy_node.h>

struct energy_op
{
	state_t (*init)				(void **c);
	state_t (*dispose)			(void **c);
	state_t (*datasize)			(size_t *size);
	state_t (*frequency)		(ulong *freq);
	state_t (*dc_read)			(void *c, void *emj);
	state_t (*dc_time_read)		(void *c, void *emj, ulong *tms);
	state_t (*ac_read)			(void *c, void *em);
	state_t (*units)			(uint *uints);
	state_t (*accumulated)		(ulong *e, void *init, void *end);
	state_t (*energy_to_str)	(char *str, void *end);
} energy_ops;

static char energy_objc[SZ_PATH];
static int  energy_loaded  = 0;
const  int  energy_nops    = 10;
const char *energy_names[] = {
		"energy_init",
		"energy_dispose",
		"energy_datasize",
		"energy_frequency",
		"energy_dc_read",
		"energy_dc_time_read",
		"energy_ac_read",
		"energy_units",
		"energy_accumulated",
		"energy_to_str"
};

state_t energy_init(cluster_conf_t *conf, void **c)
{
	state_t ret;

	if (conf == NULL) {
		state_return_msg(EAR_BAD_ARGUMENT, 0,
			 "the conf value cannot be NULL if the plugin is not loaded");
	}

	sprintf(energy_objc, "%s/energy/%s",
			conf->install.dir_plug, conf->install.obj_ener);
	debug("using ear.conf energy plugin '%s'", conf->install.obj_ener);
	debug("loading shared object '%s'", energy_objc);

	//
	ret = symplug_open(energy_objc, (void **) &energy_ops, energy_names, energy_nops);

	if (state_fail(ret)) {
		debug("symplug_open() returned %d (%s)", ret, intern_error_str);
		return ret;
	}

	//
	energy_loaded = 1;

	if (energy_ops.init != NULL) {
		ret = energy_ops.init(c);
		return ret;
	}

	return EAR_ERROR;
}

state_t energy_dispose(void **c)
{
	state_t s = EAR_SUCCESS;

	if (energy_ops.dispose != NULL) {
		s = energy_ops.dispose(c);
	}

	energy_handler_clean(*c);

	// By now, libraries are not unloadable
#if 0
	energy_ops.init               = NULL;
	energy_ops.dispose            = NULL;
	energy_ops.data_length_get    = NULL;
	energy_ops.data_frequency_get = NULL;
	energy_ops.dc_read            = NULL;
	energy_ops.dc_time_read       = NULL;
	energy_ops.ac_read            = NULL;
#endif

	return s;
}

state_t energy_handler_clean(void *c)
{
	memset(c, 0, sizeof(void *));
	return EAR_SUCCESS;
}

state_t energy_datasize(void *c, size_t *size)
{
	preturn(energy_ops.datasize, size);
}

state_t energy_frequency(void *c, ulong *fus)
{
	int intents = 0;
	timestamp ts1;
	timestamp ts2;
	ulong e1;
	ulong e2;

	// Dedicated frequency
	if (energy_ops.frequency != NULL) {
		return energy_ops.frequency(fus);
	}

	if (energy_ops.dc_read == NULL) {
		return EAR_UNDEFINED;
	}

	// Generic frequency
	if (state_ok(energy_ops.dc_read(c, &e1))) {
		do {
			energy_ops.dc_read(c, &e2);
			intents++;
		} while ((e1 == e2) && (intents < 5000));

		//
		if (intents == 5000) {
			return 0;
		}

		timestamp_getprecise(&ts1);
		e1 = e2;

		do {
			energy_ops.dc_read(c, &e2);
		} while (e1 == e2);

		timestamp_getprecise(&ts2);
		*fus = (timestamp_diff(&ts2, &ts1, TIME_USECS) / 2) / MAX_POWER_ERROR;
	}

	return EAR_SUCCESS;
}

state_t energy_dc_read(void *c, void *emj)
{
	preturn(energy_ops.dc_read, c, emj);
}

state_t energy_dc_time_read(void *c, void *emj, ulong *tms)
{
	preturn(energy_ops.dc_time_read, c, emj, tms);
}

state_t energy_ac_read(void *c, void *emj)
{
	preturn(energy_ops.ac_read, c, emj);
}

/* Energy units are 1=Joules, 1000=mJ, 1000000=uJ, 1000000000nJ */
state_t energy_units(void *c, uint *units)
{
	*units = 1;
	preturn(energy_ops.units, units);
}

state_t energy_accumulated(void *c, ulong *e, void *init, void *end)
{
	*e = 0;
	preturn(energy_ops.accumulated, e, init, end);
}

state_t energy_to_str(void *c, char *str, void *e)
{
	preturn(energy_ops.energy_to_str, str, e);
}
