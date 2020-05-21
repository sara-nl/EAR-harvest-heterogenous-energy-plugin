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

#ifndef _PC_STATUS_H_
#define _PC_STATUS_H_

#define POWERCAP_IDLE_PERC 1

#define DOMAIN_NODE 0
#define DOMAIN_CPU 1
#define DOMAIN_DRAM     2
#define DOMAIN_GPU      3

#define NUM_DOMAINS 4


#define PC_STATUS_OK      0
#define PC_STATUS_GREEDY  1
#define PC_STATUS_RELEASE 2
#define PC_STATUS_ASK_DEF 3
#define PC_STATUS_IDLE    4
#define PC_STATUS_STOP		5
#define PC_STATUS_START 	6
#define PC_STATUS_RUN			7

#define PC_DVFS						50
#define PC_POWER					51


#define PC_MODE_LIMIT			200
#define PC_MODE_TARGET		201				

#define PC_STATUS_ERROR   100

#define AUTO_CONFIG 0
#define EARL_CONFIG 1

#define TEMP_NUM_NODES 10

typedef struct node_powercap_opt {
  pthread_mutex_t lock;
  uint def_powercap; /* powercap default value defined in ear.conf */
  uint powercap_idle; /* powercap to be applied at idle periods */
  uint current_pc;    /* Current limit */
  uint last_t1_allocated; /* Power allocated at last T1,guaranteed */
  uint released;      /* Power released since last T1 */
  uint max_node_power; /* Maximum node power */
  uint th_inc; /* Percentage to mark powercap_status as greedy */
  uint th_red; /* Percentage to mark powercap_status as released */
  uint th_release; /* Percentage of power to be released */
  uint powercap_status; /* Current status */
  uint max_inc_new_jobs; /* Max extra power for new jobs */
  uint requested; /* Extra power requested, used when node is greedy or powercap < def_powercap */
}node_powercap_opt_t;

typedef struct powercap_status{
	uint total_nodes;
	uint idle_nodes; 	        /* Total number of idle nodes */
	uint released;        		/* Accumulated released power in last T1 */
  uint requested;         	/*accumulated new_req */
	uint num_greedy;          /* Number of greedy nodes */
	int *greedy_nodes;        /* List of greedy nodes */
	uint *greedy_req; 				/* extra_power requested */
  uint *extra_power;				/* extra power already allocated */
	#if POWERCAP_EXT
	performance_t	*perf_info;	/* Performance metrics used by powercap scheduler */
	#endif
	uint current_power; 	    /* Accumulated power */
  uint total_powercap;      /* Accumulated current powercap limits */
}powercap_status_t;

#define TEMP_NUM_NODES 10

typedef struct powercap_opt{
  uint num_greedy;      /* Number of greedy nodes */
  int greedy_nodes[TEMP_NUM_NODES];     /* List of greedy nodes */
  int extra_power[TEMP_NUM_NODES];    /* Extra power received by each greedy node */
  uint max_inc_new_jobs; /* Max power allowed to be received by new jobs since last powercap reported */
}powercap_opt_t;

typedef struct dom_power{
	uint platform;
	uint cpu;
	uint dram;
	uint gpu;
}dom_power_t;



#endif
