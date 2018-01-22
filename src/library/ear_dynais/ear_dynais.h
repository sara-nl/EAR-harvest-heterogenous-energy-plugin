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
//
void dynais_metrics_print();
//
unsigned int dynais_metrics_get_loop_calls(unsigned int *level);
#endif

#if ANALYSIS
//
unsigned int dynais_loop_data(unsigned long *_samples,
                              unsigned int *_sizes,
                              unsigned int *_crcs,
                              unsigned int level,
                              unsigned int size);
#endif

int dynais(unsigned long sample, unsigned int *size, unsigned int *level);
int dynais_init(unsigned int window, unsigned int levels);
void dynais_dispose();

#endif //EAR_TEST_DYNAIS_H_H