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
#include <common/output/verbose.h>

void app_mgt_new_job(app_mgt_t *a)
{
	if (a==NULL) return;
	memset(a,0,sizeof(app_mgt_t));
}
void app_mgt_end_job(app_mgt_t *a)
{
	if (a==NULL) return;
	memset(a,0,sizeof(app_mgt_t));
}
void print_app_mgt_data(app_mgt_t *a)
{
	if (a==NULL) return;

	verbose(1,"App_info:master_rank %u ppn %u nodes %u total_processes %u max_ppn %u",a->master_rank,a->ppn,a->nodes,a->total_processes,a->max_ppn);
}
uint is_app_master(app_mgt_t *a)
{
	if (a==NULL) return 0;
	return (a->master_rank==0);	
}



