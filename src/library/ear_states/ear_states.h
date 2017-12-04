/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_BASE_H_
#define _EAR_BASE_H
//Executed at application start. Reads EAR configuration env. variables
void  states_begin_job(int my_id,FILE *ear_fd,char *app_name);
//Executed at NEW_LOOP or END_NEW_LOOP DynAIS event
void  states_begin_period(int my_id,FILE *ear_fd,unsigned long new_period,unsigned int size);
//Executed at NEW_ITERATION DynAIS event
void  states_begin_new_iteration(int my_id,FILE *ear_fd,unsigned int size,int iterations,unsigned long event,unsigned int level);
//Executed at END_LOOP or END_NEW_LOOP DynAIS event
void  states_end_period(int my_id,FILE *ear_fd,unsigned int size,int iterations,unsigned long event);
//Executed at application end
void  states_end_job(int my_id,FILE *ear_fd,char *app_name);
// Returns the current EAR state
int   states_my_state();

#define NO_PERIOD 0
#define FIRST_ITERATION 1
#define EVALUATING_SIGNATURE 2
#define SIGNATURE_STABLE 3
#define PROJECTION_ERROR 4
#define RECOMPUTING_N 5
#define SIGNATURE_HAS_CHANGED 6
#define DPD_NUM_STATES 7



#else
#endif
