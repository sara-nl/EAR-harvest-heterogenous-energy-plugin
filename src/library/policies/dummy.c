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
#include <unistd.h>
#include <common/config.h>
#include <common/states.h>
#include <library/policies/policy_api.h>

#define SHOW_DEBUG 1
#if SHOW_DEBUG
#define debug_func() printf(__FUNCTION__);
#define debug(...) printf(__VA_ARGS__);
#else
#define debug_func()
#define debug(...)
#endif


state_t policy_init(polctx_t *c)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_apply(polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_get_default_freq(polctx_t *c, ulong *freq_set)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_ok(polctx_t *c, signature_t *curr_sig,signature_t *prev_sig,int *ok)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_max_tries(polctx_t *c,int *intents)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_end(polctx_t *c)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_loop_init(polctx_t *c,loop_id_t *loop_id)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_loop_end(polctx_t *c,loop_id_t *loop_id)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_new_iter(polctx_t *c,loop_id_t *loop_id)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_mpi_init(polctx_t *c)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_mpi_end(polctx_t *c)
{
	debug_func();
	return EAR_SUCCESS;
}
state_t policy_configure(polctx_t *c)
{
	debug_func();
	return EAR_SUCCESS;
}


