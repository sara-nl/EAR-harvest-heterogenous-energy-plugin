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
#include <control/frequency.h>

#include <library/models/models_api.h>


#define SHOW_DEBUGS 1
#ifdef SHOW_DEBUGS
#define debug(...) fprintf(stderr, __VA_ARGS__); 
#else
#define debug(...) 
#endif


static coefficient_t **coefficients;
static coefficient_t *coefficients_sm;
static int num_coeffs;
static uint num_pstates;
static uint basic_model_init=0;


#define MAX_FREQ_AVX512 2300000
#define MAX_FREQ_AVX2 2900000
unsigned long  min(unsigned long x,unsigned long y)
{
	if (x<y) return x;
	return y;
}
	

static double perc_avx512(signature_t *sign)
{
	return (double)(sign->FLOPS[3]+sign->FLOPS[7])/ (double)sign->instructions;	
}
static double perc_avx2(signature_t *sign)
{
	return (double)(sign->FLOPS[2]+sign->FLOPS[6])/ (double)sign->instructions;	
}
static double perc_basic(signature_t *sign)
{
	unsigned long long non_avx;
	non_avx=sign->FLOPS[3]+sign->FLOPS[7]+sign->FLOPS[2]+sign->FLOPS[6];
	return (double)(sign->instructions-non_avx)/(double)sign->instructions;
}

static int valid_range(ulong from,ulong to)
{
	if ((from<num_pstates) && (to<num_pstates)) return 1;
	else return 0;
}

/* This function loads any information needed by the energy model */
state_t model_init(char *etc,char *tmp,uint pstates)
{
  char coeff_file[128];
  char coeff_default_file[128];
  char coeff_file_fn[128];
  int begin_pstate, end_pstate;
  int i, ref;

	debug("Using vector model\n");

	num_pstates=pstates;

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
      }
    }
  }
	basic_model_init=1;	
	return EAR_SUCCESS;
}

double default_project_cpi(signature_t *sign, coefficient_t *coeff)
{
	return (coeff->D * sign->CPI) +
		   (coeff->E * sign->TPI) +
		   (coeff->F);
}

static state_t project_time(signature_t *sign,ulong from,ulong to,double *ptime)
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

state_t model_project_time(signature_t *sign,ulong from,ulong to,double *ptime)
{
	double ptime_avx512,ptime_avx2,ptime_basic;
	project_time(sign,from,min(MAX_FREQ_AVX512,to),&ptime_avx512);
	project_time(sign,from,min(MAX_FREQ_AVX2,to),&ptime_avx2);
	project_time(sign,from,to,&ptime_basic);
	*ptime=ptime_avx512*perc_avx512(sign)+ptime_avx2*perc_avx2(sign)+ptime_basic*perc_basic(sign);
	debug("project_time:avx512 %.3lf avx2 %.3lf basic %.3lf\n",perc_avx512(sign),perc_avx2(sign),perc_basic(sign));
	return EAR_SUCCESS;
}

static state_t project_power(signature_t *sign, ulong from,ulong to,double *ppower)
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
state_t model_project_power(signature_t *sign, ulong from,ulong to,double *ppower)
{
	double ppower_avx512,ppower_avx2,ppower_basic;
	project_power(sign,from,min(MAX_FREQ_AVX512,to),&ppower_avx512);
	project_power(sign,from,min(MAX_FREQ_AVX2,to),&ppower_avx2);
	project_power(sign,from,to,&ppower_basic);
	*ppower=ppower_avx512*perc_avx512(sign)+ppower_avx2*perc_avx2(sign)+ppower_basic*perc_basic(sign);
	debug("project_power:avx512 %.3lf avx2 %.3lf basic %.3lf\n",perc_avx512(sign),perc_avx2(sign),perc_basic(sign));
	return EAR_SUCCESS;
}

state_t model_projection_available(ulong from,ulong to)
{
	if (coefficients[from][to].available) return EAR_SUCCESS;
	else EAR_ERROR;
}

