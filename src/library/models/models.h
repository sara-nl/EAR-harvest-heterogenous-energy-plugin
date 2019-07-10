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



#ifndef EAR_MODELS_H_
#define EAR_MODELS_H_

#include <common/environment.h>
#include <common/types/coefficient.h>
#include <common/types/projection.h>
#include <common/types/application.h>


/** Selects policy based on EAR_POWER_POLICY */
void init_power_policy();

/** Opens Coefficient db and reads coefficients */
void init_power_models();

/** Executed when a new_loop is reported by dyanis */
void policy_new_loop();
/** Executed when a end_loop is reported by dyanis */
void policy_end_loop();

/** Given the computed application signature, applies the selected power policy
*   and returns the “optimal” frequency. */
ulong policy_power(uint whole_app, signature_t *sig,int *ready);

/** Given a performance projection, the actual application signature and the last
*   computed application signature values, returns true if policy decision has been
*   the correct ones. */
uint policy_ok(projection_t *proj, signature_t *curr_sig, signature_t *last_sig);

/** Executed when we have done several "tries" and the policy is not able to select the "good" freq for this app
*/
ulong policy_default_configuration();

/** This function sets the frequency "by hand" when  using global synchronizations */
void force_global_frequency(ulong new_f);

uint get_global_def_pstate();

ulong get_global_def_freq();

double get_global_th();

/* */
void policy_global_reconfiguration();

/* Returns the maximum tries per policy */
int policy_max_tries();

/* Returns the default freq for the current policy */
ulong policy_get_default_freq();

/** */
uint get_global_min_pstate();

#endif
