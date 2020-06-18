/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef EAR_POLICIES_API_H
#define EAR_POLICIES_API_H

#include <common/states.h>
#include <library/policies/policy_ctx.h>
#include <common/types/configuration/policy_conf.h>
state_t policy_init(polctx_t *c);
state_t policy_apply(polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready);
state_t policy_get_default_freq(polctx_t *c, ulong *freq_set);
state_t policy_ok(polctx_t *c, signature_t *curr_sig,signature_t *prev_sig,int *ok);
state_t policy_max_tries(polctx_t *c,int *intents);
state_t policy_end(polctx_t *c);
state_t policy_loop_init(polctx_t *c,loop_id_t *loop_id);
state_t policy_loop_end(polctx_t *c,loop_id_t *loop_id);
state_t policy_new_iteration(polctx_t *c,loop_id_t *loop_id);
state_t policy_mpi_init(polctx_t *c);
state_t policy_mpi_end(polctx_t *c);
state_t policy_configure(polctx_t *c);
state_t policy_set_risk(policy_conf_t *ref,policy_conf_t *current,ulong risk_level,ulong opt_target,ulong mfreq,ulong *nfreq,ulong *f_list,uint nump);



#endif //EAR_POLICIES_API_H
