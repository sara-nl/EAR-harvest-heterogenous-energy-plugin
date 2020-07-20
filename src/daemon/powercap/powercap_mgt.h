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

#ifndef _PWR_MGT_H
#define _PWR_MGT_H
#include <common/states.h>
#include <daemon/powercap/powercap_status_conf.h>

typedef void * pwr_mgt_t;



state_t pmgt_init();
state_t pmgt_enable(pwr_mgt_t *phandler);
state_t pmgt_disable(pwr_mgt_t *phandler);
state_t pmgt_handler_alloc(pwr_mgt_t **phandler);
state_t pmgt_disable_policy(pwr_mgt_t *phandler,uint pid);
state_t pmgt_disable_policies(pwr_mgt_t *phandler);
state_t pmgt_set_powercap_value(pwr_mgt_t *phandler,uint pid,uint domain,uint limit);
state_t pmgt_get_powercap_value(pwr_mgt_t *phandler,uint pid,uint *powercap);
uint pmgt_is_powercap_enabled(pwr_mgt_t *phandler,uint pid);
void pmgt_print_powercap_value(pwr_mgt_t *phandler,int fd);
void pmgt_powercap_to_str(pwr_mgt_t *phandler,char *b);
void pmgt_set_status(pwr_mgt_t *phandler,uint status);
uint pmgt_get_powercap_strategy(pwr_mgt_t *phandler);
void pmgt_set_pc_mode(pwr_mgt_t *phandler,uint mode);
void pmgt_set_power_per_domain(pwr_mgt_t *phandler,dom_power_t *pdomain,uint st);
void pmgt_set_app_req_freq(pwr_mgt_t *phandler,ulong f);
void pmgt_new_job(pwr_mgt_t *phandler);
void pmgt_end_job(pwr_mgt_t *phandler);
void pmgt_idle_to_run(pwr_mgt_t *phandler);
void pmgt_run_to_idle(pwr_mgt_t *phandler);
void pmgt_powercap_node_reallocation();


#endif
