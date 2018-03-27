/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _EAR_GLOBAL_H
#define _EAR_GLOBAL_H

#include <linux/limits.h>
#include <common/types/application.h>
#include <common/shared_configuration.h>

extern loop_t loop;
extern application_t loop_signature;
extern char loop_summary_path[PATH_MAX];

extern application_t application;
extern char app_summary_path[PATH_MAX];

extern unsigned long EAR_default_frequency;
extern unsigned long ear_frequency;
extern int ear_use_turbo;
extern int ear_whole_app;
extern int ear_my_rank;
extern int my_job_id;
extern uint dynais_enabled;
extern uint loop_with_signature;
// To be removed with new dynais version
extern ulong last_first_event;
extern ulong last_calls_in_loop;
extern ulong last_loop_size;
extern ulong last_loop_level;


extern ear_conf_t *system_conf;
extern char ear_app_name[PATH_MAX];

#else
#endif
