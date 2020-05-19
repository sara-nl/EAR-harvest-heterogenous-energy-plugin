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
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <common/config.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/hardware/frequency.h>
#include <common/types/projection.h>
#include <library/policies/policy_api.h>
#include <daemon/powercap_status.h>

static uint last_pc=0;

#ifdef EARL_RESEARCH
extern unsigned long ext_def_freq;
#define DEF_FREQ(f) (!ext_def_freq?f:ext_def_freq)
#else
#define DEF_FREQ(f) f
#endif



state_t policy_init(polctx_t *c)
{
	return EAR_SUCCESS;
}
state_t policy_apply(polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready)
{
	ulong eff_f,f;
	
	*ready=1;
	f=DEF_FREQ(c->app->def_freq);
	*new_freq=f;
	
	return EAR_SUCCESS;
}
state_t policy_ok(polctx_t *c, signature_t *curr_sig,signature_t *prev_sig,int *ok)
{
	ulong eff_f;
	uint power_status,next_status;
	*ok=1;

	return EAR_SUCCESS;
}


state_t policy_get_default_freq(polctx_t *c, ulong *freq_set)
{
	*freq_set=DEF_FREQ(c->app->def_freq);
	return EAR_SUCCESS;
}

state_t policy_max_tries(polctx_t *c,int *intents)
{
	*intents=0;
	return EAR_SUCCESS;
}


