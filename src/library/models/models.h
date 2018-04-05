/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

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
void init_power_models(uint p_states, ulong *p_states_list);

/** Executed when a new_loop is reported by dyanis */
void policy_new_loop();
/** Executed when a end_loop is reported by dyanis */
void policy_end_loop();

/** Given the computed application signature, applies the selected power policy
*   and returns the “optimal” frequency. */
ulong policy_power(uint whole_app, application_t *sig);


/** Given a performance projection, the actual application signature and the last
*   computed application signature values, returns true if policy decision has been
*   the correct ones. */
uint policy_ok(projection_t *proj, application_t *curr_sig, application_t *last_sig);


#else
#endif
