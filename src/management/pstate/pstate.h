/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#ifndef MANAGEMENT_PSTATE
#define MANAGEMENT_PSTATE

#include <common/states.h>
#include <common/plugins.h>
#include <common/hardware/topology.h>

typedef struct pstate_s {
	uint   idx;
	ullong khz;
	ullong mhz;
} pstate_t;

typedef struct mgt_ps_driver_ops_s
{
	state_t (*init)               (ctx_t *c);
	state_t (*dispose)            (ctx_t *c);
	state_t (*get_available_list) (ctx_t *c, const ullong **freq_list, uint *freq_count);
	state_t (*get_current_list)   (ctx_t *c, const ullong **freq_list);
	state_t (*get_governor)       (ctx_t *c, uint *governor);
	state_t (*set_current_list)   (ctx_t *c, uint *freq_index);
	state_t (*set_current)        (ctx_t *c, uint freq_index);
	state_t (*set_governor)       (ctx_t *c, uint governor);
} mgt_ps_driver_ops_t;

typedef struct mgt_ps_ops_s
{
	state_t (*init)                 (ctx_t *c, mgt_ps_driver_ops_t *driver_ops);
	state_t (*dispose)              (ctx_t *c);
	state_t (*count)                (ctx_t *c, uint *pstate_count);
	state_t (*get_available_list)   (ctx_t *c, pstate_t *pstate_list, uint *pstate_count);
	state_t (*get_current_list)     (ctx_t *c, pstate_t *pstate_list);
	state_t (*get_nominal)          (ctx_t *c, uint *pstate_index);
	state_t (*get_governor)         (ctx_t *c, uint *governor);
	state_t (*get_index)            (ctx_t *c, ullong freq_khz, uint *pstate_index, uint closest);
	state_t (*set_current_list)     (ctx_t *c, uint *pstate_index);
	state_t (*set_current)          (ctx_t *c, uint pstate_index);
	state_t (*set_governor)         (ctx_t *c, uint governor);
} mgt_ps_ops_t;

struct governor_s {
	uint conservative;
	uint performance;
    uint userspace;
    uint powersave;
    uint ondemand;
    uint other;
	uint last;
	uint init;
} Governor __attribute__((weak)) = {
	.conservative = 0,
	.performance = 1,
    .userspace = 2,
	.powersave = 3,
	.ondemand = 4,
	.other = 5,
    .last = 6,
	.init = 7,
};

struct goverstr_s {
	char *conservative;
	char *performance;
	char *userspace;
	char *powersave;
	char *ondemand;
	char *other;
} Goverstr __attribute__((weak)) = {
	.conservative = "conservative",
	.performance = "performance",
	.userspace = "userspace",
	.powersave = "powersave",
	.ondemand = "ondemand",
	.other = "other",
};

state_t mgt_pstate_load(topology_t *tp);

state_t mgt_pstate_init(ctx_t *c);

state_t mgt_pstate_dispose(ctx_t *c);

/** Getters */
//Counts the available P_STATEs.
state_t mgt_pstate_count(ctx_t *c, uint *pstate_count);

//Returns the available P_STATE (struct) list. The allocated list depends on the user.
state_t mgt_pstate_get_available_list(ctx_t *c, pstate_t *pstate_list, uint *pstate_count);

//Returns the current P_STATE (struct) per CPU.
state_t mgt_pstate_get_current_list(ctx_t *c, pstate_t *pstate_list);

//Returns the nominal P_STATE index.
state_t mgt_pstate_get_nominal(ctx_t *c, uint *pstate_index);

//
state_t mgt_pstate_get_governor(ctx_t *c, uint *governor);

//Given a frequency in KHz, returns its available P_STATE index.
state_t mgt_pstate_get_index(ctx_t *c, ullong freq_khz, uint *pstate_index, uint closest);

/** Setters */
//Sets a P_STATE and userspace governor per CPU. The allocated list depends on the user.
state_t mgt_pstate_set_current_list(ctx_t *c, uint *pstate_index);

// Set a P_STATE in all CPUs. The  allocated list depends on the user.
state_t mgt_pstate_set_current(ctx_t *c, uint pstate_index);

// Sets the governor (take a look to Governor global variable.
state_t mgt_pstate_set_governor(ctx_t *c, uint governor);

/** Helpers. */
state_t mgt_pstate_governor_tostr(uint governor, char *buffer);

state_t mgt_pstate_governor_toint(char *buffer, uint *governor);

#endif //MANAGEMENT_PSTATE
