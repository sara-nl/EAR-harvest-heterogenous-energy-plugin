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

#ifndef _POWERCAP_STATUS_H
#define _POWERCAP_STATUS_H
#include <daemon/powercap.h>

/** Given a powercap settings and a given power consumption computes the power to be released */
uint compute_power_to_release(node_powercap_opt_t *pc_opt,uint current);

/** Given a powercap settings and a given power consumption computes the extra power to ask for */
uint compute_power_to_ask(node_powercap_opt_t *pc_opt,uint current);

/** Given a powercap settings and a given power consumption estimates if the application needs more power: GREEDY state */
uint more_power(node_powercap_opt_t *pc_opt,uint current);

/** Given a powercap settings and a given power consumption estimates if the application can release power :RELEASE state */
uint free_power(node_powercap_opt_t *pc_opt,uint current);

/** Given a powercap settings and a given power consumption estimates if the application is ok with the allocated power */
uint ok_power(node_powercap_opt_t *pc_opt,uint current);
#endif

