/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _db_DB_H
#define _db_DB_H

#include <types/application.h>

#define CURRENT_SIGNATURE(f) ((application_t *) &CURRENT_APP[PSTATE(f)])

// initializes the data structures to deal with app info. db_init allocates
// memory for the list of cpu p_states
// whole_app is no longer used (REMOVE)
void db_init(unsigned int whole_app,char *app_name);
// Executed at application end (empty)
// whole_app is no longer used (REMOVE)
void db_end(unsigned int whole_app);
// Returns a pointer to the information of the application with the current freq.
application_t * db_current_app();
// Asks ear_daemon to write the MY_APP info in the EAR_DB_NAME database file
void db_update_historical(unsigned int whole_app,application_t *MY_APP);

// Returns true if EAR_APP_NAME is defined. Anyway, my_name[out] is set with the
// application name: EAR_APP_NAME or PAPI provided name or UnknownApplication
// otherwise
int db_get_app_name(char *my_name);
// Asks the ear_daemon to change the frequency and updates current freq data.
unsigned long db_change_frequency(unsigned long f);
// Executed at new period (empty)
void db_new_period();
// Returns true if application signature A and B are different (considering a 
// maximum different of ACCEPTED_TH. It is used at policy validation
int db_signature_has_changed(application_t *A,application_t *B);

// Set/Get the different application fields
void db_set_policy(application_t *MY_APP,char *policy_name);
void db_set_th(application_t *MY_APP,double th);
void db_set_frequency(application_t *MY_APP,unsigned long f);
void db_set_default(application_t *MY_APP,unsigned long f);
void db_set_procs(application_t *MY_APP,unsigned int procs);
void db_set_GBS(application_t *MY_APP,double gbs);
void db_set_POWER(application_t *MY_APP,double power);
void db_set_TPI(application_t *MY_APP,double tpi);
void db_set_seconds(application_t *MY_APP,double seconds);
void db_set_CPI(application_t *MY_APP,double CPI);
void db_set_Gflops(application_t *MY_APP,double Gflops);
void db_set_EDP(application_t *MY_APP,double EDP);
void db_set_CYCLES(application_t *MY_APP,long long cycles);
void db_set_INSTRUCTIONS(application_t *MY_APP,long long instr);
void db_set_DRAM_POWER(application_t *MY_APP,double dram_power);
void db_set_PCK_POWER(application_t *MY_APP,double pck_power);
char * db_get_name(application_t *MY_APP);
unsigned long db_get_frequency(application_t *MY_APP);
unsigned int  db_get_procs(application_t *MY_APP);
double db_get_GBS(application_t *MY_APP);
double db_get_Gflops(application_t *MY_APP);
double db_get_EDP(application_t *MY_APP);
double db_get_POWER(application_t *MY_APP);
double db_get_TPI(application_t *MY_APP);
double db_get_CPI(application_t *MY_APP);
double db_get_seconds(application_t *MY_APP);

// When debug is on, it prints APP fields in the stderr
void db_print_app_info(application_t *APP);

#else
#endif
