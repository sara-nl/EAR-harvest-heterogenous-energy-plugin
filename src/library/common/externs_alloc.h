/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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


#include <linux/limits.h>
#include <common/types/application.h>
#include <common/types/loop.h>
#include <common/shared_configuration.h>


loop_t loop;
application_t loop_signature;
char loop_summary_path[PATH_MAX];

application_t application;
char app_summary_path[PATH_MAX];

ulong EAR_default_frequency; 
uint EAR_default_pstate;
ulong ear_frequency; 
int ear_use_turbo = USE_TURBO; 
int ear_whole_app;
int ear_my_rank;
int my_job_id;
uint dynais_enabled=1;
uint loop_with_signature=0;
// To be removed whith new dynais version
ulong last_first_event;
ulong last_calls_in_loop;
ulong last_loop_size;
ulong last_loop_level;

int EAR_VERBOSE_LEVEL;

ear_conf_t *system_conf;
char ear_app_name[PATH_MAX]; //TODO: use application.app_id
