/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_TRACES_H_
#define _EAR_TRACES_H_

/*
        timestamp:1:            Period id
        timestamp:2:            Period length
        timestamp:3:            Period iterations
        timestamp:4:            Period time
        timestamp:5:            Period CPI
        timestamp:6:            Period TPI
        timestamp:7:            Period GBs
        timestamp:8:            Period power
        timestamp:9:            Period time projection
        timestamp:10:           Period CPI projection
        timestamp:11:           Period power projection
        timestamp:12:           Frequency
*/

#include <types.h>

#define PERIOD_ID               1
#define PERIOD_LENGTH           2
#define PERIOD_ITERATIONS       3
#define PERIOD_TIME		4
#define PERIOD_CPI		5
#define PERIOD_TPI		6
#define PERIOD_GBS		7
#define PERIOD_POWER		8
#define PERIOD_TIME_PROJ	9
#define PERIOD_CPI_PROJ		10
#define PERIOD_POWER_PROJ	11
#define PERIOD_FREQ		12
#define APP_ENERGY		13


#define MIN_FREQ_FOR_SAMPLING 500000
#ifdef EAR_GUI

// Executed at application start/end
void traces_init(int gwho,int lwho,char *appname,char *nodename,int nodes, int mpis,int ppnode);
void traces_end(int gwho,int lwho,unsigned long int total_ener);

//Executed when application signature is computed at EVALUATING_SIGNATURE and SIGANTURE_STABLE states
void traces_new_signature(int gwho,int lwho,double seconds,double CPI,double TPI,double GBS,double POWER);
void traces_frequency(int gwho, int lwho,unsigned long f);
void traces_PP(int gwho,int lwho,double seconds,double CPI,double POWER);

// Executed when each time a new loop is detected, the loop ends, or a new iteration are reported
void traces_new_n_iter(int gwho,int lwho,int period_id,int period_size, int iterations,int my_state);
void traces_new_period(int gwho,int lwho,int period_id);
void traces_end_period(int gwho,int lwho);

// Executed at each mpi_call
void traces_mpi_call(int gwho,int lwho, ulong timestamp, ulong event, ulong arg1,ulong arg2,ulong arg3);

#else
#define traces_init(g,l,a,n,n1,m,p)
#define traces_new_n_iter(g,l,p,s,i,st)
#define traces_end(g,l,e)
#define traces_new_signature(g,l,s,c,t,gb,p)
#define traces_frequency(g,l,f)
#define traces_PP(g,l,s,c,p)
#define traces_new_period(g,l,p)
#define traces_end_period(g,l)
#define traces_mpi_call(g,l,t,e,a1,a2,a3);
#endif

#else
#endif
