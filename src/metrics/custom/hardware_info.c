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

#include <stdio.h>
#include <string.h>

#include <metrics/custom/hardware_info.h>
#include <common/states.h>

#define CREGS()           \
    unsigned int eax = 0; \
    unsigned int ebx = 0; \
    unsigned int ecx = 0; \
    unsigned int edx = 0;

#define CPUID(EAX,ECX)    \
    eax = EAX;            \
    ebx = 0;              \
    ecx = ECX;            \
    edx = 0;              \
    native_cpuid(&eax, &ebx, &ecx, &edx);

#define IS_LEAF(num)      \
    is_cpu_leaf_present(num) == EAR_SUCCESS

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
								unsigned int *ecx, unsigned int *edx)
{
	asm volatile("cpuid"
		: "=a" (*eax),
		  "=b" (*ebx),
		  "=c" (*ecx),
		  "=d" (*edx)
		: "0"  (*eax), "2" (*ecx));
}

static unsigned int extract_bits(unsigned int reg, int left_bit, int right_bit)
{
	unsigned int shift = left_bit - right_bit;
	unsigned int mask = (((0x01 << shift) - 1) << 1) + 1;
	return ((reg >> right_bit) & mask);
}

static unsigned int is_cpu_leaf_present(unsigned int leaf)
{
    CREGS();

    CPUID(0,0);
    // If max leafs are less
    if (eax < leaf)
        return EAR_ERROR;

    CPUID(leaf,0);
    // EBX confirms its presence
    if (ebx == 0)
        return EAR_ERROR;

    return EAR_SUCCESS;
}

int is_cpu_examinable()
{
    char vendor_id[16];
    get_vendor_id(vendor_id);
    if(strcmp(INTEL_VENDOR_NAME, vendor_id) != 0)
        return EAR_ERROR;
    if(!IS_LEAF(11))
        return EAR_WARNING;
    return EAR_SUCCESS;
}

int get_vendor_id(char *vendor_id)
{
    CREGS();
	int *pointer = (int *) vendor_id;

	CPUID(0,0);
	pointer[0] = ebx;
	pointer[1] = edx;
	pointer[2] = ecx;
	return 1;
}

int get_family()
{
    CREGS();
	CPUID(1,0);
	return extract_bits(eax, 11, 8);
}

int get_model()
{
    CREGS();
	int model_full;
	int model_low;

	CPUID(1,0);
	model_low  = extract_bits(eax, 7, 4);
	model_full = extract_bits(eax, 19, 16);
	return (model_full << 4) | model_low;
}

int is_aperf_compatible()
{
    CREGS();

    if(!IS_LEAF(11))
        return EAR_ERROR;

    CPUID(6,0);
    return ecx & 0x01;
}

int get_cache_line_size()
{
    CREGS();
    unsigned int level = 0;
    unsigned int max_level = 0;
    unsigned int line_size = 0;
	int index = 0;

	while (1)
	{
		CPUID(4,index);
		if (!(eax & 0x0F)) return line_size;
        level = extract_bits(eax, 7, 5);

        if (level >= max_level) {
            max_level = level;
            line_size = extract_bits(ebx, 11, 0) + 1;
		}

		index = index + 1;
	}
}

int is_cpu_hyperthreading_capable()
{
    CREGS();
    CPUID(1,0);
    return extract_bits(edx, 28, 28);
}

// References:
// 	- Intel System Programming Guide
//    Vol. 2A, Software Developer Manual
//	  3-193, INSTRUCTION SET REFERENCE, A-L
// 	  Thermal and Power Management Leaf
int is_cpu_boost_enabled()
{
	CREGS();

	CPUID(6,0);

	return extract_bits(eax, 1, 1);
}

int get_cpu_logical_processors()
{
    CREGS();

    if(!IS_LEAF(11))
        return EAR_ERROR;

    return EAR_ERROR;
}

int get_cpu_physical_processors()
{
	return EAR_ERROR;
}

int is_cpu_topology_mapable()
{
    return EAR_ERROR;
}