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

#ifndef _PWR_MGT_H
#define _PWR_MGT_H
#include <common/states.h>
#include <daemon/powercap_status_conf.h>

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
void pmgt_set_power_per_domain(pwr_mgt_t *phandler,dom_power_t *pdomain);

#endif
