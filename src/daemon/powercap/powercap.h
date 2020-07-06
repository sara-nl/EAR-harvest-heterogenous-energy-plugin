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

#ifndef _EAR_POWERCAP_H
#define _EAR_POWERCAP_H
#include <pthread.h>
#include <daemon/powercap/powercap_status_conf.h>



int powercap_init();
void powercap_end();
void get_powercap_status(powercap_status_t *my_status);
void set_powercap_opt(powercap_opt_t *opt);
uint get_powercap_value();
int set_powercap_value(uint domain,uint limit);
int is_powercap_set();
void print_powercap_opt(powercap_opt_t *opt);
int powercap_idle_to_run();
int powercap_run_to_idle();
int powercap_init();
int periodic_metric_info(dom_power_t *pdomain,uint use_earl,ulong eff_f);
int is_powercap_set();
int is_powercap_on();

void set_powercapstatus_mode(uint mode);

void copy_node_powercap_opt(node_powercap_opt_t *dst);
uint powercap_get_strategy();
void powercap_set_app_req_freq(ulong f);
void powercap_release_idle_power(pc_release_data_t *release);
void powercap_set_default();


#endif
