/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*   
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*   
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING  
*/


#ifndef CLUSTER_POWERCAP_H
#define CLUSTER_POWERCAP_H
#include <daemon/eard_rapi.h>


typedef struct cluster_powercap_status{
  uint idle_nodes;         /* Total number of idle nodes */
  uint released;       /* Accumulated released power in last T1 */
  uint requested;       /* Asking for default power */
  uint num_greedy;             /* Number of greedy nodes */
  int *greedy_nodes;           /* List of greedy nodes */
  uint *greedy_req;
  uint *extra_power;
  uint current_power;         /* Accumulated power */
  uint total_powercap;        /* Accumulated current powercap limits */
}cluster_powercap_status_t;

void aggregate_powercap_status(powercap_status_t *my_cluster_power_status,int num_st,cluster_powercap_status_t *cluster_status);
void allocate_free_power_to_greedy_nodes(cluster_powercap_status_t *cluster_status,powercap_opt_t *cluster_options,uint *total_free);
void reduce_allocation(cluster_powercap_status_t *cluster_status,powercap_opt_t *cluster_options,uint min_reduction);
void powercap_reallocation(cluster_powercap_status_t *cluster_status,powercap_opt_t *cluster_options,int nun_nodes);
void send_powercap_options_to_cluster(powercap_opt_t *cluster_options);
void print_cluster_power_status(int num_power_status,powercap_status_t *my_cluster_power_status);
void cluster_powercap_init();
int cluster_power_limited();
void cluster_check_powercap();
#endif
