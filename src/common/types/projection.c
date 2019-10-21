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
#include <dlfcn.h>
#include <common/config.h>
#include <common/types/projection.h>
#include <common/system/symplug.h>
#include <common/output/verbose.h>
#include <common/types/configuration/cluster_conf.h>


#define freturn(call, ...) \
   { \
   if (call == NULL) { \
       return EAR_SUCCESS; \
   } \
   return call (__VA_ARGS__); \
   }

typedef struct model_symbols {
  state_t (*init)        (char *etc,char *tmp,uint pstates);
  state_t (*project_time)     (signature_t *sign,ulong from,ulong to,double *ptime);
  state_t (*project_power)    (signature_t *sign,ulong from,ulong to,double *ppower);
  state_t (*projection_available)    (ulong from,ulong to);
} models_sym_t;



// Static data
static models_sym_t models_syms_fun;
static void    *models_syms_obj = NULL;
const int       models_funcs_n = 4;
const char     *models_syms_nam[] = {
  "model_init",
  "model_project_time",
  "model_project_power",
  "model_projection_available",
};

static state_t models_load(char *obj_path)
{
	return symplug_open(obj_path,(void **) &models_syms_fun, models_syms_nam,  models_funcs_n);
}

state_t projections_init(uint user_type, conf_install_t *data, uint pstates)
{
	char basic_path[SZ_PATH_INCOMPLETE];
	char *obj_path = getenv("SLURM_EAR_POWER_MODEL");
	state_t st;

	if (data->obj_power_model!=NULL) debug("obj_power_model defined with %s\n",data->obj_power_model);
	if (data->obj_power_model== NULL) debug("obj_power_model NULL\n");
	if ((obj_path == NULL) || (user_type != AUTHORIZED))
	{
		if ((strcmp(data->obj_power_model,"default")==0) || (data->obj_power_model==NULL)){
			sprintf(basic_path, "%s/models/basic_model.so", data->dir_plug);
		}else if (data->obj_power_model!=NULL){
			sprintf(basic_path, "%s/models/%s.so", data->dir_plug,data->obj_power_model);
		}
		obj_path = basic_path;
		
	}
	debug("Using power model path  %s", obj_path);
	
	st = models_load(obj_path);
	
	if (st == EAR_SUCCESS) {
		freturn(models_syms_fun.init,data->dir_conf, data->dir_temp, pstates);
	}else{
		debug("Error when loading shared object %s",obj_path);
	}
	
	return st;
}

// Projections
state_t project_time(signature_t *sign, ulong from,ulong to,double *ptime)
{
	freturn(models_syms_fun.project_time,sign,from,to,ptime);
}

state_t project_power(signature_t *sign, ulong from,ulong to,double *ppower)
{
	freturn(models_syms_fun.project_power,sign,from,to,ppower);
}

state_t projection_available(ulong from,ulong to)
{
	freturn(models_syms_fun.projection_available,from,to);
}

// Inherited
static projection_t *projections;
static int proj_pstates=0;

void projection_create(uint p_states)
{
	// Projections allocation
	projections = (projection_t *) malloc(sizeof(projection_t) * p_states);

	proj_pstates=p_states;

	if (projections == NULL) {
		//verbose(0, "EAR: Error allocating memory for perf. projections\n");
		exit(1);
	}
}

projection_t *projection_get(int pstate)
{
	if (pstate>proj_pstates) return NULL;
	return &projections[pstate];
}

void projection_set(int i, double TP, double PP)
{
	projections[i].Time=TP;
	projections[i].Power=PP;
}

void projection_reset(uint p_states)
{
	int i;
	for (i=0;i<p_states;i++){
		projections[i].Time=0;
		projections[i].Power=0;
	}
}

double basic_project_cpi(signature_t *sign, coefficient_t *coeff)
{
  return (coeff->D * sign->CPI) +
       (coeff->E * sign->TPI) +
       (coeff->F);
}

double basic_project_time(signature_t *sign,coefficient_t *coeff)
{
	double ptime=0;
  if (coeff->available){
      double proj_cpi = basic_project_cpi(sign, coeff);
      double freq_src = (double) coeff->pstate_ref;
      double freq_dst = (double) coeff->pstate;

      ptime= ((sign->time * proj_cpi) / sign->CPI) *
       (freq_src / freq_dst);
  }
	return ptime;
}

double basic_project_power(signature_t *sign, coefficient_t *coeff)
{
	double ppower=0;
  if (coeff->available){
      ppower= (coeff->A * sign->DC_power) +
       (coeff->B * sign->TPI) +
       (coeff->C);
  }
	return ppower;
}

