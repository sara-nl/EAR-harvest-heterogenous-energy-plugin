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


#ifndef _EAR_TYPES_SIGNATURE
#define _EAR_TYPES_SIGNATURE

#include <common/types/generic.h>
#include <common/config.h>
#if USE_GPUS_LIB
#include <metrics/gpu/gpu.h>
#endif

// 0: float
// 1: 128 float
// 2: 256 float
// 3: 512 float
// 4: double
// 5: 128 double
// 6: 256 double
// 7: 512 double
#define FLOPS_EVENTS 8

typedef struct mini_sig
{
	float DC_power;
	float GBS;
	float CPI;
	float Gflops;
  float time;
  ulong avg_f;
  ulong def_f;
}ssig_t;

typedef struct signature
{
    double DC_power;
    double DRAM_power;
    double PCK_power;
#if USE_GPUS
    double GPU_power;
#if USE_GPUS_LIB
		gpu_t gpu_metrics;
#endif
#endif
    double EDP;
    double GBS;
    double TPI;
    double CPI;
    double Gflops;
    double time;
    ull FLOPS[FLOPS_EVENTS];
    ull L1_misses;
    ull L2_misses;
    ull L3_misses;
    ull instructions;
    ull cycles;
    ulong avg_f;
    ulong def_f;
} signature_t;

// Misc

/** Initializes all values of the signature to 0.*/
void signature_init(signature_t *sig);

/** Replicates the signature in *source to *destiny */
void signature_copy(signature_t *destiny, signature_t *source);

/** Outputs the signature contents to the file pointed by the fd. */
void signature_print_fd(int fd, signature_t *sig, char is_extended);
void compute_vpi(double *vpi,signature_t *sig);

void print_signature_fd_binary(int fd, signature_t *sig);
void read_signature_fd_binary(int fd, signature_t *sig);

void adapt_signature_to_node(signature_t *dest,signature_t *src,float ratio_PPN);
void signature_print_simple_fd(int fd, signature_t *sig);

void from_sig_to_mini(ssig_t *minis,signature_t *s);
void copy_mini_sig(ssig_t *dst,ssig_t *src);
void minis_to_str(ssig_t *s,char *b);


#endif
