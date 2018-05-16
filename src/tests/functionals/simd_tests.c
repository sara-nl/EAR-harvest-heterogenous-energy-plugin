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



#define _GNU_SOURCE
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <emmintrin.h> // -msse2
#include <immintrin.h> // -mavx -mfma
#include <metrics/custom/hardware_info.h>

// HASWELL tests
void sse2_dp_add128_hsw(ulong n_iterations)
{
    ulong i;
    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {
        __asm__(
        "vaddpd %xmm1,%xmm0,%xmm2\n\t"
        "vaddpd %xmm1,%xmm0,%xmm3\n\t"
        "vaddpd %xmm1,%xmm0,%xmm4\n\t"
        "vaddpd %xmm1,%xmm0,%xmm5\n\t"
        "vaddpd %xmm1,%xmm0,%xmm6\n\t"
        "vaddpd %xmm1,%xmm0,%xmm7\n\t"
        "vaddpd %xmm1,%xmm0,%xmm8\n\t"
        "vaddpd %xmm1,%xmm0,%xmm10\n\t"
        "vaddpd %xmm1,%xmm0,%xmm11\n\t"
        "vaddpd %xmm1,%xmm0,%xmm12\n\t" //10
        "vaddpd %xmm1,%xmm0,%xmm13\n\t"
        "vaddpd %xmm1,%xmm0,%xmm14\n\t"
        "vaddpd %xmm1,%xmm0,%xmm15\n\t"
        "vaddpd %xmm1,%xmm0,%xmm2\n\t"
        "vaddpd %xmm1,%xmm0,%xmm3\n\t"
        "vaddpd %xmm1,%xmm0,%xmm4\n\t"
        "vaddpd %xmm1,%xmm0,%xmm5\n\t"
        "vaddpd %xmm1,%xmm0,%xmm6\n\t"
        "vaddpd %xmm1,%xmm0,%xmm7\n\t"
        "vaddpd %xmm1,%xmm0,%xmm8\n\t" //20
        "vaddpd %xmm1,%xmm0,%xmm10\n\t"
        "vaddpd %xmm1,%xmm0,%xmm11\n\t"
        "vaddpd %xmm1,%xmm0,%xmm12\n\t"
        "vaddpd %xmm1,%xmm0,%xmm13\n\t"
        "vaddpd %xmm1,%xmm0,%xmm14\n\t"
        "vaddpd %xmm1,%xmm0,%xmm15\n\t"
        "vaddpd %xmm1,%xmm0,%xmm2\n\t"
        "vaddpd %xmm1,%xmm0,%xmm3\n\t"
        "vaddpd %xmm1,%xmm0,%xmm4\n\t"
        "vaddpd %xmm1,%xmm0,%xmm5\n\t" //30
        "vaddpd %xmm1,%xmm0,%xmm6\n\t"
        "vaddpd %xmm1,%xmm0,%xmm7\n\t"
        "vaddpd %xmm1,%xmm0,%xmm8\n\t"
        "vaddpd %xmm1,%xmm0,%xmm10\n\t"
        "vaddpd %xmm1,%xmm0,%xmm11\n\t"
        "vaddpd %xmm1,%xmm0,%xmm12\n\t"
        "vaddpd %xmm1,%xmm0,%xmm13\n\t"
        "vaddpd %xmm1,%xmm0,%xmm14\n\t"
        "vaddpd %xmm1,%xmm0,%xmm15\n\t"
        "vaddpd %xmm1,%xmm0,%xmm2\n\t" //40
        "vaddpd %xmm1,%xmm0,%xmm3\n\t"
        "vaddpd %xmm1,%xmm0,%xmm4\n\t"
        "vaddpd %xmm1,%xmm0,%xmm5\n\t"
        "vaddpd %xmm1,%xmm0,%xmm6\n\t"
        "vaddpd %xmm1,%xmm0,%xmm7\n\t"
        "vaddpd %xmm1,%xmm0,%xmm8\n\t"
        "vaddpd %xmm1,%xmm0,%xmm10\n\t"
        "vaddpd %xmm1,%xmm0,%xmm11\n\t"
        "vaddpd %xmm1,%xmm0,%xmm12\n\t"
        "vaddpd %xmm1,%xmm0,%xmm13\n\t" //50
        "vaddpd %xmm1,%xmm0,%xmm14\n\t"
        "vaddpd %xmm1,%xmm0,%xmm15\n\t"
        "vaddpd %xmm1,%xmm0,%xmm2\n\t"
        "vaddpd %xmm1,%xmm0,%xmm3\n\t"
        "vaddpd %xmm1,%xmm0,%xmm4\n\t"
        "vaddpd %xmm1,%xmm0,%xmm5\n\t"
        "vaddpd %xmm1,%xmm0,%xmm6\n\t"
        "vaddpd %xmm1,%xmm0,%xmm7\n\t"
        "vaddpd %xmm1,%xmm0,%xmm8\n\t"
        "vaddpd %xmm1,%xmm0,%xmm10\n\t" //60
        "vaddpd %xmm1,%xmm0,%xmm11\n\t"
        "vaddpd %xmm1,%xmm0,%xmm12\n\t"
        "vaddpd %xmm1,%xmm0,%xmm13\n\t"
        "vaddpd %xmm1,%xmm0,%xmm14\n\t"
        );

    }

}

void sse2_dp_mul128_hsw(ulong n_iterations)
{
    ulong i;


    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {
        __asm__(
        "vmulpd %xmm1,%xmm0,%xmm2\n\t"
        "vmulpd %xmm1,%xmm0,%xmm3\n\t"
        "vmulpd %xmm1,%xmm0,%xmm4\n\t"
        "vmulpd %xmm1,%xmm0,%xmm5\n\t"
        "vmulpd %xmm1,%xmm0,%xmm6\n\t"
        "vmulpd %xmm1,%xmm0,%xmm7\n\t"
        "vmulpd %xmm1,%xmm0,%xmm8\n\t"
        "vmulpd %xmm1,%xmm0,%xmm10\n\t"
        "vmulpd %xmm1,%xmm0,%xmm11\n\t"
        "vmulpd %xmm1,%xmm0,%xmm12\n\t" //10
        "vmulpd %xmm1,%xmm0,%xmm13\n\t"
        "vmulpd %xmm1,%xmm0,%xmm14\n\t"
        "vmulpd %xmm1,%xmm0,%xmm15\n\t"
        "vmulpd %xmm1,%xmm0,%xmm2\n\t"
        "vmulpd %xmm1,%xmm0,%xmm3\n\t"
        "vmulpd %xmm1,%xmm0,%xmm4\n\t"
        "vmulpd %xmm1,%xmm0,%xmm5\n\t"
        "vmulpd %xmm1,%xmm0,%xmm6\n\t"
        "vmulpd %xmm1,%xmm0,%xmm7\n\t"
        "vmulpd %xmm1,%xmm0,%xmm8\n\t" //20
        "vmulpd %xmm1,%xmm0,%xmm10\n\t"
        "vmulpd %xmm1,%xmm0,%xmm11\n\t"
        "vmulpd %xmm1,%xmm0,%xmm12\n\t"
        "vmulpd %xmm1,%xmm0,%xmm13\n\t"
        "vmulpd %xmm1,%xmm0,%xmm14\n\t"
        "vmulpd %xmm1,%xmm0,%xmm15\n\t"
        "vmulpd %xmm1,%xmm0,%xmm2\n\t"
        "vmulpd %xmm1,%xmm0,%xmm3\n\t"
        "vmulpd %xmm1,%xmm0,%xmm4\n\t"
        "vmulpd %xmm1,%xmm0,%xmm5\n\t" //30
        "vmulpd %xmm1,%xmm0,%xmm6\n\t"
        "vmulpd %xmm1,%xmm0,%xmm7\n\t"
        "vmulpd %xmm1,%xmm0,%xmm8\n\t"
        "vmulpd %xmm1,%xmm0,%xmm10\n\t"
        "vmulpd %xmm1,%xmm0,%xmm11\n\t"
        "vmulpd %xmm1,%xmm0,%xmm12\n\t"
        "vmulpd %xmm1,%xmm0,%xmm13\n\t"
        "vmulpd %xmm1,%xmm0,%xmm14\n\t"
        "vmulpd %xmm1,%xmm0,%xmm15\n\t"
        "vmulpd %xmm1,%xmm0,%xmm2\n\t" //40
        "vmulpd %xmm1,%xmm0,%xmm3\n\t"
        "vmulpd %xmm1,%xmm0,%xmm4\n\t"
        "vmulpd %xmm1,%xmm0,%xmm5\n\t"
        "vmulpd %xmm1,%xmm0,%xmm6\n\t"
        "vmulpd %xmm1,%xmm0,%xmm7\n\t"
        "vmulpd %xmm1,%xmm0,%xmm8\n\t"
        "vmulpd %xmm1,%xmm0,%xmm10\n\t"
        "vmulpd %xmm1,%xmm0,%xmm11\n\t"
        "vmulpd %xmm1,%xmm0,%xmm12\n\t"
        "vmulpd %xmm1,%xmm0,%xmm13\n\t" //50
        "vmulpd %xmm1,%xmm0,%xmm14\n\t"
        "vmulpd %xmm1,%xmm0,%xmm15\n\t"
        "vmulpd %xmm1,%xmm0,%xmm2\n\t"
        "vmulpd %xmm1,%xmm0,%xmm3\n\t"
        "vmulpd %xmm1,%xmm0,%xmm4\n\t"
        "vmulpd %xmm1,%xmm0,%xmm5\n\t"
        "vmulpd %xmm1,%xmm0,%xmm6\n\t"
        "vmulpd %xmm1,%xmm0,%xmm7\n\t"
        "vmulpd %xmm1,%xmm0,%xmm8\n\t"
        "vmulpd %xmm1,%xmm0,%xmm10\n\t" //60
        "vmulpd %xmm1,%xmm0,%xmm11\n\t"
        "vmulpd %xmm1,%xmm0,%xmm12\n\t"
        "vmulpd %xmm1,%xmm0,%xmm13\n\t"
        "vmulpd %xmm1,%xmm0,%xmm14\n\t"
        );

    }

}

void fma_dp_fmadd128_hsw(ulong n_iterations)
{
    ulong i;
    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {
    __asm__(
        "vfmadd213pd %xmm1,%xmm0,%xmm2\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm3\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm4\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm5\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm6\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm7\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm8\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm10\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm11\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm12\n\t" //10
        "vfmadd213pd %xmm1,%xmm0,%xmm13\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm14\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm15\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm2\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm3\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm4\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm5\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm6\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm7\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm8\n\t" //20
        "vfmadd213pd %xmm1,%xmm0,%xmm10\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm11\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm12\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm13\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm14\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm15\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm2\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm3\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm4\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm5\n\t" //30
        "vfmadd213pd %xmm1,%xmm0,%xmm6\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm7\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm8\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm10\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm11\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm12\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm13\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm14\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm15\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm2\n\t" //40
        "vfmadd213pd %xmm1,%xmm0,%xmm3\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm4\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm5\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm6\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm7\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm8\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm10\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm11\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm12\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm13\n\t" //50
        "vfmadd213pd %xmm1,%xmm0,%xmm14\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm15\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm2\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm3\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm4\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm5\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm6\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm7\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm8\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm10\n\t" //60
        "vfmadd213pd %xmm1,%xmm0,%xmm11\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm12\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm13\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm14\n\t"
	);
	}



}

void avx_dp_add256_hsw(ulong n_iterations)
{
    ulong i;

    __asm__(
    ".align 64\n\t"
	);
    for (i = 0; i < n_iterations; i++) {
        __asm__(
        "vaddpd %ymm1,%ymm0,%ymm2\n\t"
        "vaddpd %ymm1,%ymm0,%ymm3\n\t"
        "vaddpd %ymm1,%ymm0,%ymm4\n\t"
        "vaddpd %ymm1,%ymm0,%ymm5\n\t"
        "vaddpd %ymm1,%ymm0,%ymm6\n\t"
        "vaddpd %ymm1,%ymm0,%ymm7\n\t"
        "vaddpd %ymm1,%ymm0,%ymm8\n\t"
        "vaddpd %ymm1,%ymm0,%ymm10\n\t"
        "vaddpd %ymm1,%ymm0,%ymm11\n\t"
        "vaddpd %ymm1,%ymm0,%ymm12\n\t" //10
        "vaddpd %ymm1,%ymm0,%ymm13\n\t"
        "vaddpd %ymm1,%ymm0,%ymm14\n\t"
        "vaddpd %ymm1,%ymm0,%ymm15\n\t"
        "vaddpd %ymm1,%ymm0,%ymm2\n\t"
        "vaddpd %ymm1,%ymm0,%ymm3\n\t"
        "vaddpd %ymm1,%ymm0,%ymm4\n\t"
        "vaddpd %ymm1,%ymm0,%ymm5\n\t"
        "vaddpd %ymm1,%ymm0,%ymm6\n\t"
        "vaddpd %ymm1,%ymm0,%ymm7\n\t"
        "vaddpd %ymm1,%ymm0,%ymm8\n\t" //20
        "vaddpd %ymm1,%ymm0,%ymm10\n\t"
        "vaddpd %ymm1,%ymm0,%ymm11\n\t"
        "vaddpd %ymm1,%ymm0,%ymm12\n\t"
        "vaddpd %ymm1,%ymm0,%ymm13\n\t"
        "vaddpd %ymm1,%ymm0,%ymm14\n\t"
        "vaddpd %ymm1,%ymm0,%ymm15\n\t"
        "vaddpd %ymm1,%ymm0,%ymm2\n\t"
        "vaddpd %ymm1,%ymm0,%ymm3\n\t"
        "vaddpd %ymm1,%ymm0,%ymm4\n\t"
        "vaddpd %ymm1,%ymm0,%ymm5\n\t" //30
        "vaddpd %ymm1,%ymm0,%ymm6\n\t"
        "vaddpd %ymm1,%ymm0,%ymm7\n\t"
        "vaddpd %ymm1,%ymm0,%ymm8\n\t"
        "vaddpd %ymm1,%ymm0,%ymm10\n\t"
        "vaddpd %ymm1,%ymm0,%ymm11\n\t"
        "vaddpd %ymm1,%ymm0,%ymm12\n\t"
        "vaddpd %ymm1,%ymm0,%ymm13\n\t"
        "vaddpd %ymm1,%ymm0,%ymm14\n\t"
        "vaddpd %ymm1,%ymm0,%ymm15\n\t"
        "vaddpd %ymm1,%ymm0,%ymm2\n\t" //40
        "vaddpd %ymm1,%ymm0,%ymm3\n\t"
        "vaddpd %ymm1,%ymm0,%ymm4\n\t"
        "vaddpd %ymm1,%ymm0,%ymm5\n\t"
        "vaddpd %ymm1,%ymm0,%ymm6\n\t"
        "vaddpd %ymm1,%ymm0,%ymm7\n\t"
        "vaddpd %ymm1,%ymm0,%ymm8\n\t"
        "vaddpd %ymm1,%ymm0,%ymm10\n\t"
        "vaddpd %ymm1,%ymm0,%ymm11\n\t"
        "vaddpd %ymm1,%ymm0,%ymm12\n\t"
        "vaddpd %ymm1,%ymm0,%ymm13\n\t" //50
        "vaddpd %ymm1,%ymm0,%ymm14\n\t"
        "vaddpd %ymm1,%ymm0,%ymm15\n\t"
        "vaddpd %ymm1,%ymm0,%ymm2\n\t"
        "vaddpd %ymm1,%ymm0,%ymm3\n\t"
        "vaddpd %ymm1,%ymm0,%ymm4\n\t"
        "vaddpd %ymm1,%ymm0,%ymm5\n\t"
        "vaddpd %ymm1,%ymm0,%ymm6\n\t"
        "vaddpd %ymm1,%ymm0,%ymm7\n\t"
        "vaddpd %ymm1,%ymm0,%ymm8\n\t"
        "vaddpd %ymm1,%ymm0,%ymm10\n\t" //60
        "vaddpd %ymm1,%ymm0,%ymm11\n\t"
        "vaddpd %ymm1,%ymm0,%ymm12\n\t"
        "vaddpd %ymm1,%ymm0,%ymm13\n\t"
        "vaddpd %ymm1,%ymm0,%ymm14\n\t"
        );

    }

}

void avx_dp_mul256_hsw(ulong n_iterations)
{
    ulong i;


    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {
        __asm__(
        "vmulpd %ymm1,%ymm0,%ymm2\n\t"
        "vmulpd %ymm1,%ymm0,%ymm3\n\t"
        "vmulpd %ymm1,%ymm0,%ymm4\n\t"
        "vmulpd %ymm1,%ymm0,%ymm5\n\t"
        "vmulpd %ymm1,%ymm0,%ymm6\n\t"
        "vmulpd %ymm1,%ymm0,%ymm7\n\t"
        "vmulpd %ymm1,%ymm0,%ymm8\n\t"
        "vmulpd %ymm1,%ymm0,%ymm10\n\t"
        "vmulpd %ymm1,%ymm0,%ymm11\n\t"
        "vmulpd %ymm1,%ymm0,%ymm12\n\t" //10
        "vmulpd %ymm1,%ymm0,%ymm13\n\t"
        "vmulpd %ymm1,%ymm0,%ymm14\n\t"
        "vmulpd %ymm1,%ymm0,%ymm15\n\t"
        "vmulpd %ymm1,%ymm0,%ymm2\n\t"
        "vmulpd %ymm1,%ymm0,%ymm3\n\t"
        "vmulpd %ymm1,%ymm0,%ymm4\n\t"
        "vmulpd %ymm1,%ymm0,%ymm5\n\t"
        "vmulpd %ymm1,%ymm0,%ymm6\n\t"
        "vmulpd %ymm1,%ymm0,%ymm7\n\t"
        "vmulpd %ymm1,%ymm0,%ymm8\n\t" //20
        "vmulpd %ymm1,%ymm0,%ymm10\n\t"
        "vmulpd %ymm1,%ymm0,%ymm11\n\t"
        "vmulpd %ymm1,%ymm0,%ymm12\n\t"
        "vmulpd %ymm1,%ymm0,%ymm13\n\t"
        "vmulpd %ymm1,%ymm0,%ymm14\n\t"
        "vmulpd %ymm1,%ymm0,%ymm15\n\t"
        "vmulpd %ymm1,%ymm0,%ymm2\n\t"
        "vmulpd %ymm1,%ymm0,%ymm3\n\t"
        "vmulpd %ymm1,%ymm0,%ymm4\n\t"
        "vmulpd %ymm1,%ymm0,%ymm5\n\t" //30
        "vmulpd %ymm1,%ymm0,%ymm6\n\t"
        "vmulpd %ymm1,%ymm0,%ymm7\n\t"
        "vmulpd %ymm1,%ymm0,%ymm8\n\t"
        "vmulpd %ymm1,%ymm0,%ymm10\n\t"
        "vmulpd %ymm1,%ymm0,%ymm11\n\t"
        "vmulpd %ymm1,%ymm0,%ymm12\n\t"
        "vmulpd %ymm1,%ymm0,%ymm13\n\t"
        "vmulpd %ymm1,%ymm0,%ymm14\n\t"
        "vmulpd %ymm1,%ymm0,%ymm15\n\t"
        "vmulpd %ymm1,%ymm0,%ymm2\n\t" //40
        "vmulpd %ymm1,%ymm0,%ymm3\n\t"
        "vmulpd %ymm1,%ymm0,%ymm4\n\t"
        "vmulpd %ymm1,%ymm0,%ymm5\n\t"
        "vmulpd %ymm1,%ymm0,%ymm6\n\t"
        "vmulpd %ymm1,%ymm0,%ymm7\n\t"
        "vmulpd %ymm1,%ymm0,%ymm8\n\t"
        "vmulpd %ymm1,%ymm0,%ymm10\n\t"
        "vmulpd %ymm1,%ymm0,%ymm11\n\t"
        "vmulpd %ymm1,%ymm0,%ymm12\n\t"
        "vmulpd %ymm1,%ymm0,%ymm13\n\t" //50
        "vmulpd %ymm1,%ymm0,%ymm14\n\t"
        "vmulpd %ymm1,%ymm0,%ymm15\n\t"
        "vmulpd %ymm1,%ymm0,%ymm2\n\t"
        "vmulpd %ymm1,%ymm0,%ymm3\n\t"
        "vmulpd %ymm1,%ymm0,%ymm4\n\t"
        "vmulpd %ymm1,%ymm0,%ymm5\n\t"
        "vmulpd %ymm1,%ymm0,%ymm6\n\t"
        "vmulpd %ymm1,%ymm0,%ymm7\n\t"
        "vmulpd %ymm1,%ymm0,%ymm8\n\t"
        "vmulpd %ymm1,%ymm0,%ymm10\n\t" //60
        "vmulpd %ymm1,%ymm0,%ymm11\n\t"
        "vmulpd %ymm1,%ymm0,%ymm12\n\t"
        "vmulpd %ymm1,%ymm0,%ymm13\n\t"
        "vmulpd %ymm1,%ymm0,%ymm14\n\t"
        );
	}
}
void fma_dp_fmadd256_hsw(ulong n_iterations)
{
    ulong i;
    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {

    __asm__(
        "vfmadd213pd %ymm1,%ymm0,%ymm2\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm3\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm4\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm5\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm6\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm7\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm8\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm10\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm11\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm12\n\t" //10
        "vfmadd213pd %ymm1,%ymm0,%ymm13\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm14\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm15\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm2\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm3\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm4\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm5\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm6\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm7\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm8\n\t" //20
        "vfmadd213pd %ymm1,%ymm0,%ymm10\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm11\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm12\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm13\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm14\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm15\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm2\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm3\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm4\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm5\n\t" //30
        "vfmadd213pd %ymm1,%ymm0,%ymm6\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm7\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm8\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm10\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm11\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm12\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm13\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm14\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm15\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm2\n\t" //40
        "vfmadd213pd %ymm1,%ymm0,%ymm3\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm4\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm5\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm6\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm7\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm8\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm10\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm11\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm12\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm13\n\t" //50
        "vfmadd213pd %ymm1,%ymm0,%ymm14\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm15\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm2\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm3\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm4\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm5\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm6\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm7\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm8\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm10\n\t" //60
        "vfmadd213pd %ymm1,%ymm0,%ymm11\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm12\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm13\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm14\n\t"
        );
}

}
// SKYLAKE tests

void sse2_dp_add128_skl(ulong n_iterations)
{
    ulong i;
    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {
        __asm__(
        "vaddpd %xmm1,%xmm0,%xmm2\n\t"
        "vaddpd %xmm1,%xmm0,%xmm3\n\t"
        "vaddpd %xmm1,%xmm0,%xmm4\n\t"
        "vaddpd %xmm1,%xmm0,%xmm5\n\t"
        "vaddpd %xmm1,%xmm0,%xmm6\n\t"
        "vaddpd %xmm1,%xmm0,%xmm7\n\t"
        "vaddpd %xmm1,%xmm0,%xmm8\n\t"
        "vaddpd %xmm1,%xmm0,%xmm10\n\t"
        "vaddpd %xmm1,%xmm0,%xmm11\n\t"
        "vaddpd %xmm1,%xmm0,%xmm12\n\t"
        "vaddpd %xmm1,%xmm0,%xmm13\n\t"
        "vaddpd %xmm1,%xmm0,%xmm14\n\t"
        "vaddpd %xmm1,%xmm0,%xmm15\n\t"
        "vaddpd %xmm1,%xmm0,%xmm16\n\t"
        "vaddpd %xmm1,%xmm0,%xmm17\n\t"
        "vaddpd %xmm1,%xmm0,%xmm18\n\t"
        "vaddpd %xmm1,%xmm0,%xmm2\n\t"
        "vaddpd %xmm1,%xmm0,%xmm3\n\t"
        "vaddpd %xmm1,%xmm0,%xmm4\n\t"
        "vaddpd %xmm1,%xmm0,%xmm5\n\t"
        "vaddpd %xmm1,%xmm0,%xmm6\n\t"
        "vaddpd %xmm1,%xmm0,%xmm7\n\t"
        "vaddpd %xmm1,%xmm0,%xmm8\n\t"
        "vaddpd %xmm1,%xmm0,%xmm10\n\t"
        "vaddpd %xmm1,%xmm0,%xmm11\n\t"
        "vaddpd %xmm1,%xmm0,%xmm12\n\t"
        "vaddpd %xmm1,%xmm0,%xmm13\n\t"
        "vaddpd %xmm1,%xmm0,%xmm14\n\t"
        "vaddpd %xmm1,%xmm0,%xmm15\n\t"
        "vaddpd %xmm1,%xmm0,%xmm16\n\t"
        "vaddpd %xmm1,%xmm0,%xmm17\n\t"
        "vaddpd %xmm1,%xmm0,%xmm18\n\t"
        "vaddpd %xmm1,%xmm0,%xmm2\n\t"
        "vaddpd %xmm1,%xmm0,%xmm3\n\t"
        "vaddpd %xmm1,%xmm0,%xmm4\n\t"
        "vaddpd %xmm1,%xmm0,%xmm5\n\t"
        "vaddpd %xmm1,%xmm0,%xmm6\n\t"
        "vaddpd %xmm1,%xmm0,%xmm7\n\t"
        "vaddpd %xmm1,%xmm0,%xmm8\n\t"
        "vaddpd %xmm1,%xmm0,%xmm10\n\t"
        "vaddpd %xmm1,%xmm0,%xmm11\n\t"
        "vaddpd %xmm1,%xmm0,%xmm12\n\t"
        "vaddpd %xmm1,%xmm0,%xmm13\n\t"
        "vaddpd %xmm1,%xmm0,%xmm14\n\t"
        "vaddpd %xmm1,%xmm0,%xmm15\n\t"
        "vaddpd %xmm1,%xmm0,%xmm16\n\t"
        "vaddpd %xmm1,%xmm0,%xmm17\n\t"
        "vaddpd %xmm1,%xmm0,%xmm18\n\t"
        "vaddpd %xmm1,%xmm0,%xmm2\n\t"
        "vaddpd %xmm1,%xmm0,%xmm3\n\t"
        "vaddpd %xmm1,%xmm0,%xmm4\n\t"
        "vaddpd %xmm1,%xmm0,%xmm5\n\t"
        "vaddpd %xmm1,%xmm0,%xmm6\n\t"
        "vaddpd %xmm1,%xmm0,%xmm7\n\t"
        "vaddpd %xmm1,%xmm0,%xmm8\n\t"
        "vaddpd %xmm1,%xmm0,%xmm10\n\t"
        "vaddpd %xmm1,%xmm0,%xmm11\n\t"
        "vaddpd %xmm1,%xmm0,%xmm12\n\t"
        "vaddpd %xmm1,%xmm0,%xmm13\n\t"
        "vaddpd %xmm1,%xmm0,%xmm14\n\t"
        "vaddpd %xmm1,%xmm0,%xmm15\n\t"
        "vaddpd %xmm1,%xmm0,%xmm16\n\t"
        "vaddpd %xmm1,%xmm0,%xmm17\n\t"
        "vaddpd %xmm1,%xmm0,%xmm18\n\t"
        );

    }

}

void sse2_dp_mul128_skl(ulong n_iterations)
{
    ulong i;


    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {
        __asm__(
        "vmulpd  %xmm2,%xmm0,%xmm0\n\t"
        "vmulpd  %xmm1, %xmm0,%xmm20\n\t"
        "vmulpd  %xmm3, %xmm4,%xmm4\n\t"
        "vmulpd  %xmm15, %xmm5,%xmm5\n\t"
        "vmulpd  %xmm6, %xmm7,%xmm7\n\t"
        "vmulpd  %xmm8, %xmm9,%xmm9\n\t"
        "vmulpd  %xmm10,%xmm11,%xmm11\n\t"
        "vmulpd  %xmm12,%xmm13,%xmm13\n\t"
        "vmulpd  %xmm2,%xmm0,%xmm0\n\t"
        "vmulpd  %xmm1, %xmm0,%xmm20\n\t"
        "vmulpd  %xmm3, %xmm4,%xmm4\n\t"
        "vmulpd  %xmm15, %xmm5,%xmm5\n\t"
        "vmulpd  %xmm6, %xmm7,%xmm7\n\t"
        "vmulpd  %xmm8, %xmm9,%xmm9\n\t"
        "vmulpd  %xmm10,%xmm11,%xmm11\n\t"
        "vmulpd  %xmm12,%xmm13,%xmm13\n\t"
        "vmulpd  %xmm2,%xmm0,%xmm0\n\t"
        "vmulpd  %xmm1, %xmm0,%xmm20\n\t"
        "vmulpd  %xmm3, %xmm4,%xmm4\n\t"
        "vmulpd  %xmm15, %xmm5,%xmm5\n\t"
        "vmulpd  %xmm6, %xmm7,%xmm7\n\t"
        "vmulpd  %xmm8, %xmm9,%xmm9\n\t"
        "vmulpd  %xmm10,%xmm11,%xmm11\n\t"
        "vmulpd  %xmm12,%xmm13,%xmm13\n\t"
        "vmulpd  %xmm2,%xmm0,%xmm0\n\t"
        "vmulpd  %xmm1, %xmm0,%xmm20\n\t"
        "vmulpd  %xmm3, %xmm4,%xmm4\n\t"
        "vmulpd  %xmm15, %xmm5,%xmm5\n\t"
        "vmulpd  %xmm6, %xmm7,%xmm7\n\t"
        "vmulpd  %xmm8, %xmm9,%xmm9\n\t"
        "vmulpd  %xmm10,%xmm11,%xmm11\n\t"
        "vmulpd  %xmm12,%xmm13,%xmm13\n\t"
        "vmulpd  %xmm2,%xmm0,%xmm0\n\t"
        "vmulpd  %xmm1, %xmm0,%xmm20\n\t"
        "vmulpd  %xmm3, %xmm4,%xmm4\n\t"
        "vmulpd  %xmm15, %xmm5,%xmm5\n\t"
        "vmulpd  %xmm6, %xmm7,%xmm7\n\t"
        "vmulpd  %xmm8, %xmm9,%xmm9\n\t"
        "vmulpd  %xmm10,%xmm11,%xmm11\n\t"
        "vmulpd  %xmm12,%xmm13,%xmm13\n\t"
        "vmulpd  %xmm2,%xmm0,%xmm0\n\t"
        "vmulpd  %xmm1, %xmm0,%xmm20\n\t"
        "vmulpd  %xmm3, %xmm4,%xmm4\n\t"
        "vmulpd  %xmm15, %xmm5,%xmm5\n\t"
        "vmulpd  %xmm6, %xmm7,%xmm7\n\t"
        "vmulpd  %xmm8, %xmm9,%xmm9\n\t"
        "vmulpd  %xmm10,%xmm11,%xmm11\n\t"
        "vmulpd  %xmm12,%xmm13,%xmm13\n\t"
        "vmulpd  %xmm2,%xmm0,%xmm0\n\t"
        "vmulpd  %xmm1, %xmm0,%xmm20\n\t"
        "vmulpd  %xmm3, %xmm4,%xmm4\n\t"
        "vmulpd  %xmm15, %xmm5,%xmm5\n\t"
        "vmulpd  %xmm6, %xmm7,%xmm7\n\t"
        "vmulpd  %xmm8, %xmm9,%xmm9\n\t"
        "vmulpd  %xmm10,%xmm11,%xmm11\n\t"
        "vmulpd  %xmm12,%xmm13,%xmm13\n\t"
        "vmulpd  %xmm2,%xmm0,%xmm0\n\t"
        "vmulpd  %xmm1, %xmm0,%xmm20\n\t"
        "vmulpd  %xmm3, %xmm4,%xmm4\n\t"
        "vmulpd  %xmm15, %xmm5,%xmm5\n\t"
        "vmulpd  %xmm6, %xmm7,%xmm7\n\t"
        "vmulpd  %xmm8, %xmm9,%xmm9\n\t"
        "vmulpd  %xmm10,%xmm11,%xmm11\n\t"
        "vmulpd  %xmm12,%xmm13,%xmm13\n\t"
        );

    }

}

void fma_dp_fmadd128_skl(ulong n_iterations)
{
    ulong i;
    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {
    __asm__(
        "vfmadd213pd %xmm1,%xmm0,%xmm2\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm3\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm4\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm5\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm6\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm7\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm8\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm10\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm11\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm12\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm13\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm14\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm15\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm16\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm17\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm18\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm2\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm3\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm4\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm5\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm6\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm7\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm8\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm10\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm11\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm12\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm13\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm14\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm15\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm16\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm17\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm18\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm2\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm3\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm4\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm5\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm6\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm7\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm8\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm10\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm11\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm12\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm13\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm14\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm15\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm16\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm17\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm18\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm2\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm3\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm4\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm5\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm6\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm7\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm8\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm10\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm11\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm12\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm13\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm14\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm15\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm16\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm17\n\t"
        "vfmadd213pd %xmm1,%xmm0,%xmm18\n\t"
	);
	}



}

void avx_dp_add256_skl(ulong n_iterations)
{
    ulong i;

    __asm__(
    ".align 64\n\t"
	);
    for (i = 0; i < n_iterations; i++) {
        __asm__(
        "vaddpd %ymm1,%ymm0,%ymm2\n\t"
        "vaddpd %ymm1,%ymm0,%ymm3\n\t"
        "vaddpd %ymm1,%ymm0,%ymm4\n\t"
        "vaddpd %ymm1,%ymm0,%ymm5\n\t"
        "vaddpd %ymm1,%ymm0,%ymm6\n\t"
        "vaddpd %ymm1,%ymm0,%ymm7\n\t"
        "vaddpd %ymm1,%ymm0,%ymm8\n\t"
        "vaddpd %ymm1,%ymm0,%ymm10\n\t"
        "vaddpd %ymm1,%ymm0,%ymm11\n\t"
        "vaddpd %ymm1,%ymm0,%ymm12\n\t"
        "vaddpd %ymm1,%ymm0,%ymm13\n\t"
        "vaddpd %ymm1,%ymm0,%ymm14\n\t"
        "vaddpd %ymm1,%ymm0,%ymm15\n\t"
        "vaddpd %ymm1,%ymm0,%ymm16\n\t"
        "vaddpd %ymm1,%ymm0,%ymm17\n\t"
        "vaddpd %ymm1,%ymm0,%ymm18\n\t"
        "vaddpd %ymm1,%ymm0,%ymm2\n\t"
        "vaddpd %ymm1,%ymm0,%ymm3\n\t"
        "vaddpd %ymm1,%ymm0,%ymm4\n\t"
        "vaddpd %ymm1,%ymm0,%ymm5\n\t"
        "vaddpd %ymm1,%ymm0,%ymm6\n\t"
        "vaddpd %ymm1,%ymm0,%ymm7\n\t"
        "vaddpd %ymm1,%ymm0,%ymm8\n\t"
        "vaddpd %ymm1,%ymm0,%ymm10\n\t"
        "vaddpd %ymm1,%ymm0,%ymm11\n\t"
        "vaddpd %ymm1,%ymm0,%ymm12\n\t"
        "vaddpd %ymm1,%ymm0,%ymm13\n\t"
        "vaddpd %ymm1,%ymm0,%ymm14\n\t"
        "vaddpd %ymm1,%ymm0,%ymm15\n\t"
        "vaddpd %ymm1,%ymm0,%ymm16\n\t"
        "vaddpd %ymm1,%ymm0,%ymm17\n\t"
        "vaddpd %ymm1,%ymm0,%ymm18\n\t"
        "vaddpd %ymm1,%ymm0,%ymm2\n\t"
        "vaddpd %ymm1,%ymm0,%ymm3\n\t"
        "vaddpd %ymm1,%ymm0,%ymm4\n\t"
        "vaddpd %ymm1,%ymm0,%ymm5\n\t"
        "vaddpd %ymm1,%ymm0,%ymm6\n\t"
        "vaddpd %ymm1,%ymm0,%ymm7\n\t"
        "vaddpd %ymm1,%ymm0,%ymm8\n\t"
        "vaddpd %ymm1,%ymm0,%ymm10\n\t"
        "vaddpd %ymm1,%ymm0,%ymm11\n\t"
        "vaddpd %ymm1,%ymm0,%ymm12\n\t"
        "vaddpd %ymm1,%ymm0,%ymm13\n\t"
        "vaddpd %ymm1,%ymm0,%ymm14\n\t"
        "vaddpd %ymm1,%ymm0,%ymm15\n\t"
        "vaddpd %ymm1,%ymm0,%ymm16\n\t"
        "vaddpd %ymm1,%ymm0,%ymm17\n\t"
        "vaddpd %ymm1,%ymm0,%ymm18\n\t"
        "vaddpd %ymm1,%ymm0,%ymm2\n\t"
        "vaddpd %ymm1,%ymm0,%ymm3\n\t"
        "vaddpd %ymm1,%ymm0,%ymm4\n\t"
        "vaddpd %ymm1,%ymm0,%ymm5\n\t"
        "vaddpd %ymm1,%ymm0,%ymm6\n\t"
        "vaddpd %ymm1,%ymm0,%ymm7\n\t"
        "vaddpd %ymm1,%ymm0,%ymm8\n\t"
        "vaddpd %ymm1,%ymm0,%ymm10\n\t"
        "vaddpd %ymm1,%ymm0,%ymm11\n\t"
        "vaddpd %ymm1,%ymm0,%ymm12\n\t"
        "vaddpd %ymm1,%ymm0,%ymm13\n\t"
        "vaddpd %ymm1,%ymm0,%ymm14\n\t"
        "vaddpd %ymm1,%ymm0,%ymm15\n\t"
        "vaddpd %ymm1,%ymm0,%ymm16\n\t"
        "vaddpd %ymm1,%ymm0,%ymm17\n\t"
        "vaddpd %ymm1,%ymm0,%ymm18\n\t"
        );

    }

}

void avx_dp_mul256_skl(ulong n_iterations)
{
    ulong i;


    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {
        __asm__(
        "vmulpd %ymm1,%ymm0,%ymm2\n\t"
        "vmulpd %ymm1,%ymm0,%ymm3\n\t"
        "vmulpd %ymm1,%ymm0,%ymm4\n\t"
        "vmulpd %ymm1,%ymm0,%ymm5\n\t"
        "vmulpd %ymm1,%ymm0,%ymm6\n\t"
        "vmulpd %ymm1,%ymm0,%ymm7\n\t"
        "vmulpd %ymm1,%ymm0,%ymm8\n\t"
        "vmulpd %ymm1,%ymm0,%ymm10\n\t"
        "vmulpd %ymm1,%ymm0,%ymm11\n\t"
        "vmulpd %ymm1,%ymm0,%ymm12\n\t"
        "vmulpd %ymm1,%ymm0,%ymm13\n\t"
        "vmulpd %ymm1,%ymm0,%ymm14\n\t"
        "vmulpd %ymm1,%ymm0,%ymm15\n\t"
        "vmulpd %ymm1,%ymm0,%ymm16\n\t"
        "vmulpd %ymm1,%ymm0,%ymm17\n\t"
        "vmulpd %ymm1,%ymm0,%ymm18\n\t"
        "vmulpd %ymm1,%ymm0,%ymm2\n\t"
        "vmulpd %ymm1,%ymm0,%ymm3\n\t"
        "vmulpd %ymm1,%ymm0,%ymm4\n\t"
        "vmulpd %ymm1,%ymm0,%ymm5\n\t"
        "vmulpd %ymm1,%ymm0,%ymm6\n\t"
        "vmulpd %ymm1,%ymm0,%ymm7\n\t"
        "vmulpd %ymm1,%ymm0,%ymm8\n\t"
        "vmulpd %ymm1,%ymm0,%ymm10\n\t"
        "vmulpd %ymm1,%ymm0,%ymm11\n\t"
        "vmulpd %ymm1,%ymm0,%ymm12\n\t"
        "vmulpd %ymm1,%ymm0,%ymm13\n\t"
        "vmulpd %ymm1,%ymm0,%ymm14\n\t"
        "vmulpd %ymm1,%ymm0,%ymm15\n\t"
        "vmulpd %ymm1,%ymm0,%ymm16\n\t"
        "vmulpd %ymm1,%ymm0,%ymm17\n\t"
        "vmulpd %ymm1,%ymm0,%ymm18\n\t"
        "vmulpd %ymm1,%ymm0,%ymm2\n\t"
        "vmulpd %ymm1,%ymm0,%ymm3\n\t"
        "vmulpd %ymm1,%ymm0,%ymm4\n\t"
        "vmulpd %ymm1,%ymm0,%ymm5\n\t"
        "vmulpd %ymm1,%ymm0,%ymm6\n\t"
        "vmulpd %ymm1,%ymm0,%ymm7\n\t"
        "vmulpd %ymm1,%ymm0,%ymm8\n\t"
        "vmulpd %ymm1,%ymm0,%ymm10\n\t"
        "vmulpd %ymm1,%ymm0,%ymm11\n\t"
        "vmulpd %ymm1,%ymm0,%ymm12\n\t"
        "vmulpd %ymm1,%ymm0,%ymm13\n\t"
        "vmulpd %ymm1,%ymm0,%ymm14\n\t"
        "vmulpd %ymm1,%ymm0,%ymm15\n\t"
        "vmulpd %ymm1,%ymm0,%ymm16\n\t"
        "vmulpd %ymm1,%ymm0,%ymm17\n\t"
        "vmulpd %ymm1,%ymm0,%ymm18\n\t"
        "vmulpd %ymm1,%ymm0,%ymm2\n\t"
        "vmulpd %ymm1,%ymm0,%ymm3\n\t"
        "vmulpd %ymm1,%ymm0,%ymm4\n\t"
        "vmulpd %ymm1,%ymm0,%ymm5\n\t"
        "vmulpd %ymm1,%ymm0,%ymm6\n\t"
        "vmulpd %ymm1,%ymm0,%ymm7\n\t"
        "vmulpd %ymm1,%ymm0,%ymm8\n\t"
        "vmulpd %ymm1,%ymm0,%ymm10\n\t"
        "vmulpd %ymm1,%ymm0,%ymm11\n\t"
        "vmulpd %ymm1,%ymm0,%ymm12\n\t"
        "vmulpd %ymm1,%ymm0,%ymm13\n\t"
        "vmulpd %ymm1,%ymm0,%ymm14\n\t"
        "vmulpd %ymm1,%ymm0,%ymm15\n\t"
        "vmulpd %ymm1,%ymm0,%ymm16\n\t"
        "vmulpd %ymm1,%ymm0,%ymm17\n\t"
        "vmulpd %ymm1,%ymm0,%ymm18\n\t"
        );
	}
}
void fma_dp_fmadd256_skl(ulong n_iterations)
{
    ulong i;
    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {

    __asm__(
        "vfmadd213pd %ymm1,%ymm0,%ymm2\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm3\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm4\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm5\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm6\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm7\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm8\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm10\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm11\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm12\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm13\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm14\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm15\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm16\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm17\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm18\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm2\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm3\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm4\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm5\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm6\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm7\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm8\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm10\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm11\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm12\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm13\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm14\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm15\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm16\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm17\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm18\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm2\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm3\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm4\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm5\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm6\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm7\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm8\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm10\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm11\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm12\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm13\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm14\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm15\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm16\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm17\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm18\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm2\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm3\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm4\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm5\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm6\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm7\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm8\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm10\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm11\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm12\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm13\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm14\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm15\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm16\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm17\n\t"
        "vfmadd213pd %ymm1,%ymm0,%ymm18\n\t"
        );
}

}

void avx512_dp_add512_skl(ulong n_iterations)
{
    ulong i;



    __asm__(
    ".align 64\n\t"
	);
    for (i = 0; i < n_iterations; i++) {


	__asm__(
        "vaddpd %zmm1,%zmm0,%zmm2\n\t"
        "vaddpd %zmm1,%zmm0,%zmm3\n\t"
        "vaddpd %zmm1,%zmm0,%zmm4\n\t"
        "vaddpd %zmm1,%zmm0,%zmm5\n\t"
        "vaddpd %zmm1,%zmm0,%zmm6\n\t"
        "vaddpd %zmm1,%zmm0,%zmm7\n\t"
        "vaddpd %zmm1,%zmm0,%zmm8\n\t"
        "vaddpd %zmm1,%zmm0,%zmm10\n\t"
        "vaddpd %zmm1,%zmm0,%zmm11\n\t"
        "vaddpd %zmm1,%zmm0,%zmm12\n\t"
        "vaddpd %zmm1,%zmm0,%zmm13\n\t"
        "vaddpd %zmm1,%zmm0,%zmm14\n\t"
        "vaddpd %zmm1,%zmm0,%zmm15\n\t"
        "vaddpd %zmm1,%zmm0,%zmm16\n\t"
        "vaddpd %zmm1,%zmm0,%zmm17\n\t"
        "vaddpd %zmm1,%zmm0,%zmm18\n\t"
        "vaddpd %zmm1,%zmm0,%zmm2\n\t"
        "vaddpd %zmm1,%zmm0,%zmm3\n\t"
        "vaddpd %zmm1,%zmm0,%zmm4\n\t"
        "vaddpd %zmm1,%zmm0,%zmm5\n\t"
        "vaddpd %zmm1,%zmm0,%zmm6\n\t"
        "vaddpd %zmm1,%zmm0,%zmm7\n\t"
        "vaddpd %zmm1,%zmm0,%zmm8\n\t"
        "vaddpd %zmm1,%zmm0,%zmm10\n\t"
        "vaddpd %zmm1,%zmm0,%zmm11\n\t"
        "vaddpd %zmm1,%zmm0,%zmm12\n\t"
        "vaddpd %zmm1,%zmm0,%zmm13\n\t"
        "vaddpd %zmm1,%zmm0,%zmm14\n\t"
        "vaddpd %zmm1,%zmm0,%zmm15\n\t"
        "vaddpd %zmm1,%zmm0,%zmm16\n\t"
        "vaddpd %zmm1,%zmm0,%zmm17\n\t"
        "vaddpd %zmm1,%zmm0,%zmm18\n\t"
        "vaddpd %zmm1,%zmm0,%zmm2\n\t"
        "vaddpd %zmm1,%zmm0,%zmm3\n\t"
        "vaddpd %zmm1,%zmm0,%zmm4\n\t"
        "vaddpd %zmm1,%zmm0,%zmm5\n\t"
        "vaddpd %zmm1,%zmm0,%zmm6\n\t"
        "vaddpd %zmm1,%zmm0,%zmm7\n\t"
        "vaddpd %zmm1,%zmm0,%zmm8\n\t"
        "vaddpd %zmm1,%zmm0,%zmm10\n\t"
        "vaddpd %zmm1,%zmm0,%zmm11\n\t"
        "vaddpd %zmm1,%zmm0,%zmm12\n\t"
        "vaddpd %zmm1,%zmm0,%zmm13\n\t"
        "vaddpd %zmm1,%zmm0,%zmm14\n\t"
        "vaddpd %zmm1,%zmm0,%zmm15\n\t"
        "vaddpd %zmm1,%zmm0,%zmm16\n\t"
        "vaddpd %zmm1,%zmm0,%zmm17\n\t"
        "vaddpd %zmm1,%zmm0,%zmm18\n\t"
        "vaddpd %zmm1,%zmm0,%zmm2\n\t"
        "vaddpd %zmm1,%zmm0,%zmm3\n\t"
        "vaddpd %zmm1,%zmm0,%zmm4\n\t"
        "vaddpd %zmm1,%zmm0,%zmm5\n\t"
        "vaddpd %zmm1,%zmm0,%zmm6\n\t"
        "vaddpd %zmm1,%zmm0,%zmm7\n\t"
        "vaddpd %zmm1,%zmm0,%zmm8\n\t"
        "vaddpd %zmm1,%zmm0,%zmm10\n\t"
        "vaddpd %zmm1,%zmm0,%zmm11\n\t"
        "vaddpd %zmm1,%zmm0,%zmm12\n\t"
        "vaddpd %zmm1,%zmm0,%zmm13\n\t"
        "vaddpd %zmm1,%zmm0,%zmm14\n\t"
        "vaddpd %zmm1,%zmm0,%zmm15\n\t"
        "vaddpd %zmm1,%zmm0,%zmm16\n\t"
        "vaddpd %zmm1,%zmm0,%zmm17\n\t"
        "vaddpd %zmm1,%zmm0,%zmm18\n\t"
	);

    }

}

void avx512_dp_mul512_skl(ulong n_iterations)
{
    ulong i;


    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {
        __asm__(
        "vmulpd %zmm1,%zmm0,%zmm2\n\t"
        "vmulpd %zmm1,%zmm0,%zmm3\n\t"
        "vmulpd %zmm1,%zmm0,%zmm4\n\t"
        "vmulpd %zmm1,%zmm0,%zmm5\n\t"
        "vmulpd %zmm1,%zmm0,%zmm6\n\t"
        "vmulpd %zmm1,%zmm0,%zmm7\n\t"
        "vmulpd %zmm1,%zmm0,%zmm8\n\t"
        "vmulpd %zmm1,%zmm0,%zmm10\n\t"
        "vmulpd %zmm1,%zmm0,%zmm11\n\t"
        "vmulpd %zmm1,%zmm0,%zmm12\n\t"
        "vmulpd %zmm1,%zmm0,%zmm13\n\t"
        "vmulpd %zmm1,%zmm0,%zmm14\n\t"
        "vmulpd %zmm1,%zmm0,%zmm15\n\t"
        "vmulpd %zmm1,%zmm0,%zmm16\n\t"
        "vmulpd %zmm1,%zmm0,%zmm17\n\t"
        "vmulpd %zmm1,%zmm0,%zmm18\n\t"
        "vmulpd %zmm1,%zmm0,%zmm2\n\t"
        "vmulpd %zmm1,%zmm0,%zmm3\n\t"
        "vmulpd %zmm1,%zmm0,%zmm4\n\t"
        "vmulpd %zmm1,%zmm0,%zmm5\n\t"
        "vmulpd %zmm1,%zmm0,%zmm6\n\t"
        "vmulpd %zmm1,%zmm0,%zmm7\n\t"
        "vmulpd %zmm1,%zmm0,%zmm8\n\t"
        "vmulpd %zmm1,%zmm0,%zmm10\n\t"
        "vmulpd %zmm1,%zmm0,%zmm11\n\t"
        "vmulpd %zmm1,%zmm0,%zmm12\n\t"
        "vmulpd %zmm1,%zmm0,%zmm13\n\t"
        "vmulpd %zmm1,%zmm0,%zmm14\n\t"
        "vmulpd %zmm1,%zmm0,%zmm15\n\t"
        "vmulpd %zmm1,%zmm0,%zmm16\n\t"
        "vmulpd %zmm1,%zmm0,%zmm17\n\t"
        "vmulpd %zmm1,%zmm0,%zmm18\n\t"
        "vmulpd %zmm1,%zmm0,%zmm2\n\t"
        "vmulpd %zmm1,%zmm0,%zmm3\n\t"
        "vmulpd %zmm1,%zmm0,%zmm4\n\t"
        "vmulpd %zmm1,%zmm0,%zmm5\n\t"
        "vmulpd %zmm1,%zmm0,%zmm6\n\t"
        "vmulpd %zmm1,%zmm0,%zmm7\n\t"
        "vmulpd %zmm1,%zmm0,%zmm8\n\t"
        "vmulpd %zmm1,%zmm0,%zmm10\n\t"
        "vmulpd %zmm1,%zmm0,%zmm11\n\t"
        "vmulpd %zmm1,%zmm0,%zmm12\n\t"
        "vmulpd %zmm1,%zmm0,%zmm13\n\t"
        "vmulpd %zmm1,%zmm0,%zmm14\n\t"
        "vmulpd %zmm1,%zmm0,%zmm15\n\t"
        "vmulpd %zmm1,%zmm0,%zmm16\n\t"
        "vmulpd %zmm1,%zmm0,%zmm17\n\t"
        "vmulpd %zmm1,%zmm0,%zmm18\n\t"
        "vmulpd %zmm1,%zmm0,%zmm2\n\t"
        "vmulpd %zmm1,%zmm0,%zmm3\n\t"
        "vmulpd %zmm1,%zmm0,%zmm4\n\t"
        "vmulpd %zmm1,%zmm0,%zmm5\n\t"
        "vmulpd %zmm1,%zmm0,%zmm6\n\t"
        "vmulpd %zmm1,%zmm0,%zmm7\n\t"
        "vmulpd %zmm1,%zmm0,%zmm8\n\t"
        "vmulpd %zmm1,%zmm0,%zmm10\n\t"
        "vmulpd %zmm1,%zmm0,%zmm11\n\t"
        "vmulpd %zmm1,%zmm0,%zmm12\n\t"
        "vmulpd %zmm1,%zmm0,%zmm13\n\t"
        "vmulpd %zmm1,%zmm0,%zmm14\n\t"
        "vmulpd %zmm1,%zmm0,%zmm15\n\t"
        "vmulpd %zmm1,%zmm0,%zmm16\n\t"
        "vmulpd %zmm1,%zmm0,%zmm17\n\t"
        "vmulpd %zmm1,%zmm0,%zmm18\n\t"
        );

    }

}

void avx512_dp_fmadd512_skl(ulong n_iterations)
{
    ulong i;


    __asm__(
	"movl    $1, %eax\n\t"
        "kmovb   %eax, %k3\n\t"
	);
    __asm__(
    ".align 64\n\t"
	);

    for (i = 0; i < n_iterations; i++) {

    __asm__(
        "vfmadd213pd %zmm1,%zmm0,%zmm2\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm3\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm4\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm5\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm6\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm7\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm8\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm10\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm11\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm12\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm13\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm14\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm15\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm16\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm17\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm18\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm2\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm3\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm4\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm5\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm6\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm7\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm8\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm10\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm11\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm12\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm13\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm14\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm15\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm16\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm17\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm18\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm2\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm3\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm4\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm5\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm6\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm7\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm8\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm10\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm11\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm12\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm13\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm14\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm15\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm16\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm17\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm18\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm2\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm3\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm4\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm5\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm6\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm7\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm8\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm10\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm11\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm12\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm13\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm14\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm15\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm16\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm17\n\t"
        "vfmadd213pd %zmm1,%zmm0,%zmm18\n\t"
        );
}


}
// SKYLAKE tests

void run_test(int index,ulong n_iterations,int cpu)
{
    if ((cpu==CPU_SKYLAKE) || (cpu==CPU_SKYLAKE_X))
    {
        switch(index)
        {
            case 0: sse2_dp_add128_skl(n_iterations); break;
            case 1: sse2_dp_mul128_skl(n_iterations); break;
            case 2: fma_dp_fmadd128_skl(n_iterations); break;
            case 3: avx_dp_add256_skl(n_iterations); break;
            case 4: avx_dp_mul256_skl(n_iterations); break;
            case 5: fma_dp_fmadd256_skl(n_iterations); break;
            case 6: avx512_dp_add512_skl(n_iterations); break;
            case 7: avx512_dp_mul512_skl(n_iterations); break;
            case 8: avx512_dp_fmadd512_skl(n_iterations); break;
        }
    } else if ((cpu==CPU_HASWELL) || (cpu==CPU_HASWELL_X))
    {
        switch(index)
        {
            case 0: sse2_dp_add128_hsw(n_iterations); break;
            case 1: sse2_dp_mul128_hsw(n_iterations); break;
            case 2: fma_dp_fmadd128_hsw(n_iterations); break;
            case 3: avx_dp_add256_hsw(n_iterations); break;
            case 4: avx_dp_mul256_hsw(n_iterations); break;
            case 5: fma_dp_fmadd256_hsw(n_iterations); break;
            case 6:
            case 7:
            case 8:break;
        }
    }else{
    	return;
    }
}

char *get_test_name(int index,int cpu)
{
    switch(index) {
        case 0: return "SSE2 DP ADD128";
        case 1: return "SSE2 DP MUL128";
        case 2: return "FMA DP FMADD128";
        case 3: return "AVX DP ADD256";
        case 4: return "AVX DP MUL256";
        case 5: return "FMA DP FMADD256";
        case 6: 
		if ((cpu == CPU_SKYLAKE) || (cpu==CPU_SKYLAKE_X)) return "AVX-512F DP ADD512";
		else return "NOT_SUPPORTED";
        case 7: 
		if ((cpu == CPU_SKYLAKE) || (cpu==CPU_SKYLAKE_X)) return "AVX-512F DP MUL512";
		else return "NOT_SUPPORTED";
        case 8: 
		if ((cpu==CPU_SKYLAKE) || (cpu==CPU_SKYLAKE_X)) return "AVX-512F DP FMADD512";
		else return "NOT_SUPPORTED";
    }
}

uint get_test_ops(int index,int cpu)
{
    switch(index) {
        case 0: return (128 / 64);
        case 1: return (128 / 64);
        case 2: return (128 / 64) * 2;
        case 3: return (256 / 64);
        case 4: return (256 / 64);
        case 5: return (256 / 64) * 2;
        case 6: if ((cpu==CPU_SKYLAKE) || (cpu==CPU_SKYLAKE_X)) return (512 / 64);
		else return 0;
        case 7: if ((cpu==CPU_SKYLAKE) || (cpu==CPU_SKYLAKE_X)) return (512 / 64);
		else return 0;
        case 8: if ((cpu==CPU_SKYLAKE) || (cpu==CPU_SKYLAKE_X)) return (512 / 64) * 2;
		else return 0;

    }
}
int test_available(int i,int cpu)
{
	switch(i){
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			return 1;
		case 6:
		case 7:
		case 8:
			 if ((cpu==CPU_SKYLAKE) || (cpu==CPU_SKYLAKE_X)) return 1;
			else return 0;
	}
}
