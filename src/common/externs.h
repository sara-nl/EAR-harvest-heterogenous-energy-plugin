/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_GLOBAL_H
#define _EAR_GLOBAL_H
// ear_db
extern char ear_node_name[MAX_APP_NAME]; //ear_states
extern double performance_penalty_th;
extern unsigned long EAR_default_frequency; // ear_models
extern int ear_my_rank; // ear_models ear_states ear_papi

// ear_models
extern char ear_app_name[MAX_APP_NAME]; // ear_states
extern unsigned long ear_frequency; // ear_states ear_papi
extern unsigned int EAR_default_pstate;
extern int ear_use_turbo;
extern int ear_whole_app;

// ear_states
extern int report;

// ear_papi
extern int ear_use_turbo;
extern int ear_resources;
extern int power_model_policy;
extern char ear_policy_name[MAX_APP_NAME];
#else
#endif
