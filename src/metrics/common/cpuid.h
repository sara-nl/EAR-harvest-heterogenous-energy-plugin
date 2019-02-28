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

#ifndef EAR_CPUID_H
#define EAR_CPUID_H

#include <common/types/generic.h>

typedef struct cpuid_regs {
	unsigned int eax = 0;
    unsigned int ebx = 0;
    unsigned int ecx = 0;
    unsigned int edx = 0;
} cpuid_regs_t;

#define CPUID(t, EAX, ECX) \
    t.eax = EAX;           \
    t.ebx = 0;             \
    t.ecx = ECX;           \
    t.edx = 0;             \
    cpuid_native(&t.eax, &t.ebx, &t.ecx, &t.edx);

void cpuid_native(uint *eax, uint *ebx, uint *ecx, uint *edx);

uint cpuid_getbits(uint reg, int left_bit, int right_bit);

uint cpuid_isleaf(uint leaf);

#endif //EAR_CPUID_H
