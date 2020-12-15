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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/config.h>
//#define SHOW_DEBUGS 1
#include <common/states.h>
#include <common/output/verbose.h>
#include <management/cpufreq/connector.h>
#include <common/types/projection.h>
#include <library/policies/policy_api.h>
#include <daemon/local_api/eard_api.h>
#include <library/common/externs.h>
#include <daemon/powercap/powercap_status.h>
#include <library/policies/policy_state.h>


typedef unsigned long ulong;
#ifdef EARL_RESEARCH
extern unsigned long ext_def_freq;
#define FREQ_DEF(f) (!ext_def_freq?f:ext_def_freq)
#else
#define FREQ_DEF(f) f
#endif

static ulong req_f;
static timestamp pol_time_init;
static volatile uint global_sig_ready=0;
static signature_t gsig;
extern signature_t policy_last_global_signature;
static int LB_node_cp,LB_rank_cp;

static shsignature_t *lb_local_signatures;
static int last_node_cp;

/* Global statistics */
static timestamp app_init;
static mpi_information_t mpi_stats;
static int force_granularity=0;
static int my_policy_granularity;


state_t policy_init(polctx_t *c)
{
	char *polg;
	//debug("LOAD BALANCE");
	if (c!=NULL){ 
	  sig_shared_region[my_node_id].mpi_info.mpi_time=0;
    sig_shared_region[my_node_id].mpi_info.total_mpi_calls=0;
    sig_shared_region[my_node_id].mpi_info.exec_time=0;
    sig_shared_region[my_node_id].mpi_info.perc_mpi=0;

    /* Gobal mpi statistics */
    mpi_stats.mpi_time = 0;
    mpi_stats.total_mpi_calls = 0;
    mpi_stats.exec_time = 0;
    mpi_stats.perc_mpi = 0;
    mpi_stats.rank = sig_shared_region[my_node_id].mpi_info.rank;
    timestamp_getfast(&app_init);
		lb_local_signatures = calloc(lib_shared_region->num_processes,sizeof(shsignature_t));

		polg = getenv(EAR_POLICY_GRAIN);
		if (polg != NULL){
			force_granularity = 1;
			if (!strcmp(polg,"CORE")){
				my_policy_granularity = POL_GRAIN_CORE;
			}else{
				my_policy_granularity = POL_GRAIN_NODE;
			}
		}

		return EAR_SUCCESS;
	}else return EAR_ERROR;
}

state_t policy_end(polctx_t *c)
{
  timestamp end;
  ullong elap;
  char buff[256];
  char *stats=getenv(EAR_STATS);
  /* Global statistics */
  timestamp_getfast(&end);
  elap=timestamp_diff(&end,&app_init,TIME_USECS);
  mpi_stats.exec_time = elap;
  mpi_stats.perc_mpi = (float) mpi_stats.mpi_time/(float)mpi_stats.exec_time;
  if (stats != NULL){
    mpi_info_to_str(&mpi_stats,buff,sizeof(buff));
    verbose(0,buff);
  }
  return EAR_SUCCESS;
}



state_t policy_loop_init(polctx_t *c,loop_id_t *l)
{
	if (c!=NULL){
    projection_reset(c->num_pstates);
		return EAR_SUCCESS;
	}else{
		return EAR_ERROR;
	}
}

state_t policy_loop_end(polctx_t *c,loop_id_t *l)
{
    if ((c!=NULL) && (c->reset_freq_opt))
    {
        *(c->ear_frequency) = eards_change_freq(FREQ_DEF(c->app->def_freq));
    }
	return EAR_SUCCESS;
}

state_t policy_apply(polctx_t *c,signature_t *sig,ulong *new_freq,int *ready)
{
  *ready = EAR_POLICY_GLOBAL_EV;
  if (c==NULL) return EAR_ERROR;
  if (c->app==NULL) return EAR_ERROR;
  return EAR_SUCCESS;
}

ulong load_balance_for_process(polctx_t *c,float max_node_penalty,uint local_id)
{
	signature_t *my_app;
	int i,min_pstate;
	unsigned int ref;
	double power_proj,time_proj,energy_proj,best_solution,energy_ref;
	double power_ref,time_ref,max_penalty,time_max,local_penalty,cpi_p;
	float my_useful,cp_useful,useful_p;
	char buff[512];
	double my_cpi, cp_cpi;
	signature_t csig;

  ulong best_freq,best_pstate,freq_ref,eff_f;
	ulong curr_freq,nominal;
	ulong curr_pstate,def_pstate,def_freq;
	state_t st;
	unsigned long long my_useful_time,cp_useful_time;
	uint power_status;
	int my_shid,cp_shid;
	int num_proj_found = 0;
	float time_ratio,useful_ratio,useful_cp_ratio;

    if (c->use_turbo) min_pstate=0;
    else min_pstate=frequency_closest_pstate(c->app->max_freq);

		nominal=frequency_pstate_to_freq(min_pstate);

		from_minis_to_sig(&csig,&lb_local_signatures[local_id].sig);
    my_app=&csig;

		#if 0
    signature_to_str(my_app,buff,sizeof(buff));
		debug("LOCAL_SIG[%d]= %s",local_id,buff);
		#endif


		def_freq=FREQ_DEF(c->app->def_freq);
		def_pstate=frequency_closest_pstate(def_freq);
    #ifdef POWERCAP
    if (c->pc_limit > 0){
			curr_freq=frequency_closest_high_freq(my_app->avg_f,1);
		}else{
    	curr_freq=*(c->ear_frequency);
		}
    #else
    curr_freq=*(c->ear_frequency);
    #endif

		curr_pstate = frequency_closest_pstate(curr_freq);
		eff_f=frequency_closest_high_freq(my_app->avg_f,1);

		/* MOVE to a function */
		/* If is not the default P_STATE selected in the environment, a projection
		 is made for the reference P_STATE in case the coefficents were available.*/
		 
		if (curr_freq != def_freq) /* Use configuration when available */
		{
		if (projection_available(curr_pstate,def_pstate)==EAR_SUCCESS)
		{
				st=project_power(my_app,curr_pstate,def_pstate,&power_ref);
				st=project_time(my_app,curr_pstate,def_pstate,&time_ref);
				best_freq=def_freq;
        //debug("projecting from %lu to %lu\t time: %.2lf\t power: %.2lf", curr_pstate, def_pstate, time_ref, power_ref);
		}
		else
		{
				time_ref=my_app->time;
				power_ref=my_app->DC_power;
				best_freq=curr_freq;
		}
		}
	  /*
		 If it is the default P_STATE selected in the environment, then a projection
		 is not needed, so the signature will be enough as a reference. 
		*/
		else
		{ /* we are running at default frequency , signature is our reference*/
			if (curr_freq == nominal){
				/* And we are the nominal freq */
				time_ref=my_app->time;
				power_ref=my_app->DC_power;
				best_freq=curr_freq;
			}else{
				/* Nominal is the reference , if projections are ready, we project time and power */
         debug("current_freq is not the nominal");

				if (projection_available(curr_pstate,min_pstate) == EAR_SUCCESS){
					project_power(my_app,curr_pstate,min_pstate,&power_ref);
					project_time(my_app,curr_pstate,min_pstate,&time_ref);
					best_freq=nominal;
          /*debug("projecting to nominal\t time: %.2lf\t power: %.2lf", time_ref, power_ref);*/
				}else{
        	time_ref=my_app->time;
        	power_ref=my_app->DC_power;
        	best_freq=curr_freq;
				}
			}

		}
		energy_ref=power_ref*time_ref;
		best_solution=energy_ref;

		cpi_p = (my_app->CPI / lb_local_signatures[last_node_cp].sig.CPI)-1.0;

		useful_cp_ratio = (float)lb_local_signatures[last_node_cp].mpi_info.exec_time-(float)lb_local_signatures[last_node_cp].mpi_info.mpi_time;
		useful_ratio = (float)lb_local_signatures[local_id].mpi_info.exec_time-(float)lb_local_signatures[local_id].mpi_info.mpi_time;
		useful_p = useful_cp_ratio / useful_ratio;

		local_penalty = ear_min(cpi_p,useful_p);
		if ((local_penalty < 0) || (local_penalty > 1)) local_penalty = 0;
		max_penalty = local_penalty + max_node_penalty;


	/* We compute the maximum performance loss*/
	time_max = time_ref + (time_ref * max_penalty);
	#if 0
	debug("LOCAL_SIG[%d][%d] CPIP %.3f MPIP %.3f my CPI %.3f CP_CPI %.3lf  my useful %.1f CP useful %.1f",masters_info.my_master_rank,local_id,cpi_p,useful_p,my_app->CPI,lb_local_signatures[last_node_cp].sig.CPI,useful_ratio/1000000.0,useful_cp_ratio/1000000.0);
	debug("LOCAL_SIG[%d][%d] Penalty=%.1f (CPI=%lf GBS=%lf Power=%lf Time=%lf TPI=%lf TLIMIT=%lf)",masters_info.my_master_rank,local_id,max_penalty*100.0,my_app->CPI,my_app->GBS,my_app->DC_power,my_app->time,my_app->TPI,time_max);
	#endif

	/* MIN_ENERGY_TO_SOLUTION ALGORITHM
	 Calcular el min_pstate que este dentro del limite*/
	for (i = min_pstate; i < c->num_pstates;i++)
	{
		/*debug("MR[%d] Checking pstate %d",masters_info.my_master_rank,i);*/
		if (projection_available(curr_pstate,i)==EAR_SUCCESS)
		{
				num_proj_found++;
				st=project_power(my_app,curr_pstate,i,&power_proj);
				st=project_time(my_app,curr_pstate,i,&time_proj);
				projection_set(i,time_proj,power_proj);
				energy_proj=power_proj*time_proj;
        debug("projected from %lu to %d\t time: %.2lf\t power: %.2lf energy: %.2lf", curr_pstate, i, time_proj, power_proj, energy_proj);
			if ((energy_proj < best_solution) && (time_proj < time_max))
			{
          debug("new best solution found");
					best_freq = frequency_pstate_to_freq(i);
					best_solution = energy_proj;
					best_pstate=i;
			}
		}
	}
	debug("LOCAL_SIG[%d][%d] selected frequency %lu projections %d",masters_info.my_master_rank,local_id,best_freq,num_proj_found);
	return best_freq;

}


state_t policy_app_apply(polctx_t *c,signature_t *sig,ulong *new_freq,int *ready)
{
	signature_t *my_app;
	int i,min_pstate;
	unsigned int ref;
	double power_proj,time_proj,energy_proj,best_solution,energy_ref;
	double power_ref,time_ref,max_penalty,time_max;
	char buff[512];
	double my_cpi, cp_cpi;
	int num_proj_found = 0;
	/*debug("APP_POLICY");*/

  ulong best_freq,best_pstate,freq_ref,eff_f;
	ulong curr_freq,nominal;
	ulong curr_pstate,def_pstate,def_freq;
	state_t st;
	unsigned long long my_useful_time,cp_useful_time;
	uint power_status;
	int my_shid,cp_shid;

	if ((c!=NULL) && (c->app!=NULL)){
    if (c->use_turbo) min_pstate=0;
    else min_pstate=frequency_closest_pstate(c->app->max_freq);

		nominal=frequency_pstate_to_freq(min_pstate);

		if (c->affinity == 0){
			*new_freq=*(c->ear_frequency);
		}else{
			for (i=0;i<lib_shared_region->num_processes;i++){
				new_freq[i]=*(c->ear_frequency);
			}
		}
    my_app=sig;
    if (global_sig_ready){
      *ready=EAR_POLICY_READY;
      global_sig_ready=0;
    }else{
      *new_freq=*(c->ear_frequency);
      *ready=EAR_POLICY_CONTINUE;
      return EAR_SUCCESS;
    }

    signature_to_str(my_app,buff,sizeof(buff));
		debug("POLICY_SIG %s",buff);

		if (LB_node_cp == masters_info.my_master_rank){
			verbose(1,"MR[%d] My Node is the CP, I will not change the frequency %lu",masters_info.my_master_rank,*new_freq);
			return EAR_SUCCESS;
		}


		/* Default values*/
		my_shid = my_shsig_id();
		cp_shid = shsig_id(LB_node_cp,LB_rank_cp);
		debug("MR[%d] Node_CP=%d , Rank_CP=%d. My shid is %d CP_shid is %d",masters_info.my_master_rank,LB_node_cp,LB_rank_cp,my_shid,cp_shid);
		my_cpi = masters_info.nodes_info[my_shid].sig.CPI;
		cp_cpi = masters_info.nodes_info[cp_shid].sig.CPI;
		max_penalty = (1 - my_cpi / cp_cpi)+ c->app->settings[0];
		debug("MR[%d] my CPI %.3lf CP_CPU %.3lf penalty %.3lf",masters_info.my_master_rank,my_cpi,cp_cpi,max_penalty);

		if (max_penalty < 0){
			*new_freq=*(c->ear_frequency);
      *ready=EAR_POLICY_READY;
			debug("Warning CPU time process %d is greather than CP %d, I will not change the frequency %lu",my_shid,cp_shid,*new_freq);
			debug("my_exec %llu my_mpi %llu cp_exec %llu cp_mpi %llu",masters_info.nodes_info[my_shid].mpi_info.exec_time,masters_info.nodes_info[my_shid].mpi_info.mpi_time,masters_info.nodes_info[cp_shid].mpi_info.exec_time,masters_info.nodes_info[cp_shid].mpi_info.mpi_time);
			return EAR_SUCCESS;
		}
		if ((c->affinity == 0) || (force_granularity && (my_policy_granularity == POL_GRAIN_NODE))){
		debug("MR[%d] affinity is not set, selecting a single frequency",masters_info.my_master_rank);
		best_freq = load_balance_for_process(c,max_penalty,last_node_cp);
		*new_freq=best_freq;
		sig_shared_region[my_node_id].new_freq=best_freq;
	}else{
		debug("MR[%d] affinity is set, selecting a list of frequencies",masters_info.my_master_rank);
		for (i=0;i<lib_shared_region->num_processes;i++){
			new_freq[i] = load_balance_for_process(c,max_penalty,i);
		}
		*ready=EAR_POLICY_READY;
	}
	}else{ 
		*ready=EAR_POLICY_CONTINUE;
		return EAR_ERROR;
	}
	return EAR_SUCCESS;
}


state_t policy_ok(polctx_t *c,signature_t *curr_sig,signature_t *last_sig,int *ok)
{
	double energy_last, energy_curr;
	uint power_status,next_status;
	ulong eff_f;

	if ((c==NULL) || (curr_sig==NULL) || (last_sig==NULL)) return EAR_ERROR;


	if (curr_sig->def_f==last_sig->def_f) *ok=1;


	energy_last = last_sig->time*last_sig->DC_power;
	energy_curr = curr_sig->time * curr_sig->DC_power;

	if ((energy_curr<energy_last)&&(curr_sig->time<(last_sig->time*c->app->settings[0]))) *ok=1;
	else *ok=0;
	return EAR_SUCCESS;
}

state_t  policy_get_default_freq(polctx_t *c, ulong *freq_set)
{
		if (c!=NULL){
			*freq_set=c->app->max_freq;
		}else EAR_ERROR;
		return EAR_SUCCESS;
}

state_t policy_max_tries(polctx_t *c,int *intents)
{
  *intents=1;
  return EAR_SUCCESS;
}


state_t policy_mpi_init(polctx_t *c)
{
  timestamp_getfast(&pol_time_init);
  return EAR_SUCCESS;
}
state_t policy_mpi_end(polctx_t *c)
{
  timestamp end;
  ullong elap;
  timestamp_getfast(&end);
  elap=timestamp_diff(&end,&pol_time_init,TIME_USECS);
  sig_shared_region[my_node_id].mpi_info.mpi_time=sig_shared_region[my_node_id].mpi_info.mpi_time+elap;
  sig_shared_region[my_node_id].mpi_info.total_mpi_calls++;

  /* Global statistics */
  mpi_stats.mpi_time+=elap;
  mpi_stats.total_mpi_calls++;

  return EAR_SUCCESS;
}
state_t policy_new_iteration(polctx_t *c,loop_id_t *loop_id)
{
  state_t ret;
	char buff[512];
	int ml;
	shsignature_t *per_node_shsig; 
  if (masters_info.my_master_rank>=0){
		/* We will only check th mpi info when we have already used it */
		if (!global_sig_ready){
    ret = check_mpi_info(&masters_info,&LB_node_cp,&LB_rank_cp,report_all_sig);
    if (ret == EAR_SUCCESS){
			global_sig_ready=1;
			debug("policy_new_iteration Node_CP %d Rank_CP %d",LB_node_cp,LB_rank_cp);
      compute_avg_app_signature(&masters_info,&gsig);
			signature_copy(&policy_last_global_signature,&gsig);
    }
		}
		/* If we are not waiting for global info we can send again */
		if (masters_info.node_info_pending == 0){
    ret = check_node_signatures(&masters_info,lib_shared_region,sig_shared_region);
    if (ret == EAR_SUCCESS){
      debug("Node signatures ready");
      if (sh_sig_per_proces){
        ret = send_node_signatures(&masters_info,lib_shared_region,sig_shared_region,sig_shared_region,report_node_sig);
      }else{
				ml=compute_per_node_most_loaded_process(lib_shared_region,sig_shared_region);
				debug("Process %d selected as the most loaded",ml);
				last_node_cp = ml;
				per_node_shsig = &sig_shared_region[ml];
				// print_sh_signature(ml,per_node_shsig);
        ret = send_node_signatures(&masters_info,lib_shared_region,per_node_shsig,sig_shared_region,report_node_sig);
      }
			/* We copy information sent to other processes to be used later */
			copy_my_sig_info(lib_shared_region,sig_shared_region,lb_local_signatures);
			#if RESET_STATISTICS_AT_SIGNATURE
			clean_mpi_info(lib_shared_region,sig_shared_region);
			#endif
    }
		}

  }

  return EAR_SUCCESS;
}


state_t policy_granularity(polctx_t *c,int *grain)
{
	*grain = POL_GRAIN_CORE;
	return EAR_SUCCESS;
}

