/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef EAR_TEST_DYNAIS_H_H
#define EAR_TEST_DYNAIS_H_H

#define METRICS         0
#define MAX_LEVELS      10
#define METRICS_WINDOW  40000
#define METRICS_CHUNCK  METRICS_WINDOW * MAX_LEVELS

// Dynais output states
#define END_LOOP       -1
#define NO_LOOP         0
#define IN_LOOP         1
#define NEW_ITERATION   2
#define NEW_LOOP        3
#define END_NEW_LOOP    4

// Functions
#if METRICS
void dynais_metrics_print();
unsigned int dynais_metrics_get_loop_calls(unsigned int *level);
#endif

// applies dynais algorithm. Sample[in] is the event to be considered. size[out] and level[out] are sets by 
// dynais, in case sample belongs to a loop,with the loop size and the level (number of nested loops) 
// of the current loop reported. 
// Return values: It returns one of the DynAIS output states
int dynais(unsigned long sample, unsigned int *size, unsigned int *level);
// Initilize dynais data structures for a maximum of "levels", each one with "windows" buffer sizes. Levels 
// sets the number of recursive levels dynais supports, and window is the buffer size at each level. With these
// two arguments the maximum loop size dynais could detect is configured
// If levels=1 and window=100, loop sizes of maximum 100 mpi calls inside can be detected
// If levels=2 and window=100, nested loops (loops with loops) with a maximum of 100 inner loops in the outer loop could be detected, each one with a maximum of 100 mpi calls inside it.
// Return values: 0 on succes -1 on error
int dynais_init(unsigned int window, unsigned int levels);
// releases resources allocated for dynais
void dynais_dispose();

#endif //EAR_TEST_DYNAIS_H_H
