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

#include <dlfcn.h>
#include <common/includes.h>
#include <common/symplug.h>
#include <library/models/policies/policy.h>

typedef struct policy_symbols {
	state_t (*init)        (polctx_t *c);
	state_t (*apply)       (polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready);
	state_t (*get_default_freq)   (polctx_t *c, ulong *freq_set);
	state_t (*set_default_freq)   (polctx_t *c);
	state_t (*ok)          (polctx_t *c, signature_t *curr_sig,signature_t *prev_sig,int *ok);
	state_t (*max_tries)   (int *intents);
	state_t (*end)         ();
	state_t (*loop_init)   (loop_id_t *loop_id);
	state_t (*loop_end)    (loop_id_t *loop_id);
	state_t (*new_iter)    (loop_id_t *loop_id);
	state_t (*mpi_init)    ();
	state_t (*mpi_end)     ();
	state_t (*configure) (polctx_t *c);
} polsym_t;

// Static data
static polsym_t polsyms_fun;
static void    *polsyms_obj = NULL;
const int       polsyms_n = 13;
const char     *polsyms_nam[] = {
	"ear_policy_init",
	"ear_policy_apply",
	"ear_policy_get_default_freq",
	"ear_policy_set_default_freq",
	"ear_policy_ok",
	"ear_policy_max_tries",
	"ear_policy_end",
	"ear_policy_loop_init",
	"ear_policy_loop_end",
	"ear_policy_new_iteration",
	"ear_policy_mpi_init",
	"ear_policy_mpi_end"
	"ear_policy_configure",
};
polctx_t my_pol_ctx;

state_t policy_load(char *obj_path)
{
	return symplug_open(obj_path, &polsyms_fun, polsyms_nam, polsyms_n);
}

state_t init_power_policy(settings_conf_t *app_settings,resched_t *res)
{
  char basic_path[SZ_PATH_INCOMPLETE];
	conf_install_t *data=&app_settings->installation;

  char *obj_path = getenv("SLURM_EAR_POWER_POLICY");
  if ((obj_path==NULL) || (app_settings->user_type!=AUTHORIZED)){
    	sprintf(basic_path,"%s/%s.so",data->dir_plug,app_settings->policy);
    	obj_path=basic_path;
	}
  debug("loading policy",obj_path);
	policy_load(obj_path);
	my_pol_ctx.app=app_settings;
	my_pol_ctx.reconfigure=res;
	my_pol_ctx.user_selected_freq=app_settings->def_freq;
	my_pol_ctx.reset_freq_opt=get_ear_reset_freq();
	my_pol_ctx.ear_frequency=&ear_frequency;
	return __policy_init();
}


#define preturn(call, ...) \
	if (call == NULL) { \
		return EAR_SUCCESS; \
	} \
	return call (__VA_ARGS__);

/* Policy functions previously included in models */

/*
 *
 * Policy wrappers
 *
 */

state_t _policy_init()
{
	polctx_t *c=&my_pol_ctx;
	preturn(polsyms_fun.init, c);
}

state_t policy_apply(signature_t *my_sig,ulong *freq_set, int *ready)
{
	polctx_t *c=&my_pol_ctx;
	state_t st=EAR_ERROR;
	if (polsyms_fun.apply!=NULL){
		st=polsyms_fun.apply(c, my_sig,freq_set,ready);
  	if (*freq_set != *(c->ear_frequency))
  	{
    	debug("earl: Changing Frequency to %u at the beggining of iteration",*freq_set);
    	*(c->ear_frequency) = max_freq = eards_change_freq(*freq_set);
    	if (max_freq != *freq_set) {
      	*freq_set = max_freq;
    	}
  } 
	return st;
}

state_t policy_get_default_freq(ulong *freq_set)
{
	polctx_t *c=&my_pol_ctx;
	preturn(polsyms_fun.get_default_freq, c, freq_set);
}
state_t policy_set_default_freq()
{
  polctx_t *c=&my_pol_ctx;
  preturn(polsyms_fun.set_default_freq, c);
}



state_t _policy_ok(signature_t *curr,signature_t *prev,nt *ok)
{
	polctx_t *c=&my_pol_ctx;
	preturn(polsyms_fun.ok, c, curr,prev,ok);
}

state_t _policy_max_tries(int *intents)
{ 
	preturn(polsyms_fun.max_tries, intents);
}

state_t policy_end()
{
	preturn(polsyms_fun.end);
}
/** LOOPS */
state_t policy_loop_init(loop_id_t *loop_id)
{
	preturn(polsyms_fun.loop_init, loop_id);
}

state_t policy_loop_end(loop_id_t *loop_id)
{
	preturn(polsyms_fun.loop_end, loop_id);
}
state_t policy_new_iteration(loop_id_t *loop_id)
{
	preturn(polsyms_fun.new_iter,loop_id);
}

/** MPI CALLS */

state_t policy_mpi_init()
{
	preturn(polsyms_fun.mpi_init);
}

state_t policy_mpi_end()
{
	preturn(polsyms_fun.mpi_end);
}

/** GLOBAL EVENTS */

state_t policy_configure()
{
  polctx_t *c=&my_pol_ctx;
  preturn(polsyms_fun.configure, c);
}

