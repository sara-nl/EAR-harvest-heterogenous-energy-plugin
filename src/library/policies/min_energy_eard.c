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
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/types/risk.h>
#include <common/types/configuration/policy_conf.h>
#include <common/hardware/frequency.h>

#define WARNING1_INC 0.05
#define WARNING1_MAX_PSTATES_LESS 0
#define WARNING1_DEF_PSTATES_LESS 0

#define WARNING2_INC 0.05
#define WARNING2_MAX_PSTATES_LESS 1
#define WARNING2_DEF_PSTATES_LESS 1

#define PANIC_INC_TH 0
#define PANIC_MAX_PSTATES_LESS 1
#define PANIC_DEF_PSTATES_LESS 1


state_t policy_set_risk(policy_conf_t *ref,policy_conf_t *current,risk_t risk_level,ulong opt_target,ulong mfreq,ulong *nfreq,ulong *f_list,uint nump)
{
  debug("min_energy risk set to %lu",(unsigned long)risk_level);
	debug("current conf %lf def_pstate %u max_freq %lu",ref->settings[0],ref->p_state,mfreq);
  int risk1,risk2,panic;
	unsigned long f;

  risk1=is_risk_set(risk_level,WARNING1);
  risk2=is_risk_set(risk_level,WARNING2);
  panic=is_risk_set(risk_level,PANIC);
  if (risk1){
    debug("We are in risk1");
		current->settings[0]=ref->settings[0]+WARNING1_INC;
		current->p_state=ref->p_state+WARNING1_MAX_PSTATES_LESS;
  }
  if (risk2){
    debug("We are in risk2");
		current->settings[0]=ref->settings[0]+WARNING1_INC+WARNING2_INC;
		current->p_state=ref->p_state+WARNING1_MAX_PSTATES_LESS+WARNING2_MAX_PSTATES_LESS;
  }
  if (panic){
		current->settings[0]=ref->settings[0]+WARNING1_INC+WARNING2_INC+PANIC_INC_TH;
		current->p_state=ref->p_state+WARNING1_MAX_PSTATES_LESS+WARNING2_MAX_PSTATES_LESS+PANIC_MAX_PSTATES_LESS;
  	debug("We are in panic");
  }
	f=frequency_pstate_to_freq_list(current->p_state,f_list,nump);
	current->def_freq=(float)f/1000000.0;
  *nfreq=f;
	debug("new conf %lf def_pstate %u def_freq %f max_freq %lu",current->settings[0],current->p_state,current->def_freq,*nfreq);
  return EAR_SUCCESS;
}

