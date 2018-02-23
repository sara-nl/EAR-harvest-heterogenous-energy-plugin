/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _EAR_GLOBAL_H
#define _EAR_GLOBAL_H

#include <common/types/application.h>
#include <linux/limits.h>

extern application_t *application;
extern char loop_summary_path[PATH_MAX];
extern char app_summary_path[PATH_MAX];
extern unsigned long ear_frequency;
extern unsigned int ear_resources;
extern int power_model_policy;
extern int ear_use_turbo;
extern int ear_whole_app;
extern int ear_papi_init;
extern int ear_my_rank;
extern int report;

#define MAX_APP_NAME 1024
extern char ear_node_name[MAX_APP_NAME]; //TODO: CLEAN
extern char ear_app_name[MAX_APP_NAME]; //TODO: CLEAN
extern char ear_policy_name[MAX_APP_NAME]; //TODO: CLEAN
extern unsigned long EAR_default_frequency; //TODO: CLEAN
extern unsigned int EAR_default_pstate; //TODO: CLEAN
extern double performance_penalty_th; //TODO: CLEAN (from ear_models)

#else
#endif
