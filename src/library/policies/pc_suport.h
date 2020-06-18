/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

#ifndef PC_SUPPORT_H
#define PC_SUPPORT_H
#include <daemon/powercap_status_conf.h>
#include <common/types/signature.h>
#if POWERCAP

ulong pc_support_adapt_freq(node_powercap_opt_t *pc,ulong f,signature_t *s);
void pc_support_compute_next_state(node_powercap_opt_t *pc,signature_t *s);

#endif
#endif

