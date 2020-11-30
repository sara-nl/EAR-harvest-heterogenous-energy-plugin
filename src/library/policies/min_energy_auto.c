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
#include <management/pstate/connector.h>
#include <common/types/projection.h>
#include <library/policies/policy_api.h>
#include <daemon/local_api/eard_api.h>

#define MIN_EFF 0.75
typedef unsigned long ulong;
#ifdef EARL_RESEARCH
extern unsigned long ext_def_freq;
#define FREQ_DEF(f) (!ext_def_freq?f:ext_def_freq)
#else
#define FREQ_DEF(f) f
#endif

static double *ratios_time;
static double *ratios_energy;
static int *valid_ratio;
static signature_t *reals;
static int error_comp_state=0;
static double error_next_power,error_prev_power;
static ulong hw_nominal;


state_t policy_init(polctx_t *c)
{
	if (c!=NULL){ 
		ratios_time=malloc(sizeof(double)*c->num_pstates);
		ratios_energy=malloc(sizeof(double)*c->num_pstates);
		valid_ratio=malloc(sizeof(int)*c->num_pstates);


		return EAR_SUCCESS;
	}else return EAR_ERROR;
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

state_t my_project_power(signature_t *s,ulong from,ulong to,double *new_power)
{
	double ppower;
	state_t st;
	st=project_power(s,from,to,&ppower);
	if ((from==hw_nominal) && (to!=from)){
		*new_power=ppower+error_next_power;
	}else *new_power=ppower;
		
	//debug("Projected power %lf corrected %lf",ppower,*new_power);
	return st;
}

void compute_projection_error(unsigned long src,unsigned long dst)
{
	double power_nominal,power_next,proj_prev,proj_next;
	power_nominal=reals[src].DC_power;
	power_next=reals[dst].DC_power;

	project_power(&reals[src],src,dst,&proj_next);
	project_power(&reals[dst],dst,src,&proj_prev);

	error_next_power=power_next-proj_next;
	error_prev_power=power_nominal-proj_prev;
	
	debug("Error power computed NEXT:(real %lf, proj %lf) %lf ",power_next,proj_next,error_next_power);
}

state_t policy_apply(polctx_t *c,signature_t *sig,ulong *new_freq,int *ready)
{
	signature_t *my_app = sig;
	int i,min_pstate;
	unsigned int ref;
	double power_proj,time_proj,energy_proj,best_solution,energy_ref;
	double power_ref,time_ref,max_penalty,time_max;
	double energy_nominal,time_nominal;
	double last_p=1.0,last_t=1.0;

  ulong best_freq,best_pstate,freq_ref;
	ulong curr_freq,nominal;
	ulong curr_pstate,def_pstate,def_freq;
	state_t st;


	if ((c!=NULL) && (c->app!=NULL)){
		memset(ratios_time,0,sizeof(double)*c->num_pstates);
		memset(ratios_energy,0,sizeof(double)*c->num_pstates);
		memset(valid_ratio,0,sizeof(int)*c->num_pstates);

    if (c->use_turbo) min_pstate=0;
    else min_pstate=frequency_closest_pstate(c->app->max_freq);


		nominal=frequency_pstate_to_freq(min_pstate);

	ratios_time[min_pstate]=1;
	ratios_energy[min_pstate]=1;


	// Default values
  max_penalty=c->app->settings[0];
	def_freq=FREQ_DEF(c->app->def_freq);
	def_pstate=frequency_closest_pstate(def_freq);
	


		// This is the frequency at which we were running
		curr_freq=*(c->ear_frequency);
		curr_pstate = frequency_closest_pstate(curr_freq);
#if 0
    signature_copy(&reals[curr_pstate],my_app);
		switch(error_comp_state){
		case 0:
      *new_freq=frequency_pstate_to_freq(min_pstate+1);
      debug("Selecting Special case freq %lu (%d)",*new_freq,min_pstate+1);
      *ready=1;
      error_comp_state++;
      return EAR_SUCCESS;
		case 1:compute_projection_error(min_pstate,min_pstate+1);
      error_comp_state++;
			break;
		default:break;
    }
#endif

		*ready=1;

		// If is not the default P_STATE selected in the environment, a projection
		// is made for the reference P_STATE in case the coefficents were available.
		if (curr_freq != def_freq) // Use configuration when available
		{
		if (projection_available(curr_pstate,def_pstate)==EAR_SUCCESS)
		{
				st=project_power(my_app,curr_pstate,def_pstate,&power_ref);
				st=project_time(my_app,curr_pstate,def_pstate,&time_ref);
				best_freq=def_freq;
		}
		else
		{
				time_ref=my_app->time;
				power_ref=my_app->DC_power;
				best_freq=curr_freq;
		}
		}
		// If it is the default P_STATE selected in the environment, then a projection
		// is not needed, so the signature will be enough as a reference. 
		else
		{ // we are running at default frequency , signature is our reference
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
          debug("projecting to nominal\t time: %.2lf\t power: %.2lf", time_ref, power_ref);
				}else{
        	time_ref=my_app->time;
        	power_ref=my_app->DC_power;
        	best_freq=curr_freq;
				}
			}

		}
		energy_ref=power_ref*time_ref;
		best_solution=energy_ref;
		energy_nominal=energy_ref;
		time_nominal=time_ref;

	// We compute the maximum performance loss
	time_max = time_ref + (time_ref * max_penalty);

   debug("Max_freq set to %lu min_pstate = %d nominal %lu curr_frequency %lu curr_pstate %lu time_max: %.2lf",c->app->max_freq,min_pstate,nominal,curr_freq,curr_pstate,time_max);

	debug("Policy Signature (CPI=%lf GBS=%lf Power=%lf Time=%lf TPI=%lf)",my_app->CPI,my_app->GBS,my_app->DC_power,my_app->time,my_app->TPI);

	// MIN_ENERGY_TO_SOLUTION ALGORITHM
	for (i = min_pstate; i < c->num_pstates;i++)
	{
		if (projection_available(curr_pstate,i)==EAR_SUCCESS)
		{
				st=project_power(my_app,curr_pstate,i,&power_proj);
				st=project_time(my_app,curr_pstate,i,&time_proj);
				projection_set(i,time_proj,power_proj);
				energy_proj=power_proj*time_proj;
				if ((i==curr_pstate) && (i==min_pstate)){
					ratios_energy[i]=1.0;
					ratios_time[i]=1.0;
				}else{
					ratios_energy[i]=((last_p-power_proj)/last_p)*100.0;
					ratios_time[i]=((time_proj-last_t)/last_t)*100.0;
				}
				last_p=power_proj;
				last_t=time_proj;

      debug("projected from %lu to %d\t time: %.2lf\t power: %.2lf energy: %.2lf energy_ratio %lf time_ratio %lf energy_nominal %lf time_proj %lf", curr_pstate, i, time_proj, power_proj, energy_proj,ratios_energy[i],ratios_time[i],energy_nominal,time_nominal);
			if ((ratios_energy[i]<(ratios_time[i]*MIN_EFF)) || (time_proj>time_max)){
				valid_ratio[i]=0;
			}else valid_ratio[i]=1;
			if (valid_ratio[i] && (energy_proj < best_solution) && (time_proj < time_max))
			{
          debug("new best solution found");
					best_freq = frequency_pstate_to_freq(i);
					best_solution = energy_proj;
			}
		}
	}
	}else{ 
		*ready=0;
		return EAR_ERROR;
	}
	*new_freq=best_freq;
	return EAR_SUCCESS;
}


state_t policy_ok(polctx_t *c,signature_t *curr_sig,signature_t *last_sig,int *ok)
{
	double energy_last, energy_curr;

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

