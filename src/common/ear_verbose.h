/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/

#ifndef EAR_VERBOSE_H_
#define EAR_VERBOSE_H

#include <common/config.h>

extern int EAR_VERBOSE_LEVEL;

/** Prints a formatted string in case the level passed is greater or equal than
 *  the selected level of verbosity. */
#define ear_verbose(level,...) \
	if (level <= EAR_VERBOSE_LEVEL) fprintf(stderr, __VA_ARGS__);

/** Prints a formatted string in case the level passed is greater or equal than
 *  the selected level of verbosity. This function prints the prefix string
 *  constant __NAME__, and a new line character as a suffix. */
#define VERBOSE_N(level, ...) \
        if (level <= EAR_VERBOSE_LEVEL) { \
                fprintf(stderr, "%s: ", __NAME__); \
                fprintf(stderr, __VA_ARGS__); \
                fprintf(stderr, "\n"); \
        }

/** Prints a formatted string in case the level passed is greater or equal then
 *  the selected level of debugging.*/
#if BUILD(DEBUG)
	#define ear_debug(level,...) \
		if (level <= DEBUG) fprintf(stderr, __VA_ARGS__);
#else
	#define ear_debug(level,...)
#endif

/** Prints a formatted string in case the level passed is greater or equal than
 *  the selected level of debugging. This function prints the prefix string
 *  constant __FILE__, and a new line character as a suffix. */
#if BUILD(DEBUG)
	#define DEBUG_F(level, ...) \
		if (level <= DEBUG) { \
			fprintf(stderr, "%s: ", __FILE__); \
			fprintf(stderr, __VA_ARGS__); \
			fprintf(stderr, "\n"); \
		}
#else
	#define DEBUG_F(level, ...)
#endif

#endif
