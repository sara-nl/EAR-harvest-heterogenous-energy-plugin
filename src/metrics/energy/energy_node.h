/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */



#ifndef METRICS_ENERGY_NODE_H
#define METRICS_ENERGY_NODE_H

#include <common/includes.h>
#include <common/system/time.h>
#include <metrics/accumulators/types.h>
#include <common/types/configuration/cluster_conf.h>

state_t energy_init(cluster_conf_t *conf, ehandler_t *eh);

state_t energy_dispose(ehandler_t *eh);

state_t energy_handler_clean(ehandler_t *eh);

state_t energy_datasize(ehandler_t *eh, size_t *size);

state_t energy_frequency(ehandler_t *eh, ulong *freq_us);

state_t energy_dc_read(ehandler_t *eh, edata_t  energy_mj);

state_t energy_dc_time_read(ehandler_t *eh, edata_t energy_mj, ulong *time_ms);

state_t energy_ac_read(ehandler_t *eh, edata_t energy_mj);

/* Energy units are 1=Joules, 1000=mJ, 1000000=uJ, 1000000000nJ */
state_t energy_units(ehandler_t *eh,uint *units);

state_t energy_accumulated(ehandler_t *eh,unsigned long *e,edata_t init,edata_t end);

state_t energy_to_str(ehandler_t *eh,char *str,edata_t e);

state_t energy_set_power_limit(ehandler_t *eh,ulong limit,ulong target);

#endif //EAR_ENERGY_H

