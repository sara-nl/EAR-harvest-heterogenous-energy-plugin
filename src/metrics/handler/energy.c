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
#include <metrics/finder/energy.h>
#include <metrics/handler/energy.h>
#include <metrics/custom/hardware_info.h>

struct energy_op
{
	state_t (*init)               (void **c);
	state_t (*dispose)            (void **c);
	state_t (*datasize)           (size_t *size);
	state_t (*frequency)          (ulong *freq);
	state_t (*dc_read)            (void *c, edata_t emj);
	state_t (*dc_time_read)       (void *c, edata_t emj, ulong *tms);
	state_t (*ac_read)            (void *c, edata_t em);
	state_t (*units)							(uint *uints);
	state_t (*accumulated)				(ulong *e,edata_t init, edata_t end);
} energy_ops;
static char energy_manu[SZ_NAME_MEDIUM];
static char energy_prod[SZ_NAME_MEDIUM];
static char energy_objc[SZ_PATH];
static int  energy_loaded  = 0;
const int   energy_nops    = 9;
const char *energy_names[] = {
	"energy_init",
	"energy_dispose",
	"energy_datasize",
	"energy_frequency",
	"energy_dc_read",
	"energy_dc_time_read",
	"energy_ac_read",
	"energy_units",
	"energy_accumulated"
};

state_t energy_init(cluster_conf_t *conf, ehandler_t *eh)
{
	state_t ret;
	int cpu_model;
	int found;

	if (energy_loaded)
	{
		strcpy(eh->product, energy_prod);
                strcpy(eh->manufacturer, energy_manu);
		
		ret = energy_ops.init(&eh->context);
		debug("energy_ops.init() returned %d", ret);

		return ret;
	}

	if (conf == NULL) {
		state_return_msg(EAR_BAD_ARGUMENT, 0,
		 "the conf value cannot be NULL if the plugin is not loaded");
	}

	found = (strcmp(conf->install.obj_ener, "default") != 0);

	if (found)
	{
		sprintf(energy_objc, "%s/energy/%s",
			conf->install.dir_plug, conf->install.obj_ener);
		sprintf(energy_prod, "custom");
		sprintf(energy_manu, "custom");
	}
	else
	{
		// IPMI
		found = finder_energy(energy_manu, energy_prod);

		if (!found) {
			return EAR_NOT_FOUND;
		}

		//
		cpu_model = get_model();

		switch (cpu_model)
		{
			case CPU_HASWELL_X:
			case CPU_BROADWELL_X:
				if (strinc(energy_prod, "NX360")) {
					found = 0;
				} else {
					found = 0;
				}
				break;
			case CPU_SKYLAKE_X:
				if (strinc(energy_prod, "SD530")) {
					sprintf(energy_objc, "%s/energy/ipmi.node.manager.so",
							conf->install.dir_plug);
					found = 1;
				} else if (strinc(energy_prod, "SR650")) {
					found = 0;
				} else if (strinc(energy_prod, "SD650")) {
					found = 0;
				} else {
					found = 0;
				}
				break;
			default:
				break;
		}
		
		debug(0, "energy: detected product name '%s'", energy_prod);
		debug(0, "energy: detected manufacturer '%s'", energy_manu);
	}
	debug("loading shared object '%s'", energy_objc);

	if (found)
	{
		//
		ret = symplug_open(energy_objc, (void **) &energy_ops, energy_names, energy_nops);
		debug("symplug_open() returned %d (%s)", ret, intern_error_str);		

		if (state_fail(ret)) {
			return ret;
		}

		//
		energy_loaded = 1;
		ret = energy_init(conf, eh);
	} else {
		ret = EAR_NOT_FOUND;
	}

	return ret;
}

state_t energy_dispose(ehandler_t *eh)
{
	state_t s = EAR_SUCCESS;

	if (energy_ops.dispose != NULL) {
		s = energy_ops.dispose(&eh->context);
	}

	energy_handler_clean(eh);

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

state_t energy_handler_clean(ehandler_t *eh)
{
	memset(eh, 0, sizeof(ehandler_t));
	return EAR_SUCCESS;
}

state_t energy_datasize(ehandler_t *eh, size_t *size)
{
	preturn (energy_ops.datasize, size);
}

state_t energy_frequency(ehandler_t *eh, ulong *fus)
{
	int intents = 0;
	timestamp ts1;
	timestamp ts2;
	ulong e1;
	ulong e2;
	void *c;

	c = (void *) eh->context;

	// Dedicated frequency
	if (energy_ops.frequency != NULL) {
		return energy_ops.frequency ( fus);
	}

	if (energy_ops.dc_read == NULL) {
		return EAR_UNDEFINED;
	}

	// Generic frequency
	if (state_ok(energy_ops.dc_read(eh->context, &e1)))
	{
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

state_t energy_dc_read(ehandler_t *eh, edata_t emj)
{
	preturn (energy_ops.dc_read, eh->context, emj);
}

state_t energy_dc_time_read(ehandler_t *eh, edata_t emj, ulong *tms)
{
	preturn (energy_ops.dc_time_read, eh->context, emj, tms);
}

state_t energy_ac_read(ehandler_t *eh, edata_t emj)
{
	preturn (energy_ops.ac_read, eh->context, emj);
}

/* Energy units are 1=Joules, 1000=mJ, 1000000=uJ, 1000000000nJ */
state_t energy_units(ehandler_t *eh,uint *units)
{
  *units=1;
  preturn (energy_ops.units, units);
}

state_t energy_accumulated(ehandler_t *eh,unsigned long *e,edata_t init,edata_t end)
{
  *e=0;
  preturn (energy_ops.accumulated,e,init,end );
}

