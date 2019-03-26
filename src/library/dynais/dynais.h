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


#ifndef EAR_DYNAIS_H
#define EAR_DYNAIS_H

#include <immintrin.h> // -mavx -mfma
#include <common/types/generic.h>

#define MAX_LEVELS      10
#define METRICS_WINDOW  40000

// DynAIS output states
#define END_LOOP       -1
#define NO_LOOP         0
#define IN_LOOP         1
#define NEW_ITERATION   2
#define NEW_LOOP        3
#define END_NEW_LOOP    4

// Functions
/** Given a sample and its size, returns the state the application is in (in
*   a loop, in an iteration, etc.). */
short dynais(ushort sample, ushort *size, ushort *level);

/** Converts a long sample to short sample. */
ushort dynais_sample_convert(ulong sample);

/** Allocates memory in preparation to use dynais. Returns 0 on success */
int dynais_init(ushort window, ushort levels);

/** Frees the memory previously allocated. */
void dynais_dispose();

#endif //EAR_DYNAIS_H
