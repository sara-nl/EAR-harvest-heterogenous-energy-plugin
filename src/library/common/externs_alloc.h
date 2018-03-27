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

ulong EAR_default_frequency; 
ulong ear_frequency; 
int ear_use_turbo = USE_TURBO; 
int ear_whole_app;
int ear_my_rank;
int my_job_id;
uint dynais_enabled=1;
uint loop_with_signature=0;
// To be removed whith new dynais version
ulong last_first_event;
ulong last_calls_in_loop;
ulong last_loop_size;
ulong last_loop_level;

int EAR_VERBOSE_LEVEL;

ear_conf_t *system_conf;
char ear_app_name[PATH_MAX]; //TODO: use application.app_id
