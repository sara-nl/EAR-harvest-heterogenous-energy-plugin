/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _EAR_GLOBAL_H
#define _EAR_GLOBAL_H

#define MAX_APP_NAME 1024

extern char ear_node_name[MAX_APP_NAME];
extern char ear_app_name[MAX_APP_NAME];
extern char ear_policy_name[MAX_APP_NAME];
extern unsigned long EAR_default_frequency;
extern unsigned long ear_frequency;
extern unsigned int EAR_default_pstate;
extern unsigned int ear_resources;
extern int power_model_policy;
extern int ear_use_turbo;
extern int ear_whole_app;
extern int ear_papi_init;
extern int ear_my_rank;
extern int report;

#else
#endif
