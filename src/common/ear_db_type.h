/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_DB_TYPE_H
#define _EAR_DB_TYPE_H

#define MAX_APP_NAME 1024
#define BUFFSIZE 128

#define FLOPS_EVENTS 8

struct App_info_extended{
		long long L1_misses;
		long long L2_misses;
		long long L3_misses;
		long long FLOPS[FLOPS_EVENTS];
};

struct App_info {
		// App identification: job;node;user;app
        	char job_id[MAX_APP_NAME];
        	char node_id[MAX_APP_NAME];
        	char user_id[MAX_APP_NAME];
        	char app_id[MAX_APP_NAME];
		char policy[MAX_APP_NAME];
		double th;
		unsigned long f;
		unsigned int procs;
		double seconds;
        	double GIPS_f0;
        	double GBS_f0;
        	double POWER_f0;
        	double TPI_f0;
		double CPI;
		unsigned int nominal;
		// Extended data
		long long CYCLES;
		long long INSTRUCTIONS;
		double POWER_DC;
		double DRAM_POWER;
		double PCK_POWER;
		double Gflops;
		double EDP;
};

#define NOMINAL_POWER(app) app->POWER_f0
#define NOMINAL_TPI(app) app->TPI_f0
#define NOMINAL_CPI(app) app->CPI
#define NOMINAL_TIME(app) app->seconds


#else
#endif
