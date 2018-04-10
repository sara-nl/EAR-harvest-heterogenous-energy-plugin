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


#ifndef _EAR_GLOBAL_H
#define _EAR_GLOBAL_H

#include <linux/limits.h>
#include <common/types/application.h>
#include <common/shared_configuration.h>

extern loop_t loop;
extern application_t loop_signature;
extern char loop_summary_path[PATH_MAX];

extern application_t application;
extern char app_summary_path[PATH_MAX];

extern unsigned long EAR_default_frequency;
extern unsigned long ear_frequency;
extern int ear_use_turbo;
extern int ear_whole_app;
extern int ear_my_rank;
extern int my_job_id;
extern uint dynais_enabled;
extern uint loop_with_signature;
// To be removed with new dynais version
extern ulong last_first_event;
extern ulong last_calls_in_loop;
extern ulong last_loop_size;
extern ulong last_loop_level;


extern ear_conf_t *system_conf;
extern char ear_app_name[PATH_MAX];

#else
#endif
