/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _db_DB_H
#define _db_DB_H

#include <types.h>

#define CURRENT_SIGNATURE(f) ((struct App_info *) &CURRENT_APP[PSTATE(f)])

// initializes the data structures to deal with app info. db_init allocates
// memory for the list of cpu p_states
// whole_app is no longer used (REMOVE)
void db_init(unsigned int whole_app,char *app_name);
// Executed at application end (empty)
// whole_app is no longer used (REMOVE)
void db_end(unsigned int whole_app);
// Returns a pointer to the information of the application with the current freq.
struct App_info * db_current_app();
// Asks ear_daemon to write the MY_APP info in the EAR_DB_NAME database file
void db_update_historical(unsigned int whole_app,struct App_info *MY_APP);
void db_update_historical_extended(unsigned int whole_app,struct App_info_extended *MY_APP);

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
int db_signature_has_changed(struct App_info *A,struct App_info *B);


// Set/Get the different application fields
void db_set_policy(struct App_info *MY_APP,char *policy_name);
void db_set_th(struct App_info *MY_APP,double th);
void db_set_frequency(struct App_info *MY_APP,unsigned long f);
void db_set_default(struct App_info *MY_APP,unsigned long f);
void db_set_procs(struct App_info *MY_APP,unsigned int procs);
void db_set_GBS(struct App_info *MY_APP,double gbs);
void db_set_GIPS(struct App_info *MY_APP,double gips);
void db_set_POWER(struct App_info *MY_APP,double power);
void db_set_TPI(struct App_info *MY_APP,double tpi);
void db_set_seconds(struct App_info *MY_APP,double seconds);
void db_set_CPI(struct App_info *MY_APP,double CPI);
void db_set_Gflops(struct App_info *MY_APP,double Gflops);
void db_set_EDP(struct App_info *MY_APP,double EDP);
void db_set_CYCLES(struct App_info *MY_APP,long long cycles);
void db_set_INSTRUCTIONS(struct App_info *MY_APP,long long instr);
void db_set_POWER_DC(struct App_info *MY_APP,double power_dc);
void db_set_DRAM_POWER(struct App_info *MY_APP,double dram_power);
void db_set_PCK_POWER(struct App_info *MY_APP,double pck_power);
char * db_get_name(struct App_info *MY_APP);
unsigned long db_get_frequency(struct App_info *MY_APP);
unsigned int  db_get_procs(struct App_info *MY_APP);
double db_get_GBS(struct App_info *MY_APP);
double db_get_Gflops(struct App_info *MY_APP);
double db_get_EDP(struct App_info *MY_APP);
double db_get_GIPS(struct App_info *MY_APP);
double db_get_POWER(struct App_info *MY_APP);
double db_get_TPI(struct App_info *MY_APP);
double db_get_CPI(struct App_info *MY_APP);
double db_get_seconds(struct App_info *MY_APP);

// When debug is on, it prints APP fields in the stderr
void db_print_app_info(struct App_info *APP);

#else
#endif
