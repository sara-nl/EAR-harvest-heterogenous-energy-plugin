/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef EAR_MODELS_H_
#define EAR_MODELS_H_

#include <ear_db_type.h>
#include <ear_configuration.h>

#if 0
#define MIN_ENERGY_TO_SOLUTION 0
#define MIN_TIME_TO_SOLUTION 1
#define MONITORING_ONLY 2
#endif


struct Coefficients_info{
unsigned long pstate;
unsigned int available;
// For power projection
double A;
double B;
double C;
// For CPI projection
double D;
double E;
double F;
};

struct PerfProjection{
	double Time;
	double Power;
	double CPI;
};

void set_performance_projection(int i,double TP,double PP, double CPI);
void reset_performance_projection(unsigned int p_states);

// Selects policy based on EAR_POWER_POLICY
void init_power_policy();
// Opens Coefficient db and reads coefficients
void init_power_models(unsigned int p_states,unsigned long *p_states_list);
void models_new_period();

// Given the computed application signature, applies the selected power policy and returns the “optimal” frequency.
unsigned long policy_power(unsigned int whole_app,struct App_info* MY_SIGNATURE);
// Internal to ear_models, it is used byy policy_power
unsigned long policy_power_for_application(unsigned int whole_app,struct App_info * SIGNATURE);
//Computes the optimal frequency for a given th and application signature when using min_energy. bestPP and bestTP are updated with power and time projections
unsigned long optimal_freq_min_energy(double th,struct App_info * SIGNATURE,double *bestPP,double *bestTP);

// Given a performance projection and an application signature returns true if they are “equal”
unsigned int performance_projection_ok(struct PerfProjection *PREDICTION,struct App_info *SIGNATURE);
// Given a performance projection, the actual application signature and the last computed application signature values, returns true if policy decision has been the correct ones.
unsigned int policy_ok(struct PerfProjection *PREDICTION,struct App_info *SIGNATURE,struct App_info *LAST_SIGNATURE);
// Returns the performance projection for a given frequency
struct PerfProjection * performance_projection(unsigned long f);


#else
#endif
