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
#include <daemon/eard_conf_rapi.h>
#include <daemon/powercap_status_conf.h>


typedef struct node_powercap_opt {
	pthread_mutex_t lock;
  uint def_powercap; /* powercap default value defined in ear.conf */
  uint powercap_idle; /* powercap to be applied at idle periods */
	uint current_pc;		/* Current limit */
	uint last_t1_allocated; /* Power allocated at last T1,guaranteed */
	uint released;			/* Power released since last T1 */
	uint max_node_power; /* Maximum node power */
  uint th_inc; /* Percentage to mark powercap_status as greedy */
  uint th_red; /* Percentage to mark powercap_status as released */
 	uint th_release; /* Percentage of power to be released */
	uint powercap_status; /* Current status */
	uint max_inc_new_jobs; /* Max extra power for new jobs */
	uint requested; /* Extra power requested, used when node is greedy or powercap < def_powercap */
}node_powercap_opt_t;

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
int periodic_metric_info(double cp);
int is_powercap_set();
int is_powercap_on();

void set_powercapstatus_mode(uint mode);

void copy_node_powercap_opt(node_powercap_opt_t *dst);
uint powercap_get_strategy();


#endif
