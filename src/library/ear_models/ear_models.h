/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef EAR_MODELS_H_
#define EAR_MODELS_H_

#include <environment.h>
#include <types/projection.h>
#include <types/application.h>

void set_performance_projection(int i, double TP, double PP, double CPI);

void reset_performance_projection(uint p_states);

// Selects policy based on EAR_POWER_POLICY
void init_power_policy();

// Opens Coefficient db and reads coefficients
void init_power_models(uint p_states, ulong *p_states_list);

void models_new_period();

// Given the computed application signature, applies the selected power policy
// and returns the “optimal” frequency.
ulong policy_power(uint whole_app, application_t *MY_SIGNATURE);

// Internal to ear_models, it is used byy policy_power
ulong policy_power_for_application(uint whole_app, application_t *SIGNATURE);

//Computes the optimal frequency for a given th and application signature when
// using min_energy. bestPP and bestTP are updated with power and time projections
ulong optimal_freq_min_energy(double th, application_t *SIGNATURE, double *bestPP, double *bestTP);

// Given a performance projection and an application signature returns true if
// they are “equal”
uint performance_projection_ok(projection_t *PREDICTION, application_t *SIGNATURE);

// Given a performance projection, the actual application signature and the last
// computed application signature values, returns true if policy decision has been
// the correct ones.
uint policy_ok(projection_t *PREDICTION, application_t *SIGNATURE, application_t *LAST_SIGNATURE);

// Returns the performance projection for a given frequency
projection_t *performance_projection(ulong f);


#else
#endif
