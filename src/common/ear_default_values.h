/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/


#ifndef _EAR_DEFAULT_H_
#define _EAR_DEFAULT_H_

#define DEFAULT_LEARNING_PHASE 	0
#define DEFAULT_RESET_FREQ 		0
#define DEFAULT_VERBOSE			0
#define DEFAULT_DB_PATHNAME		".ear_system_db"
#define DEFAULT_USER_DB_PATHNAME ".ear_user_db"
#define DEFAULT_COEFF_PATHNAME 	"COEFFICIENTS/COEFFS_"
#define DEFAULT_POWER_POLICY 	0
#define DEFAULT_MIN_ENERGY_TH 	0.1
#define DEFAULT_MIN_TIME_TH PERFORMANCE_GAIN
#define DEFAULT_PERFORMANCE_ACURACY 1000000
#define DEFAULT_VERBOSE			0
#define DEFAULT_MAX_P_STATE		1
#define DEFAULT_MIN_P_STATE EAR_MIN_P_STATE
#define DEFAULT_P_STATE			DEFAULT_MAX_P_STATE
#define DYNAIS_TIMEOUT			30
#define DEFAULT_DYNAIS_LEVELS		4
#define DEFAULT_DYNAIS_WINDOW_SIZE	300


#define MIN_ENERGY_TO_SOLUTION 0
#define MIN_TIME_TO_SOLUTION 1
#define MONITORING_ONLY 2


#else
#endif
