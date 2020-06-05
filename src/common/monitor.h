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

#ifndef COMMON_APIS_SUSCRIPTOR_H
#define COMMON_APIS_SUSCRIPTOR_H

#include <common/types.h>
#include <common/states.h>
#include <common/system/time.h>

typedef state_t (*suscall_f) (void *);
typedef state_t (*suscribe_f) (void *);

typedef struct suscription_s
{
	suscribe_f 	suscribe;
	suscall_f	call_init;
	suscall_f	call_main;
	void		*memm_init;
	void		*memm_main;
	int			time_relax; // In miliseconds.
	int			time_burst; // In miliseconds.
	int 		*bursting;
    int			id;
} suscription_t;

// Example (test will receive NULL):
//	state_t test(void *arg);
//
// 	suscription_t sus;
// 	state_t s;
//
// 	sus = suscription();
//	sus->call_main  = test;
//	sus->time_relax = 1000;
//	sus->time_burst =  300;
//	
//	s = monitor_init();
//	s = monitor_register(sus);
//	s = monitor_burst(sus);
//	s = monitor_relax(sus);

state_t monitor_init();

state_t monitor_dispose();

state_t monitor_register(suscription_t *suscription);

state_t monitor_unregister(suscription_t *suscription);

state_t monitor_burst(suscription_t *suscription);

state_t monitor_relax(suscription_t *suscription);

suscription_t *suscription();

#endif //EAR_STASH_MONITOR_H
