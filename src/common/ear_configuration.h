/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_CONFIGURATION_H_
#define _EAR_CONFIGURATION_H_
#include <ear_default_values.h>

char * getenv_ear_tmp();
char * getenv_ear_install_pathname();
char * getenv_ear_db_pathname();
char * getenv_ear_user_db_pathname();
char * getenv_ear_gui_pathname();
char * getenv_ear_coeff_db_pathname();
char * getenv_ear_app_name();
void set_ear_app_name(char *name);
int getenv_ear_learning_phase();
int getenv_ear_power_policy();
double getenv_ear_power_policy_th();
int getenv_ear_reset_freq();
unsigned long getenv_ear_p_state();
double getenv_ear_performance_accuracy();
int getenv_ear_verbose();
int getenv_ear_local_id();
int getenv_ear_num_nodes();


char * get_ear_tmp();
void set_ear_tmp(char *new_tmp);
char * get_ear_install_pathname();
char * get_ear_db_pathname();
char * get_ear_user_db_pathname();
char * get_ear_gui_pathname();
char * get_ear_coeff_db_pathname();
char * get_ear_app_name();
int get_ear_learning_phase();
int get_ear_power_policy();
double get_ear_power_policy_th();
int get_ear_reset_freq();
unsigned long get_ear_p_state();
double get_ear_performance_accuracy();
int get_ear_verbose();
void set_ear_verbose(int verb);
int get_ear_num_nodes();
void set_ear_total_processes(int procs);
int get_ear_total_processes();


void ear_lib_environment();
void ear_print_lib_environment();
void ear_daemon_environment();
void ear_print_daemon_environment();

#else
#endif
