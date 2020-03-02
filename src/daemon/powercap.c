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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <common/config.h>
//#define SHOW_DEBUGS 0
#include <common/output/verbose.h>
#include <common/states.h>
#include <daemon/eard_conf_rapi.h>
#include <daemon/power_monitor.h>
void get_powercap_status(powercap_status_t *my_status)
{
	my_status->idle_nodes=powermon_is_idle();
	my_status->released_power=0;
	my_status->num_greedy=0;
	my_status->greedy_nodes=NULL;
	my_status->num_newjob_nodes=0;
	my_status->powerdef_nodes=NULL;
	my_status->current_power=powermon_current_power();
	my_status->total_powercap=powermon_get_powercap();
	verbose(1,"powercap_status idle=%u current_power %u powercap %u",my_status->idle_nodes,my_status->current_power,my_status->total_powercap);
}


void print_powercap_opt(powercap_opt_t *opt)
{
	int i;
	fprintf(stderr,"num_greedy %u\n",opt->num_greedy);	
	for (i=0;i<opt->num_greedy;i++){
		fprintf(stderr,"greedy_node %d extra power %u\n",opt->greedy_nodes[i],opt->extra_power[i]);
	}
	fprintf(stderr,"Max extra power for new jobs %u\n",opt->max_inc_new_jobs);
}

void set_powercap_opt(powercap_opt_t *opt)
{
	print_powercap_opt(opt);
}

