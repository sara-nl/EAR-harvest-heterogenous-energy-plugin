/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <ear_arch_type.h>
#include <string.h>

#define CPUIDREGS(EAX,ECX)  \
    unsigned int eax = EAX; \
    unsigned int ebx = 0;   \
    unsigned int ecx = ECX; \
    unsigned int edx = 0;   \
    native_cpuid(&eax, &ebx, &ecx, &edx);

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

static int get_vendor_id(char *vendor_id)
{
	int *pointer = (int *) vendor_id;
	CPUIDREGS(0,0);
	pointer[0] = ebx;
	pointer[1] = edx;
	pointer[2] = ecx;
	return 1;
}

static int get_family()
{
	CPUIDREGS(1,0);
	return extract_bits(eax, 11, 8);
}

int get_model()
{
	char vendor_id[12];
	int model_full;
	int model_low;

	get_vendor_id(vendor_id);

	if (strncmp(vendor_id, "GenuineIntel", 12)) {
		return CPU_UNIDENTIFIED;
	}

	if (get_family() != 6) {
		return CPU_UNIDENTIFIED;
	}

	CPUIDREGS(1,0);
	model_low  = extract_bits(eax, 7, 4);
	model_full = extract_bits(eax, 19, 16);
	return (model_full << 4) | model_low;
}

int is_aperf_compatible()
{
	CPUIDREGS(6,0);
	return ecx & 0x01;
}

int get_cache_line_size()
{
    unsigned int level = 0;
    unsigned int max_level = 0;
    unsigned int line_size = 0;
	int index = 0;

	while (1)
	{
		CPUIDREGS(4,index);
		if (!(eax & 0x0F)) return line_size;
        level = extract_bits(eax, 7, 5);

        if (level >= max_level) {
            max_level = level;
            line_size = extract_bits(ebx, 11, 0) + 1;
		}

		index = index + 1;
	}
}