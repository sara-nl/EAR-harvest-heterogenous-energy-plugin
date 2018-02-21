/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef EAR_VERBOSE_H_
#define EAR_VERBOSE_H

extern int EAR_VERBOSE_LEVEL;

#define ear_verbose(level,...) if (level<=EAR_VERBOSE_LEVEL) fprintf(stderr, __VA_ARGS__);
#ifdef DEBUG
#define ear_debug(level,...) if (level<=DEBUG) fprintf(stderr, __VA_ARGS__);
#else
#define ear_debug(level,...)
#endif

#ifdef DEBUG
#define DEBUG_F(level, ...) \
	if (level <= DEBUG) { \
		fprintf(stderr, "%s: ", __FILE__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	}
#else
#define DEBUG_F(level, ...)
#endif

// Define __NAME__ constant before use this macro
#define VERBOSE_N(level, ...) \
	if (level <= EAR_VERBOSE_LEVEL) { \
		fprintf(stderr, "%s: ", __NAME__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	}

#else
#endif
