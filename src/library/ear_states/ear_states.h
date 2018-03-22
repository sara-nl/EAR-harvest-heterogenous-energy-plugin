/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EAR_BASE_H_
#define _EAR_BASE_H

//Executed at application start. Reads EAR configuration env. variables
void states_begin_job(int my_id, FILE *ear_fd, char *app_name);
//Executed at NEW_LOOP or END_NEW_LOOP DynAIS event
void states_begin_period(int my_id,FILE *ear_fd, ulong new_period, uint size);
//Executed at NEW_ITERATION DynAIS event
void states_new_iteration(int my_id, uint size, uint iterations, uint level, ulong event,ulong mpi_calls_iter);
//Executed at END_LOOP or END_NEW_LOOP DynAIS event
void states_end_period(uint iterations);
//Executed at application end
void states_end_job(int my_id, FILE *ear_fd, char *app_name);
// Returns the current EAR state
int states_my_state();

#else
#endif
