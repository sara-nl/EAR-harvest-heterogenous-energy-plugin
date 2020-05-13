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

#ifndef INM_COMMANDS_H
#define INM_COMMANDS_H
#include <common/states.h>


state_t inm_disable_powercap_policy(uint pid);
state_t inm_disable_powercap_policies();
state_t inm_enable_powercap_policies();
state_t inm_set_powercap_value(uint pid,uint domain,uint limit);
state_t inm_get_powercap_value(uint pid,uint *powercap);
uint inm_is_powercap_policy_enabled(uint pid);
void inm_print_powercap_value(int fd);
void inm_powercap_to_str(char *b);


state_t inm_enable();
state_t inm_disable();


void inm_set_status(uint status);
uint inm_get_powercap_stragetgy();
void inm_set_pc_mode(uint mode);



#endif
