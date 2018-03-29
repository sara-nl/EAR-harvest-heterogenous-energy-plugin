/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef EAR_DYNAIS_H
#define EAR_DYNAIS_H

#define MAX_LEVELS      10
#define METRICS_WINDOW  40000

// Dynais output states
#define END_LOOP       -1
#define NO_LOOP         0
#define IN_LOOP         1
#define NEW_ITERATION   2
#define NEW_LOOP        3
#define END_NEW_LOOP    4

// Functions
int dynais(unsigned long sample, unsigned int *size, unsigned int *level);

int dynais_init(unsigned int window, unsigned int levels);

void dynais_dispose();

#endif //EAR_DYNAIS_H
