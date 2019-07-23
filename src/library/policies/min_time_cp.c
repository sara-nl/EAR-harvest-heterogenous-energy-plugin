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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/config.h>
#include <common/states.h>
#include <control/frequency.h>
#include <common/time.h>
#include <common/types/projection.h>
#include <common/math_operations.h>
#include <daemon/eard_api.h>
#include <library/policies/policy_api.h>
#include <library/policies/global_comm.h>


#define SHOW_DEBUGS
#ifdef SHOW_DEBUGS
#define debug(...) fprintf(stderr, __VA_ARGS__); 
#else
#define debug(...) 
#endif

typedef unsigned long ulong;
typedef unsigned int uint;
typedef struct policy_info
{
	double mpi_time;
	double time;
	ulong freq;
	uint num_mpis;
}policy_info_t;

static policy_info_t *cp_info;
static policy_info_t cp_my_info;
static MPI_Request cp_req;
static int cp_num_nodes;
static int cp_rank;
static uint cp_num_mpis=0;
static uint cp_waiting=0;
static timestamp cp_init_mpi;
static unsigned long long cp_total_mpi;
static ulong cp_iterations=0;
static char cp_node[128];
static int in_mpi_call=0;

state_t policy_init(polctx_t *c)
{
	cp_iterations=0;
	if (c==NULL) return EAR_ERROR;
	PMPI_Comm_size(c->mpi.master_comm,&cp_num_nodes);
	PMPI_Comm_rank(c->mpi.master_comm,&cp_rank);
	/* Allocating memory to share info */
	cp_info=(policy_info_t *)malloc(sizeof(policy_info_t)*cp_num_nodes);
	if (cp_info==NULL) return EAR_ERROR;
	memset(cp_info,0,sizeof(policy_info_t)*cp_num_nodes);
	gethostname(cp_node,sizeof(cp_node));
	strtok(cp_node,".");
	return EAR_SUCCESS;
}


state_t policy_loop_init(polctx_t *c,loop_id_t *loop_id)
{
		cp_total_mpi=0;
		cp_iterations=0;
		cp_num_mpis=0;
		if (c!=NULL){ 
			projection_reset(c->num_pstates);
			return EAR_SUCCESS;
		}else{
			return EAR_ERROR;
		}
		
}

void share_with_all(MPI_Comm comm,double mpi_time,double iter_time, ulong f,uint num_mpis)
{
	cp_my_info.mpi_time=mpi_time;
	cp_my_info.time=iter_time;
	cp_my_info.freq=f;
	cp_my_info.num_mpis=num_mpis;
	ishare_global_info(comm,(char *)&cp_my_info,sizeof(policy_info_t),
		(char *)cp_info,sizeof(policy_info_t),&cp_req);
	cp_waiting=1;
}

state_t policy_loop_end(polctx_t *c,loop_id_t *loop_id)
{
    if ((c!=NULL) && (c->reset_freq_opt))
    {
        // Use configuration when available
        *(c->ear_frequency) = eards_change_freq(c->app->def_freq);
    }
		return EAR_SUCCESS;
}


// This is the main function in this file, it implements power policy
state_t policy_apply(polctx_t *c,signature_t *sig,ulong *new_freq,int *ready)
{
    signature_t *my_app;
    int i,min_pstate;
    unsigned int ref,try_next;
    double freq_gain,perf_gain,min_eff_gain;
    double power_proj,time_proj;
    double power_ref,time_ref,time_current;
    ulong best_freq,best_pstate,freq_ref;
		ulong curr_freq;
		ulong curr_pstate,def_pstate,def_freq;
		state_t st;
		double mpi_perc,mpi_per_iter;
    my_app=sig;

		*ready=1;


		if (c==NULL) return EAR_ERROR;
		if (c->app==NULL) return EAR_ERROR;

    if (c->use_turbo) min_pstate=0;
    else min_pstate=frequency_freq_to_pstate(c->app->max_freq);

		// % of MPI 
		
		mpi_per_iter=(double)cp_total_mpi/(double)cp_iterations;
		mpi_per_iter=mpi_per_iter/1000000000.0; // secs

		share_with_all(c->mpi.master_comm,mpi_per_iter,my_app->time,*(c->ear_frequency),cp_num_mpis/cp_iterations);

		// start again counting
		cp_iterations=0;
		cp_total_mpi=0;
		cp_num_mpis=0;

		// Default values
		
		min_eff_gain=c->app->settings[0];
		def_freq=c->app->def_freq;
		def_pstate=frequency_freq_to_pstate(def_freq);

    // This is the frequency at which we were running
		curr_freq=*(c->ear_frequency);
    curr_pstate = frequency_freq_to_pstate(curr_freq);
		

    // If is not the default P_STATE selected in the environment, a projection
    // is made for the reference P_STATE in case the projections were available.
    if (curr_freq != def_freq) // Use configuration when available
    {
				if (projection_available(curr_pstate,def_pstate)==EAR_SUCCESS)
        {
					st=project_power(my_app,curr_pstate,def_pstate,&power_ref);
					st=project_time(my_app,curr_pstate,def_pstate,&time_ref);
          best_freq=def_freq;
					best_pstate=def_pstate;
        }
        else
        {
          time_ref=my_app->time;
          power_ref=my_app->DC_power;
          best_freq=curr_freq;
					best_pstate=curr_pstate;
        }
    }
    else
    { 
        time_ref=my_app->time;
        power_ref=my_app->DC_power;
        best_freq=curr_freq;
				best_pstate=curr_pstate;
    }


	// ref=1 is nominal 0=turbo, we are not using it

		try_next=1;
		i=best_pstate-1;
		time_current=time_ref;


		while(try_next && (i >= min_pstate))
		{
			if (projection_available(curr_pstate,i)==EAR_SUCCESS)
			{
				st=project_power(my_app,curr_pstate,i,&power_proj);
				st=project_time(my_app,curr_pstate,i,&time_proj);
				projection_set(i,time_proj,power_proj);
				freq_ref=frequency_pstate_to_freq(i);
				freq_gain=min_eff_gain*(double)(freq_ref-best_freq)/(double)best_freq;
				perf_gain=(time_current-time_proj)/time_current;

				// OK
				if (perf_gain>=freq_gain)
				{
					best_freq=freq_ref;
					time_current = time_proj;
					i--;
				}
				else
				{
					try_next = 0;
				}
			} // Projections available
			else{
				try_next=0;
			}
		}	
		*new_freq=best_freq;
		return EAR_SUCCESS;
}


state_t policy_ok(polctx_t *c,signature_t *curr_sig,signature_t *last_sig,int *ok)
{

	state_t st=EAR_SUCCESS;

	if ((c==NULL) || (curr_sig==NULL) || (last_sig==NULL)) return EAR_ERROR;
	
	if (curr_sig->def_f==last_sig->def_f) *ok=1;

	// Check that efficiency is enough
	if (curr_sig->time < last_sig->time) *ok=1;

	// If time is similar and it was same freq it is ok	
	if (equal_with_th(curr_sig->time , last_sig->time,0.1) && (curr_sig->def_f == last_sig->def_f)) *ok=1;
	
	// If we run at a higher freq but we are slower, we are not ok	
	if ((curr_sig->time > last_sig->time) && (curr_sig->def_f > last_sig->def_f)) *ok=0;

	else *ok=0;

	return st;

}
state_t  policy_get_default_freq(polctx_t *c,ulong *f)
{
		if ((c!=NULL) && (c->app!=NULL)){
    // Just in case the bestPstate was the frequency at which the application was running
        *f=c->app->def_freq;
    }
		return EAR_SUCCESS;
}
	
state_t policy_max_tries(polctx_t *c,int *intents)
{
  *intents=2;
  return EAR_SUCCESS;
}

static void show_shared_info()
{
	int i;
	double perc_mpi,avg_time_mpi;
	for (i=0;i<cp_num_nodes;i++){
		perc_mpi=(cp_info[i].mpi_time/cp_info[i].time)*100.0;
		avg_time_mpi=cp_info[i].mpi_time/(double)cp_info[i].num_mpis;
		debug("Node %s-%d mpi_time %.3lf iter time %.3lf freq %lu num_mpis %u\n",
		cp_node,i,cp_info[i].mpi_time,cp_info[i].time,cp_info[i].freq,cp_info[i].num_mpis);
		debug("Percentage mpi %.2lf total mpis avg time per mpi %.5lf\n",
		perc_mpi,avg_time_mpi);					
	}
}

state_t policy_new_iteration(polctx_t *c,loop_id_t *loop_id)
{
	cp_iterations++;
	if (cp_waiting){
		if (is_info_ready(&cp_req)==EAR_SUCCESS){
			cp_waiting=0;
			show_shared_info();
		}
	}
}
state_t policy_mpi_init(polctx_t *c)
{
	timestamp_get(&cp_init_mpi);	
}
state_t policy_mpi_end(polctx_t *c)
{
	unsigned long long time_difff;
	timestamp cp_end_mpi;
	timestamp_get(&cp_end_mpi);
	time_difff=timestamp_diff(&cp_end_mpi,&cp_init_mpi,1);
	cp_total_mpi+=time_difff;
	cp_num_mpis++;
}

