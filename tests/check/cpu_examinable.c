#include <assert.h>
#include <hardware.h>

int main ()
{
    int examinable = is_cpu_examinable();
    assert(examinable == EAR_SUCCESS);
    return 0;
}
