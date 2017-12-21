#include <cpuid.h>
#include <assert.h>
#include <intel_model_list.h>

int main ()
{
    char vendor_id[12];
    int *p = (int *) &vendor_id;
    int result;

    unsigned int model;
    unsigned int lev = 0;
    unsigned int eax = 0;
    unsigned int ebx = 0;
    unsigned int ecx = 0;
    unsigned int edx = 0;

    // Vendor
    lev = 0;
    result = __get_cpuid(lev, &eax, &ebx, &ecx, &edx);
    assert(result == 1);

    p[0] = ebx;
    p[1] = edx;
    p[2] = ecx;
    assert(strcmp(vendor_id, INTEL_VENDOR_NAME) == 0);

    // Model
    lev = 1;
    result = __get_cpuid(lev, &eax, &ebx, &ecx, &edx);
    assert(result == 1);

    model = (eax >> 4) & 0x0000000F;
    model = model | ((eax >> 12) & 0x000000FF);
    //assert(IS_MY_INTEL_CPU_COMPATIBLE(model));

    return 0;
}
