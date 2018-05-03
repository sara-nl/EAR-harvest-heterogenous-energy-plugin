/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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


#ifndef _EAR_TYPES_SIGNATURE
#define _EAR_TYPES_SIGNATURE

#include <common/types/generic.h>
#include <common/config.h>

#define FLOPS_EVENTS 8

typedef struct signature
{
    double DC_power;
    double DRAM_power;
    double PCK_power;
    double EDP;
    double GBS;
    double TPI;
    double CPI;
    double Gflops;
    double time;
    double max_DC_power;
    double min_DC_power;
    ull FLOPS[FLOPS_EVENTS];
    ull L1_misses;
    ull L2_misses;
    ull L3_misses;
    ull instructions;
    ull cycles;
    ulong avg_f;
    ulong def_f;
} signature_t;


// Function declarations

/** Replicates the signature in *source to *destiny */
void copy_signature(signature_t *destiny, signature_t *source);

/** Initializes all values of the signature to 0.*/
void init_signature(signature_t *sig);

/** returns true if basic values for sig1 and sig2 are equal with a maximum %
*   of difference defined by threshold (th) */
uint are_equal(signature_t *sig1,signature_t *sig2,double th);

/** Outputs the signature contents to the file pointed by the fd. */
void print_signature_fd(int fd, signature_t *sig);

#endif
