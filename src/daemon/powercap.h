/**************************************************************
 * * Energy Aware Runtime (EAR)
 * * This program is part of the Energy Aware Runtime (EAR).
 * *
 * * EAR provides a dynamic, transparent and ligth-weigth solution for
 * * Energy management.
 * *
 * *     It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017  
 * * BSC Contact   mailto:ear-support@bsc.es
 * * Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * * EAR is free software; you can redistribute it and/or
 * * modify it under the terms of the GNU Lesser General Public
 * * License as published by the Free Software Foundation; either
 * * version 2.1 of the License, or (at your option) any later version.
 * * 
 * * EAR is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * * Lesser General Public License for more details.
 * * 
 * * You should have received a copy of the GNU Lesser General Public
 * * License along with EAR; if not, write to the Free Software
 * * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * * The GNU LEsser General Public License is contained in the file COPYING  
 * */

#ifndef _EAR_POWERCAP_H
#define _EAR_POWERCAP_H
#include <pthread.h>
#include <daemon/powercap_status_conf.h>



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


#endif
