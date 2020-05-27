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
#if POWERCAP
#include <common/types/pc_app_info.h>
#include <daemon/powercap_status_conf.h>

void pcapp_info_new_job(pc_app_info_t *t)
{
	t->req_f=0;
	t->req_power=0;
	t->pc_status=PC_STATUS_IDLE;
}
void pcapp_info_end_job(pc_app_info_t *t)
{
	t->req_f=0;
	t->req_power=0;
	t->pc_status=PC_STATUS_IDLE;
}
void pcapp_info_set_req_f(pc_app_info_t *t,ulong f)
{
	t->req_f=f;
	t->pc_status=PC_STATUS_OK;
}

#endif

