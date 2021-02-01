/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#define _GNU_SOURCE
#include <sched.h>
#if MPI
#include <mpi.h>
#endif
#include <dlfcn.h>
//#define SHOW_DEBUGS 1
#include <common/includes.h>
#include <common/config/config_env.h>
#include <common/system/symplug.h>
#include <management/cpufreq/frequency.h>
#include <library/policies/policy_ctx.h>
#include <library/policies/policy.h>
#include <library/common/externs.h>
#include <library/common/global_comm.h>
#include <common/environment.h>
#include <daemon/local_api/eard_api.h>
#if POWERCAP
#include <daemon/powercap/powercap_status_conf.h>
#include <common/types/pc_app_info.h>
#include <library/policies/pc_suport.h>
#include <library/policies/policy_state.h>
#endif

extern masters_info_t masters_info;
extern cpu_set_t ear_process_mask;
extern int ear_affinity_is_set;

signature_t policy_last_local_signature,policy_last_global_signature;

#ifdef EARL_RESEARCH
extern unsigned long ext_def_freq;
#define DEF_FREQ(f) (!ext_def_freq?f:ext_def_freq)
#else
#define DEF_FREQ(f) f
#endif

#if POWERCAP
extern pc_app_info_t *pc_app_info_data;
#endif

typedef struct policy_symbols {
	state_t (*init)        (polctx_t *c);
	state_t (*node_policy_apply)       (polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready);
	state_t (*app_policy_apply)       (polctx_t *c, signature_t *my_sig, ulong *new_freq,int *ready);
	state_t (*get_default_freq)   (polctx_t *c, ulong *freq_set);
	state_t (*ok)          (polctx_t *c, signature_t *curr_sig,signature_t *prev_sig,int *ok);
	state_t (*max_tries)   (polctx_t *c,int *intents);
	state_t (*end)         (polctx_t *c);
	state_t (*loop_init)   (polctx_t *c,loop_id_t *loop_id);
	state_t (*loop_end)    (polctx_t *c,loop_id_t *loop_id);
	state_t (*new_iter)    (polctx_t *c,loop_id_t *loop_id);
	state_t (*mpi_init)    (polctx_t *c);
	state_t (*mpi_end)     (polctx_t *c);
	state_t (*configure) (polctx_t *c);
	state_t (*ganularity) (polctx_t *c,int *grain);
} polsym_t;

// Static data
static polsym_t polsyms_fun,gpu_polsyms_fun;
static void    *polsyms_obj = NULL;
const int       polsyms_n = 14;
const char     *polsyms_nam[] = {
	"policy_init",
	"policy_apply",
  "policy_app_apply",
	"policy_get_default_freq",
	"policy_ok",
	"policy_max_tries",
	"policy_end",
	"policy_loop_init",
	"policy_loop_end",
	"policy_new_iteration",
	"policy_mpi_init",
	"policy_mpi_end",
	"policy_configure",
	"policy_granularity",
};
polctx_t my_pol_ctx;

static uint policy_gpu_model;
static int my_policy_grain = POL_GRAIN_NODE;
static ulong *policy_freq_list,*freq_per_core;


state_t policy_load(char *obj_path,polsym_t *psyms)
{
	//return symplug_open(obj_path, (void **)&polsyms_fun, polsyms_nam, polsyms_n);
	return symplug_open(obj_path, (void **)psyms, polsyms_nam, polsyms_n);
}

state_t init_power_policy(settings_conf_t *app_settings,resched_t *res)
{
  char basic_path[SZ_PATH_INCOMPLETE];
  char basic_gpu_path[SZ_PATH_INCOMPLETE];
	conf_install_t *data=&app_settings->installation;
	state_t ret;

  char *obj_path = getenv(SCHED_EAR_POWER_POLICY);
	char *ins_path = getenv(SCHED_EARL_INSTALL_PATH);
	char *app_mgr_policy = getenv(USE_APP_MGR_POLICIES);
	char use_path[SZ_PATH_INCOMPLETE];
	int app_mgr=0;
	if (app_mgr_policy != NULL) app_mgr = atoi(app_mgr_policy);
	#if SHOW_DEBUGS
	if (masters_info.my_master_rank >=0){
	if (obj_path!=NULL){ 
		debug("%s = %s",SCHED_EAR_POWER_POLICY,obj_path);
	}else{
		debug("%s undefined",SCHED_EAR_POWER_POLICY);
	}
	if (ins_path !=NULL){
		debug("%s = %s",SCHED_EARL_INSTALL_PATH,ins_path);
	}else{
		debug("%s undefined",SCHED_EARL_INSTALL_PATH);
	}
	}
	#endif
  if (((obj_path==NULL) && (ins_path == NULL)) || (app_settings->user_type!=AUTHORIZED)){
			if (!app_mgr){
    		sprintf(basic_path,"%s/policies/%s.so",data->dir_plug,app_settings->policy_name);
			}else{
    		sprintf(basic_path,"%s/policies/app_%s.so",data->dir_plug,app_settings->policy_name);
			}
    	obj_path=basic_path;
	}else{
		if (obj_path == NULL){
			if (ins_path != NULL){
				sprintf(use_path,"%s/plugins/policies", ins_path);	
			}else{
				sprintf(use_path,"%s/policies", data->dir_plug);
			}
			if (!app_mgr){
				sprintf(basic_path,"%s/%s.so",use_path,app_settings->policy_name);		
			}else{
				sprintf(basic_path,"%s/app_%s.so",use_path,app_settings->policy_name);		
			}
			obj_path=basic_path;
		}
	}
  if (masters_info.my_master_rank>=0) verbose(1,"loading policy %s",obj_path);
	if (policy_load(obj_path,&polsyms_fun)!=EAR_SUCCESS){
		error("Error loading policy %s",obj_path);
	}
	ear_frequency									= DEF_FREQ(app_settings->def_freq);
	my_pol_ctx.app								= app_settings;
	my_pol_ctx.reconfigure				= res;
	my_pol_ctx.user_selected_freq	= DEF_FREQ(app_settings->def_freq);
	my_pol_ctx.reset_freq_opt			= get_ear_reset_freq();
	my_pol_ctx.ear_frequency			=	&ear_frequency;
	my_pol_ctx.num_pstates				=	frequency_get_num_pstates();
	my_pol_ctx.use_turbo 					= ear_use_turbo;
	my_pol_ctx.affinity  					= ear_affinity_is_set;
	#if POWERCAP
	my_pol_ctx.pc_limit						= app_settings->pc_opt.current_pc;
	#else
	my_pol_ctx.pc_limit           = 0;
	#endif
	#if MPI
	if (PMPI_Comm_dup(masters_info.new_world_comm,&my_pol_ctx.mpi.comm)!=MPI_SUCCESS){
		error("Duplicating COMM_WORLD in policy");
	}
	if (masters_info.my_master_rank>=0){
	if (PMPI_Comm_dup(masters_info.masters_comm, &my_pol_ctx.mpi.master_comm)!=MPI_SUCCESS){
    error("Duplicating master_comm in policy");
  }
	}
	#endif

	/********* GPU PART **********/
	if (masters_info.my_master_rank>=0){
	#if USE_GPUS
  obj_path = getenv(SCHED_EAR_GPU_POWER_POLICY);
  #if SHOW_DEBUGS
  if (obj_path!=NULL){ 
    debug("%s = %s",SCHED_EAR_GPU_POWER_POLICY,obj_path);
  }else{
    debug("%s undefined",SCHED_EAR_GPU_POWER_POLICY);
  }
  #endif
  if (((obj_path==NULL) && (ins_path == NULL)) || (app_settings->user_type!=AUTHORIZED)){
      sprintf(basic_path,"%s/policies/gpu_%s.so",data->dir_plug,app_settings->policy_name);
      obj_path=basic_path;
  }else{
    if (obj_path == NULL){
      sprintf(basic_path,"%s/plugins/policies/gpu_%s.so",ins_path,app_settings->policy_name);
      obj_path=basic_path;
    }
	}
  debug("loading policy %s",obj_path);
  if (policy_load(obj_path,&gpu_polsyms_fun)!=EAR_SUCCESS){
    error("Error loading policy %s",obj_path);
  }
	my_pol_ctx.gpu_mgt_ctx_on=0;	
	if (gpu_lib_load(app_settings) != EAR_SUCCESS){
		error("gpu_lib_load in init_power_policy");
	}else{
		if (gpu_lib_init(&my_pol_ctx.gpu_mgt_ctx) == EAR_SUCCESS){
			my_pol_ctx.gpu_mgt_ctx_on=1;
			debug("gpu_lib_init success");
      gpu_lib_model(&my_pol_ctx.gpu_mgt_ctx,&policy_gpu_model);
			if (policy_gpu_model == MODEL_DUMMY){ 
				debug("Setting policy num gpus to 0 because DUMMY");
				my_pol_ctx.num_gpus = 0;
			}else{
				gpu_lib_count(&my_pol_ctx.num_gpus);
			}
			debug("Num gpus detected in policy_load %u",my_pol_ctx.num_gpus);	
		}else{
			error("gpu_lib_init");
		}
	}
	#endif
	}
	
	return policy_init();
}


/* Policy functions previously included in models */

/*
 *
 * Policy wrappers
 *
 */


state_t policy_init()
{
	polctx_t *c=&my_pol_ctx;
	state_t ret,retg=EAR_SUCCESS;
	if (polsyms_fun.init != NULL){
		ret=polsyms_fun.init(c);
	}

	if (polsyms_fun.ganularity != NULL){
		polsyms_fun.ganularity(c,&my_policy_grain);
		if (my_policy_grain == POL_GRAIN_CORE){
			policy_freq_list = calloc(MAX_CPUS_SUPPORTED,sizeof(ulong));
		}else{
			policy_freq_list=calloc(1,sizeof(ulong));
		}
		freq_per_core = calloc(MAX_CPUS_SUPPORTED,sizeof(ulong));
	}else{
		policy_freq_list=calloc(1,sizeof(ulong));
		freq_per_core = calloc(MAX_CPUS_SUPPORTED,sizeof(ulong));
	}
	signature_init(&policy_last_local_signature);
	signature_init(&policy_last_global_signature);

	#if USE_GPUS
	if (masters_info.my_master_rank>=0){
	if (gpu_polsyms_fun.init != NULL){
		debug("Loading gpu init");
		retg=gpu_polsyms_fun.init(c);
	}else{
		debug("gpu init policy null");
	}
	}	
	#endif
	#if POWERCAP
	if (masters_info.my_master_rank>=0){ 
		pc_support_init(c);
	}
	#endif
	if ((ret == EAR_SUCCESS) && (retg == EAR_SUCCESS)) return EAR_SUCCESS;
	else return EAR_ERROR;
}

static ulong compute_avg_freq(ulong *my_policy_list)
{
	int i;
	ulong total=0;
	for (i=0; i<lib_shared_region->num_processes;i++)
	{
		total+=my_policy_list[i];
	}
	return total/lib_shared_region->num_processes;
}
static void print_freq_per_core()
{
	int i;
	for (i=0;i<arch_desc.top.cpu_count;i++)
  {
		verbosen(2,"CPU[%d]=%.2f ",i,(float)freq_per_core[i]/1000000.0);
	}
	verbose(2," ");
}
static void from_proc_to_core()
{
	int p,c;
	ulong f;
	int ccount;
	cpu_set_t m;
	for (p=0;p<lib_shared_region->num_processes;p++)
	{
		f = policy_freq_list[p];
		ccount = arch_desc.top.cpu_count;
		m = sig_shared_region[p].cpu_mask;
		/*debug("Freq selected for rank %d is %lu cpus %d",p,f,ccount);*/
		for (c=0;c<ccount;c++){
			if (CPU_ISSET(c,&m)){ 
				freq_per_core[c] = f;
			}
		}
	}
	return;
}
static void policy_cpu_freq_selection(signature_t *my_sig,ulong *freq_set)
{
	polctx_t *c=&my_pol_ctx;
	int i;
	char pol_grain_str[128];
	debug("policy_cpu_freq_selection");
#if POWERCAP
  if (pc_app_info_data->mode==PC_DVFS){
      ulong f;
      pcapp_info_set_req_f(pc_app_info_data,*freq_set);
      f=pc_support_adapt_freq(c,&my_pol_ctx.app->pc_opt,*freq_set,my_sig);
      debug("Adapting frequency because pc: selected %lu new %lu",*freq_set,f);
      *freq_set=f;
  }else{
      //debug("PC mode %u (should be PC_POWER)",pc_app_info_data->mode);
      pcapp_info_set_req_f(pc_app_info_data,*freq_set);
  }
#endif
	memset(freq_per_core,0,sizeof(ulong)*MAX_CPUS_SUPPORTED);
	if (my_policy_grain == POL_GRAIN_CORE) sprintf(pol_grain_str,"POL_GRAIN_CORE");
	else sprintf(pol_grain_str,"POL_GRAIN_NODE");
	verbose(2,"my_policy_grain %s and affinity %d",pol_grain_str,ear_affinity_is_set);
	/* Assumption: If affinity is set for master, it is set for all, we could check individually */
	if ((my_policy_grain == POL_GRAIN_CORE) && (ear_affinity_is_set)){
		from_proc_to_core();		
	}else{
		debug("Setting same freq in all node %lu",policy_freq_list[0]);
		for (i=0;i<MAX_CPUS_SUPPORTED;i++) freq_per_core[i]=policy_freq_list[0];
	}
	// print_freq_per_core();
  if (*freq_set != *(c->ear_frequency))
  {
		/* *(c->ear_frequency) =  eards_change_freq(*freq_set);*/
		*(c->ear_frequency) = eards_change_freq_with_list(arch_desc.top.cpu_count,freq_per_core);
		verbose(2,"MR[%d]: Setting frequency_list to %lu (ret=%lu)",masters_info.my_master_rank,*freq_set,*(c->ear_frequency));
		return;
      if(ear_affinity_is_set == 0){
				verbose(2,"MR[%d] Setting frequency_node to %lu",masters_info.my_master_rank,*freq_set);
        *(c->ear_frequency) =  eards_change_freq(*freq_set);
      }else{
				verbose(2,"MR[%d] Setting frequency_mask to %lu",masters_info.my_master_rank,*freq_set);
        /* How to manage cores vs CPUS */
        *(c->ear_frequency) =  eards_change_freq_with_mask(*freq_set,&ear_process_mask);
      }
  }
}

state_t policy_app_apply(ulong *freq_set, int *ready)
{
	polctx_t *c=&my_pol_ctx;
	state_t st = EAR_SUCCESS;
	*ready=1;
	if (polsyms_fun.app_policy_apply == NULL){
		*ready = EAR_POLICY_LOCAL_EV;
		return st;
	}
	if (!eards_connected() || (masters_info.my_master_rank<0)){
			*ready=EAR_POLICY_CONTINUE;
			return st;
	}
	if (my_policy_grain == POL_GRAIN_CORE) memset(policy_freq_list,0,sizeof(ulong)*MAX_CPUS_SUPPORTED);
	st=polsyms_fun.app_policy_apply(c, &policy_last_global_signature, policy_freq_list,ready);
  if (*ready == EAR_POLICY_READY){
    	if (my_policy_grain == POL_GRAIN_CORE && ear_affinity_is_set){
				*freq_set = compute_avg_freq(policy_freq_list);
			}else{
				*freq_set = policy_freq_list[0];
			}
			debug("Average frequency after app_policy is %lu",*freq_set );
			policy_cpu_freq_selection(&policy_last_global_signature,freq_set);
	}
	/* GPU part doesn't exist yet */
	return st;
}

state_t policy_node_apply(signature_t *my_sig,ulong *freq_set, int *ready)
{
	polctx_t *c=&my_pol_ctx;
	signature_t node_sig;
	int i;
	state_t st=EAR_ERROR,stg=EAR_SUCCESS;
	*ready=1;
	if (polsyms_fun.node_policy_apply!=NULL){
		
		if (!eards_connected() || (masters_info.my_master_rank<0)){
			*ready=EAR_POLICY_CONTINUE;
			return EAR_SUCCESS;
		}
		signature_copy(&node_sig,my_sig);
		node_sig.DC_power=sig_node_power(my_sig);

		signature_copy(&policy_last_local_signature,&node_sig);
  	if (my_policy_grain == POL_GRAIN_CORE) memset(policy_freq_list,0,sizeof(ulong)*MAX_CPUS_SUPPORTED);
		st=polsyms_fun.node_policy_apply(c, &node_sig,policy_freq_list,ready);
		if ((*ready == EAR_POLICY_READY) || (*ready == EAR_POLICY_TRY_AGAIN)){
      if (my_policy_grain == POL_GRAIN_CORE){
        *freq_set = compute_avg_freq(policy_freq_list);
      }else{
        *freq_set = policy_freq_list[0];
      }
			policy_cpu_freq_selection(my_sig,freq_set);
			if (*ready == EAR_POLICY_TRY_AGAIN) *ready = EAR_POLICY_CONTINUE;
		} /* Stop*/
  } else{
		if (polsyms_fun.app_policy_apply != NULL ){
			*ready = EAR_POLICY_GLOBAL_EV;
		}else{
			if (c!=NULL) *freq_set=DEF_FREQ(c->app->def_freq);
		}
	}
	#if USE_GPUS
	/* At this point we are the master */
	/* GPU frequency must be integrated in arguments, hardcoded for now */
	if ((gpu_polsyms_fun.node_policy_apply!=NULL) && (my_pol_ctx.num_gpus)) {
		ulong *gpu_f=calloc(my_pol_ctx.num_gpus,sizeof(ulong));
		stg=gpu_polsyms_fun.node_policy_apply(c, my_sig,gpu_f,ready);
		/* We must apply a gpu_freq change */
		if (*ready == EAR_POLICY_READY ){
		#if POWERCAP
			for (i=0;i<my_sig->gpu_sig.num_gpus;i++){
				debug("GPU[%d] freq requested %lu",i,gpu_f[i]);
			}
			pc_app_info_data->num_gpus_used=my_sig->gpu_sig.num_gpus;
			memcpy(pc_app_info_data->req_gpu_f,gpu_f,my_pol_ctx.num_gpus*sizeof(ulong));
    	if (pc_app_info_data->mode==PC_DVFS){
      	pc_support_adapt_gpu_freq(c,&my_pol_ctx.app->pc_opt,gpu_f,my_sig);
    	}
			for (i=0;i<my_sig->gpu_sig.num_gpus;i++){
				debug("GPU[%d] freq selected after powercap filter %lu",i,gpu_f[i]);
			}
		#endif
			eards_gpu_set_freq(my_pol_ctx.num_gpus,gpu_f);
		}
		free(gpu_f);
	}
	#endif
	if ((st == EAR_SUCCESS) && (stg == EAR_SUCCESS)) return EAR_SUCCESS;
	else return EAR_ERROR;
}

state_t policy_get_default_freq(ulong *freq_set)
{
	polctx_t *c=&my_pol_ctx;
	state_t ret,retg=EAR_SUCCESS;
	if (polsyms_fun.get_default_freq!=NULL){
		ret = polsyms_fun.get_default_freq( c, freq_set);
		return ret;
	}else{
		if (c!=NULL) *freq_set=DEF_FREQ(c->app->def_freq);
		return EAR_SUCCESS;
	}
}
state_t policy_set_default_freq()
{
  polctx_t *c=&my_pol_ctx;
	state_t st;
	if (polsyms_fun.get_default_freq!=NULL){
  	st=polsyms_fun.get_default_freq(c,c->ear_frequency);
  	if (masters_info.my_master_rank>=0) eards_change_freq(*(c->ear_frequency));
		return st;
	}else{ 
		return EAR_ERROR;
	}
}



state_t policy_ok(signature_t *curr,signature_t *prev,int *ok)
{
	polctx_t *c=&my_pol_ctx;
	state_t ret,retg=EAR_SUCCESS;
	if (polsyms_fun.ok!=NULL){
#if POWERCAP
		pc_support_compute_next_state(c,&my_pol_ctx.app->pc_opt,curr);
#endif
		ret = polsyms_fun.ok(c, curr,prev,ok);
		#if USE_GPUS
		if (masters_info.my_master_rank>=0){
		if ((gpu_polsyms_fun.ok!=NULL) && (my_pol_ctx.num_gpus)){
			retg = gpu_polsyms_fun.ok(c, curr,prev,ok);
		}
		}
		#endif
		if ((ret == EAR_SUCCESS) && (retg == EAR_SUCCESS)) return EAR_SUCCESS;
		else return EAR_ERROR;
		
	}else{
		*ok=1;
		return EAR_SUCCESS;
	}
}

state_t policy_max_tries(int *intents)
{ 
	polctx_t *c=&my_pol_ctx;
	if (polsyms_fun.max_tries!=NULL){
		return	polsyms_fun.max_tries( c,intents);
	}else{
		*intents=1;
		return EAR_SUCCESS;
	}
}

state_t policy_end()
{
	polctx_t *c=&my_pol_ctx;
	preturn(polsyms_fun.end,c);
}
/** LOOPS */
state_t policy_loop_init(loop_id_t *loop_id)
{
	polctx_t *c=&my_pol_ctx;
	preturn(polsyms_fun.loop_init,c, loop_id);
}

state_t policy_loop_end(loop_id_t *loop_id)
{
	polctx_t *c=&my_pol_ctx;
	preturn(polsyms_fun.loop_end,c, loop_id);
}
state_t policy_new_iteration(loop_id_t *loop_id)
{
	polctx_t *c=&my_pol_ctx;
	preturn(polsyms_fun.new_iter,c,loop_id);
}

/** MPI CALLS */

state_t policy_mpi_init()
{
	polctx_t *c=&my_pol_ctx;
	preturn_opt(polsyms_fun.mpi_init,c);
}

state_t policy_mpi_end()
{
	polctx_t *c=&my_pol_ctx;
	preturn_opt(polsyms_fun.mpi_end,c);
}

/** GLOBAL EVENTS */

state_t policy_configure()
{
  polctx_t *c=&my_pol_ctx;
  preturn(polsyms_fun.configure, c);
}

state_t policy_force_global_frequency(ulong new_f)
{
  ear_frequency=new_f;
  if (masters_info.my_master_rank>=0) eards_change_freq(ear_frequency);
	return EAR_SUCCESS;
}

void print_policy_ctx(polctx_t *p)
{
	fprintf(stderr,"user_type %u\n",p->app->user_type);
	fprintf(stderr,"policy_name %s\n",p->app->policy_name);
	fprintf(stderr,"setting 0 %lf\n",p->app->settings[0]);
	fprintf(stderr,"def_freq %lu\n",DEF_FREQ(p->app->def_freq));
	fprintf(stderr,"def_pstate %u\n",p->app->def_p_state);
	fprintf(stderr,"reconfigure %d\n",p->reconfigure->force_rescheduling);
	fprintf(stderr,"user_selected_freq %lu\n",p->user_selected_freq);
	fprintf(stderr,"reset_freq_opt %lu\n",p->reset_freq_opt);
	fprintf(stderr,"ear_frequency %lu\n",*(p->ear_frequency));
	fprintf(stderr,"num_pstates %u\n",p->num_pstates);
	fprintf(stderr,"use_turbo %u\n",p->use_turbo);
}
