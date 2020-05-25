/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/config.h>
#include <common/states.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <daemon/eard_rapi.h>
#include <common/types/configuration/cluster_conf.h>


#ifndef _ENERGY_CLUSTER_STATUS_H
#define _ENERGY_CLUSTER_STATUS_H

#define EARGM_NO_PROBLEM  3
#define EARGM_WARNING1  2
#define EARGM_WARNING2  1
#define EARGM_PANIC   0


typedef struct node_info{
	uint dist_pstate;
	int ip;
	float power_red;
	uint victim;
}node_info_t;

state_t get_nodes_status(cluster_conf_t my_cluster_conf,uint *nnodes,node_info_t **einfo);
void manage_warning(risk_t * risk,uint level,cluster_conf_t my_cluster_conf,float target);
void create_risk(risk_t *my_risk,int wl);


#endif


