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
#include <common/states.h>
#include <common/types/signature.h>
#include <daemon/shared_configuration.h>
#include <common/hardware/frequency.h>
#include <common/hardware/architecture.h>

#include <library/models/models_api.h>


static coefficient_t **coefficients;
static coefficient_t *coefficients_sm;
static int num_coeffs;
static uint num_pstates;
static uint basic_model_init=0;
#define SHOW_DEBUGS 0
#ifdef SHOW_DEBUGS
#define debug(...) fprintf(stderr, __VA_ARGS__); 
#else
#define debug(...) 
#endif

static int valid_range(ulong from,ulong to)
{
	if ((from<num_pstates) && (to<num_pstates)) return 1;
	else return 0;
}

/* This function loads any information needed by the energy model */
state_t model_init(char *etc,char *tmp,architecture_t *myarch)
{
  char coeff_file[128];
  char coeff_default_file[128];
  char coeff_file_fn[128];
  int begin_pstate, end_pstate;
  int i, ref;

	debug("Using basic_model\n");
	num_pstates=(uint)myarch->pstates;
	debug("Using %u pstates",num_pstates);

  coefficients = (coefficient_t **) malloc(sizeof(coefficient_t *) * num_pstates);
  if (coefficients == NULL) {
		return EAR_ERROR;
  }
  for (i = 0; i < num_pstates; i++)
  {
    coefficients[i] = (coefficient_t *) malloc(sizeof(coefficient_t) * num_pstates);
    if (coefficients[i] == NULL) {
			return EAR_ERROR;
    }

    for (ref = 0; ref < num_pstates; ref++)
    {

      coefficients[i][ref].pstate_ref = frequency_pstate_to_freq(i);
      coefficients[i][ref].pstate = frequency_pstate_to_freq(ref);
      coefficients[i][ref].available = 0;
    }
  }


  char coeffs_path[GENERIC_NAME];
  get_coeffs_default_path(tmp,coeffs_path);
  num_coeffs=0;
  coefficients_sm=attach_coeffs_default_shared_area(coeffs_path,&num_coeffs);
  if (num_coeffs>0){
    num_coeffs=num_coeffs/sizeof(coefficient_t);
    int ccoeff;
    for (ccoeff=0;ccoeff<num_coeffs;ccoeff++){
      ref=frequency_freq_to_pstate(coefficients_sm[ccoeff].pstate_ref);
      i=frequency_freq_to_pstate(coefficients_sm[ccoeff].pstate);
      if (frequency_is_valid_pstate(ref) && frequency_is_valid_pstate(i)){
				memcpy(&coefficients[ref][i],&coefficients_sm[ccoeff],sizeof(coefficient_t));
                debug("initializing coeffs for ref: %d i: %d\n", ref, i);
      }
    }
  }
	basic_model_init=1;	
    for (ref = 0; ref < num_pstates; ref++)
    {
        for (i = 0; i < num_pstates; i++)
                debug("coefficient from ref: %d i: %d available: %d\n", ref, i, coefficients[ref][i].available);
    }
	return EAR_SUCCESS;
}

double default_project_cpi(signature_t *sign, coefficient_t *coeff)
{
	return (coeff->D * sign->CPI) +
		   (coeff->E * sign->TPI) +
		   (coeff->F);
}

state_t model_project_time(signature_t *sign,ulong from,ulong to,double *ptime)
{
	state_t st=EAR_SUCCESS;
	coefficient_t *coeff;
	if ((basic_model_init) && (valid_range(from,to))){
		coeff=&coefficients[from][to];
		if (coeff->available){
			double proj_cpi = default_project_cpi(sign, coeff);
			double freq_src = (double) coeff->pstate_ref;
			double freq_dst = (double) coeff->pstate;

			*ptime= ((sign->time * proj_cpi) / sign->CPI) *
		   (freq_src / freq_dst);
		}else{
			*ptime=0;
			st=EAR_ERROR;
		}
	}else st=EAR_ERROR;
	return st;
}

state_t model_project_power(signature_t *sign, ulong from,ulong to,double *ppower)
{
	state_t st=EAR_SUCCESS;
	coefficient_t *coeff;
	if ((basic_model_init) && (valid_range(from,to))){
		coeff=&coefficients[from][to];
		if (coeff->available){
			*ppower= (coeff->A * sign->DC_power) +
		   (coeff->B * sign->TPI) +
		   (coeff->C);
		}else{
			*ppower=0;
			st=EAR_ERROR;
		}
	}else st=EAR_ERROR;
	return st;
}

state_t model_projection_available(ulong from,ulong to)
{
    debug("checking if coefficient from %lu to %lu is available: %lu\n", from, to, coefficients[from][to].available);
	if (valid_range(from, to) && coefficients[from][to].available) return EAR_SUCCESS;
	else return EAR_ERROR;
}

