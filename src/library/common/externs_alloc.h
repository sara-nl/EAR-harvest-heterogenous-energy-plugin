/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <linux/limits.h>
#include <common/types/application.h>
#include <common/shared_configuration.h>


loop_t loop;
application_t loop_signature;
char loop_summary_path[PATH_MAX];

application_t application;
char app_summary_path[PATH_MAX];

unsigned long EAR_default_frequency; // TODO: why ulong?
unsigned long ear_frequency; // TODO: why ulong?
int ear_use_turbo = USE_TURBO; // TODO: review
int ear_whole_app;
int ear_my_rank;
int report; // TODO: what is its utility?

int EAR_VERBOSE_LEVEL;

ear_conf_t *system_conf;
// TODO: CLEAN
char ear_app_name[PATH_MAX]; //TODO: use application.app_id
