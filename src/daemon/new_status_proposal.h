/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

typedef node_info{
ulong avg_freq; // In KH
ulong temp; // In degres, No creo que haya falta enviar un unsigned long long
ulong power; // In Watts 
ulong max_freq;// in KH
} node_info_t;
typedef app_info{
uint job_id;
uint step_id;
}app_info_t;

typedef struct eard_policy_info{
    ulong freq; /* default freq in KH, divide by 1000000 to show Ghz */
    uint th;     /* th x 100, divide by 100 */
}eard_policy_info_t;



typedef struct status{
    unsigned int     ip;
    char    ok;
    eard_policy_info_t    policy_conf[TOTAL_POLICIES];
    node_info_t  node;
    app_info_t  app;
} status_t;
