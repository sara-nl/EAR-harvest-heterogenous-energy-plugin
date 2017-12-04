#include <assert.h>
#include <cpuid.h>

int main ()
{
    unsigned int aperf;
    unsigned int lev = 0;
    unsigned int eax = 0;
    unsigned int ebx = 0;
    unsigned int ecx = 0;
    unsigned int edx = 0;
    int result;

    lev = 6;
    result = __get_cpuid(lev, &eax, &ebx, &ecx, &edx);
    assert(result == 1);
    assert((ecx & 0x00000001));

    return 0;
}
