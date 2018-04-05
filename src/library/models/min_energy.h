/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
/**
*    \file min_energy.h
*    \brief This file implements the policy API for the minimize_energy_to_solution policy
*
*/

#ifndef _MIN_ENERGY_H
#define _MIN_ENERGY_H

void min_energy_init(uint num_pstates);
void min_energy_new_loop();
void min_energy_end_loop();
ulong min_energy_policy(application_t *sig);
ulong min_energy_policy_ok(projection_t *proj, application_t *curr_sig, application_t *last_sig);

#else
#endif
