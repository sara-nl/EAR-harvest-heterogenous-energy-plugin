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
#include <dlfcn.h>

#include <common/config.h>
#include <common/types/projection.h>
#include <common/output/verbose.h>

#define SHOW_DEBUGS 1

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
  "ear_model_init",
  "ear_model_project_time",
  "ear_model_project_power",
  "ear_model_projection_available",
};

static void models_set_symbols(void *obj, const char *nam[], void *sym[], int n)
{
  int i;

  for (i = 0; i < n; ++i) {
    sym[i] = dlsym(obj, nam[i]);
  }
}
static state_t models_load(char *obj_path)
{

  models_syms_obj = dlopen(obj_path, RTLD_LOCAL | RTLD_LAZY);

  if (!models_syms_obj) {
    error("Error loading models plugin '%s' (%s)", obj_path, dlerror());
    return EAR_ERROR;
  }

  models_set_symbols(models_syms_obj, models_syms_nam, (void **) &models_syms_fun, models_funcs_n);

  return EAR_SUCCESS;
}



state_t projections_init(conf_install_t *data,uint pstates)
{
  state_t st;
	char basic_path[SZ_PATH_INCOMPLETE];
  char *obj_path = getenv("SLURM_EAR_POWER_MODEL");
  if (obj_path==NULL){
		sprintf(basic_path,"%s/libbasic_model.so",data->dir_inst);
    obj_path=basic_path;
		debug("SLURM_EAR_POWER_MODEL not defined using default %s",obj_path);
  }
	st=models_load(obj_path);
  if (st==EAR_SUCCESS) return models_syms_fun.init(data->dir_conf,data->dir_temp,pstates);
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
