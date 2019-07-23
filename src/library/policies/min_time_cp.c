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
#include <daemon/eard_api.h>
#include <library/policies/policy_api.h>
#include <common/math_operations.h>

typedef unsigned long ulong;
<<<<<<< HEAD
static timestamp cp_init_mpi;
static unsigned long long cp_total_mpi;
=======
static timestamp init_mpi,end_mpi;
static unsigned long long total_mpi;
static unsigned long cp_iterations=0;
>>>>>>> d4bb80c92e8a0f5c38ca95750104193dfd62ea05

state_t policy_init(polctx_t *c)
{
	cp_iterations=0;
	if (c!=NULL) return EAR_SUCCESS;
	else return EAR_ERROR;
}


state_t policy_loop_init(polctx_t *c,loop_id_t *loop_id)
{
<<<<<<< HEAD
		cp_total_mpi=0;
=======
		total_mpi=0;
		cp_iterations=0;
>>>>>>> d4bb80c92e8a0f5c38ca95750104193dfd62ea05
		if (c!=NULL){ 
			projection_reset(c->num_pstates);
			return EAR_SUCCESS;
		}else{
			return EAR_ERROR;
		}
		
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
		
		fprintf(stderr,"total_mpi %lu cp_iterations %lu time %lf \n",total_mpi,cp_iterations,my_app->time);
		mpi_per_iter=(double)total_mpi/(double)cp_iterations;
		mpi_per_iter=mpi_per_iter/1000.0; // usecs
		mpi_perc=(mpi_per_iter/my_app->time)+100.0;
		fprintf(stderr,"Percentage of MPI per signature %lf\n",mpi_perc);

		// start again counting
		cp_iterations=0;
		total_mpi=0;

		// Default values
		
		min_eff_gain=c->app->settings[0];
		def_freq=c->app->def_freq;
		def_pstate=frequency_freq_to_pstate(def_freq);

    // This is the frequency at which we were running
    curr_pstate = frequency_freq_to_pstate(curr_freq);
		curr_freq=*(c->ear_frequency);
		


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

<<<<<<< HEAD
=======
state_t policy_new_iteration(polctx_t *c,loop_id_t *loop_id)
{
	cp_iterations++;
}
>>>>>>> d4bb80c92e8a0f5c38ca95750104193dfd62ea05
state_t policy_mpi_init(polctx_t *c)
{
	timestamp_get(&cp_init_mpi);	
}
state_t policy_mpi_end(polctx_t *c)
{
	unsigned long long time_difff;
<<<<<<< HEAD
	timestamp cp_end_mpi;
	timestamp_get(&cp_end_mpi);
	time_difff=timestamp_diff(&cp_end_mpi,&cp_init_mpi,1000000);
	cp_total_mpi+=time_difff;
=======
	timestamp_get(&end_mpi);
	time_difff=timestamp_diff(&end_mpi,&init_mpi,1);
	total_mpi+=time_difff;
>>>>>>> d4bb80c92e8a0f5c38ca95750104193dfd62ea05
}

