/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_CONFIGURATION_H_
#define _EAR_CONFIGURATION_H_

#define DEFAULT_LEARNING_PHASE          0
#define DEFAULT_RESET_FREQ              0
#define DEFAULT_VERBOSE                 0
#define DEFAULT_DB_PATHNAME             ".ear_system_db"
#define DEFAULT_USER_DB_PATHNAME        ".ear_user_db"
#define DEFAULT_COEFF_PATHNAME          "COEFFICIENTS/COEFFS_"
#define DEFAULT_POWER_POLICY            0
#define DEFAULT_MIN_ENERGY_TH           0.1
#define DEFAULT_MIN_TIME_TH             PERFORMANCE_GAIN
#define DEFAULT_PERFORMANCE_ACURACY     2000000
#define DEFAULT_VERBOSE                 0
#define DEFAULT_MAX_P_STATE             1
#define DEFAULT_MIN_P_STATE             EAR_MIN_P_STATE
#define DEFAULT_P_STATE                 DEFAULT_MAX_P_STATE
#define DEFAULT_DYNAIS_LEVELS           4
#define DEFAULT_DYNAIS_WINDOW_SIZE      300
#define DYNAIS_TIMEOUT                  30
#define MIN_ENERGY_TO_SOLUTION          0
#define MIN_TIME_TO_SOLUTION            1
#define MONITORING_ONLY                 2

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
int getenv_ear_dynais_levels();
int getenv_ear_dynais_window_size();

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
int get_ear_dynais_levels();
int get_ear_dynais_window_size();

void ear_lib_environment();
void ear_print_lib_environment();
void ear_daemon_environment();
void ear_print_daemon_environment();

int get_total_resources();

#else
#endif
