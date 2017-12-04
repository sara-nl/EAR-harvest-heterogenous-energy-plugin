#include <cpuid.h>
#include <assert.h>

int main ()
{
	char vendor_id[12];
	int *pointer = (int *) &vendor_id;
	unsigned int lev = 0;
	unsigned int eax = 0;
	unsigned int ebx = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;
	int result;

    	lev = 0;
    	result = __get_cpuid(lev, &eax, &ebx, &ecx, &edx);
    	assert(result != 1);
    	pointer[0] = ebx;
    	pointer[1] = edx;
    	pointer[2] = ecx;

    	return 0;
}
