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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <common/config.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/types/projection.h>
#include <library/policies/policy_api.h>
#include <common/system/time.h>
#include <library/common/library_shared_data.h>
#include <library/common/externs.h>

#ifdef EARL_RESEARCH
extern unsigned long ext_def_freq;
#define DEF_FREQ(f) (!ext_def_freq?f:ext_def_freq)
#else
#define DEF_FREQ(f) f
#endif

static timestamp pol_time_init;


state_t policy_init(polctx_t *c)
{
  
  if (c!=NULL){ 
    sig_shared_region[my_node_id].mpi_info.mpi_time=0;
    sig_shared_region[my_node_id].mpi_info.total_mpi_calls=0;
    sig_shared_region[my_node_id].mpi_info.exec_time=0;
    sig_shared_region[my_node_id].mpi_info.perc_mpi=0;

    
    return EAR_SUCCESS;
  }else return EAR_ERROR;

}
state_t policy_apply(polctx_t *c,signature_t *my_sig, ulong *new_freq,int *ready)
{
	*ready=1;
	*new_freq=DEF_FREQ(c->app->def_freq);
	return EAR_SUCCESS;
}
state_t policy_ok(polctx_t *c, signature_t *curr_sig,signature_t *prev_sig,int *ok)
{
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
  elap=timestamp_diff(&end,&pol_time_init,(ullong)1);
  sig_shared_region[my_node_id].mpi_info.mpi_time=sig_shared_region[my_node_id].mpi_info.mpi_time+elap;
  sig_shared_region[my_node_id].mpi_info.total_mpi_calls++;
  return EAR_SUCCESS;
}


state_t policy_new_iteration(polctx_t *c,loop_id_t *loop_id)
{
  int node_cp,rank_cp;
  if (masters_info.my_master_rank>=0){
    check_mpi_info(&masters_info,&node_cp,&rank_cp,report_all_sig);
    if (rank_cp>=0){
      verbose(1,"Shared data ready");
      verbose(1,"Node cp %d and rank cp %d",node_cp,rank_cp);
      if (rank_cp==ear_my_rank){
				verbose(1,"I'm the critical path");
      }
      if (node_cp==masters_info.my_master_rank){
        verbose(1,"I'm in the node CP");
      }
    }
    check_node_signatures(&masters_info,lib_shared_region,sig_shared_region,report_node_sig);
  }
  return EAR_SUCCESS;
}

